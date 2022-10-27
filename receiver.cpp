#include <iostream>
#include <iterator>
#include <algorithm>
#include <condition_variable>
#include <ranges>
#include <cmath>

#include "connectors.hpp"
#include "data.hpp"
#include "transport.hpp"
#include "launcher.hpp"


using namespace latency;

class receiver final {
public:

    receiver(std::string name)
        : transport_(name.c_str()) {
    }

    void run() {
        transport_lock _(transport_);

        data::header h;
        for (auto i = 0; i < 10; ++i) {
            transport_.recv(h);
        }

        std::unique_lock lock(m_);
        cv_.notify_all();
    }

    void stop() {
        std::unique_lock lock(m_);
        cv_.wait(lock);
    }

    void print() const {
        std::cout << transport_.get_latencies().size() << std::endl;
        if (transport_.get_latencies().size() > 0)
            std::ranges::copy(transport_.get_latencies(), std::ostream_iterator<data::time_stamp_t>(std::cout, ", "));

        // copy
        auto arr = transport_.get_latencies();
        std::sort(std::begin(arr), std::end(arr));

        // I have had implemented a streaming percentiles for one of the projects, based on the methodology described
        // https://github.com/sengelha/streaming-percentiles/blob/main/doc/methodology/CKMS05.pdf
        int q50 = static_cast<int>(std::roundf(0.5 * arr.size()));
        std::cout << "q50 = " << arr[q50] << " ticks" << std::endl;
        int q95 = static_cast<int>(std::roundf(0.95 * arr.size()));
        std::cout << "q95 = " << arr[q95] << " ticks" << std::endl;
        int q99 = static_cast<int>(std::roundf(0.99 * arr.size()));
        std::cout << "q99 = " << arr[q99] << " ticks" << std::endl;
    }

private:
    transport<connectors::shmem> transport_;
    std::mutex m_;
    std::condition_variable cv_;
};

int main(int, char**) {
    
    tools::launcher<receiver> app("/tmp/fifo123");

    app.start();
    app.stop();
    app->print();
}

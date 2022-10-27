#include <iostream>
#include <iterator>
#include <algorithm>
#include <condition_variable>

#include "connectors.hpp"
#include "data.hpp"
#include "transport.hpp"
#include "launcher.hpp"


using namespace latency;

class sender final {
public:

    sender(std::string name)
        : transport_(name.c_str()) {
    }

    void run() {
        transport_lock _(transport_);

        data::header h;
        for (auto i = 0; i < 10; ++i) {
            h.payload = i;
            transport_.send(h);
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
    }

private:
    transport<connectors::shmem> transport_;
    std::mutex m_;
    std::condition_variable cv_;
};

int main(int, char**) {
    
    tools::launcher<sender> app("/tmp/fifo123");

    app.start();
    app.stop();
}

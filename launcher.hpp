#pragma once

#include <thread>
#include <mutex>
#include <utility>

namespace tools {

    template <typename T>
    class launcher final {
    public:
        template <typename... Params>
        launcher(Params&&... params) 
            : instance_(std::forward<Params>(params)...)
            , runner_()
        { }
        ~launcher() { }

        template <typename... Args>
        void start(Args&&... args);
        void stop();

        auto operator*() { return instance_; }
        auto operator->() { return &instance_; }
        auto operator*() const { return instance_; }
        auto operator->() const { return &instance_; }
    private:
        T instance_;
        std::thread runner_;
        std::mutex m_;
    };


    template <typename T>
    template <typename... Args>
    inline void launcher<T>::start(Args&&... args) {
        std::unique_lock _(m_);
        runner_ = std::thread(&T::run, std::ref(instance_), std::forward<Args>(args)...);
    }

    template <typename T>
    inline void launcher<T>::stop() {
        std::unique_lock _(m_);
        instance_.stop();
        runner_.join();
    }

}


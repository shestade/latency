#pragma once

#include <iostream>
#include <vector>

#include <unistd.h>

#include "rdtsc.hpp"
#include "data.hpp"

namespace latency {

template <typename T>
struct transport_lock {
    transport_lock(T& t) : t(t) {
        t.open();
    }
    ~transport_lock() {
        t.close();
    }
private:
    T& t;
};


template <typename Connector>
class transport {
public:
    transport(const char* name) : con_(name) {
        latencies_.reserve(1024);
    }

    void open() {
        auto res = con_.open();
        if (res == -1) std::cerr << "Transport open error" << std::endl;
    }

    void close() {
        con_.close();
    }

    inline bool send(data::header& hdr) {
        
        hdr.tsc = rdtsc();
        con_.write(&hdr, sizeof(hdr));
        return true;
    }

    inline bool recv(data::header& hdr) {
        con_.read(&hdr, sizeof(hdr));
        latencies_.emplace_back(rdtsc() - hdr.tsc);
        return true;
    }

    const std::vector<data::time_stamp_t>& get_latencies() const {
        return latencies_;
    }
    std::vector<data::time_stamp_t>& get_latencies() {
        return latencies_;
    }

private:
    Connector con_;
    std::vector<data::time_stamp_t> latencies_;
};

}
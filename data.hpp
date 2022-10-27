#pragma once

namespace latency::data {

    using time_stamp_t = unsigned long long;

    struct alignas(64) header {
        time_stamp_t tsc;
        int payload;
    };
    
}
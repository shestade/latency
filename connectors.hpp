#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

namespace latency::connectors {

// Define a new media/connector with interface
// class <name>> {
// public:
//     int open();
//     int close();
//     int read(void *buf, size_t n);
//     int write(const void *buf, size_t n);
// };


///////////////////////////////////////////////////////////////////////////////

class fifo final { //AKA named pipe
public:
    fifo(std::string name) : name_(std::move(name)) { }

    ~fifo() { }

    int open() {
        int ret;
        if ((ret = ::mkfifo(name_.c_str(), 0777)) != 0 ) {
            if (ret != EEXIST) {
                perror("fifo::mkfifo");
            }
        }
        fd_ = ::open(name_.c_str(), O_RDWR);
        if (fd_ == -1) {
            perror("fifo::open");
            return 1;
        }
        return 0;
    }

    int close() {
        if (::close(fd_)) {
            perror("fifo::close");
            return 1;
        }
        #if 0 // clean if required
        if (::remove(name_)) {
            perror("fifo::remove");
            return 1;
        }
        #endif
        return 0;
    }

    int read(void *buf, size_t n) {
        return ::read(handle(), buf, n);
    }

    int write(const void *buf, size_t n) {
        return ::write(handle(), buf, n);
    }

    int handle() const { return fd_; }

private:
    int fd_;
    std::string name_;
};

///////////////////////////////////////////////////////////////////////////////

class shmem final {
    static constexpr size_t FRAME_SIZE = 1024;
public:

    shmem(std::string name) : name_(std::move(name)) { }

    ~shmem() { }

    int open() {
        fd_ = ::open(name_.c_str(), O_CREAT | O_RDWR, 0777);
        if (fd_ == -1) {
            perror("shmem::open");
            return 1;
        }

        if ((::ftruncate(fd_, FRAME_SIZE) == 0)) {
            int result = ::lseek(fd_, FRAME_SIZE - 1, SEEK_SET);
            if (result == -1) {
                perror("shmem::lseek");   
                ::close(fd_);
                return 1; 
            }

            result = ::write(fd_, "", 1);
            if (result != 1) {
                perror("shmem::write");
                ::close(fd_);
                return 1;
            }
        }

        ptr_ = ::mmap(NULL, FRAME_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
        if (ptr_ == MAP_FAILED || ptr_ == NULL) {
            perror("shmem::mmap");
            ::close(fd_);
            return 1;
        }
        return 0;
    }

    int close() {
        if (::close(fd_)) {
            perror("shmem::close");
            return 1;
        }
        #if 0 // clean if required
        if (::remove(name_)) {
            perror("shmem::remove");
            return 1;
        }
        #endif
        return 0;
    }

    int read(void *buf, size_t n) {
        memcpy(buf, ptr_, n);;
        return 0;
    }

    int write(const void *buf, size_t n) {
        memcpy(ptr_, buf, n);;
        return 0;
    }

private:
    int fd_;
    void *ptr_;
    std::string name_;
};

}
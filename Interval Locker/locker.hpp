#pragma once

#ifndef LOCKER_HPP_
#define LOCKER_HPP_

#include <cstddef>
#include "interval_tree.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>

using keytype = INTERVAL_TREE_HPP_::interval_tree<size_t>::key_type;

class locker;
class exclusive_lock;
enum class LockType{ SHARED, EXCLUSIVE, INVALID };
struct LockInfo {
    LockType type;
    int lockCount;
};

class shared_lock {
public:
    shared_lock(locker& l, keytype& i);
    shared_lock() noexcept {}  // produce invalid object

    shared_lock(const shared_lock&) = delete; // no support for copy
    shared_lock(shared_lock&&) noexcept; // move, invalidate source object
    shared_lock& operator=(const shared_lock&) = delete; // no support for copy
    
    shared_lock& operator=(shared_lock&& other) noexcept {
        if (this == &other)
            return *this;

         unlock();

        l_ptr = other.l_ptr;
        interval = other.interval;
        other.l_ptr = nullptr;

        return *this;
    } // unlock `*this` (if not invalid), move, invalidate source object

    ~shared_lock(); // unlock (if not invalid), noexcept by default

    void unlock() noexcept;  // unlock (if not invalid), invalidate
    exclusive_lock upgrade();   // BLOCKING, upgrade to exclusive_lock, invalidate `*this`
    locker* l_ptr = nullptr;
    keytype interval;
};

class exclusive_lock {
public:
    exclusive_lock(locker& l, keytype& i);
    exclusive_lock() noexcept {}  // produce invalid object

    exclusive_lock(const exclusive_lock&) = delete; // no support for copy
    exclusive_lock(exclusive_lock&&) noexcept; // move, invalidate source object
    exclusive_lock& operator=(const exclusive_lock&) = delete; // no support for copy
    exclusive_lock& operator=(exclusive_lock&& other) noexcept {
        if (this == &other)
            return *this;

   
        unlock();

        l_ptr = other.l_ptr;
        interval = other.interval;
        other.l_ptr = nullptr;
        return *this;
    } // unlock `*this` (if not invalid), move, invalidate source object

    ~exclusive_lock(); // unlock (if not invalid), noexcept by default

    void unlock() noexcept;  // unlock (if not invalid), invalidate
    shared_lock downgrade() noexcept;   // downgrade to shared_lock, invalidate `*this`

    locker* l_ptr = nullptr;
    keytype interval;
};


class locker {
public:
    using size_type = std::size_t;
    locker(){}

    locker(const locker&) = delete; // no support for copy
    locker(locker&&) = delete; // no support for move
    locker& operator=(const locker&) = delete; // no support for copy
    locker& operator=(locker&&) = delete; // no support for move

    void remove_from_tree(keytype interval);

    ~locker();  // BLOCKING, wait until all locks are removed

    shared_lock lock_shared(size_type b, size_type e);  // BLOCKING, create a shared_lock
    exclusive_lock lock_exclusive(size_type b, size_type e);    // BLOCKING, create an exclusive_lock

    shared_lock downgrade(keytype interval);
    exclusive_lock upgrade(keytype interval);
    interval_tree<LockInfo> lock_tree;
    //std::unique_lock<std::mutex> m_lock;
    std::mutex mtx;
    std::condition_variable cv;
};

#endif
#include "locker.hpp"

    shared_lock::shared_lock(locker& l, LOCKER_HPP_::keytype& i) {
        l_ptr = &l;
        interval = i;
    }

    void shared_lock::unlock() noexcept {
        if (l_ptr) {
            
            l_ptr->remove_from_tree(interval);
            l_ptr->cv.notify_all();
        }
        //std::cout << "shared " << interval.first << "," << interval.second << "unlocked" << std:: endl;
        l_ptr = nullptr;
    }
    // BLOCKING, upgrade to exclusive_lock, invalidate `*this`
    exclusive_lock shared_lock::upgrade() {
        auto exl = l_ptr->upgrade(interval);
        l_ptr = nullptr;
        return exl;
    }
    shared_lock::shared_lock(shared_lock&& other) noexcept {
        l_ptr = other.l_ptr;
        interval = other.interval;
        other.l_ptr = nullptr;
    }



    shared_lock::~shared_lock() {
        unlock();
    }


    exclusive_lock::exclusive_lock(locker& l, LOCKER_HPP_::keytype& i) {
        l_ptr = &l;
        interval = i;
    }

    shared_lock exclusive_lock::downgrade() noexcept {
        auto shl = l_ptr->downgrade(interval);
        l_ptr = nullptr;
        return shl;
    }

    void exclusive_lock::unlock() noexcept {
        if (l_ptr) {
            l_ptr->remove_from_tree(interval);
            l_ptr->cv.notify_all();
        }
        //std::cout << "exclusive" << interval.first << "," << interval.second << "unlocked" << std::endl;
        l_ptr = nullptr;
    }

    exclusive_lock locker::upgrade(LOCKER_HPP_::keytype interval) {
        std::unique_lock<std::mutex> lock(mtx);
        auto existing_lock = lock_tree.get_overlap(interval, true);
        while (existing_lock != lock_tree.end()) {
            cv.wait(lock);
            existing_lock = lock_tree.get_overlap(interval, true);
        }
        lock_tree.emplace(interval, LOCKER_HPP_::LockInfo{ LOCKER_HPP_::LockType::EXCLUSIVE, 1 });
        return exclusive_lock(*this, interval);
    }

    shared_lock locker::downgrade(LOCKER_HPP_::keytype interval) {
        std::lock_guard<std::mutex> lock(mtx);
        auto existing_lock = lock_tree.find(interval);
        existing_lock->value.type = LOCKER_HPP_::LockType::SHARED;
        existing_lock->value.lockCount = 1;
        ////unlock();
        auto shl = shared_lock(*this, interval);
        //std::cout << "downgraded" << interval.first << "," << interval.second << std::endl;
        cv.notify_all();
        return shl;
    }

    // BLOCKING, create a shared_lock 
    shared_lock locker::lock_shared(size_type b, size_type e) {
        std::unique_lock<std::mutex> lock(mtx);
        auto interval = std::make_pair(b, e);
        while (true) {
            auto existing_lock = lock_tree.get_overlap(interval);
            if (existing_lock == lock_tree.end()) {
                lock_tree.emplace(interval, LOCKER_HPP_::LockInfo{ LOCKER_HPP_::LockType::SHARED, 1});
                //std::cout << "interval" << b << "," << e << " " << std::endl;

                return shared_lock(*this, interval);
            }
            else if (existing_lock->value.type == LOCKER_HPP_::LockType::SHARED) {
                existing_lock->value.lockCount++;
                return shared_lock(*this, existing_lock->key);
            }
            else {
                cv.wait(lock);
            }
        }
    }
    // BLOCKING, create an exclusive_lock
    exclusive_lock locker::lock_exclusive(size_type b, size_type e) {
        
        std::unique_lock<std::mutex> lock(mtx);
        auto interval = std::make_pair(b, e);

        auto existing_lock = lock_tree.get_overlap(interval);
        while (existing_lock != lock_tree.end()) {
            cv.wait(lock);
            existing_lock = lock_tree.get_overlap(interval);
        }
        lock_tree.emplace(interval, LOCKER_HPP_::LockInfo{ LOCKER_HPP_::LockType::EXCLUSIVE, 1});
        return exclusive_lock(*this, interval);
    }

    exclusive_lock::exclusive_lock(exclusive_lock&& other) noexcept {
        l_ptr = other.l_ptr;
        interval = other.interval;
        other.l_ptr = nullptr;
    }

    exclusive_lock::~exclusive_lock() {

        unlock();
    }

    void locker::remove_from_tree(LOCKER_HPP_::keytype interval) {
        std::lock_guard<std::mutex> lock(mtx);
        auto lock_node = lock_tree.find(interval);
        //std::cout << lock_node->value.lockCount << std::endl;
        
        if (lock_node->value.lockCount == 1) {
            lock_tree.erase(interval);
            return;
        }

        lock_node->value.lockCount--;

    }

    locker::~locker() {
        //cv.wait(lock);
        //cv.notify_all();
        std::unique_lock<std::mutex> lock(mtx);
        while (true) {
            auto node = lock_tree.find_min();
            if (node != lock_tree.end()) {
                cv.wait(lock);
            }
            else {
                break;
            }
        }
    }

/*
    Every lock is associated with an interval `[b,e)`.
    Intervals `[b1,e1)` and `[b2,e2)` overlap if `b1<e2 && b2<e1`.
    Two locks with overlapping intervals may simultaneously exist only if both are shared.

    `lock_...` and `upgrade` function will not return until all conflicting locks are unlocked by their holders.
    `~locker` destructor will not return until all locks are unlocked by their holders.

    There are no timeouts and no deadlock prevention/detection.
    (Deadlock detection is impossible because there are no defined owners (transactions/threads/...) of the locks.)

    `locker` object must be thread-safe (`lock_...` functions may be called simultaneously from many threads).
    Each `..._lock` object will be used only by one thread; therefore the object alone does not have to be thread-safe.
*/

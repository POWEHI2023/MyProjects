#ifndef LOCKER_
#define LOCKER_

#include <shared_mutex>
#include <mutex>

/**
 * 使用这些模版类的要求是必须包含mtx成员，作为mutex的实例
 * 读写锁需要shared_mutex实例
 * 互斥锁需要mutex的实例
 */ 

// 读写锁的读锁
template <typename T>
class SRLocker {
public:
          SRLocker(const T* _tar): tar(_tar) {
                    // while (tar->mtx.try_lock_shared()) { }
                    tar->mtx.lock_shared();
          }
          ~SRLocker() {
                    tar->mtx.unlock_shared();
          }
private:
          const T* const tar;
};

// 读写锁的写锁
template <typename T>
class SWLocker {
public:
          SWLocker(const T* _tar): tar(_tar) {
                    // while (tar->mtx.try_lock()) { }
                    tar->mtx.lock();
          }
          ~SWLocker() {
                    tar->mtx.unlock();
          }
private:
          const T* const tar;
};

// 互斥锁
template <typename T>
class Locker {
public:
          Locker(const T* _tar): tar(_tar) {
                    tar->mtx.lock();
          }
          ~Locker() {
                    tar->mtx.unlock();
          }
private:
          const T* const tar;
};

#endif
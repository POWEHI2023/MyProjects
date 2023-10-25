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
          SRLocker(const T* tar);
          ~SRLocker();
};

// 读写锁的写锁
template <typename T>
class SWLocker {
public:
          SWLocker(const T* tar);
          ~SWLocker();
};

// 互斥锁
template <typename T>
class Locker {
public:
          Locker(const T* tar);
          ~Locker();
};
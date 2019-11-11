#ifndef _LOCK_H_
#define _LOCK_H_

#include <atomic>
#include <mutex>
#include <unistd.h>

// 自旋锁
class SpinLock{
private:
	std::atomic_flag flag_;

public:
	// flag_置为‘0’
	SpinLock() {	flag_.clear(); }
	void lock() { while (flag_.test_and_set()) sleep(0); }
	void unlock() { flag_.clear(); }
};

// 互斥锁（直接用库文件的，没什么好说的）
class MutexLock {
private:
	std::mutex mutex_;

public:
	MutexLock() {};
	~MutexLock() {};
	void lock() {mutex_.lock();}
	void unlock() {mutex_.unlock();}
};

#endif // ! _LOCK_H_

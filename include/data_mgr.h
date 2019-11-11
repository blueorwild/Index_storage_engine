#ifndef _DATA_MGR_H_
#define _DATA_MGR_H_

#include <unistd.h>
#include <stdint.h>
#include <atomic>
#include <sys/stat.h>

#include "../include/lock.h"
#include "../include/code.h"

// 单例模式，因为写是追加写，所以同一时刻仅有一个文件打开
class DataMgr {
private:
    const int max_file_size_ = 8000000;  // 单个文件大小阈值
	const char  *dir_ = "../data/";
	std::atomic_int32_t file_no_;      // 正在写的文件号(可代表名字)
	FILE* file_ptr_;      // 正在写的文件指针
	int cur_file_size_;  // 正在写的文件大小

	DataMgr();
	static DataMgr *p_;

	// 简单的读写锁
	MutexLock lock_write_; 
	MutexLock lock_reader_count_;
	int reader_count_;
	void ReadLock(){
		lock_reader_count_.lock();
        if(1 == ++reader_count_) lock_write_.lock();
        lock_reader_count_.unlock();
	}
	void ReadUnlock(){
		lock_reader_count_.lock();
		if(0 == --reader_count_) lock_write_.unlock();
		lock_reader_count_.unlock();
	}

	// 遍历某个文件获取结果
	void ScanFile(FILE* file_ptr, int column_num, int64_t min, 
    	int64_t max, int64_t (*result) [100], int &row_num);

public:
	static DataMgr *Initial() { return p_; }
	// 往文件追加一条记录
	void Add(char* data);

	// 遍历文件查询一条记录
	void Query(int file_no, int column_num, int64_t min,
		int64_t max,int64_t (*result) [100], int &row_num);

	// 获取当前正在写的文件号
	uint32_t GetFileNo(){
		return file_no_.load(std::memory_order_release);
	}
};

#endif // ! _DATA_MGR_H_

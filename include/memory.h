#ifndef _MEMORY_H_
#define _MEMORY_H_
#include <vector>
#include <stdint.h>
class Memory {
private:
    const int block_size_ = 4096;       // 4KB,一个比较合适的内存块大小
	std::vector<char*> blocks_;     // 存放若干指针，每个指针指向一块内存
	char* unuse_ptr_;                // 指向当前内存块未使用空间的首址
	uint32_t unuse_size_;              // 当前内存块未使用空间的大小

public:
	Memory();
	~Memory();

	// 返回一个指向size大小的内存块地址首址的指针
	char* Allocate(uint32_t size);
};

#endif // !_MEMORY_H_
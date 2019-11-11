// 为了整齐，跳表头节点data也是16字节
#ifndef _SKIPLIST_H_
#define _SKIPLIST_H_

#include <memory.h>
#include <string.h>

#include "random.h"
#include "memory.h"
#include "code.h"

class SkipList {
private: 
	const int MAX_LEVEL = 6;   // 跳表限制最大高度（0~5）
	int max_level_;                     // 当前跳表实际高度
    Memory* memory_;          // 统一的内存分配
	Random* random_;          // 随机数生成

	// 跳表节点结构体
	struct Node {         
	public:   
		char* data;     // 节点的值(所有内容拼接成一个字符串)
		Node* Next(int level){
			return (Node*) (next_[level]);
		}
		void SetNext(int level, Node* node){
			next_[level] = node;
		}
	private:
		void* next_[1]; 
	};
	Node* head_;              // 跳表头节点

	// 创建一个值为data，层数为level的新节点
	Node* NewNode(int level, char* data);
    // 生成特殊的随机层数
    int RandomLevel(int maxLevel);

public:
	SkipList();
	~SkipList();

	char* Allocate(uint32_t size){return memory_->Allocate(size);}
	int GetLevel(){return max_level_;}
	// 往SkipList中插入一条数据
	void Add(char* data);

	// 生成索引到缓冲,上层索引格式为 数值(8byte)+下层偏移(4byte)
	// 底层索引格式为 数值+位置,即node 的 data
	// 返回值是已用缓冲大小
	uint32_t GenerateIndex(char* buf, int buf_size, int level, int &position);
};

#endif // !_SKIPLIST_H_

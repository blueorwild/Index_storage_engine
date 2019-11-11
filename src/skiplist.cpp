#include "../include/skiplist.h"

SkipList::SkipList(){
		max_level_ = 0;    // 初始化默认实际最大高度为1层，即下标为0
		memory_ = new Memory;
		random_ = new Random(16807);  // 7^5
		char* buf = memory_->Allocate(16);
		IntToStr(-1, buf);  // 表示跳表还未完成使命
		head_ = NewNode(MAX_LEVEL - 1, buf);	
}

SkipList::~SkipList(){
    delete random_;
    delete memory_;
}

SkipList::Node* SkipList::NewNode(int level, char* data){
	//根据当前节点的层高来分配不同大小的内存
	char* mem = memory_->Allocate(sizeof(Node) + level * sizeof(void*));
	Node* new_node = new (mem) Node;   // placement new
	
	new_node->data = data;
	for (int i = 0; i <= level; i++) new_node->SetNext(i, NULL);
	
	return new_node;
}

int SkipList::RandomLevel(int maxLevel){
    // 考虑16叉树，即高层节点的数目是低层的1/16
    int height = 0;
    // 即层数有1/16的几率+1
    while ((random_->Rand() & 15) == 0 && height < maxLevel) 
        ++height;
    return height;
}

void SkipList::Add(char* data){
	Node* p_node = head_;      // 从头开始找位置
	Node* next = NULL;
	Node* prev_nodes[MAX_LEVEL];     // 存放需要更新的前置节点
	for(int i = 0; i < MAX_LEVEL; ++i) prev_nodes[i] = head_;
	// 统计需要更新的前置节点
	for (int i = max_level_; i >= 0; --i) {
		next = p_node->Next(i);
		while (next && StrToInt64(next->data) < StrToInt64(data) ) {
			p_node = next;
			next = p_node->Next(i);
		}
        prev_nodes[i] = p_node;
	}

	// 生成新节点并可能更新高度
	int level = RandomLevel(MAX_LEVEL);	
	Node* new_node = NewNode(level, data);
	if (level > max_level_) max_level_ = level;

	// 写入
	for (int i = 0; i <= level; ++i) { 
		new_node->SetNext(i, prev_nodes[i]->Next(i));
		prev_nodes[i]->SetNext(i, new_node);
	}
}

uint32_t SkipList::GenerateIndex(char* buf, int buf_size, int level, int &position){
	Node* p_node = head_, *p_low_node = head_;
	for(int i = 0; i < position; ++i) p_node = p_node->Next(level);
	int size = 0, offset = 0, down_level = level - 1 ;
	char offset_str[4] = {0};  // 存放上层对应的下层offset(+1)

	if(level >= 1){  // 上层索引
		while(p_node != NULL && size < buf_size){
			while(p_low_node != p_node) {
				p_low_node = p_low_node->Next(down_level);
				++offset;
			}
			memcpy(buf + size, p_node->data, 8);
			IntToStr(offset, offset_str);
			memcpy(buf + size + 8, offset_str, 4);
			size+=12;
			p_node = p_node->Next(level);
			p_low_node = head_;
			++position;
		}
	}
	else{   // 底层索引
		while(p_node != NULL && size < buf_size){
			memcpy(buf + size, p_node->data, 16);
			size+=16;
			p_node = p_node->Next(0);
			++position;
		}
	}
	return size;
}


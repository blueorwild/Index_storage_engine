// index有两种 与跳表层数有关
// 底层格式为 数值(8byte)+位置(8byte,文件号+偏移)
// 上层格式为 数值(8byte)+下层偏移(4byte)
// 其中顶层文件前4字节存放层数(从0计算)，接着4字节存放索引对应的最新文件号
#ifndef _INDEX_MGR_H_
#define _INDEX_MGR_H_

#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdint.h>

#include "code.h"
#include "skiplist.h"
#include "data_mgr.h"

class IndexMgr {
private:
	const  char* dir_ = "../index/"; 
	const  char* data_dir_ = "../data/"; 

	// 扫描数据文件内容，为某列建立跳表索引结构
	void ScanDataFile(FILE* file_ptr, int file_no, 
		int column_num, SkipList* skiplist);
	// 把内存中的跳表索引写到磁盘文件
	void WriteIndexFile(int max_file_no, int column_num, SkipList* skiplist);
	
	// 扫描某个dir(每列一个dir)下的上层索引文件获取底层索引
	// 返回值即目标在底层索引文件的偏移
	int ScanIndexFile(char* dir, int level, int step, int64_t threshold);
	// 根据数据文件号和对应的偏移获取结果
	void ReadByIndex(int64_t result[100], int file_no, int offset);

public:
	IndexMgr(){
		if(0 != access(dir_, 0)) mkdir(dir_, 0777);
	};
	~IndexMgr(){};

	// 判断某列上的索引是否已经建立完成
	// 若是，返回索引对应的最新数据文件号，并传出索引层数
	int IndexIsExist(int column_num, int &level);

	// 建立某列的索引
	void NewIndex(int column_num);
	
	// 利用索引进行查询
	void Query(int level, int column_num, int64_t min, 
		int64_t max, int64_t (*result) [100], int &row_num);	
};

#endif // ! _INDEX_MGR_H_

#ifndef _DB_H_
#define _DB_H_

#include <stdint.h>
#include "code.h"
#include "data_mgr.h"
#include "index_mgr.h"

// 新增一条记录
extern void Set(int64_t *data);
// 建立索引
extern void SetIndex(int column_num);
// 范围查询
extern void Get(int column_num, int64_t min, int64_t max,
    int64_t (*result) [100], int &row_num);


#endif // !_DB_H_
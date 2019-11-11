#include "../include/db.h"

 DataMgr*  dm_ = DataMgr::Initial();
 IndexMgr* im_ = new IndexMgr();

void Set(int64_t* data){
    char buf [800] = {0};
    for(int i = 0; i < 100; ++i)
        Int64ToStr(data[i], buf +(i<<3));
    dm_->Add(buf);
}

extern void SetIndex(int column_num){
    im_->NewIndex(column_num);
}

void Get(int column_num, int64_t min, int64_t max,
    int64_t (*result) [100], int &row_num){
    row_num = 0;
    int level = 0, file_no = 0;
    if((file_no = im_->IndexIsExist(column_num, level)) > 0){
        im_->Query(level, column_num, min, max, result, row_num);
        dm_->Query(file_no, column_num, min, max, result, row_num);
    }
    else dm_->Query(0, column_num, min, max, result, row_num);
}
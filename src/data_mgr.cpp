#include "../include/data_mgr.h"

DataMgr* DataMgr::p_ = new DataMgr();

DataMgr::DataMgr(){
    reader_count_ = 0;
    cur_file_size_ = 0;
    // 扫描数据文件夹，获取最新文件名
    char file_path[256] = {0};
    int file_no = 1;
   sprintf(file_path, "%s%d", dir_, file_no);
    while(0 == access(file_path, 0)) 
        sprintf(file_path, "%s%d", dir_, ++file_no);
    sprintf(file_path, "%s%d", dir_, --file_no);
    file_ptr_= fopen(file_path, "ab+");
    file_no_.store(file_no, std::memory_order_relaxed);
}

void DataMgr::ScanFile(FILE* file_ptr, int column_num, int64_t min, 
    int64_t max, int64_t (*result) [100], int &row_num){
    char buf[80000] = {0};
    int subs = 0;
    size_t size = fread(buf, 1, 80000, file_ptr);
    while(size > 0){
        for(int i = column_num * 8; i < size; i+=800){
            int64_t tmp = StrToInt64(buf+i);
            if(tmp >= min && tmp <= max){
                if(row_num < 10)
                    for(int j = i - column_num * 8, subs = 0; subs < 100; j+=8)
                        result[row_num][subs++] = StrToInt64(buf + j);
                ++row_num;
            }
        }
        size = fread(buf, 1, 80000, file_ptr);
    }
}

void DataMgr::Add(char* data){
    lock_write_.lock();
    // 判断文件大小是否超过阈值
    char file_path[256] = {0};
    if(max_file_size_ <= cur_file_size_){
        cur_file_size_ = 0;
        fclose(file_ptr_);
        ++file_no_;   // 原子操作
        sprintf(file_path, "%s%d", dir_, file_no_.load(std::memory_order_release));
        file_ptr_ = fopen(file_path, "ab+");
    }
    fwrite(data, 800, 1, file_ptr_);
    cur_file_size_ += 800;
    lock_write_.unlock();
}

void DataMgr::Query(int file_no, int column_num, int64_t min, 
    int64_t max,int64_t (*result) [100], int &row_num){
    char file_path[256] = {0};
    FILE *file_ptr = NULL;
    
    // 从file_no开始遍历
    sprintf(file_path, "%s%d", dir_, file_no);
    while(0 == access(file_path, 0)) {
        bool is_concurrency = (file_no == file_no_.load(std::memory_order_release));
        // 若读写并发加读锁
        if(is_concurrency) ReadLock();
        file_ptr = fopen(file_path, "rb");
        ScanFile(file_ptr, column_num,  min, max, result, row_num);
        fclose(file_ptr);
        if(is_concurrency) ReadUnlock();
        
        if(10 == row_num) break;
        sprintf(file_path, "%s%d", dir_, ++file_no);
    }
}

#include "../include/index_mgr.h"

void IndexMgr::ScanDataFile(FILE* file_ptr, int file_no, int column_num, SkipList* skiplist){
    // 先大块读入内存再逐行扫描
    char buf[80000] = {0};
    char* one_record = NULL;
    int times = 0;
    char location[8] = {0};
    uint32_t size = fread(buf, 1, 80000, file_ptr);
    while(size > 0){   
        for(int i = column_num * 8; i < size; i+=800){
            one_record = skiplist->Allocate(16);
            memcpy(one_record, buf + i, 8);
            IntToStr(file_no,  location);
            IntToStr(i - column_num * 8 + times*80000, location + 4);
            memcpy(one_record + 8, location, 8);
            skiplist->Add(one_record);
        }
        size = fread(buf, 1, 80000, file_ptr);
        ++times;
    }
}
void IndexMgr::WriteIndexFile(int max_file_no, int column_num, SkipList* skiplist){
    // 创建此索引的文件夹（若不存在）
    char path[256] = {0};
    sprintf(path, "%s%d", dir_, column_num);
    if(0 != access(path, 0)) mkdir(path, 0777);

    // 为跳表的每一层创建一个文件（若存在就覆盖
    char buf[72000] = {0};   // 批量写索引到文件的缓冲
    FILE* file_ptr = NULL;
    int level = skiplist->GetLevel();
    for(int i = level, j = 0; i >= 0; ++j, --i ){
        sprintf(path, "%s%d/%d", dir_, column_num, j);
        file_ptr = fopen(path, "wb");  // 只写模式打开，若存在清空
        int position = 0, size = 0;
        size = skiplist->GenerateIndex(buf, 72000, i, position);
        while(size  > 0){
            fwrite(buf, size, 1, file_ptr);
            size = skiplist->GenerateIndex(buf, 72000, i, position);
        }
        fclose(file_ptr);
    }
    // 索引建立完成后，在顶层文件开头写入标记(level和file_no)，表示可以使用
    sprintf(path, "%s%d/%d", dir_, column_num, 0);
    file_ptr = fopen(path, "rb+");
    char flag[8] = {0};
    IntToStr(level, flag);
    IntToStr(max_file_no, flag+4);
    fwrite(flag, 8, 1, file_ptr);
    fclose(file_ptr);
}

int IndexMgr::ScanIndexFile(char* dir, int level, int step, int64_t threshold){
    char buf[72000] = {0};
    FILE* file_ptr = NULL;
    char file_path[256] = {0};

    // 逐层扫描找到索引
    int offset = 1;  // 跳过头节点
    for(int i = 0; i < level; ++i){
        // 读一块到缓冲
        sprintf(file_path, "%s%d", dir, i);
        file_ptr = fopen(file_path, "rb");
        fseek(file_ptr, offset*step, SEEK_SET);
        uint32_t size = fread(buf, 1, 72000, file_ptr);
        // 遍历缓冲
        while(size > 0){
            for(int j = 0; j < size; j+= step){
                if(StrToInt64(buf + j) >= threshold) {
                    if(1 == offset) offset  = 1;   // 每次都跳过头节点
                    else offset = StrToInt(buf + j - step + 8);
                    goto next_level;
                }
            }
            size = fread(buf, 1, 72000, file_ptr);
        }
        // 到这说明当前层索引中全是比threshold小的
        fseek(file_ptr, step, SEEK_END);
        fread(buf, step, 1, file_ptr);
        offset = StrToInt(buf + 8);
        next_level:
        fclose(file_ptr);	
    }
    return offset;
}
void IndexMgr::ReadByIndex(int64_t result[100], int file_no, int offset){
    char file_path[256] = {0};
    char buf[800] = {0};
    sprintf(file_path, "%s%d", data_dir_, file_no);
    FILE* file_ptr = fopen(file_path, "rb");
    fseek(file_ptr, offset, SEEK_SET);
    fread(buf, 800, 1, file_ptr);
    fclose(file_ptr);
    for(int i = 0; i < 100 ; ++i) result[i] = StrToInt64(buf + (i<<3));
}

int IndexMgr::IndexIsExist(int column_num, int &level){
    char path[256] = {0};
    sprintf(path, "%s%d", dir_, column_num);
    if(0 != access(path, 0)) return -1;  // 索引目录不存在

    sprintf(path, "%s%d/%d", dir_, column_num, 0);
    if(0 != access(path, 0)) return -1;  // 索引文件不存在
    
    FILE* file_ptr = fopen(path, "r+");
    char location[8] = {0};
    int size = fread(location, 1, 8, file_ptr);
    fclose(file_ptr);
    if(0 == size || (level = StrToInt(location))  == -1)   return -1;// 索引文件未完成
    return StrToInt(location+4);
}

void IndexMgr::NewIndex(int column_num){
    // 标记当前正在写的文件，索引建立到该文件之前
    DataMgr* dm = DataMgr::Initial();
    int  max_file_no = dm->GetFileNo();
    if(0 == max_file_no) return;

    char file_path[256] = {0};
    FILE* file_ptr = NULL;
    SkipList* skiplist = new SkipList();
    int file_no = 0;
    // 读取某列数据文件到内存跳表
    while(file_no != max_file_no){
        sprintf(file_path, "%s%d", data_dir_, file_no);
        file_ptr = fopen(file_path, "rb");
        ScanDataFile(file_ptr, file_no, column_num, skiplist);
        fclose(file_ptr);
        ++file_no;
    }
    // 把内存的索引写到文件持久化
    WriteIndexFile(max_file_no, column_num, skiplist);
    delete skiplist;
};

void IndexMgr::Query(int level, int column_num, int64_t min, 
    int64_t max, int64_t (*result) [100], int &row_num){
    char file_path[256] = {0};
    FILE *file_ptr = NULL;
    char buf[72000] = {0};
    
    // 逐层扫描找到底层索引
    sprintf(file_path, "%s%d/", dir_, column_num);
    int offset = ScanIndexFile(file_path, level, 12, min);

    // 最底层单独考虑
    sprintf(file_path, "%s%d/%d", dir_, column_num, level);
    file_ptr = fopen(file_path, "rb");
    fseek(file_ptr, offset*16, SEEK_SET);
    uint32_t size = fread(buf, 1, 72000, file_ptr);
    while(size > 0){
        for(int i = 0; i < size; i+=16){ 
            int64_t num = StrToInt64(buf + i);
            if(num >= min)
                if(num <= max){  // 找到最终索引，读取之
                    if(row_num < 10)
                        ReadByIndex(result[row_num], StrToInt(buf+i+8), StrToInt(buf+i+12));
                    ++row_num;
                }
                else goto last; // 已经超过范围
        }
        size = fread(buf, 1, 72000, file_ptr);
    }
    last:
    fclose(file_ptr);
}

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <thread>
#include <time.h>

#include "../include/db.h"
#include "../include/code.h"
using namespace std;
void test1(){
    int64_t data[100] = {0};
    for(int times = 1; times<=500000; ++times){
        for(int i = 0;i<100;i++) data[i] = i*times;
        Set(data);
    }
}
void test2(){
    int64_t data[100] = {0};
    for(int times = 1; times<=500000; ++times){
        for(int i = 0;i<100;i++) data[i] = -i*times;
        Set(data);
    }
}
void test3(){
    int64_t result[10][100];
    int row_num = 0, column = 1;
    int64_t min = -500, max = 666;
    Get(column, min, max, result, row_num);
    cout << "row_num: " << row_num << endl;
    for(int i = 0; i < row_num && i < 10; ++i){
        for(int j = 0; j<100; ++j) cout << result[i][j]<<' ';
        cout << endl;
    }
    cout << endl;
}
void test4(){
    SetIndex(1);
}
int main(){
    time_t  start, end;
    start = clock();
    //thread t1(test1);
    //thread t2(test2);
    //t1.join();
    //t2.join();
    test3();
    //test4();
    end = clock();
    double time = (double)(end - start) / CLOCKS_PER_SEC;
    cout << "use time:" << time << "s" << endl;
    return 0;
}
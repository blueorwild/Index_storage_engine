#ifndef _RANDOM_H_
#define _RANDOM_H_
#include <stdint.h>

class Random {
private:
	uint32_t seed_;

public:
	Random(uint32_t s) :seed_(s) {};

	uint32_t Rand() {
		// 经前人测试这一对数据效果较好
		static const uint32_t M = 2147483647L;   // 2^31-1
		static const uint64_t A = 16807;  // 7^5
		uint64_t tmp = seed_ * A;
		seed_ = (uint32_t)((tmp >> 31) + (tmp & M));  
		// 上行代码实际意义是 seed_ = tmp % M,可证明相等,如此可提升效率													   
		// 只是 (tmp & M)应该是(tmp & M) % M, 不应该有可能等于M,所以有下面的操作
		if (seed_ > M) {
			seed_ -= M;
		}
		return seed_;
	}
};

#endif // !_MY_DB_RANDOM_H_
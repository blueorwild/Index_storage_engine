#include "../include/memory.h"

Memory::Memory(){
	    unuse_ptr_ = NULL;
	    unuse_size_ = 0;
}

Memory::~Memory(){
        uint32_t size = blocks_.size();
        for (uint32_t i = 0; i < size; i++) 
		    delete[] blocks_[i];
}

char* Memory::Allocate(uint32_t size){
    char* result;
    if (size <= unuse_size_) {
        result = unuse_ptr_;
        unuse_ptr_ += size;
        unuse_size_ -= size;
    }
    else {
        result = new char[block_size_];
        blocks_.push_back(result);
        unuse_ptr_ = result + size;
        unuse_size_ = block_size_ - size;
    }
    return result;
}
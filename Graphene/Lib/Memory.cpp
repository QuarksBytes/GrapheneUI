#ifndef __UI_LIB_MEMORYOPERATIONS__
#define __UI_LIB_MEMORYOPERATIONS__

#include<stdint.h>
#include<cstring>

namespace lib{

  namespace memory{

    void fill(void* dest,uint8_t c,size_t size){
      if(dest){
        memset(dest,c,size);
      }
    }

    void zero(void* dest,uint64_t srcSize){
      fill(dest,0,srcSize);
    }

    void copy(void*  dest,const void*  src,uint64_t srcSize){
      if(dest && src){
        memcpy(dest,src,srcSize);
      }
    }

    // length=length of memory to shift
    // total=total bytes to shift
    void shiftRight(void*  mem,const uint64_t length,const uint64_t total){
      if(mem){
        memmove(((uint8_t*)mem)+total,mem,length);
      }
    }

  // length=length of memory to shift
  // total=total bytes to shift
    void shiftLeft(void*  mem,uint64_t length,uint64_t total){
      if(mem){
        memmove(((uint8_t*)mem)-total,mem,length);
      }
    }


  }

  uint64_t getAppropriateResizeLength(uint64_t len){
    if(len>0x100000)
      len+=0x200000;
    else if(len<0x100)
      len+=0x40;
    else
      len+=0x400;
    return len;
  }


}

#endif

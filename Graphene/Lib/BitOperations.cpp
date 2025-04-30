#ifndef __UI_LIB_BITOPERATIONS__
#define __UI_LIB_BITOPERATIONS__

#include<string>
#include<stdint.h>

namespace lib{

  namespace bits{
    constexpr unsigned char bits_numberToBits[256]={
      0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
    };

    void stringToBits(const std::string& str,long unsigned int& bits1,unsigned int& bits2){

      uint64_t length=str.length();
      const char* text=str.c_str();
      long unsigned int bits_1=0;
      unsigned int bits_2=0;

      char chr;
      while(length--){
        chr=*(text++);
        if(chr>0x1f && chr<0x7f){
          if(chr<0x60){
            bits_1|=(1<<(chr-0x20));
          }else
            bits_2|=(1<<(chr-0x60));
        }
      }

      bits1=bits_1;
      bits2=bits_2;
    }

    void stringToBits(char* text,uint64_t length,uint64_t& bits1,uint32_t& bits2){
      if(!text)
        return;

      uint64_t bits_1=0;
      uint32_t bits_2=0;

      char chr;
      while(length--){
        chr=*(text++);
        if(chr>0x1f && chr<0x7f){
          if(chr<0x60){
            bits_1|=(((uint64_t)1)<<(chr-0x20));
          }else
            bits_2|=(((uint64_t)1)<<(chr-0x60));
        }
      }

      bits1=bits_1;
      bits2=bits_2;
    }

  }
}
#endif
/*
#include<stdio.h>
int main(){
  uint64_t bits_1=0;
  uint32_t bits_2=0;
  lib::bits::stringToBits("java",bits_1,bits_2);
  printf("Bits 1 0x%x Bits 2 0x%x\n",bits_1,bits_2);
}*/

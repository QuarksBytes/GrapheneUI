#ifndef __UI_LIB_VECTOR__
#define __UI_LIB_VECTOR__

#include<stdlib.h>

#include"Lib/Memory.cpp"

namespace lib{

  template<typename T>
  class vector{
  private:
    T *pointer=NULL;

    unsigned int length=0;
    unsigned int capacity=0;

    unsigned int getAppropriateResizeLength(unsigned int len){
      if(len>0x100000)
        len+=0x200000;
      else if(len<0x100)
        len+=0x40;
      else
        len+=0x4000;
      return len;
    }

    int resizeMem(unsigned int nlength){
      nlength=getAppropriateResizeLength(nlength);
      void* ptr=realloc(pointer,sizeof(T)*nlength);
      if(!ptr)
        return 0;
      pointer=(T*)ptr;
      capacity=nlength;
      if(length>capacity){
        length=capacity;
      }
      return 1;
    }

  public:

    vector(){
    }

    vector(unsigned int nlength){
      resizeMem(nlength);
    }

    ~vector(){
      if(pointer)
        free(pointer);

      pointer=NULL;
      length=capacity=0;
    }

    bool reserve(unsigned int len){ // for increasize capacity
      if(len<=capacity)
        return true;

      return resizeMem(len);
    }

    void resize(unsigned int nlength){ // for increasing length
      if(nlength>capacity){
        resizeMem(nlength);
      }

      length=nlength;
    }

    void reset(){
      length=0;
    }

    void zero(unsigned int index,unsigned int nlength){
      if(index>length)
        return;

      unsigned int t32_1=index+nlength;

      if(t32_1>capacity){
        if(!resizeMem(t32_1))
          return ;
      }

      lib::memory::zero(pointer+index,nlength*sizeof(T));

      if(t32_1>length){
        length=t32_1;
      }

    }

    bool add(const T& val){ // push back implementation
      if(length>=capacity){
        if(!resizeMem(length))
          return false;
      }
      pointer[length++]=val;

      return true;
    }

    bool add(unsigned int index,T& val){
      if(index>length)
        index=length;

      if(length>=capacity){
        if(!resizeMem(length))
          return false;
      }

      memory::shiftRight(pointer+index,((length++)-index)*sizeof(T),sizeof(T));
      pointer[index]=val;
    }

    bool remove(unsigned int index){
      if(index>length)
        return false;

      memory::shiftLeft(pointer+index+1,((--length)-index)*sizeof(T),sizeof(T));
      return true;
    }

    bool push_back(const T& val){
      return add(val);
    }

    bool pop(const T& val){
      if(length){
        val=pointer[length-1];
        --length;
        return true;
      }
      return false;
    }

    // nlength= total length of mem to shift
    // toShift= total bytes to shift
    void shiftLeft(unsigned int index,unsigned int nlength,unsigned int toShift){
      if(index>length || toShift>index)
        return;

      unsigned int t32_1=length-index;
      if(nlength>t32_1)
        nlength=t32_1;

      memory::shiftLeft(pointer+index,nlength*sizeof(T),toShift*sizeof(T));

    }

    void shiftRight(unsigned int index,unsigned int nlength,unsigned int toShift){
      if(index>length)
        return;
      unsigned int t32_1=length-index;

      if(nlength>=t32_1)
        return;

      t32_1-=nlength;

      if(toShift>t32_1)
        toShift=t32_1;

      memory::shiftRight(pointer+index,nlength*sizeof(T),toShift*sizeof(T));

    }


    unsigned int start() const {
      return 0;
    }

    unsigned int begin() const {
      return 0;
    }

    unsigned int end() const {
      return length;
    }

    unsigned int size() const {
      return length;
    }

    T* getPointer() const {
      return pointer;
    }


    T& operator[](unsigned int index) const {
      return pointer[index];
    }

    T* operator+(unsigned int index) const {
      return pointer+index;
    }

    void operator<<=(unsigned int nlength){ // shift every thing left
      shiftLeft(nlength,length-nlength,nlength);
    }

    void operator>>=(unsigned int nlength){ // shift every thing right
      shiftRight(0,length-nlength,nlength);
    }

    void operator+=(const T& val){ // push back value
      add(val);
    }

    void operator-=(unsigned int index){ // remove value
      remove(index);
    }



  };

}

#endif

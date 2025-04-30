#ifndef __UI_LIB_UNDO_REDO__
#define __UI_LIB_UNDO_REDO__

#include<string>

#include"Lib/Vector.cpp"

#define UNDOREDO_OPERATIONS_LIMIT 100000

#define UNDOREDO_MODE_INSERTION 0
#define UNDOREDO_MODE_DELETION 1

#define UNDOREDO_MODE_SHIFT 4
#define UNDOREDO_MODE_SHIFT_MASK 0xf0

#define UNDOREDO_TEMP_BUFFER_LENGTH 0x8
struct ____UndoRedo__Internals__Container__{
  union{
    struct{
      uint32_t index;
      uint32_t length;
    }buffer;
    uint32_t length;
    char array[UNDOREDO_TEMP_BUFFER_LENGTH]; // since 8 =sizeof(uint64_t)
  };
  uint32_t textIndex;
  uint32_t mode;
};

#define UndoRedo_InsertText(name) void name(void* ptr,uint64_t index,const char* text,uint32_t length)
#define UndoRedo_DeleteText(name) void name(void* ptr,const char* saveBuffer,const char* text,uint32_t length)


struct UndoRedo{

  void (*insertText)(void* ,const char* ,uint32_t);

  std::string string;
  lib::vector<____UndoRedo__Internals__Container__> list;

  ____UndoRedo__Internals__Container__ temporary={};

  uint32_t currentIndex=0;

  void pushTempDeleteBuf(const char* buf,uint64_t index,uint32_t length){
    if(length){

      ____UndoRedo__Internals__Container__ container;

      container.textIndex=index;
      container.mode=UNDOREDO_MODE_DELETION;

      if(length<=UNDOREDO_TEMP_BUFFER_LENGTH){
        for(int i=0;i<length;++i){
          container.array[i]=buf[i];
        }
        container.mode|=(length<<UNDOREDO_MODE_SHIFT);
      }else{
        container.buffer.index=string.length();
        container.buffer.length=length;

        string.append(buf,length);
      }
      list.append(container);
      ++currentIndex;
    }
  }

  void pushTempDeleteBuf(){
    if(temporary.mode>>UNDOREDO_MODE_SHIFT ){
      temporary.mode|=UNDOREDO_MODE_DELETION;
      list.append(temporary);
      ++currentIndex;
    }
    temporary=(____UndoRedo__Internals__Container__){0};

  }

  void pushTempInsertBuf(){
    if(temporary.length){
      temporary.mode=UNDOREDO_MODE_INSERTION;
      list.append(temporary);
      ++currentIndex;
    }
    temporary=(____UndoRedo__Internals__Container__){0};

  }

  void insertText(uint64_t cursor,uint32_t length){
    if(temporary.mode==UNDOREDO_MODE_DELETION){
      pushTempDeleteBuf();

      temporary.textIndex=cursor;
      temporary.length=length;
      if(length>1){
        pushTempInsertBuf();
      }
      return;
    }


    if(length>1){
      pushTempInsertBuf();
      temporary.textIndex=cursor;
      temporary.length=length;
      pushTempInsertBuf();
    }else{
      if(temporary.length<UNDOREDO_TEMP_BUFFER_LENGTH){
        if(cursor==temporary.textIndex+temporary.length){
          ++temporary.length;
        }else{
          pushTempInsertBuf();
          temporary.textIndex=cursor;
          temporary.length=length;
        }

      }else{
        pushTempInsertBuf();
        temporary.index=cursor;
        temporary.length=length;
      }
    }

  }

  void copyToBuf(char* dest ,const char* ptr,uint32_t length){ // can't copy more than sizeof(uint64_t)
    for(int i=0;i<length;++i){
      dest[i]=ptr[i];
    }
  }

  void deleteText(const char* ptr,uint64_t cursor,uint32_t length){ // ptr= text+cursor
    if(temporary.mode==UNDOREDO_MODE_INSERTION){
      pushTempInsertBuf();

      if(length>UNDOREDO_TEMP_BUFFER_LENGTH){
        pushTempDeleteBuf(ptr,cursor,length);
        temporary=(____UndoRedo__Internals__Container__){0};
      }else{
        copyToBuf(temporary.array, ptr,length);
        temporary.textIndex=cursor;
        temporary.mode=UNDOREDO_MODE_DELETION|(length<<UNDOREDO_MODE_SHIFT);
      }

      return ;
    }

    uint32_t t32_1=temporary.mode>>UNDOREDO_MODE_SHIFT;
    if(length>1){
      if(temporary.mode>>UNDOREDO_MODE_SHIFT){
        pushTempDeleteBuf();
      }
      pushTempDeleteBuf(ptr,cursor,length);
    }else{
      if(t32_1<UNDOREDO_TEMP_BUFFER_LENGTH){
        if(cursor==temporary.textIndex){ // deleted text
          copyToBuf(temporary.array+t32_1,ptr,length);
          temporary.mode=((t32_1+length)<<UNDOREDO_MODE_SHIFT)^UNDOREDO_MODE_DELETION;
        }else{
          pushTempDeleteBuf();
          copyToBuf(temporary.array, ptr,length);
          temporary.textIndex=cursor;
          temporary.mode=UNDOREDO_MODE_DELETION|(length<<UNDOREDO_MODE_SHIFT);
        }

      }else{
        pushTempDeleteBuf();
        copyToBuf(temporary.array, ptr,length);
        temporary.textIndex=cursor;
        temporary.mode=UNDOREDO_MODE_DELETION|(length<<UNDOREDO_MODE_SHIFT);
      }
    }

  }


  void undo(){
    if(temporary.mode){ // delete mode is set
      if(temporary.mode&UNDOREDO_MODE_SHIFT_MASK){
        // text is inside buffer
        return ;
      }
    }else{
      if(temporary.length){
        // text is in buffer
        return ;
      }
    }
      // text is not in temp buffer need to pop it off



  }


};

#undef UNDOREDO_MODE_INSERTION
#undef UNDOREDO_MODE_DELETION


#endif

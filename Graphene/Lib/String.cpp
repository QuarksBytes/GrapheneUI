#ifndef __UI_LIB_STRING__
#define __UI_LIB_STRING__

#include<cstring>
#include<cstdlib>

#include"Lib/Memory.cpp"

namespace lib{

  struct string{

    char* pointer=nullptr;

    long int length=0;

    long int capacity=0;


    ~string(){
      if(pointer){
        free(pointer);
        pointer=nullptr;
      }
    }


    static string* create(){
      return new(std::nothrow) string;
    }

    int resize(long int nlength){
      nlength=lib::getAppropriateResizeLength(nlength);
      void* ptr=realloc(pointer,sizeof(char)*(nlength+1));
      if(!ptr)
        return 0;
      pointer=(char*)ptr;
      capacity=nlength;
      if(length>capacity){
        length=capacity;
        pointer[length]='\0';
      }
      return 1;
    }

    void reset(){
      length=0;
      if(pointer)
        *pointer='\0';
      else
        capacity=0;
    }

    int setText(const std::string& text){
      uint64_t size=text.length();
      uint64_t len=length+size;
      if(++len>=capacity){
        if(!resize(len))
          return 0;
      }
      memory::copy(pointer,text.c_str(),size);
      pointer[size]=0;
      length=size;
      return 1;
    }

    bool insertChar(char chr,long unsigned int index){
      long int len=length;
      if(index>len)
        return false;
      char* add;
      if(++len>=capacity){
        if(!resize(len))
          return false;
      }
      add=pointer+index;
      memory::shiftRight((void*)add,length-index,sizeof(char));
      *add=chr;
      pointer[++length]=0;
      return true;
    }

    bool insertText(const std::string& buffer,uint64_t index){
      long int len=buffer.length();
      uint64_t temp=length+len;
      char* buf;
      if(++temp>capacity){
        if(!resize(temp))
          return false;
      }
      buf=pointer+index;
      memory::shiftRight((void*)buf,length-index,len);
      memory::copy((void*)buf,buffer.c_str(),len);
      length=--temp;
      pointer[temp]=0;
      return true;
    }



    void deleteChar(uint64_t index){
      if(index>=length)
        return ;
      char* ptr=pointer+(++index);
      memory::shiftLeft((void*)ptr,length-index,1);
      pointer[--length]='\0';
    }

    void deleteText(uint64_t index,uint64_t len){
      uint64_t temp=index+len;
      if(index+len>length)
        return ;
      char* ptr=pointer+temp;
      memory::shiftLeft(ptr,length-temp,len);
      length-=len;
      pointer[length]='\0';
    }

  //12 22 43 32 23 00 43 12  z=(x^y)  z&(z>>32) z&(z>>16) z&(z>>8)

    void replaceChar(char chr,char replacement){
      uint64_t tlength=length>>3;
      char* ptr=pointer;
      if(tlength){
        uint64_t chrtest;
        uint64_t tvar;
        uint64_t tmp;
        chrtest=((uint64_t)chr)^(((uint64_t)chr)<<8);
        chrtest=((uint64_t)chrtest)^(((uint64_t)chrtest)<<16);
        chrtest=((uint64_t)chrtest)^(((uint64_t)chrtest)<<32);
        do{
          tvar=*((uint64_t*)ptr)^chrtest;
          tmp=tvar;
          tmp=(tmp>>32)&tmp;
          tmp=(tmp>>16)&tmp;
          tmp=(tmp>>8)&tmp;
          if(!((uint8_t)tmp)){
            tmp=0;
            while(!(tmp&0x8)){
              if(!((uint8_t)tvar)){
                *((uint8_t*)(ptr+tmp))=replacement;
              }
              tvar>>=8;
              ++tmp;
            }
          }
          ptr+=sizeof(uint64_t);
        }while(--tlength);
      }
      tlength=length&0x7;
      while(tlength--){
        if(*ptr==chr)
          *ptr=replacement;
        ++ptr;
      }
    }

    bool replaceText(std::string& src,std::string& replacement){
      char *ptr;
      char *ptroff;
      char *csrc=(char*)src.c_str();
      int64_t tmp;
      int64_t tmp1;
      int64_t slen=src.size();
      int64_t dlen=replacement.size();
      int64_t stored;
      if(slen>=dlen){
        ptr=pointer;
        ptroff=ptr;
        stored=0;
        while(*ptroff){
          if(*ptroff==*csrc){
            for(tmp=1;tmp<slen && ptroff[tmp] && ptroff[tmp]==csrc[tmp];++tmp);
            if(tmp==slen){
              for(tmp=0;tmp<dlen;++tmp){
                ptr[tmp]=replacement[tmp];
              }
              ptr+=dlen;
              ptroff+=slen;
              ++stored;
              continue;
            }
          }
          if(ptroff!=ptr)
            *ptr=*ptroff;
          ++ptr;
          ++ptroff;
        }
        length-=(stored*(slen-dlen));
        pointer[length]=0;
      }else{
        ptr=pointer;
        for(tmp=0;*ptr;++ptr){
          if(*ptr==*csrc){
            for(tmp1=0;tmp1<slen && ptr[tmp1] && ptr[tmp1]==csrc[tmp1];++tmp1);
            if(tmp1==slen){
              ptr+=slen;
              ++tmp;
              continue;
            }
          }
        }
        tmp1=(tmp*(dlen-slen))+length;
        if(tmp1>capacity){
          if(!resize(tmp1))
            return 0;
        }
        ptr=pointer;
        ptroff=ptr+length;
        char* sop=csrc+slen-1,*rop=(char*)replacement.c_str()+dlen-1,*ptrmain;
        int64_t val=0;
        tmp1=dlen-slen;
        tmp*=tmp1;
        stored=tmp;
        slen*=-1;
        dlen*=-1;
        ptrmain=ptroff+tmp1;
        while(--ptroff>=ptr){
          if(*ptroff==*sop){
            for(val=-1;val>slen && ptroff>=ptr && ptroff[val]==sop[val];--val);
            if(val==slen){
              --ptrmain;
              for(val=0;val>dlen;--val){
                ptrmain[val]=rop[val];
              }
              ptrmain+=dlen+1;
              ptroff+=slen+1;
              continue;
            }
          }
          *(--ptrmain)=*ptroff;
        }
        length+=stored;
        pointer[length]='\0';
      }
      return 1;
    }


    bool append(const std::string& text){
      unsigned int len=text.length();
      long int t64_1;
      if((t64_1=length+len)+1>=capacity){
        if(!resize(t64_1+1))
          return false;
      }
      memory::copy(pointer+length,text.c_str(),len);
      length=t64_1;
      pointer[t64_1]='\0';
      return true;
    }

    bool append(const char* text,unsigned int len){
      uint64_t t64_1;
      if((t64_1=length+len)+1>=capacity){
        if(!resize(t64_1+1))
          return false;
      }
      memory::copy(pointer+length,text,len);
      length=t64_1;
      pointer[t64_1]='\0';
      return true;
    }

    bool append(char chr){

      long int t64_1;
      if((t64_1=length+2)>=capacity){
        if(!resize(t64_1))
          return false;
      }
      pointer[length++]=chr;
      pointer[length]='\0';
      return true;
    }

    int64_t rfindChar(int64_t position,char chr){
      if(position>=0 && position<length){
        char* ptr=pointer;
        do{
          if(ptr[position]==chr)
            return position;
        }while(--position>=0);
      }
      return (int64_t)(-1);
    }

    long int findChar(long unsigned int position,char chr){
      long int length=this->length;
      char* ptr=pointer;
      if(position<length){
        do{
          if(ptr[position]==chr)
            return position;
        }while(++position<length);
      }
      return -1;
    }

  };

}
/*
 * int main(){
 *  initMemory();
 *  String* str=newString();
 *  insertChar(str,'c',0);
 *  printf("FirstInsert %s\n",str->pointer);
 *  insertText(str,"@Oh my gosh first text",0);
 *  printf("Second Insert %s length %d\n",str->pointer,str->length);
 *  deleteChar(str,2);
 *  printf("Third delete char %s length %d\n",str->pointer,str->length);
 *  deleteText(str,1,4);
 *  printf("fourth delete text %s length %d\n",str->pointer,str->length);
 *  replaceChar(str,'t','x');
 *  printf("fifth delete text %s length %d\n",str->pointer,str->length);
 *  replaceText(str,"firsx","sec");
 *  printf("six replace text %s length %d\n",str->pointer,str->length);
 *  replaceText(str," g","i");
 *  printf("sevent replace text %s length %d\n",str->pointer,str->length);
 *  finiMemory();
 * }
 */

#endif

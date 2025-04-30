#ifndef __UI_LIB_FILEOPERATION__
#define __UI_LIB_FILEOPERATION__

#include<sys/stat.h>
#include<stdio.h>


namespace lib{


  /*    printf("sb.st_mode&S_IRUSR %d\n",sb.st_mode&S_IRUSR);
      printf("sb.st_mode&S_IWUSR %d\n",sb.st_mode&S_IWUSR);
      printf("sb.st_mode&S_IXUSR %d\n",sb.st_mode&S_IXUSR);*/


  namespace io{

    constexpr int start=SEEK_SET;
    constexpr int cursor=SEEK_CUR;
    constexpr int end=SEEK_SET;

    class file{
    private:
      FILE* stream=NULL;
    public:

      bool open(const std::string& name,const std::string& mode){
        if(stream){
          fclose(stream);
        }

        stream=fopen(name.c_str(),mode.c_str());
        if(!stream)
          return false;

        return true;
      }

      void close(){
        fclose(stream);
        stream=NULL;
      }

      file(const std::string& name,const std::string& mode){
        stream=fopen(name.c_str(),mode.c_str());
      }

      ~file(){
        if(stream){
          fclose(stream);
          stream=NULL;
        }
      }

      size_t length(){
        size_t cpos=ftell(stream);
        fseek(stream,0,SEEK_END);
        size_t len=ftell(stream);
        fseek(stream,cpos,SEEK_SET);
        return len;
      }

      void setCursor(unsigned int index){
        fseek(stream,index,SEEK_SET);
      }

      void setCursor(int location,int index){
        fseek(stream,index,location);
      }

      void write(char* buffer,size_t count){
        fwrite( (void*)buffer, sizeof(char), count, stream);
      }

      void write(int* buffer,size_t count){
        fwrite( (void*)buffer, sizeof(int), count, stream);
      }

      void write(unsigned int* buffer,size_t count){
        fwrite( (void*)buffer, sizeof(unsigned int), count, stream);
      }

      void write(long int* buffer,size_t count){
        fwrite( (void*)buffer, sizeof(long int), count, stream);
      }

      void write(long unsigned int* buffer,size_t count){
        fwrite( (void*)buffer, sizeof(long unsigned int), count, stream);
      }

      void write(void* buffer,size_t vsize,size_t count){
        fwrite( buffer, vsize, count, stream);
      }

      void write(const std::string& text){
        fwrite( (void*)(text.c_str()), sizeof(char), text.length(), stream);
      }

      void read(char* buffer,size_t count){
        fread( (void*)buffer, sizeof(char), count, stream);
      }

      void read(int* buffer,size_t count){
        fread( (void*)buffer, sizeof(int), count, stream);
      }

      void read(unsigned int* buffer,size_t count){
        fread( (void*)buffer, sizeof(unsigned int), count, stream);
      }

      void read(long int* buffer,size_t count){
        fread( (void*)buffer, sizeof(long int), count, stream);
      }

      void read(long unsigned int* buffer,size_t count){
        fread( (void*)buffer, sizeof(long unsigned int), count, stream);
      }

      void read(void* buffer,size_t vsize,size_t count){
        fread( buffer, vsize, count, stream);
      }



      static bool checkPermissions(const std::string& file,uint32_t& mode){
        struct stat st;
        mode=0;
        if(stat(file.c_str(),&st)==-1){
          return false;
        }

        mode=st.st_mode;

        return true;
      }


      static bool exists(const std::string& file){
        struct stat sb;
        if(!stat(file.c_str(),&sb) && S_ISREG(sb.st_mode)){
          return true;
        }
        return false;
      }



    };


    class folder{
    private:
      DIR* dir=NULL;

    public:

      bool opened(){
        if(dir){
          return true;
        }
        return false;
      }

      bool open(const std::string& name){
        if(dir){
          closedir(dir);
        }
        dir=opendir(name.c_str());
        if(!dir){
          return false;
        }
        return true;
      }

      void close(){
        if(dir){
          printf("Closed\n");
          closedir(dir);
          dir=NULL;
        }
      }

      folder(const std::string& name){
        dir=opendir(name.c_str());
      }

      ~folder(){
        close();
      }

      struct dirent* next(){
        return readdir(dir);
      }

      static int exists(std::string& folder){
        struct stat sb;
        if(!stat(folder.c_str(),&sb) && S_ISDIR(sb.st_mode)){
          return 1;
        }
        return 0;
      }

    };

  }

}

#endif

#ifndef __UI_LIB_IO_WATCHER__
#define __UI_LIB_IO_WATCHER__

#include<thread>
#include<mutex>
#include<atomic>
// #include<chrono>

#include<string.h>
#include<errno.h>

#include <fcntl.h>
#include<unistd.h>
#include<sys/eventfd.h>
#include<sys/epoll.h>

#define IOWatcher_Read(name) static void name(void* ptr,int fd)
#define IOWATCHER_TOTAL_EVENT_BUF 128
namespace lib{
  namespace io{
    using IOWatcherFunction= void (*)(void*,int);


    struct WatcherStorage{
      IOWatcherFunction function;
      void* ptr;
      int fd;
    };


    class watcher{
    private:
      std::atomic<bool> running{false};
      std::thread thread;
      std::mutex eventPushingMutex;

      lib::vector<WatcherStorage> info;

      int eventFd;
      int epollFd;

      struct epoll_event eventBuffer[IOWATCHER_TOTAL_EVENT_BUF];

      void loopStop(){
        running.store(false);
        uint64_t var=1;
        write(eventFd,&var,sizeof(var));
      }

      bool removeFromEpoll(int fd){
        if(fd<0 || epollFd<0){
          return false;
        }

        epoll_ctl(epollFd,EPOLL_CTL_DEL,fd,nullptr);
        return true;
      }

      bool addToEpoll(int fd){
        if(fd<0 || epollFd<0){
          return false;
        }

        fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);// it should be nonblocking cause we will be running in seperate thread

        struct epoll_event event={};
        event.data.fd=fd;
        event.events=EPOLLIN|EPOLLET;

        if(epoll_ctl(epollFd,EPOLL_CTL_ADD,fd,&event)==-1){
          log("[ Epoll fd adding error ] : '%s'\n",strerror(errno));
          return false;
        }
        return true;
      }

      int getFdIndex(int fd){
        WatcherStorage* wstr=info.getPointer();
        for(uint32_t i=0,end=info.end();i<end;++i){
          if(wstr[i].fd==fd){
            return i;
          }
        }
        return -1;
      }

      void clearInfo(){
        WatcherStorage* wstr=info.getPointer();
        for(uint32_t i=0,end=info.end();i<end;++i){
          removeFromEpoll(wstr[i].fd);
        }
        info.resize(0);
      }


      void runLoop(){
        int efd;
        if(epollFd<0 || eventFd<0){
          return;
        }

        SDL_Event event;
        struct epoll_event *eventBuf=eventBuffer;

        efd=epollFd;
        std::atomic<bool>& condition=running;
        condition.store(true);
        uint64_t var;
        int32_t length,start,temp;
        while(condition.load()){
          if((length=epoll_wait(efd,eventBuf,IOWATCHER_TOTAL_EVENT_BUF,-1))==-1){
            condition.store(false);
            break;
          }

          for(start=0;start<length;++start){
            temp=eventBuf[start].data.fd;
            if(temp!=eventFd){
              event.type=FILEIO_CALLBACK;
              event.user.code=getFdIndex(temp);
              if(event.user.code==-1){
                continue;
              }
              SDL_PushEvent(&event);
            }
          }
        }
      }


    public:


      void stop(){

        clearInfo();
        loopStop();
        if(thread.joinable()){
          thread.join();
        }
      }

      void start(){
        thread=std::thread(&watcher::runLoop,this);
      }



      void remove(int fd){
        WatcherStorage* start=info.getPointer();
        WatcherStorage* end=start+info.end();


        for(uint32_t index=0;start<end;++index){
          if((start++)->fd==fd){
            info-=(index-1);
            removeFromEpoll(fd);
            if(!info.size() && running.load()){
              stop();
            }
            return;
          }
        }
      }

      bool add(int fd, IOWatcherFunction func,void* ptr){

        if(!func){
          return false;
        }

        WatcherStorage* start=info.getPointer();
        WatcherStorage* end=start+info.end();

        while(start<end){
          if((start++)->fd==fd){
            return true;
          }
        }

        WatcherStorage storage=(WatcherStorage){
          .function=func,
          .ptr=ptr,
          .fd=fd
        };

        if(!addToEpoll(fd)){
          return false;
        }

        if(!info.add(storage)){
          removeFromEpoll(fd);
          return false;
        }

        if(!running.load()){
          this->start();
        }
        return true;
      }

      WatcherStorage* getIndex(uint32_t index){
        if(index<info.end()){
          return info+index;
        }
        return nullptr;
      }


      watcher(){
        epollFd=epoll_create1(0);
        if(epollFd<0){
          log("[ Epoll init error ] : '%s'\n",strerror(errno));
        }
        eventFd=eventfd(0,EFD_NONBLOCK);
        if(eventFd<0){
          close(epollFd);
          epollFd=-1;
          log("[ Epoll event init error ] \n");
          return;
        }

        if(!addToEpoll(eventFd)){
          close(eventFd);
          close(epollFd);
          eventFd=-1;
          epollFd=-1;
          log("[ Epoll event adding init error ] \n");
          return;
        }
      }

      ~watcher(){
        stop();

        removeFromEpoll(eventFd);
        if(eventFd>=0){
          close(eventFd);
        }
        if(epollFd>=0){
          close(epollFd);
        }
        eventFd=-1;
        epollFd=-1;
      }

    };

  }
}

#undef IOWATCHER_TOTAL_EVENT_BUF


#endif

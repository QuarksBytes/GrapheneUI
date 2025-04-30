#ifndef __UI_CORE_CALLBACK__
#define __UI_CORE_CALLBACK__

#include"Core/DataTypes.cpp"
#include"Core/Window.cpp"
#include"Core/Renderer.cpp"
#include"Core/Widget.cpp"


#define Callback_Function(name) static void name(Callback* cback)

enum{
  EVENT_CALLBACK=SDL_USEREVENT+1,
  FILEIO_CALLBACK
};

namespace callback{
  using CallbackFunc=void (*)(Callback*);

  lib::vector<Callback> callbacks[2];
  lib::vector<Callback> eventCallbacks;

  unsigned int flags=0;
  unsigned int renderWidget=0;


  void addRepeat(CallbackFunc func,Data& d1,Data& d2){
    lib::vector<Callback>& callback=eventCallbacks;

    uint32_t start=0,end=callback.size();
    Callback* cback=callback+0;
    Callback tcback;
    if(start<end){
      do{
        if(cback[start].function==func && cback[start].d1.r==d1.r && cback[start].d2.r==d2.r){
          tcback=cback[start];
          if(start){
            callback.shiftLeft(start,end-start,1);
          }else{
            callback<<=1;
          }
          cback+=(end-1);
          cback->function=func;
          cback->d1.r=d1.r;
          cback->d2.r=d2.r;
          return;
        }
      }while(++start<end);
    }
    tcback.function=func;
    tcback.d1.r=d1.r;
    tcback.d2.r=d2.r;
    callback+=tcback;
    event::push(EVENT_CALLBACK);
  }

  void add(void (*func)(Callback* ),Data& d1,Data& d2){
    lib::vector<Callback>& callback=callbacks[flags&1];

    uint32_t start=0,end=callback.size();
    Callback* cback=callback+0;
    Callback tcback;
    if(start<end){
      do{
        if(cback[start].function==func && cback[start].d1.r==d1.r && cback[start].d2.r==d2.r){
          tcback=cback[start];
          if(start){
            callback.shiftLeft(start,end-start,1);
          }else{
            callback<<=1;
          }
          cback+=(end-1);
          cback->function=func;
          cback->d1.r=d1.r;
          cback->d2.r=d2.r;
          return;
        }
      }while(++start<end);
    }
    tcback.function=func;
    tcback.d1.r=d1.r;
    tcback.d2.r=d2.r;
    callback+=tcback;
  }


  void removeRepeat(Callback* rcback){
    lib::vector<Callback>& callback=eventCallbacks;

    Callback* cback=callback+0;
    Callback* cbackEnd=cback+callback.size();
    uint32_t index=0;
    if(cback<cbackEnd){
      do{
        if(cback==rcback){
          callback-=index;
          break;
        }
        ++index;
      }while(++cback<rcback);
    }
  }


  void remove(Callback* rcback){
    lib::vector<Callback>& callback=callbacks[flags&1];

    Callback* cback=callback+0;
    Callback* cbackEnd=cback+callback.size();
    uint32_t index=0;
    if(cback<cbackEnd){
      do{
        if(cback==rcback){
          callback-=index;
          break;
        }
        ++index;
      }while(++cback<rcback);
    }
  }

  void removeRepeat(uint32_t index){
    lib::vector<Callback>& callback=eventCallbacks;

    if(index<callback.size()){
      callback-=index;
    }
  }

  void remove(uint32_t index){
    lib::vector<Callback>& callback=callbacks[flags&1];

    if(index<callback.size()){
      callback-=index;
    }
  }

  void iterateRepeat(){
    lib::vector<Callback>& callback=eventCallbacks;

    Callback* cback=callback+0;
    Callback* cbackEnd=callback+callback.size();

    if(cback<cbackEnd){
      do{
        cback->function(cback);
      }while(++cback<cbackEnd);
    }

    if(callback.size()){
      event::push(EVENT_CALLBACK);
    }
  }

  void iterate(){
    lib::vector<Callback>& callback=callbacks[flags&1];

    flags^=1;

    Callback* cback=callback+0;
    Callback* cbackEnd=callback+callback.size();

    if(cback<cbackEnd){
      do{
        cback->function(cback);
      }while(++cback<cbackEnd);
      callback.resize(0);
    }

  }



  void render(){
    renderWidget=1;
  }

  Callback_Function(____resizeCallback){
    window::resized();
    renderer::resized();
    widget::resized();
    widget::render();
  }

  void resize(){
    Data d1,d2;
    d1.r=0;d2.r=0;
    add(____resizeCallback,d1,d2);
  }


  void init(){
    SDL_RegisterEvents(1);
  }

}

#endif

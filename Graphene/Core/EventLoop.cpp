#ifndef __UI_CORE_EVENT_LOOP__
#define __UI_CORE_EVENT_LOOP__



#include"Lib/IOWatcher.cpp"

lib::io::watcher IOWatcher;


class EventLoop{
private:



  void loop(Widget* root=nullptr){
    window::flags|=UI_STARTED;
    widget::root=root;
    SDL_Event event;
    Touch touch;
    Key key;

    if(root){
      root->updateCoords(DIMENSION_X|DIMENSION_Y|DIMENSION_W|DIMENSION_H|UI_DONT_CALL_RESIZE_FUNCTION);
    }

    lib::io::watcher& watcher=IOWatcher;
    lib::io::WatcherStorage* wstorage;

    callback::resize();
    callback::render();

    std::cout<<"Time taken to start "<<SDL_GetTicks()-window::tick<<std::endl;
    for(;;){
      SDL_WaitEvent(&event);
      do{
        switch(event.type){
          case SDL_MOUSEBUTTONDOWN:
            touch.coords_int.x=event.motion.x;
            touch.coords.x=touch.coords_int.x*window::widthInverse;
            touch.coords_int.y=(window::height-event.motion.y);
            touch.coords.y=touch.coords_int.y*window::heightInverse;
            touch.iterateDown(widget::root);
            break;
          case SDL_MOUSEBUTTONUP:
            touch.coords_int.x=event.motion.x;
            touch.coords.x=touch.coords_int.x*window::widthInverse;
            touch.coords_int.y=(window::height-event.motion.y);
            touch.coords.y=touch.coords_int.y*window::heightInverse;
            touch.iterateUp(widget::root);
            break;
          case SDL_MOUSEMOTION:
            if(touch.motion.function){
              touch.coords_int.x=event.motion.x;
              touch.coords.x=touch.coords_int.x*window::widthInverse;
              touch.coords_int.y=(window::height-event.motion.y);
              touch.coords.y=touch.coords_int.y*window::heightInverse;
              touch.coords_rel.x=event.motion.xrel;
              touch.coords_rel.y=((int32_t)(~((uint32_t)(event.motion.yrel))))+1;
              touch.motion.function(touch.motion.controller,touch);
            }
            break;
          case SDL_TEXTINPUT:
            if(keyboard::function){
              key=(Key){.text=event.text.text,.keycode=event.key.keysym.sym};
              keyboard::function(keyboard::controller,key);
            }
            break;
          case SDL_KEYDOWN:
            if((event.key.keysym.sym<0x20 || event.key.keysym.sym>0x7e) && keyboard::function){
              key=(Key){.text=NULL,.keycode=event.key.keysym.sym};
              keyboard::function(keyboard::controller,key);
            }
            break;
          case SDL_WINDOWEVENT:
            if(event.window.event==SDL_WINDOWEVENT_RESIZED){
              callback::resize();
            }
            break;
          case EVENT_CALLBACK:
            break;
          case FILEIO_CALLBACK:
            wstorage=watcher.getIndex(event.user.code);
            if(wstorage){
            printf("Calling func %d\n",wstorage->fd);
              wstorage->function(wstorage->ptr,wstorage->fd);
            }
            break;
          case SDL_QUIT:
            return;
        }
      }while(SDL_PollEvent(&event));
      callback::iterateRepeat();
      callback::iterate();

      if(callback::renderWidget){
        widget::render();
        callback::renderWidget=0;
      }
    }
  }

public:

  void start(Widget* widget){
    loop(widget);
  }

  void addListner(){

  }
};

#endif

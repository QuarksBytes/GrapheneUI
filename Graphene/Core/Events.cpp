#ifndef __UI_CORE_EVENTS__
#define __UI_CORE_EVENTS__

namespace event{
  void push(long unsigned int eventVal){
    SDL_Event event;
    event.type=eventVal;
    SDL_PushEvent(&event);
  }
};


#endif


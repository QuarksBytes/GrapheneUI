#ifndef __CORE_UI__
#define __CORE_UI__

#include<iostream>
#include<string>

#include<SDL.h>
#include<SDL_image.h>

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>


#ifdef _WIN32
#define WINDOWS

#elif __APPLE__
#define APPLE

#elif __ANDROID__
#define ANDROID

#else // __linux__
#define LINUX

#endif

#define FRAME_PER_SEC 500

#define uiLog SDL_Log
#define log SDL_Log

#define delay SDL_Delay

#define WINDOWPOS_UNDEFINED SDL_WINDOWPOS_UNDEFINED

#define BasicDrawInfo  uint32_t borderThickness=0.0;\
uint32_t angle=0.0;\
uint32_t blur=0.0

#define uiMain SDL_Main



#include"Setting.h"

#include"Lib/Memory.cpp"
#include"Lib/Vector.cpp"
#include"Lib/String.cpp"

#include"Core/Events.cpp"
#include"Core/DataTypes.cpp"
#include"Core/Window.cpp"
#include"Core/Themes.cpp"
#include"Core/Renderer.cpp"
#include"Core/Widget.cpp"
#include"Core/Font.cpp"
#include"Core/Callback.cpp"
#include"Core/Touch.cpp"
#include"Core/Keyboard.cpp"
#include"Core/EventLoop.cpp"
#include"Required/Icons.h"



void uiLogAndExit(int32_t errorCode,std::string errorText){
  uiLog("[ Error , Exiting ] : %s\n",errorText.c_str());
  std::exit(errorCode);
}

#include"Widgets/Info.cpp"


class Graphene{
private:
  EventLoop eloop;
public:

  static void quit(){
    SDL_Event event;
    event.type=SDL_QUIT;
    SDL_PushEvent(&event);
  }

  Graphene(std::string name="UI Application",ivec4 dimensions=(ivec4){.x=WINDOWPOS_UNDEFINED,.y=WINDOWPOS_UNDEFINED,.w=800,.h=600} ){

    window::tick=SDL_GetTicks();
    window::init(name,dimensions);
    renderer::init();
    callback::init();
    font::init();
    // IOWatcher.start();
    // std::cout<<"started\n";
    // Themes::colorP

  }
  ~Graphene(){
    // std::cout<<"stopping\n";
    IOWatcher.stop();
    renderer::fini();
    window::fini();
    font::fini();
  }

  void run(Widget* root=nullptr){
    eloop.start(root);


  }

};


#endif

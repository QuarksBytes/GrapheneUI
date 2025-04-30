#ifndef __UI__WIDGET_SCREEN__
#define __UI__WIDGET_SCREEN__


#include"Core/Widget.cpp"
#include<vector>


#define SCREEN_RESIZED 0x10

struct ____internalScreen{
  Widget* widget;
  long unsigned int flags;
};

class Screen :public Widget{

private:
  lib::vector<____internalScreen> screens;
  unsigned int current=0;
  unsigned int lastScreen=0;

public:
  Widget_Resize(screen_resize){
    Screen* screen=static_cast<Screen*>(widget);
    Widget* iter;
    for(unsigned int i=0,size=screen->screens.size();i<size;++i){
      if(i==screen->current){
        screen->screens[i].flags=0;
      }else{
        screen->screens[i].flags|=mode;
      }
    }
  }


  Screen(unsigned int totalScreens){
    flags=SETTING_RENDER|SETTING_TOUCH;
    screens.zero(0,totalScreens);
    onResize=screen_resize;
  }


  void set(unsigned int cscreen){
    unsigned int t32_1=screens.size();
    if(cscreen>=t32_1)
      return;
    screens[current].widget=child;

    current=cscreen;
    child=screens[cscreen].widget;
    if(window::flags&UI_STARTED){
      ____internalScreen& cs=screens[cscreen];
      if(cs.flags){
        Widget* iter=cs.widget;
        t32_1=cs.flags;
        cs.flags=0;
        while(iter){
          iter->updateCoords(t32_1);
          iter=iter->next;
        }
      }
      callback::render();
    }
  }

  void goBack(){
    if(lastScreen!=current)
      set(lastScreen);
  }

  void add(unsigned int cscreen,Widget* ch){
    if(!ch)
      return;
    unsigned int t32_1=screens.size();
    if(cscreen>=t32_1){
      cscreen=t32_1-1;
    }

    Widget* w=screens[cscreen].widget,*old=NULL;
    while(w){
      if(w==ch)
        return;
      old=w;
      w=w->next;
    }
    if(old){
      old->next=ch;
      ch->previous=old;
    }else{
      ch->previous=0;
      screens[cscreen].widget=ch;
      if(cscreen==current){
        child=ch;
      }
    }
    ch->parent=this;
    ch->next=NULL;

    if(window::flags&UI_STARTED){
      if(cscreen==current){
        ch->updateCoords(0xf);
        callback::render();
      }else{
        screens[cscreen].flags=0xf;
      }
    }
  }

  void remove(unsigned int cscreen,Widget* ch){
    if(!ch)
      return;
    unsigned int t32_1=screens.size();
    if(cscreen>=t32_1){
      return;
    }

    ____internalScreen& cs=screens[cscreen];

    Widget* c=cs.widget;
    while(c){
      if(c==ch){
        if(c->previous)
          c->previous->next=c->next;
        else
          cs.widget=c->next;
        if(c->next)
          c->next->previous=c->previous;
        if(cscreen==current)
          child=cs.widget;
        return;
      }
      c=c->next;
    }
    if(window::flags&UI_STARTED && cscreen==current){
      callback::render();
    }
  }
};
#endif

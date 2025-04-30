#ifndef __UI_CORE_KEYBOARD__
#define __UI_CORE_KEYBOARD__

#include"Core/DataTypes.cpp"
#include"Required/KeyboardMacros.h"

namespace keyboard{
  Widget* controller=NULL;
  void (*function)(Widget* ,Key& )=NULL;

  void giveAccessToWidget(Widget* wid,void (*tfunction)(Widget* ,Key& )){
    controller=wid;
    function=tfunction;
  }

  void removeAccessFromWidget(Widget* widget){
    if(controller==widget){
      controller=NULL;
      function=NULL;
    }
  }

  static inline struct Widget* getCurrentFocus(){
    return controller;
  }

}
#endif


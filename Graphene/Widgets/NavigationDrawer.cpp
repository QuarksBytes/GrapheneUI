#ifndef __UI__WIDGET_NAVIGATIONDRAWER__
#define __UI__WIDGET_NAVIGATIONDRAWER__


#define NAVIGATION_DRAWER_DISMISS_ON_EXTERNAL_TOUCH ((uint64_t)1)

class NavigationDrawer : public Widget{
private:
  vec2 startingRange={};
  vec2 endingRange={};
  vec2 pacing={};
public:
  BasicDrawInfo;

  Callback_Function(navigationDrawer_closeDrawer){
    NavigationDrawer* nd=static_cast<NavigationDrawer*>(cback->d1.a);
    uint32_t mode=0;
    if(nd->endingRange.x<nd->startingRange.x){
      if(nd->pos.relative.x<nd->startingRange.x){
        nd->pos.relative.x-=nd->pacing.x;
      }else{
        mode=1;
      }
    }else if(nd->pos.relative.x>nd->startingRange.x){
      nd->pos.relative.x-=nd->pacing.x;
    }else{
      mode=1;
    }

    if(nd->endingRange.y<nd->startingRange.y){
      if(nd->pos.relative.y<nd->startingRange.y){
        nd->pos.relative.y-=nd->pacing.y;
      }else{
        mode|=2;
      }
    }else if(nd->pos.relative.y>nd->startingRange.y){
      nd->pos.relative.y-=nd->pacing.y;
    }else{
      mode|=2;
    }
    nd->updatePos();
    callback::render();

    if(mode==3){
      nd->hide();
      callback::removeRepeat(cback);
    }


  }

  Callback_Function(navigationDrawer_openDrawer){
    NavigationDrawer* nd=static_cast<NavigationDrawer*>(cback->d1.a);
    uint32_t mode=0;

    nd->pos.relative.x+=nd->pacing.x;
    nd->pos.relative.y+=nd->pacing.y;

    if(nd->endingRange.x<nd->startingRange.x){
      if(nd->pos.relative.x<=nd->endingRange.x){
        mode=1;
      }
    }else if(nd->pos.relative.x>=nd->endingRange.x){
      mode=1;
    }

    if(nd->endingRange.y<nd->startingRange.y){
      if(nd->pos.relative.y<=nd->endingRange.y){
        mode|=2;
      }
    }else if(nd->pos.relative.y>=nd->endingRange.y){
      mode|=2;
    }
    if(mode==3){
      nd->pos.relative.x=nd->endingRange.x;
      nd->pos.relative.y=nd->endingRange.y;
      callback::removeRepeat(cback);
    }
    nd->updatePos();
    callback::render();

  }

  NavigationDrawer& close(){
    Data d1,d2;
    d1.a=static_cast<void*>(this);
    d2.ud.v1=0;
    pos.relative.x=endingRange.x;
    pos.relative.y=endingRange.y;
    updateCoords(DIMENSION_X|DIMENSION_Y);
    callback::addRepeat(navigationDrawer_closeDrawer,d1,d2);
    return *this;
  }

  NavigationDrawer& open(){
    Data d1,d2;
    d1.a=static_cast<void*>(this);
    pos.relative.x=startingRange.x;
    pos.relative.y=startingRange.y;
    updateCoords(DIMENSION_X|DIMENSION_Y);
    show();
    callback::addRepeat(navigationDrawer_openDrawer,d1,d2);
    return *this;
  }

  NavigationDrawer& toggle(){
    Data d1,d2;
    d1.a=static_cast<void*>(this);
    if(endingRange.x>startingRange.x){
      if(pos.relative.x>startingRange.x){
        callback::addRepeat(navigationDrawer_closeDrawer,d1,d2);
        return *this;
      }else{
        show();
        callback::addRepeat(navigationDrawer_openDrawer,d1,d2);
        return *this;
      }
    }else if(pos.relative.x<startingRange.x){
      callback::addRepeat(navigationDrawer_closeDrawer,d1,d2);
      return *this;
    }else{
      show();
      callback::addRepeat(navigationDrawer_openDrawer,d1,d2);
      return *this;
    }

    if(endingRange.y>startingRange.y){
      if(pos.relative.y>startingRange.y){
        callback::addRepeat(navigationDrawer_closeDrawer,d1,d2);
        return *this;
      }else{
        show();
        callback::addRepeat(navigationDrawer_openDrawer,d1,d2);
        return *this;
      }
    }else if(pos.relative.y<startingRange.y){
      callback::addRepeat(navigationDrawer_closeDrawer,d1,d2);
      return *this;
    }else{
      show();
      callback::addRepeat(navigationDrawer_openDrawer,d1,d2);
      return *this;
    }

  }



  Widget_Render(navigationDrawer_onRender){
    NavigationDrawer* nd=static_cast<NavigationDrawer*>(widget);

    RectangleDetails rd=(RectangleDetails){
      .coords=widget->coords,
      .textureCoords=(vec4){0},
      .textureFrameCoords=(vec4){0},
      .texture=nullptr,
      .color=*color,
      .borderColor=outline[0],
      .angle=0,
      .borderThickness=0,
      .blur=0,
      .textureColorMultiplier=0xffffffffu
    };

    renderer::renderShadow(rd,SETTING_RENDER|SETTING_SHADOW_ENABLE);
    renderer::renderRectangle(rd);
  }

  Widget_Touch(navigationDrawer_onTouchDown){
    if(widget->flags&NAVIGATION_DRAWER_DISMISS_ON_EXTERNAL_TOUCH){
      if((widget->flags&SETTING_RENDER) && !(touch.flags&(POINT_IN_WIDGET|POINT_ALSO_IN_WIDGET)) ){
        widget->flags|=MODE_DOWN;
      }
    }
    return 1;
  }

  Widget_Touch(navigationDrawer_onTouchUp){
    if(widget->flags&MODE_DOWN){
      widget->flags^=MODE_DOWN;
      (static_cast<NavigationDrawer*>(widget))->close();
      return 1;
    }
    return 0;
  }

  NavigationDrawer& setRange(float startX,float startY,float endX,float endY){
    startingRange=(vec2){.x=startX,.y=startY};
    endingRange=(vec2){.x=endX,.y=endY};

    pacing=(vec2){.x=(endX-startX)*0.1f,.y=(endY-startY)*0.1f};
    setXY(startX,startY);
    hide();

    return *this;
  }

  NavigationDrawer& dismissOnExternalTouch(){
    flags|=NAVIGATION_DRAWER_DISMISS_ON_EXTERNAL_TOUCH;
    return *this;
  }

  NavigationDrawer& noDismissOnExternalTouch(){
    flags&=(~NAVIGATION_DRAWER_DISMISS_ON_EXTERNAL_TOUCH);
    return *this;
  }

  NavigationDrawer(){
    hide();

    setRenderFunc(navigationDrawer_onRender);
    setTouchDownFunc(navigationDrawer_onTouchDown);
    setTouchUpFunc(navigationDrawer_onTouchUp);
  }
};


#endif

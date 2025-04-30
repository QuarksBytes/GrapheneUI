#ifndef __UI_CORE_WIDGET__
#define __UI_CORE_WIDGET__

#include"Core/Window.cpp"
#include"Core/Renderer.cpp"

#define Widget_Touch(name) static int name(Widget* widget,Touch& touch)
#define Widget_Motion(name) static void name(Widget* widget,Touch& touch)
#define Widget_Key(name) static void name(Widget* widget,Key& key)
#define Widget_Resize(name) static void name(Widget* widget,unsigned int mode)
#define Widget_Render(name) static void name(Widget* widget,const uint32_t* color,const uint32_t* outline,const themes::Theme* theme)

class Widget{
  public:
    Widget* next=NULL;
    Widget* previous=NULL;
    Widget* parent=NULL;
    Widget* child=NULL;
    
    uint64_t flags=SETTING_RENDER|SETTING_TOUCH;
    
    vec4 coords=(vec4){0};
    vec2 coordEnd=(vec2){0};

    union{
      vec2 relative;
      ivec2 dp;
    }pos={};

    union{
      vec2 relative;
      ivec2 dp;
    }size={};


  private:

    Widget_Render(____widget_internals_on_render){}
    Widget_Resize(____widget_internals_on_resize){}
    Widget_Touch(____widget_internals_on_touch_down){return 0;}
    Widget_Touch(____widget_internals_on_touch_up){return 0;}
    Widget_Motion(____widget_internals_on_touch_motion){}

  public:

    void (*onRender)(Widget*,const uint32_t* ,const uint32_t* ,const themes::Theme*  )=____widget_internals_on_render;
    void (*onResize)(Widget*,unsigned int )=____widget_internals_on_resize;
    int (*onTouchDown)(Widget*,Touch& )=____widget_internals_on_touch_down;
    int (*onTouchUp)(Widget*,Touch& )=____widget_internals_on_touch_up;


    uint32_t id=0;

    Widget& setRenderFunc(void (*render)(Widget*,const uint32_t* ,const uint32_t* ,const themes::Theme* )){
      if(render){
        onRender=render;
      }else{
        onRender=____widget_internals_on_render;
      }
      return *this;
    }

    Widget& setResizeFunc(void (*resize)(Widget*,unsigned int )){
      if(resize){
        onResize=resize;
      }else{
        onResize=____widget_internals_on_resize;
      }
      return *this;
    }

    Widget& setTouchDownFunc(int (*tdown)(Widget*,Touch&)){
      if(tdown){
        onTouchDown=tdown;
      }else{
        onTouchDown=____widget_internals_on_touch_down;
      }
      return *this;
    }

    Widget& setTouchUpFunc(int(*tup)(Widget*,Touch&)){
      if(tup){
        onTouchUp=tup;
      }else{
        onTouchUp=____widget_internals_on_touch_up;
      }
      return *this;
    }

    void updateCoords(){
      Widget* iter=this,*rParent=parent,*pt=parent;
      if(!pt){
        pt=iter;
        pos.relative=(vec2){.x=0.0f,.y=0.0f};
        size.relative=(vec2){.w=1.0f,.h=1.0f};
        coords=(vec4){.x=0.0,.y=0.0,.w=1.0,.h=1.0};
      }
      while(iter!=rParent){

        iter->coords.x=pt->coords.x+(iter->pos.relative.x*pt->coords.w);
        iter->coords.y=pt->coords.y+(iter->pos.relative.y*pt->coords.h);

        iter->coords.w=iter->size.relative.w*pt->coords.w;
        iter->coords.h=iter->size.relative.h*pt->coords.h;

        iter->coordEnd.x=iter->coords.x+iter->coords.w;
        iter->coordEnd.y=iter->coords.y+iter->coords.h;


        if(iter->onResize){
          iter->onResize(iter,0xf);
        }

        if(iter->child){
          pt=iter;
          iter=iter->child;
          continue;
        }
        while(iter!=rParent && !iter->next){
          iter=iter->parent;
        }
        if(iter!=rParent){
          iter=iter->next;
          pt=iter->parent;
        }
      }

    }

    void updatePos(){
      Widget* iter=this,*rParent=parent,*pt=parent;
      if(!pt){
        pt=iter;
        pos.relative=(vec2){.x=0.0f,.y=0.0f};
        size.relative=(vec2){.w=1.0f,.h=1.0f};
        coords=(vec4){.x=0.0,.y=0.0,.w=1.0,.h=1.0};
      }

      while(iter!=rParent){
        iter->coords.x=pt->coords.x+(iter->pos.relative.x*pt->coords.w);
        iter->coordEnd.x=iter->coords.x+iter->coords.w;
        iter->coords.y=pt->coords.y+(iter->pos.relative.y*pt->coords.h);
        iter->coordEnd.y=iter->coords.y+iter->coords.h;


        if(iter->child){
          pt=iter;
          iter=iter->child;
          continue;
        }
        while(iter!=rParent && !iter->next){
          iter=iter->parent;
        }
        if(iter!=rParent){
          iter=iter->next;
          pt=iter->parent;
        }
      }
    }

    void updateSize(){
      Widget* iter=this,*rParent=parent,*pt=parent;
      if(!pt){
        pt=iter;
        pos.relative=(vec2){.x=0.0f,.y=0.0f};
        size.relative=(vec2){.w=1.0f,.h=1.0f};
        coords=(vec4){.x=0.0,.y=0.0,.w=1.0,.h=1.0};
      }

      if(iter!=rParent){

        iter->coords.w=iter->size.relative.w*pt->coords.w;
        iter->coordEnd.x=iter->coords.x+iter->coords.w;
        iter->coords.h=iter->size.relative.h*pt->coords.h;
        iter->coordEnd.y=iter->coords.y+iter->coords.h;

        goto ________WIDGET__SIZE__CALCULATOR__LOOP_KDAJSKDAK_0;
        while(iter!=rParent){

          iter->coords.x=pt->coords.x+(iter->pos.relative.x*pt->coords.w);
          iter->coords.y=pt->coords.y+(iter->pos.relative.y*pt->coords.h);

          iter->coords.w=iter->size.relative.w*pt->coords.w;
          iter->coords.h=iter->size.relative.h*pt->coords.h;

          iter->coordEnd.x=iter->coords.x+iter->coords.w;
          iter->coordEnd.y=iter->coords.y+iter->coords.h;

________WIDGET__SIZE__CALCULATOR__LOOP_KDAJSKDAK_0:

          if(iter->onResize){
            iter->onResize(iter,0xf);
          }

          if(iter->child){
            pt=iter;
            iter=iter->child;
            continue;
          }
          while(iter!=rParent && !iter->next){
            iter=iter->parent;
          }
          if(iter!=rParent){
            iter=iter->next;
            pt=iter->parent;
          }
        }
      }
    }

    void updateCoords(uint64_t mode){
      Widget* iter=this,*rParent=parent,*pt=parent;
      uint32_t omode;
      uint32_t value=mode&UI_DONT_CALL_RESIZE_FUNCTION;
      mode&=0xf;
      if(!pt){
        pt=iter;
        pos.relative=(vec2){.x=0.0f,.y=0.0f};
        size.relative=(vec2){.w=1.0f,.h=1.0f};
        coords=(vec4){.x=0.0,.y=0.0,.w=1.0,.h=1.0};
      }
      while(iter!=rParent){
        omode=0;
        if(mode&DIMENSION_X){
          iter->coords.x=pt->coords.x+(iter->pos.relative.x*pt->coords.w);
          omode=DIMENSION_X;
        }
        if(mode&DIMENSION_Y){
          iter->coords.y=pt->coords.y+(iter->pos.relative.y*pt->coords.h);
          omode^=DIMENSION_Y;
        }
        if(mode&DIMENSION_W){
          iter->coords.w=iter->size.relative.w*pt->coords.w;
          omode=(omode^DIMENSION_W)|DIMENSION_X;
        }
        if(mode&DIMENSION_H){
          iter->coords.h=iter->size.relative.h*pt->coords.h;
          omode=(omode^DIMENSION_H)|DIMENSION_Y;
        }
        iter->coordEnd.x=iter->coords.x+iter->coords.w;
        iter->coordEnd.y=iter->coords.y+iter->coords.h;

        iter->flags=(iter->flags&(~SETTING_DIMENSION_MASK))^(mode<<SETTING_DIMENSION_SHIFT);
        if(!(value&UI_DONT_CALL_RESIZE_FUNCTION)){
          iter->onResize(iter,mode);
        }
        if(iter->child){
          mode=omode;
          pt=iter;
          iter=iter->child;
          continue;
        }
        while(iter!=rParent && !iter->next){
          iter=iter->parent;
          if(iter)
            mode=(iter->flags>>SETTING_DIMENSION_SHIFT)&0xf;
        }
        if(iter!=rParent){
          iter=iter->next;
          pt=iter->parent;
        }
      }
    }

    Widget& setColor(uint32_t rgba){
      if(rgba>themes::TOTAL_PALETTE_TYPES){
        rgba=0;
      }
      flags=(flags&(~SETTING_THEME_MASK))^(((uint64_t)(rgba))<<SETTING_THEME_SHIFT);
      return *this;
    }

    Widget& setXY(float x,float y){
      pos.relative=(vec2){.x=x,.y=y};
      return *this;
    }


    Widget& setWH(float w,float h){
      size.relative=(vec2){.w=w,.h=h};
      return *this;
    }

  
    void add(Widget* ch){
      if(!ch)
        return;
      Widget* w=child,*old=0;
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
        child=ch;
      }
      ch->parent=this;
      ch->next=0;
    }


    void remove(Widget* ch){
      Widget* c=child;
      while(c){
        if(c==ch){
          if(c->previous)
            c->previous->next=c->next;
          else
            child=c->next;
          if(c->next)
            c->next->previous=c->previous;
          return;
        }
        c=c->next;
      }
    }

    void setAroundPoint(float x, float y){
      // if(x+coords.w<=1.0){
      //   coords.x=x;
      //   relative.x=x;
      // }else{
      //   coords.x=x-coords.w;
      //   relative.x=coords.x;
      // }
      // if(y+coords.h<=1.0){
      //   coords.y=y;
      //   relative.y=y;
      // }else{
      //   coords.y=y-coords.h;
      //   relative.y=coords.y;
      // }
    }

    void setAroundArea(vec4 dimension){
      // if(dimension.x+coords.w<=1.0){
      //   coords.x=dimension.x;
      //   relative.x=dimension.x;
      // }else{
      //   coords.x=dimension.x+dimension.w-coords.w;
      //   relative.x=coords.x;
      // }
      // float y=dimension.y-coords.h;
      // if(y>=0.0){
      //   coords.y=y;
      //   relative.y=y;
      // }else{
      //   coords.y=dimension.y+dimension.h;
      //   relative.y=coords.y;
      // }
      // updateCoords(DIMENSION_X|DIMENSION_Y);
    }


    void fireTouchEvent(){

    }

    void hide(){
      flags&=(~(SETTING_RENDER|SETTING_TOUCH));
    }

    void show(){
      flags|=SETTING_RENDER|SETTING_TOUCH;
    }

    void toggleVisiblity(){
      if(flags&(SETTING_RENDER|SETTING_TOUCH))
        flags&=(~(SETTING_RENDER|SETTING_TOUCH));
      else
        flags|=SETTING_RENDER|SETTING_TOUCH;
    }

    void enableShadow(){
      flags=(flags&(~SETTING_SHADOW_MASK))^SETTING_SHADOW_ENABLE;
    }

    void enableShadow(uint64_t shadowMask){
      flags=(flags&(~SETTING_SHADOW_MASK))^(shadowMask&SETTING_SHADOW_MASK);
    }



};



namespace widget{
  Widget* root=NULL;
  
  void render(){
    Widget* iter;

    const uint32_t (*palette)[2];
    const uint32_t* outline;
    const themes::Theme* ctheme;
    if(themes::current){
      ctheme=themes::current;
    }else{
      ctheme=&(themes::material::blue);
    }
    palette=ctheme->array[themes::mode];
    outline=palette[themes::outline];

    renderer::clear(ctheme->clearColor[themes::mode]);

    if(root){
      iter=root;
      while(iter){
        if(iter->flags&SETTING_RENDER){
          iter->onRender(iter,palette[(iter->flags>>SETTING_THEME_SHIFT)&0xf],outline,ctheme);

          if(iter->child){
            iter=iter->child;
            continue;
          }
        }
        while(iter && !iter->next)
          iter=iter->parent;
        if(iter)
          iter=iter->next;
      }
    }
    window::swap();
  }
  
  
  void resized(){
    ReturnOnError(root);
    Widget* iter=root;
    Widget* parent=iter->parent;
    Widget* resizeDone=nullptr;
    while(iter!=parent){
      iter->onResize(iter,0xf);
      if(iter->child){
        iter=iter->child;
        continue;
      }
      while(iter!=parent && !(iter->next)){
        iter=iter->parent;
        if(iter==resizeDone){
          resizeDone=nullptr;
        }
      }
      if(iter!=parent)
        iter=iter->next;
    }
  }


}





#endif

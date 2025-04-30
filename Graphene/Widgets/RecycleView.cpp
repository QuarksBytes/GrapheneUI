#ifndef __UI__WIDGET_RECYCLEVIEW__
#define __UI__WIDGET_RECYCLEVIEW__


#define RecycleView_Touch(x) static void x(void* ptr,vec2& coords,uint32_t row,uint32_t column)
#define RecycleView_Resize(x) static void x(void* ptr,uint32_t mode)
#define RecycleView_Render(x) static void x(void* ptr,vec4& coords,uint32_t row,uint32_t column,const uint32_t *color,const uint32_t *outline,themes::Theme* theme)

class RecycleView : public Widget{

public:

    Texture* texture=NULL;

    BasicDrawInfo;

    struct{
      void (*renderWidget)(void* ,vec4& ,uint32_t,uint32_t,const uint32_t* ,const uint32_t*,themes::Theme* ); // last two for color and outline
      void (*resizeWidget)(void* ,uint32_t);
      void (*touchWidget)(void* ,vec2& ,uint32_t,uint32_t);
      void* pointer;
      uint64_t color;
    }component={};

private:


    vec2 offsets=(vec2){0};
    vec2 size=(vec2){0};
    ivec2 count=(ivec2){0};

    vec2 inverse=(vec2){0};

    struct{
      ivec2 start;
      ivec2 current;
      ivec2 end;
    }scrollData={0};



    void refreshTexture(){

      uint32_t w=coords.w*window::width;
      uint32_t h=coords.h*window::height;

      if(!texture){
        texture=Texture::create(w,h);
        if(!texture){
          uiLogAndExit(EXIT_FAILURE,"RecycleView Error");
        }
      }else if(texture->w!=w || texture->h!=h){
        texture->resize(w,h);
      }
      inverse.w=1.0/w;
      inverse.h=1.0/h;
    }


    Widget_Render(on_render){
      RecycleView* rv=static_cast<RecycleView*>(widget);
      RectangleDetails rd={
        .coords=rv->coords,
        .textureCoords=(vec4){.x=0.0,.y=0.0,.w=1.0,.h=1.0},
        .textureFrameCoords=(vec4){.x=0.0,.y=0.0,.w=1.0,.h=1.0},
        .texture=rv->texture,
        .color=*color,
        .borderColor=color[1],
        .angle=rv->angle,
        .borderThickness=rv->borderThickness,
        .blur=rv->blur,
        .textureColorMultiplier=0xffffffffu
      };
      renderer::renderRectangle(rd);

    }

    static void on_motion_callback(Data d1,Data d2){
      // RecycleView* rv=static_cast<RecycleView*>(d1.a);
      //
      // ivec2 end=rv->scrollData.end;
      //
      // int current=rv->scrollData.current.x;
      // float offset,tfloat;
      //
      // int t32_1;
      //
      //
      //
      // // offset=rv->offsets.x+(touch.coords_rel.x*rv->inverse.w);
      // //vec2 offsets=(vec2){.x=rv->offsets.x+(touch->coords_rel.x*rv->inverse.w),.y=rv->offsets.y+(touch->coords_rel.y*rv->inverse.h)};
      //
      //
      //
      // if(offset<(tfloat=-1*rv->size.w)){
      //   if(current<(t32_1=end.x-(rv->count.x+1))){
      //
      //     do{
      //       ++current;
      //       offset+=rv->size.w;
      //     }while(current<t32_1 && offset<tfloat);
      //     rv->scrollData.current.x=current;
      //     rv->offsets.x=offset;
      //   }else{
      //
      //     if(current>=t32_1+1){
      //       t32_1=end.x-rv->count.x;
      //       if(t32_1<rv->scrollData.start.x){
      //         t32_1=rv->scrollData.start.x;
      //       }
      //       rv->scrollData.current.x=t32_1;
      //       rv->offsets.x=0;
      //     }else
      //       rv->offsets.x=tfloat;
      //   }
      //
      // }else if(offset>0.0){
      //   if(current>rv->scrollData.start.x){
      //     do{
      //       offset-=rv->size.w;
      //       --current;
      //     }while(current>rv->scrollData.start.x && offset>0.0);
      //     if(offset>0.0){
      //       current=rv->scrollData.start.x;
      //       offset=0.0;
      //     }
      //     rv->offsets.x=offset;
      //     rv->scrollData.current.x=current;
      //   }else{
      //     rv->scrollData.current.x=rv->scrollData.start.x;
      //     rv->offsets.x=0.0;
      //   }
      // }else{
      //   rv->flags|=MODE_MOTION;
      //   if(current<(t32_1=end.x-(rv->count.w+1))){
      //     rv->offsets.x=offset;
      //   }else{
      //     if(touch.coords_rel.x>0){
      //       rv->offsets.x=offset;
      //     }else{
      //       if(current>=t32_1+1){
      //         t32_1=end.x-rv->count.x;
      //         if(t32_1<rv->scrollData.start.x){
      //           t32_1=rv->scrollData.start.x;
      //         }
      //         rv->scrollData.current.x=t32_1;
      //         rv->offsets.x=0.0;
      //       }else
      //         rv->offsets.x=offset;
      //     }
      //   }
      // }
      //
      // offset=rv->offsets.y+(touch.coords_rel.y*rv->inverse.h);
      // //vec2 offsets=(vec2){.x=rv->offsets.x+(touch->coords_rel.x*rv->inverse.w),.y=rv->offsets.y+(touch->coords_rel.y*rv->inverse.h)};
      // current=rv->scrollData.current.y;
      //
      // if(offset>rv->size.h){
      //   if(current<(t32_1=end.y-(rv->count.y+1))){
      //     do{
      //       ++current;
      //       offset-=rv->size.h;
      //     }while(current<t32_1 && offset>rv->size.h);
      //     rv->scrollData.current.y=current;
      //     rv->offsets.y=offset;
      //   }else{
      //     if(current>=t32_1+1){
      //       t32_1=end.x-rv->count.y;
      //       if(t32_1<rv->scrollData.start.y){
      //         t32_1=rv->scrollData.start.y;
      //       }
      //       rv->scrollData.current.y=t32_1;
      //       rv->offsets.y=0.0;
      //     }else
      //       rv->offsets.y=rv->size.h;
      //   }
      //
      // }else if(offset<0.0){
      //   if(current>rv->scrollData.start.x){
      //     do{
      //       offset+=rv->size.h;
      //       --current;
      //     }while(current>rv->scrollData.start.x && offset<0.0);
      //     if(offset<0.0){
      //       current=rv->scrollData.start.y;
      //       offset=0.0;
      //     }
      //     rv->offsets.y=offset;
      //     rv->scrollData.current.y=current;
      //   }else{
      //     rv->scrollData.current.y=rv->scrollData.start.y;
      //     rv->offsets.y=0.0;
      //   }
      // }else{
      //   rv->flags|=MODE_MOTION;
      //   if(current<(t32_1=end.y-(rv->count.y+1))){
      //     rv->offsets.y=offset;
      //   }else{
      //     if(touch.coords_rel.y<0){
      //       rv->offsets.y=offset;
      //     }else{
      //
      //       if(current>=t32_1+1){
      //         t32_1=end.y-rv->count.y;
      //         if(t32_1<rv->scrollData.start.y){
      //           t32_1=rv->scrollData.start.y;
      //         }
      //         rv->scrollData.current.y=t32_1;
      //         rv->offsets.y=0.0;
      //       }else
      //         rv->offsets.y=offset;
      //     }
      //   }
      // }
      //
      // rv->updateTexture();
      // callback::render();
    }

    Widget_Motion(on_touch_motion){
      RecycleView* rv=static_cast<RecycleView*>(widget);
      ivec2 end=rv->scrollData.end;

      int current=rv->scrollData.current.x;
      float offset,tfloat;

      int t32_1;



      offset=rv->offsets.x+(touch.coords_rel.x*rv->inverse.w);
      //vec2 offsets=(vec2){.x=rv->offsets.x+(touch->coords_rel.x*rv->inverse.w),.y=rv->offsets.y+(touch->coords_rel.y*rv->inverse.h)};



      if(offset<(tfloat=-1*rv->size.w)){
        if(current<(t32_1=end.x-(rv->count.x+1))){

          do{
            ++current;
            offset+=rv->size.w;
          }while(current<t32_1 && offset<tfloat);
          rv->scrollData.current.x=current;
          rv->offsets.x=offset;
        }else{

          if(current>=t32_1+1){
            t32_1=end.x-rv->count.x;
            if(t32_1<rv->scrollData.start.x){
              t32_1=rv->scrollData.start.x;
            }
            rv->scrollData.current.x=t32_1;
            rv->offsets.x=0;
          }else
            rv->offsets.x=tfloat;
        }

      }else if(offset>0.0){
        if(current>rv->scrollData.start.x){
          do{
            offset-=rv->size.w;
            --current;
          }while(current>rv->scrollData.start.x && offset>0.0);
          if(offset>0.0){
            current=rv->scrollData.start.x;
            offset=0.0;
          }
          rv->offsets.x=offset;
          rv->scrollData.current.x=current;
        }else{
          rv->scrollData.current.x=rv->scrollData.start.x;
          rv->offsets.x=0.0;
        }
      }else{
        rv->flags|=MODE_MOTION;
        if(current<(t32_1=end.x-(rv->count.w+1))){
          rv->offsets.x=offset;
        }else{
          if(touch.coords_rel.x>0){
            rv->offsets.x=offset;
          }else{
            if(current>=t32_1+1){
              t32_1=end.x-rv->count.x;
              if(t32_1<rv->scrollData.start.x){
                t32_1=rv->scrollData.start.x;
              }
              rv->scrollData.current.x=t32_1;
              rv->offsets.x=0.0;
            }else
              rv->offsets.x=offset;
          }
        }
      }

      offset=rv->offsets.y+(touch.coords_rel.y*rv->inverse.h);
      //vec2 offsets=(vec2){.x=rv->offsets.x+(touch->coords_rel.x*rv->inverse.w),.y=rv->offsets.y+(touch->coords_rel.y*rv->inverse.h)};
      current=rv->scrollData.current.y;

      if(offset>rv->size.h){
        if(current<(t32_1=end.y-(rv->count.y+1))){
          do{
            ++current;
            offset-=rv->size.h;
          }while(current<t32_1 && offset>rv->size.h);
          rv->scrollData.current.y=current;
          rv->offsets.y=offset;
        }else{
          if(current>=t32_1+1){
            t32_1=end.x-rv->count.y;
            if(t32_1<rv->scrollData.start.y){
              t32_1=rv->scrollData.start.y;
            }
            rv->scrollData.current.y=t32_1;
            rv->offsets.y=0.0;
          }else
            rv->offsets.y=rv->size.h;
        }

      }else if(offset<0.0){
        if(current>rv->scrollData.start.x){
          do{
            offset+=rv->size.h;
            --current;
          }while(current>rv->scrollData.start.x && offset<0.0);
          if(offset<0.0){
            current=rv->scrollData.start.y;
            offset=0.0;
          }
          rv->offsets.y=offset;
          rv->scrollData.current.y=current;
        }else{
          rv->scrollData.current.y=rv->scrollData.start.y;
          rv->offsets.y=0.0;
        }
      }else{
        rv->flags|=MODE_MOTION;
        if(current<(t32_1=end.y-(rv->count.y+1))){
          rv->offsets.y=offset;
        }else{
          if(touch.coords_rel.y<0){
            rv->offsets.y=offset;
          }else{

            if(current>=t32_1+1){
              t32_1=end.y-rv->count.y;
              if(t32_1<rv->scrollData.start.y){
                t32_1=rv->scrollData.start.y;
              }
              rv->scrollData.current.y=t32_1;
              rv->offsets.y=0.0;
            }else
              rv->offsets.y=offset;
          }
        }
      }

      rv->updateTexture();
      callback::render();
    }

    Widget_Touch(on_touch_down){
      if(touch.flags&POINT_IN_WIDGET){
        RecycleView* rv=static_cast<RecycleView*>(widget);
        rv->flags|=MODE_DOWN;
        touch.motion.controller=rv;
        touch.motion.function=on_touch_motion;
      }
      return 0;
    }

    Widget_Touch(on_touch_up){
      RecycleView* rv=static_cast<RecycleView*>(widget);
      if(rv->flags&MODE_DOWN){
        rv->flags^=MODE_DOWN;
        touch.motion.controller=(Widget*)0;
        touch.motion.function=NULL;
        if(rv->flags&MODE_MOTION){
          rv->flags^=MODE_MOTION;
        }else{

          if(rv->component.touchWidget){
            float temp;
            vec2 coord;
            unsigned int row,column;

            temp=(((touch.coords_int.x-(rv->coords.x*window::width))*rv->inverse.w)-rv->offsets.x)*rv->count.x;
            column=((int)temp);
            coord.x=temp-(float)(column);
            column+=rv->scrollData.current.x;

            temp=((((rv->coordEnd.y*window::height)-touch.coords_int.y)*rv->inverse.h)+rv->offsets.y)*rv->count.y;
            row=((int)temp);
            coord.y=temp-(float)(row);
            row+=rv->scrollData.current.y;

            if(row<rv->scrollData.end.y && column<rv->scrollData.end.x)
              rv->component.touchWidget(rv->component.pointer,coord,row,column);
          }
        }
      }
      return 0;
    }

    Widget_Resize(on_resize){
      RecycleView* rv=static_cast<RecycleView*>(widget);
      if(mode&(DIMENSION_W|DIMENSION_H)){
        rv->refreshTexture();
        if(rv->component.resizeWidget)
          rv->component.resizeWidget(rv->component.pointer,mode);
        rv->updateTexture();
      }else if(rv->component.resizeWidget)
        rv->component.resizeWidget(rv->component.pointer,mode);
    }

public:

  void setComponentRenderFunction(void (*renderWidget)(void* ,vec4& ,unsigned int ,unsigned int ,const uint32_t* ,const uint32_t*,themes::Theme* )){
    component.renderWidget=renderWidget;
  }

  void setComponentResizeFunction(void (*resizeWidget)(void* ,unsigned int )){
    component.resizeWidget=resizeWidget;
  }

  void setComponentTouchFunction(void (*touchWidget)(void* ,vec2& ,unsigned int ,unsigned int)){
    component.touchWidget=touchWidget;
  }

  void setComponentPointer(void *ptr){
    component.pointer=ptr;
  }

  void setComponentColor(uint32_t color){
    if(color>=themes::TOTAL_PALETTE_TYPES){
      color=0;
    }
    component.color=color;
  }

    bool isTextureCreated(){
      if(texture){
        return true;
      }
      return false;
    }

    ivec2 getTextureCoords(){
      if(texture){
        return (ivec2){.w=texture->w,.h=texture->h};
      }else{
        return (ivec2){0};
      }
    }

    vec2 getSize(){
      return size;
    }
    ivec2 getCount(){
      return count;
    }


    void setScrollData(int startRow,int startColumn,int endRow,int endColumn){
      scrollData.start=(ivec2){.column=startColumn,.row=startRow};
      if(endRow<startRow)
        endRow=startRow;
      if(endColumn<startColumn)
        endColumn=startColumn;
      scrollData.end=(ivec2){.column=endColumn,.row=endRow};
      scrollData.current=scrollData.start;

      offsets=(vec2){0};
    }

    void updateTexture(){
      if(!component.renderWidget)
        return;


      ivec2 current=scrollData.current;
      ivec2 end=scrollData.end;
      int currentY=current.y;

      float tempY=-1*size.h;
      float dimensionY=1.0+offsets.y-size.h;
      vec4 dimension={.x=offsets.x,.w=size.w,.h=size.h};
      void* ptr=component.pointer;

      const uint32_t *color;
      const uint32_t* outline;

      themes::Theme* ctheme;
      if(themes::current){
        ctheme=themes::current;
      }else{
        ctheme=&(themes::material::blue);
      }

      color=ctheme->array[themes::mode][component.color];
      outline=ctheme->array[themes::mode][themes::outline];

      renderer::framebuffer::start(texture);
      renderer::clear(0.0,0.0,0.0,0.0);//ctheme->clearColor[themes::mode]);


      if(current.x<end.x && dimension.x<1.0){
        do{
          current.y=currentY;
          dimension.y=dimensionY;
          if(current.y<end.y && dimension.y>tempY){
            do{
              component.renderWidget(ptr,dimension,current.y,current.x,color,outline,ctheme);
              dimension.y-=dimension.h;
            }while(++current.y<end.y && dimension.y>tempY);
          }
          dimension.x+=dimension.w;
        }while(++current.x<end.x && dimension.x<1.0);
      }
      renderer::framebuffer::stop();
    }



    RecycleView(int row=10,int column=1){
      flags=SETTING_RENDER|SETTING_TOUCH;

      onRender=on_render;
      onTouchDown=on_touch_down;
      onTouchUp=on_touch_up;
      onResize=on_resize;

      count=(ivec2){.column=column,.row=row};
      size=(vec2){.w=1.0f/column,.h=1.0f/row};
    }

    ~RecycleView(){
      Texture::destroy(texture);
      texture=NULL;
    }

};


#endif

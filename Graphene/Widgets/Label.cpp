#ifndef __UI__WIDGET_LABELWIDGET__
#define __UI__WIDGET_LABELWIDGET__

class Label : public Widget{
private:
  Texture* texture=NULL;
  vec2 offsets={};

  int64_t pageStart=0;

  vec4 textureCoords={};
  vec4 textureFrameCoords={};

  vec2 frameInv={};
  vec2 gridSize={};

  ivec2 lastRendered={};


  uint32_t columnOffset=0;

  uint32_t row=0;
  uint32_t column=0;
  uint32_t fontSize=0;


public:
  lib::string text;

  void setFontSize(unsigned int fontSize){
    if(!(fontSize))
      fontSize=GLOBAL_TEXT_SIZE;

    this->fontSize=fontSize;
    fontSize=font::dpToPx(fontSize);
    int w=font::bitmap->whRatio*fontSize;

    int width=window::width*coords.w;
    int height=window::height*coords.h;

    int row=this->row=height/(float)fontSize;
    int column=this->column=width/(float)w;

    int rw=column*w;
    int rh=row*fontSize;

    float winv=1.0/width;
    float hinv=1.0/height;

    textureCoords=(vec4){.x=((width-rw)>>1)*winv,.y=((height-rh)>>1)*hinv,.w=rw*winv,.h=rh*hinv}; // for rendering
    textureFrameCoords=(vec4){.x=0.0,.y=0.0,.w=textureCoords.w,.h=textureCoords.h}; // for rendering

    gridSize=(vec2){.w=w*winv,.h=fontSize*hinv}; // for scrolling

    frameInv=(vec2){.w=1.0f/(rw+w),.h=1.0f/(rh+fontSize)}; // for touch calculation
    offsets.y=gridSize.h;

  }

  void refreshTexture(){
    Texture* texture=this->texture;
    int width=window::width*coords.w;
    int height=window::height*coords.h;
    if(!texture){
      texture=Texture::create(width,height);
      if(!(texture)){
        this->texture=(Texture*)0;
        uiLogAndExit(EXIT_FAILURE,"TextInput texture creation error");
      }

      this->texture=texture;
    }else if(texture->w<width || texture->h<height){
      texture->resize(width,height);
    }
    setFontSize(fontSize);

  }


  void updateTexture(){
    if(!window::started()){
      return;
    }

    int array[0x20];

    char* text=this->text.pointer;
    char* textTmp=NULL;
    long unsigned int ps=pageStart;
    long unsigned int index;

    unsigned int length;

    unsigned int trow=this->row+1;
    unsigned int tcolumn=this->column+1;

    unsigned int row=0;
    unsigned int column;

    unsigned int columnOffset=this->columnOffset;
    unsigned int arrayIter=0;

    unsigned int lastRowRendered;
    unsigned int lastColumnRendered=0;

    lib::string& str=(this->text);

    vec4 tfc=(vec4){.x=textureFrameCoords.x-offsets.x,.y=textureFrameCoords.y-offsets.y,.w=textureFrameCoords.w+gridSize.w,.h=textureFrameCoords.h+gridSize.h};

    renderer::framebuffer::start(texture);
    renderer::clear(0.0,0.0,0.0,0.0);

    renderer::setTextTotalRowColumn(trow,tcolumn);
    renderer::setTextRowColumnSize(trow,tcolumn);
    renderer::setTextRenderingDimensions(tfc );
    renderer::setTextFontColor(0xffffffffu);
    renderer::setTextBackgroundColor(0);

    renderer::setTextBitmap(NULL);


    while(row<trow){
      lastRowRendered=row;
      renderer::setTextCurrentRowColumn(row++,0);
      if((index=str.findChar(ps,'\n'))!=-1){
        length=index-ps;
        if(length>=columnOffset){
          length-=columnOffset;

          if(length>lastColumnRendered)
            lastColumnRendered=length;

          if(length>tcolumn)
            length=tcolumn;

          textTmp=text+ps+columnOffset;


          arrayIter=column=0;

          while(column<length){
            array[arrayIter++]=textTmp[column++];

            if(arrayIter&0x20){
              renderer::uploadText(array,0x20);
              renderer::drawInstanced(arrayIter);
              arrayIter=0;
              renderer::setTextCurrentRowColumn(row-1,column);
            }
          }
          if(arrayIter){
            renderer::uploadText(array,arrayIter);
            renderer::drawInstanced(arrayIter);
          }
        }
        ps=index+1;

      }else{
        length=str.length-ps;

        if(length>=columnOffset){
          length-=columnOffset;

          if(length>lastColumnRendered)
            lastColumnRendered=length;

          if(length>tcolumn)
            length=tcolumn;

          textTmp=text+ps+columnOffset;


          arrayIter=column=0;
          while(column<length){
            array[arrayIter++]=textTmp[column++];

            if(arrayIter&0x20){
              renderer::uploadText(array,0x20);
              renderer::drawInstanced(arrayIter);
              arrayIter=0;
              renderer::setTextCurrentRowColumn(row-1,column);
            }
          }

          if(arrayIter){
            renderer::uploadText(array,arrayIter);
            renderer::drawInstanced(arrayIter);
          }
        }

        break;
      }
    }

    this->lastRendered.row=lastRowRendered;
    this->lastRendered.column=lastColumnRendered;
    // printf()

    renderer::framebuffer::stop();
  }

  void scrollRowUp(unsigned int num){
    if(lastRendered.y<this->row)
      return;

    long int ps=pageStart;
    long int index;

    while(num--){
      index=text.findChar(ps,'\n');
      if(index==-1){
        break;
      }
      ps=index+1;
    }
    pageStart=ps;
  }

  void scrollRowDown(unsigned int num){
    long int ps=pageStart;
    if(!ps)
      return;

    long int index;


    if(num ){
      do{
        if(ps){
          index=text.rfindChar(ps-2,'\n');
          ps=1+index;
          continue;
        }
        break;
      }while(--num);

      pageStart=ps;
    }
  }
public:

  void reset(){
    text.reset();
    columnOffset=0;
    lastRendered=(ivec2){0};
    pageStart=0;
    offsets.x=0.0;
    offsets.y=gridSize.h;

  }

  Widget_Motion(label_motion){
    Label* ti=static_cast<Label*>(widget);

    vec2 offsets=(vec2){.x=ti->offsets.x ,.y=ti->offsets.y};

    vec2 gridSize=ti->gridSize;
    int row=ti->row;
    int column=ti->column;
    int columnOffset=ti->columnOffset;
    int t32_1;

    if(touch.coords_rel.y<0){
      if(!ti->pageStart && ti->offsets.y>=gridSize.h){
        ti->offsets.y=gridSize.h;
        goto ____LABEL_MOTIONS_X_OFFSET;
      }
    }else if(ti->lastRendered.y<row && ti->offsets.y<=0.0){
      ti->offsets.y=0.0;
      goto ____LABEL_MOTIONS_X_OFFSET;
    }

    offsets.y+=(-1.0*(touch.coords_rel.y*ti->frameInv.h));

    if(offsets.y<0.0){
      t32_1=0;
      if(ti->lastRendered.y>=row){
        do{
          ++t32_1;
          offsets.y+=gridSize.h;
        }while(offsets.y<0.0);
        ti->scrollRowUp(t32_1);
        ti->offsets.y=offsets.y;
      }else
        ti->offsets.y=0.0;
    }else if(offsets.y>gridSize.h){
      if(ti->pageStart){
        t32_1=0;
        do{
          ++t32_1;
          offsets.y-=gridSize.h;
        }while(offsets.y>gridSize.h);
        ti->scrollRowDown(t32_1);
        ti->offsets.y=offsets.y;
      }else{
        ti->offsets.y=gridSize.h;
      }
    }else
      ti->offsets.y=offsets.y;


    ____LABEL_MOTIONS_X_OFFSET:

    if(touch.coords_rel.x<0){
      if(column>ti->lastRendered.x && ti->offsets.x>=gridSize.w){
        ti->offsets.x=gridSize.w;
        ti->updateTexture();
        callback::render();
        widget->flags|=MODE_MOTION;
        return;
      }
    }else if(!columnOffset && ti->offsets.x<=0.0){
      ti->offsets.x=0.0;
      ti->updateTexture();
      callback::render();
      widget->flags|=MODE_MOTION;
      return;
    }

    offsets.x+=(-1.0*(touch.coords_rel.x*ti->frameInv.w));

    if(offsets.x<0.0){
      t32_1=0;
      if(columnOffset){
        do{
          --columnOffset;
          offsets.x+=gridSize.w;
        }while(offsets.x<0.0 && columnOffset);
        if(offsets.x<0.0)
          offsets.x=0.0;
        ti->columnOffset=columnOffset;
        ti->offsets.x=offsets.x;
      }else
        ti->offsets.x=0.0;
    }else if(offsets.x>=gridSize.w){
      t32_1=ti->lastRendered.x;
      if(column<t32_1){
        do{
          ++columnOffset;
          offsets.x-=gridSize.w;
        }while(offsets.x>gridSize.w && column<--t32_1);
        ti->columnOffset=columnOffset;
        ti->offsets.x=offsets.x;
      }else
        ti->offsets.x=gridSize.w;
    }else
      ti->offsets.x=offsets.x;

    widget->flags|=MODE_MOTION;
    ti->updateTexture();
    callback::render();

  }

  Widget_Touch(label_TouchDown){
    if(touch.flags&POINT_IN_WIDGET){
      widget->flags|=MODE_DOWN;

      Label* ti=static_cast<Label*>(widget);

      touch.motion.controller=widget;
      touch.motion.function=label_motion;

    }
    return 0;
  }

  Widget_Touch(label_TouchUp){
    if(widget->flags&MODE_DOWN){
      widget->flags^=MODE_DOWN;

      touch.motion.controller=(Widget*)0;
      touch.motion.function=NULL;
      Label* ti=static_cast<Label*>(widget);

      if(widget->flags&MODE_MOTION){
        widget->flags^=MODE_MOTION;
      }
    }

    return 0;
  }

  Widget_Render(label_Render){
    Label* ti=static_cast<Label*>(widget);
    RectangleDetails rd=(RectangleDetails){
      .coords=widget->coords,
      .textureCoords=ti->textureCoords,
      .textureFrameCoords=ti->textureFrameCoords,
      .texture=ti->texture,
      .color=0x0,
      .borderColor=0x0,
      .angle=0,
      .borderThickness=0,
      .blur=0,
      .textureColorMultiplier=color[1]
    };
    renderer::renderRectangle(rd);

  }

  Widget_Resize(label_Resize){
    if(mode&(DIMENSION_W|DIMENSION_H)){
      Label* ti=static_cast<Label*>(widget);
      ti->refreshTexture();
      ti->updateTexture();
    }
  }

  ~Label(){
    Texture::destroy(texture);
    texture=NULL;
  }

  Label(unsigned int fsize=GLOBAL_TEXT_SIZE){
    fontSize=fsize;

    onTouchDown=label_TouchDown;
    onTouchUp=label_TouchUp;
    onRender=label_Render;
    onResize=label_Resize;

    flags=SETTING_RENDER|SETTING_TOUCH;
  }


};


#endif

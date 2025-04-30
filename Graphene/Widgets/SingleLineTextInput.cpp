#ifndef __UI__WIDGET_SINGLELINETEXTINPUT__
#define __UI__WIDGET_SINGLELINETEXTINPUT__

#include"Required/KeyboardMacros.h"

#define SINGLELINE_TEXTINPUT_CURSOR_IN_FOCUS 0x1

class SingleLineTextInput : public Widget{
private:
  Texture* texture=NULL;
  vec4 textureCoords;
  vec4 textureFrameCoords;

public:
  lib::string text;

  void* pointer=NULL;
  void (*onText)(void*)=NULL;

private:
  float gridSizePix;
  float gridSizeX;
  float frameInv;
  float xoffset=0;
  unsigned int pageStart=0;
  int framePix;
  unsigned int savedColumn;
  unsigned int column;
  unsigned int cursor=0;

public:
  uint32_t angle=0;
  uint32_t borderThickness=0;
  uint32_t blur=0;
  unsigned int fontSize;



  void setPointer(void* ptr){
    pointer=ptr;
  }


  void setFontSize(unsigned int fontSize){
    if(!(texture))
      return;
    this->fontSize=fontSize;
    fontSize=font::dpToPx(fontSize);
    unsigned int w=font::bitmap->whRatio*fontSize;


    int width=window::width*coords.w;
    int height=window::height*coords.h;

    int row=1;
    int column=this->column=width/(float)w;
    ++(this->column);


    int rw=column*w;

    float winv=1.0/width;
    float hinv=1.0/height;

    textureCoords=(vec4){.x=((width-rw)>>1)*winv,.y=((height-fontSize)>>1)*hinv,.w=rw*winv,.h=fontSize*hinv}; // for rendering
    textureFrameCoords=(vec4){.x=0.0,.y=0.0,.w=textureCoords.w,.h=textureCoords.h}; // for rendering

    gridSizeX=w*winv; // for scrolling
    gridSizePix=w;

    framePix=rw; // for touch calculation
    frameInv=1.0/(rw+w); // for touch calculation

  }

private:
  void refreshTexture(){
    Texture* texture=this->texture;
    int width=window::width*coords.w;
    unsigned int fs=font::dpToPx(fontSize);
    if(!texture){
      texture=Texture::create(width,fs);
      if(!(texture)){
        this->texture=(Texture*)0;
        uiLogAndExit(EXIT_FAILURE,"CodeInput texture creation error");
      }
      this->texture=texture;
    }else if(texture->w<width || texture->h<fs){
      texture->resize(width,fs);
    }
    setFontSize(fontSize);
  }

public:

  void updateTexture(){
    char* text=this->text.pointer;
    int64_t tend=this->text.length;

    uint32_t iter=pageStart;
    uint32_t textBufferIter=0;
    uint32_t t32_1;
    uint32_t column=0;
    uint32_t tcolumn=this->column;
    // uint32_t curcolumn=0;
    char chr;
    GLint textBuffer[32];


    renderer::framebuffer::start(texture);
    renderer::clear(0.0,0.0,0.0,0.0);

    renderer::setTextTotalRowColumn(1,tcolumn);
    renderer::setTextRowColumnOffset(0,0);
    renderer::setTextRowColumnSize(1,tcolumn);
    renderer::setTextRenderingDimensions((vec4){.x=textureFrameCoords.x-xoffset,.y=0.0,.w=textureFrameCoords.w+gridSizeX,.h=1.0});
    renderer::setTextFontColor(0xffffffffu);
    renderer::setTextBackgroundColor(0);
    renderer::setTextCurrentRowColumn(0,0);

    renderer::setTextBitmap(NULL);

    while(iter<tend && column++<tcolumn){
      textBuffer[textBufferIter++]=text[iter++];
      if(textBufferIter&0x20){
        renderer::uploadText(textBuffer,0x20);
        renderer::drawInstanced(0x20);
        textBufferIter=0;
        renderer::setTextCurrentRowColumn(0,column);

      }
    }

    if(textBufferIter){
      renderer::uploadText(textBuffer,textBufferIter);
      renderer::drawInstanced(textBufferIter);
      column+=textBufferIter;
    }

    if(keyboard::controller==static_cast<Widget*>(this) ){
      printf("Rendering Cursor %f\n",xoffset);
      t32_1=cursor-pageStart;
      if(t32_1<tcolumn){
        RectangleDetails rd;
        rd.coords=(vec4){.x=(gridSizeX*t32_1)-xoffset,.y=0.0f,.w=0.2f*gridSizeX,.h=1.0f};
        rd.color=0xffffffffu;
        rd.borderThickness=0.0;
        rd.angle=0.0;
        rd.blur=0.0;
        rd.textureCoords=(vec4){0,0,0,0};
        rd.texture=(Texture*)0;
        renderer::renderRectangle(rd);
      }
    }

    renderer::framebuffer::stop();
  }

  void cursorRight(){
    if(cursor<text.length){
      ++cursor;
      int t32_1=cursor-pageStart;
      if(t32_1>0){
        if(t32_1>column){
          pageStart=cursor-column+2;
          xoffset=0;
        }else if(t32_1==column-1){
          xoffset=gridSizeX;
        }
      }else{
        pageStart=cursor;
        xoffset=0.0;
      }
    }
  }

  void cursorLeft(){
    if(cursor){
      --cursor;
      if(cursor<pageStart){
        pageStart=cursor;
        xoffset=0.0;
      }else if(cursor==pageStart && xoffset){
        xoffset=0.0;
      }
    }
  }

  void backspaceChar(){
    if(cursor){
      cursorLeft();
      text.deleteChar(cursor);
    }
  }

  void deleteChar(){
    if(cursor<text.length)
      text.deleteChar(cursor);
  }


    void homePressed(){
      cursor=0;
      pageStart=0;
      xoffset=0.0;
    }
    void endPressed(){
      cursor=text.length;
      int32_t diff=cursor-pageStart;
      if(diff>=column){
        pageStart=cursor-column+2;
      }else if(diff==column-1)
        xoffset=gridSizeX;
      else if(diff==0)
        xoffset=0.0;
    }


  void insertChar(char chr){
    text.insertChar(chr,cursor++);
    int32_t diff=cursor-pageStart;

    if(diff>=column){
      pageStart=(cursor-column)+2;
      xoffset=0.0;
    }else if(diff<=0){
      if(diff){
        pageStart=cursor;
      }
      xoffset=0.0;
    }else if(diff==column-1){
      xoffset=gridSizeX;
    }
  }

  Widget_Key(singleLineTextInput_key){
    SingleLineTextInput* st=static_cast<SingleLineTextInput*>(widget);
    unsigned int col=st->cursor-st->pageStart;
    if(col>=st->column){
      if(st->cursor>st->pageStart){
        st->pageStart=st->cursor-st->column+2;
      }else{
        st->pageStart=st->cursor;
      }
      st->xoffset=0.0;
    }
    if(key.text){
      printf("Heresss\n");
      st->insertChar(*(key.text));

      printf("checHeresss\n");
      st->updateTexture();
      printf("chec11Heresss\n");

      callback::render();

      if(st->onText)
        st->onText(st->pointer);

    }else{
      switch(key.keycode){
        case KEYPRESS_BACKSPACE:
          st->backspaceChar();
          st->updateTexture();
          callback::render();
          if(st->onText)
            st->onText(st->pointer);
          break;
        case KEYPRESS_RIGHT:
          st->cursorRight();
          st->updateTexture();
          callback::render();
          break;
        case KEYPRESS_LEFT:
          st->cursorLeft();
          st->updateTexture();
          callback::render();
          break;
        case KEYPRESS_DELETE:
          st->deleteChar();
          st->updateTexture();
          callback::render();
          if(st->onText)
            st->onText(st->pointer);
          break;
        case KEYPRESS_HOME:
          st->homePressed();
          st->updateTexture();
          callback::render();
          break;
        case KEYPRESS_END:
          st->endPressed();
          st->updateTexture();
          callback::render();
          break;
      }
    }
  }

  Widget_Motion(singleLineTextInput_motion){
    SingleLineTextInput* st=static_cast<SingleLineTextInput*>(widget);
    st->flags|=MODE_MOTION;
    if(touch.coords_rel.x<0 && st->pageStart+st->column>st->text.length)
      return;
    st->xoffset-=(touch.coords_rel.x*st->frameInv);

    if(st->xoffset<0.0){
      if(st->pageStart){
        do{
          --st->pageStart;
          st->xoffset+=st->gridSizeX;
        }while(st->pageStart && st->xoffset<0.0);
        if(st->xoffset<0.0 && !st->pageStart)
          st->xoffset=0.0;
      }else
        st->xoffset=0.0;
    }else if(st->xoffset>=st->gridSizeX){
      if(st->pageStart<st->text.length){
        do{
          st->xoffset-=st->gridSizeX;
          ++st->pageStart;
        }while(st->xoffset>=st->gridSizeX && st->pageStart<st->text.length);
      }else{
        st->xoffset=0.0;
      }
    }
    st->updateTexture();
    callback::render();
  }

  Widget_Touch(singleLineTextInput_touchDown){
    if(touch.flags&POINT_IN_WIDGET){
      SingleLineTextInput* st=static_cast<SingleLineTextInput*>(widget);
      st->flags|=MODE_DOWN;
      touch.motion.controller=widget;
      touch.motion.function=singleLineTextInput_motion;
      st->savedColumn=((touch.coords_int.x +(unsigned int)(st->xoffset*st->texture->w) - (int)((st->coords.x+(st->textureCoords.x*st->coords.w))*window::width))*st->frameInv)*st->column;

    }
    return 0;
  }

  Widget_Touch(singleLineTextInput_touchUp){
    SingleLineTextInput* st=static_cast<SingleLineTextInput*>(widget);

    if(st->flags&MODE_DOWN){
      st->flags^=MODE_DOWN;

      touch.motion.controller=(Widget*)0;
      touch.motion.function=NULL;

      if(st->flags&MODE_MOTION){
        st->flags^=MODE_MOTION;
      }else{

        st->cursor=st->pageStart+st->savedColumn;
        if(st->cursor>st->text.length){
          st->cursor=st->text.length;
        }

        keyboard::giveAccessToWidget(st,singleLineTextInput_key);

        st->updateTexture();
        callback::render();
      }
    }else if(keyboard::controller==widget){

      keyboard::removeAccessFromWidget(widget);
      st->updateTexture();
      callback::render();
    }
    return 0;
  }



  Widget_Resize(singleLineTextInput_resize){
    if(mode&(DIMENSION_W|DIMENSION_H)){
      SingleLineTextInput* st=static_cast<SingleLineTextInput*>(widget);

      st->refreshTexture();
      st->updateTexture();
    }
  }


  Widget_Render(singleLineTextInput_render){
    SingleLineTextInput* st=static_cast<SingleLineTextInput*>(widget);
    RectangleDetails rd={
      .coords=widget->coords,
      .textureCoords=st->textureCoords,
      .textureFrameCoords=(vec4){.x=0.0,.y=0.0,.w=1.0,.h=1.0},
      .texture=st->texture,
      .color=*color,
      .borderColor=*outline,
      .angle=st->angle,
      .borderThickness=st->borderThickness,
      .blur=st->blur,
      .textureColorMultiplier=color[1]
    };
    renderer::renderRectangle(rd);
  }

  SingleLineTextInput(unsigned int tfsize=GLOBAL_TEXT_SIZE){
    onResize=singleLineTextInput_resize;
    onTouchDown=singleLineTextInput_touchDown;
    onTouchUp=singleLineTextInput_touchUp;
    onRender=singleLineTextInput_render;
    fontSize=tfsize;
    flags=SETTING_RENDER|SETTING_TOUCH;
  }

  ~SingleLineTextInput(){
    Texture::destroy(texture);
    texture=NULL;
  }

  void resetText(){
    text.reset();
    cursor=0;
    updateTexture();
  }

};

#undef SINGLELINE_TEXTINPUT_CURSOR_IN_FOCUS

#endif

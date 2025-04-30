#ifndef __UI__WIDGET_BUTTON__
#define __UI__WIDGET_BUTTON__

// common for all button

#define ALIGN_TEXT_RIGHT 0x1
#define ALIGN_TEXT_CENTER  0x0
#define ALIGN_TEXT_LEFT 0x2
#define ALIGN_TEXT_MASK 0x3

// common for all button end

// button exclusive
#define BUTTON_SET_TEXT 0x4
#define BUTTON_ICON 0x8
#define BUTTON_TEXT 0x0
// button exclusive end

//icon text button exclusive
#define ALIGNMENT_ICON_TEXT 0x10
#define ALIGNMENT_TEXT_ICON 0x0
//icon text button exclusive end


// two faced button exclusive
#define TWO_FACED_BUTTONS_FIRST_ICON 0x4
#define TWO_FACED_BUTTONS_SECOND_ICON 0x8
#define FACE_2 0x10
#define FACE_SHIFT 4
// two faced button exclusive end

#define Button_Pressed(name) static void name(void* ptr)
#define Button_Released(name) static void name(void* ptr)

#define TwoFacedIconButton_Pressed(name) static void name(void* ptr,uint32_t mode)
#define TwoFacedIconButton_Released(name) static void name(void* ptr,uint32_t mode)


using ButtonPressed=void(*)(void*);
using ButtonReleased=void(*)(void*);

using TwoFacedIconButtonPressed=void(*)(void*,uint32_t);
using TwoFacedIconButtonReleased=void(*)(void*,uint32_t);

class TextButton : public Widget{
  private:
  
    Texture *texture=NULL;
    vec4 textureDimensions=(vec4){0};
    float padding=0.1;
    std::string text;

  public:

    unsigned int fontSize;
    uint32_t angle=4;
    uint32_t borderThickness=0;
    uint32_t blur=4;


    ButtonPressed onPressed=nullptr;
    ButtonPressed onReleased=nullptr;
    void* pointer=NULL;


  private:
    void alignText(){
      ReturnOnError(texture);
      int bwidth=coords.w*window::width,bheight=coords.h*window::height;
      float win=1.0/bwidth;
      float hin=1.0/bheight;
      textureDimensions.w=(float)(texture->w)*win;
      textureDimensions.h=(float)(texture->h)*hin;
      textureDimensions.y=((bheight-texture->h)>>1)*hin;
      if(flags&ALIGN_TEXT_RIGHT)
        textureDimensions.x=((bwidth-texture->w)*win)-padding;
      else if(flags&ALIGN_TEXT_LEFT)
        textureDimensions.x=padding;//(1.0-button->widget.textureVertex.w)*0.5;
      else
        textureDimensions.x=((bwidth-texture->w)>>1)*win;
    }


    int refreshText(){
      Texture::destroy(texture);
      int bwidth=coords.w*window::width,bheight=coords.h*window::height;
      texture=font::createTextTexture(NULL,text.c_str(),0xffffffff,bwidth,fontSize);
      if(!texture)
        return Error;
      float win=1.0/bwidth;
      float hin=1.0/bheight;
      textureDimensions.w=(float)(texture->w)*win;
      textureDimensions.h=(float)(texture->h)*hin;
      textureDimensions.y=((bheight-texture->h)>>1)*hin;
      if(flags&ALIGN_TEXT_RIGHT)
        textureDimensions.x=((bwidth-texture->w)*win)-padding;
      else if(flags&ALIGN_TEXT_LEFT)
        textureDimensions.x=padding;//(1.0-button->widget.textureVertex.w)*0.5;
      else
        textureDimensions.x=((bwidth-texture->w)>>1)*win;
      return Success;
    }

  public:
    void setText(std::string str){
      text=str;
      if(window::flags&UI_STARTED){
        refreshText();
        callback::render();
      }
    }

    Widget_Resize(on_resize){
      if(mode&(DIMENSION_W|DIMENSION_H)){
        TextButton* but=static_cast<TextButton*>(widget);
        if(but->texture)
          but->alignText();
        else if(!(but->refreshText()))
            return;
        callback::render();
      }
    }

    Widget_Render(on_render){
      TextButton* but=static_cast<TextButton*>(widget);
      RectangleDetails rd=(RectangleDetails){
        .coords=but->coords,
        .textureCoords=but->textureDimensions,
        .textureFrameCoords=(vec4){.x=0.0,.y=0.0,.w=1.0,.h=1.0},
        .texture=but->texture,
        .color=*color,
        .borderColor=color[1],
        .angle=but->angle,
        .borderThickness=but->borderThickness,
        .blur=but->blur,
        .textureColorMultiplier=color[1]
      };
      renderer::renderRectangle(rd);
    }

    Widget_Render(on_render_pressed){
      TextButton* but=static_cast<TextButton*>(widget);
      RectangleDetails rd=(RectangleDetails){
        .coords=but->coords,
        .textureCoords=but->textureDimensions,
        .textureFrameCoords=(vec4){.x=0.0,.y=0.0,.w=1.0,.h=1.0},
        .texture=but->texture,
        .color=*color,
        .borderColor=color[1],
        .angle=but->angle,
        .borderThickness=but->borderThickness,
        .blur=but->blur,
        .textureColorMultiplier=color[1]
      };
      renderer::renderRectangle(rd);

      renderer::setRectangleTextureDimensions(0.0,0.0,0.0,0.0);
      renderer::setRectangleColor((color[1]&(~0xff))|0x40);

      renderer::draw();
    }


    Widget_Touch(on_touch_down){
      if(touch.flags&POINT_IN_WIDGET){
        TextButton* but=static_cast<TextButton*>(widget);
        but->flags|=MODE_DOWN;
        but->setRenderFunc(on_render_pressed);
        if(but->onPressed)
          but->onPressed(but->pointer);
        callback::render();
      }
      return 0;
    }

    Widget_Touch(on_touch_up) {
      TextButton* but=static_cast<TextButton*>(widget);
      if(but->flags&MODE_DOWN){
        but->flags^=MODE_DOWN;
        but->setRenderFunc(on_render);
        if(but->onReleased)
          but->onReleased(but->pointer);
        callback::render();
      }
      return 0;
    }


    TextButton& setPointer(void* ptr){
      pointer=ptr;
      return *this;
    }

    TextButton& setOnPressedCallback(ButtonPressed func){
      onPressed=func;
      return *this;
    }

    TextButton& setOnReleasedCallback(ButtonPressed func){
      onReleased=func;
      return *this;
    }

    TextButton& alignTextRight(){
      flags=(flags&(~ALIGN_TEXT_MASK))^ALIGN_TEXT_RIGHT;
      return *this;
    }

    TextButton& alignTextLeft(){
      flags=(flags&(~ALIGN_TEXT_MASK))^ALIGN_TEXT_LEFT;
      return *this;
    }

    TextButton& alignTextCenter(){
      flags&=(~ALIGN_TEXT_MASK);
      return *this;
    }

    
    TextButton(std::string str,unsigned int fsize=GLOBAL_TEXT_SIZE){
      setText(str);
      setRenderFunc(on_render);
      setResizeFunc(on_resize);
      setTouchDownFunc(on_touch_down);
      setTouchUpFunc(on_touch_up);
      fontSize=fsize;

      setColor(themes::primary);
    }

    ~TextButton(){
      Texture::destroy(texture);
      texture=NULL;
    }
    
    void update(){
      if(window::flags&UI_STARTED){
        onResize(static_cast<Widget*>(this),DIMENSION_X|DIMENSION_Y|DIMENSION_W|DIMENSION_H);
      }
    }

};


class IconButton : public Widget{
  private:
  
    Texture *texture=NULL;
    vec4 textureDimensions=(vec4){0};
    float padding=0.1;
    unsigned int icon=0;

  public:

    unsigned int fontSize;
    uint32_t angle=20;
    uint32_t borderThickness=0;
    uint32_t blur=4;


    ButtonPressed onPressed=nullptr;
    ButtonPressed onReleased=nullptr;
    void* pointer=nullptr;


  private:

    void alignIcon(){
      ReturnOnError(texture);
      int bwidth=coords.w*window::width,bheight=coords.h*window::height;
      float win=1.0/bwidth;
      float hin=1.0/bheight;
      textureDimensions.w=(float)(texture->w)*win;
      textureDimensions.h=(float)(texture->h)*hin;
      textureDimensions.y=((bheight-texture->h)>>1)*hin;
      if(flags&ALIGN_TEXT_RIGHT)
        textureDimensions.x=((bwidth-texture->w)*win)-padding;
      else if(flags&ALIGN_TEXT_LEFT)
        textureDimensions.x=padding;//(1.0-button->widget.textureVertex.w)*0.5;
      else
        textureDimensions.x=((bwidth-texture->w)>>1)*win;
    }


    int refreshIcon(){
      Texture::destroy(texture);
      int bwidth=coords.w*window::width,bheight=coords.h*window::height;
      texture=font::createIconTexture(icon,fontSize,0xffffffffu);
      if(!texture)
        return Error;
      float win=1.0/bwidth;
      float hin=1.0/bheight;
      textureDimensions.w=(float)(texture->w)*win;
      textureDimensions.h=(float)(texture->h)*hin;
      textureDimensions.y=((bheight-texture->h)>>1)*hin;
      if(flags&ALIGN_TEXT_RIGHT)
        textureDimensions.x=((bwidth-texture->w)*win)-padding;
      else if(flags&ALIGN_TEXT_LEFT)
        textureDimensions.x=padding;//(1.0-button->widget.textureVertex.w)*0.5;
      else
        textureDimensions.x=((bwidth-texture->w)>>1)*win;
      return Success;
    }
  public:
    void setIcon(unsigned int icn){
      icon=icn;
      if(window::flags&UI_STARTED){
        refreshIcon();
        callback::render();
      }
    }
    
    Widget_Resize(on_resize){
      if(mode&(DIMENSION_W|DIMENSION_H)){
        IconButton* but=static_cast<IconButton*>(widget);
        if(but->texture)
          but->alignIcon();
        else if(!but->refreshIcon())
            return;
        callback::render();
      }
    }

    Widget_Render(on_render_pressed){
      IconButton* but=static_cast<IconButton*>(widget);
      RectangleDetails rd=(RectangleDetails){
        .coords=but->coords,
        .textureCoords=but->textureDimensions,
        .textureFrameCoords=(vec4){.x=0.0,.y=0.0,.w=1.0,.h=1.0},
        .texture=but->texture,
        .color=*color,
        .borderColor=color[1],
        .angle=but->angle,
        .borderThickness=but->borderThickness,
        .blur=but->blur,
        .textureColorMultiplier=color[1]
      };
      renderer::renderRectangle(rd);

      renderer::setRectangleTextureDimensions(0.0,0.0,0.0,0.0);
      renderer::setRectangleColor((color[1]&(~0xff))|0x40);

      renderer::draw();
    }


    Widget_Render(on_render) {
      IconButton* but=static_cast<IconButton*>(widget);
      RectangleDetails rd=(RectangleDetails){
        .coords=but->coords,
        .textureCoords=but->textureDimensions,
        .textureFrameCoords=(vec4){.x=0.0,.y=0.0,.w=1.0,.h=1.0},
        .texture=but->texture,
        .color=*color,
        .borderColor=color[1],
        .angle=but->angle,
        .borderThickness=but->borderThickness,
        .blur=but->blur,
        .textureColorMultiplier=color[1]
      };
      renderer::renderRectangle(rd);
    }
    
    Widget_Touch(on_touch_down){
      if(touch.flags&POINT_IN_WIDGET){
        IconButton* but=static_cast<IconButton*>(widget);
        but->flags|=MODE_DOWN;
        but->setRenderFunc(on_render_pressed);

        if(but->onPressed)
          but->onPressed(but->pointer);
        callback::render();
      }
      return 0;
    }

    Widget_Touch(on_touch_up) {
      IconButton* but=static_cast<IconButton*>(widget);
      if(but->flags&MODE_DOWN){
        but->flags^=MODE_DOWN;
        but->setRenderFunc(on_render);
        if(but->onReleased)
          but->onReleased(but->pointer);
        callback::render();
      }
      return 0;
    }

    IconButton& setPointer(void* ptr){
      pointer=ptr;
      return *this;
    }

    IconButton& setOnPressedCallback(ButtonPressed func){
      onPressed=func;
      return *this;
    }

    IconButton& setOnReleasedCallback(ButtonPressed func){
      onReleased=func;
      return *this;
    }

    IconButton& alignTextRight(){
      flags=(flags&(~ALIGN_TEXT_MASK))^ALIGN_TEXT_RIGHT;
      return *this;
    }

    IconButton& alignTextLeft(){
      flags=(flags&(~ALIGN_TEXT_MASK))^ALIGN_TEXT_LEFT;
      return *this;
    }

    IconButton& alignTextCenter(){
      flags&=(~ALIGN_TEXT_MASK);
      return *this;
    }



    IconButton(unsigned int icn,unsigned int fsize=GLOBAL_ICON_SIZE){
      setIcon(icn);
      setRenderFunc(on_render);
      setResizeFunc(on_resize);

      setTouchDownFunc(on_touch_down);
      setTouchUpFunc(on_touch_up);
      fontSize=fsize;

      setColor(themes::primary);

    }

    ~IconButton(){
      Texture::destroy(texture);
      texture=nullptr;
    }
    
    void update(){
      if(window::flags&UI_STARTED){
        onResize(static_cast<Widget*>(this),DIMENSION_X|DIMENSION_Y|DIMENSION_W|DIMENSION_H);
      }
    }

};


class IconTextButton : public Widget{
  private:
    Texture *iconTexture=nullptr;
    Texture *textTexture=nullptr;
    vec4 textDimensions=(vec4){0};
    vec4 iconDimensions=(vec4){0};
    float padding=0.1;
    std::string text;
    unsigned int icon=0;

  public:
    unsigned int iconFontSize;
    unsigned int textFontSize;

    uint32_t angle=20;
    uint32_t borderThickness=0;
    uint32_t blur=4;


    ButtonPressed onPressed=nullptr;
    ButtonPressed onReleased=nullptr;
    void* pointer=nullptr;

  private:

    void alignText(){
      int bwidth=coords.w*window::width;
      int bheight=coords.h*window::height;
      if(!textTexture){
        textTexture=font::createTextTexture(NULL,text.c_str(),0xffffffff,0.8*bwidth,textFontSize);
        if(!textTexture)
          return ;
      }
      
      float win=1.0/bwidth;
      float hin=1.0/bheight;

      textDimensions.w=textTexture->w*win;
      textDimensions.h=textTexture->h*hin;
      textDimensions.y=((bheight-textTexture->h)>>1)*hin;
      
      if(flags&ALIGN_TEXT_RIGHT){
        textDimensions.x=0.8-(textDimensions.w+0.1);
      }else if(flags&ALIGN_TEXT_LEFT){
        textDimensions.x=0.1;
      }else{
        textDimensions.x=(0.8-textDimensions.w)*0.5;
      }
      
      
      if(flags&ALIGNMENT_ICON_TEXT)
        textDimensions.x+=0.2;
    }


    void alignIcon(){
      int bwidth=coords.w*window::width;
      int bheight=coords.h*window::height;
      if(!iconTexture){
        iconTexture=font::createIconTexture(icon,iconFontSize,0xffffffff);
        ReturnOnError(iconTexture);
      }
      
      float win=1.0/bwidth;
      float hin=1.0/bheight;

      iconDimensions.w=iconTexture->w*win;
      iconDimensions.h=iconTexture->h*hin;
      iconDimensions.y=((bheight-iconTexture->h)>>1)*hin;
      iconDimensions.x=(0.2-iconDimensions.w)*0.5; //0.2=icon size

      if(!(flags&ALIGNMENT_ICON_TEXT)){
        iconDimensions.x+=0.8;
      }
    }

    void alignAll(){
      int bwidth=coords.w*window::width;
      int bheight=coords.h*window::height;
      if(!iconTexture){
        iconTexture=font::createIconTexture(icon,iconFontSize,0xffffffff);
        if(!iconTexture)
          uiLogAndExit(EXIT_FAILURE,"Icon text button , Icon creation error");
      }
      if(!textTexture){
        textTexture=font::createTextTexture(NULL,text.c_str(),0xffffffff,0.8*bwidth,textFontSize);
        if(!textTexture)
          uiLogAndExit(EXIT_FAILURE,"Icon text button , Text creation error");
      }
      
      float win=1.0/bwidth;
      float hin=1.0/bheight;

      iconDimensions.w=iconTexture->w*win;
      iconDimensions.h=iconTexture->h*hin;
      iconDimensions.y=((bheight-iconTexture->h)>>1)*hin;
      iconDimensions.x=(0.2-iconDimensions.w)*0.5; //0.2=icon size

      textDimensions.w=textTexture->w*win;
      textDimensions.h=textTexture->h*hin;
      textDimensions.y=((bheight-textTexture->h)>>1)*hin;
      
      if(flags&ALIGN_TEXT_RIGHT){
        textDimensions.x=0.8-(textDimensions.w+0.1);
      }else if(flags&ALIGN_TEXT_LEFT){
        textDimensions.x=0.1;
      }else{
        textDimensions.x=(0.8-textDimensions.w)*0.5;
      }
      if(flags&ALIGNMENT_ICON_TEXT)
        textDimensions.x+=0.2;
      else
        iconDimensions.x+=0.8;
    }

  public:
    void refreshTextures(){
      Texture::destroy(textTexture);
      Texture::destroy(iconTexture);
      textTexture=NULL;
      iconTexture=NULL;
      alignAll();
    }

    void refreshText(){
      Texture::destroy(textTexture);
      textTexture=NULL;
      alignAll();
    }
    void refreshIcon(){
      Texture::destroy(iconTexture);
      iconTexture=NULL;
      alignAll();
    }


    void setText(std::string str){
      text=str;
      if(window::flags&UI_STARTED){
        refreshText();
        callback::render();
      }
    }
    
    void setIcon(unsigned int icn){
      icon=icn;
      if(window::flags&UI_STARTED){
        refreshIcon();
        callback::render();
      }
    }
    
    Widget_Resize(on_resize){
      if(mode&(DIMENSION_W|DIMENSION_H)){
        IconTextButton* but=static_cast<IconTextButton*>(widget);
        but->alignAll();
        callback::render();
      }
    }

    Widget_Render(on_render){
      IconTextButton* but=static_cast<IconTextButton*>(widget);
      RectangleDetails rd=(RectangleDetails){
        .coords=but->coords,
        .textureCoords=but->textDimensions,
        .textureFrameCoords=(vec4){.x=0.0,.y=0.0,.w=1.0,.h=1.0},
        .texture=but->textTexture,
        .color=*color,
        .borderColor=color[1],
        .angle=but->angle,
        .borderThickness=but->borderThickness,
        .blur=but->blur,
        .textureColorMultiplier=color[1]
      };
      renderer::renderRectangle(rd);
      
      rd.textureCoords=but->iconDimensions;
      rd.texture=but->iconTexture;
      rd.color=0;
      rd.textureColorMultiplier=color[1];
      rd.borderThickness=0.0;
      renderer::renderRectangle(rd);
    }
    Widget_Render(on_render_pressed){
      IconTextButton* but=static_cast<IconTextButton*>(widget);
      RectangleDetails rd=(RectangleDetails){
        .coords=but->coords,
        .textureCoords=but->textDimensions,
        .textureFrameCoords=(vec4){.x=0.0,.y=0.0,.w=1.0,.h=1.0},
        .texture=but->textTexture,
        .color=*color,
        .borderColor=color[1],
        .angle=but->angle,
        .borderThickness=but->borderThickness,
        .blur=but->blur,
        .textureColorMultiplier=color[1]
      };
      renderer::renderRectangle(rd);

      rd.textureCoords=but->iconDimensions;
      rd.texture=but->iconTexture;
      rd.color=(color[1]&(~0xff))|0x20;
      rd.textureColorMultiplier=color[1];
      rd.borderThickness=0.0;
      renderer::renderRectangle(rd);


      renderer::draw();
    }


    Widget_Touch(on_touch_down){
      if(touch.flags&POINT_IN_WIDGET){
        IconTextButton* but=static_cast<IconTextButton*>(widget);
        but->flags|=MODE_DOWN;
        but->setRenderFunc(on_render_pressed);
        if(but->onPressed){
          but->onPressed(but->pointer);
        }
        callback::render();
      }
      return 0;
    }

    Widget_Touch(on_touch_up) {
      IconTextButton* but=static_cast<IconTextButton*>(widget);
      if(but->flags&MODE_DOWN){
        but->flags^=MODE_DOWN;
        but->setRenderFunc(on_render);
        if(but->onReleased)
          but->onReleased(but->pointer);
        callback::render();
      }
      return 0;
    }


    IconTextButton& setPointer(void* ptr){
      pointer=ptr;
      return *this;
    }

    IconTextButton& setOnPressedCallback(ButtonPressed func){
      onPressed=func;
      return *this;
    }

    IconTextButton& setOnReleasedCallback(ButtonPressed func){
      onReleased=func;
      return *this;
    }

    IconTextButton& alignTextRight(){
      flags=(flags&(~ALIGN_TEXT_MASK))^ALIGN_TEXT_RIGHT;
      return *this;
    }

    IconTextButton& alignTextLeft(){
      flags=(flags&(~ALIGN_TEXT_MASK))^ALIGN_TEXT_LEFT;
      return *this;
    }

    IconTextButton& alignTextCenter(){
      flags&=(~ALIGN_TEXT_MASK);
      return *this;
    }

    IconTextButton& alignIconText(){
      flags|=ALIGNMENT_ICON_TEXT;
      return *this;
    }

    IconTextButton& alignTextIcon(){
      flags&=(~ALIGNMENT_ICON_TEXT);
      return *this;
    }

    
    IconTextButton(unsigned int icn,std::string str,unsigned int ifsize=GLOBAL_ICON_SIZE,unsigned int tfsize=GLOBAL_TEXT_SIZE){
      text=str;
      icon=icn;
      iconFontSize=ifsize;
      textFontSize=tfsize;
      setRenderFunc(on_render);
      setResizeFunc(on_resize);
      setTouchDownFunc(on_touch_down);
      setTouchUpFunc(on_touch_up);
      setColor(themes::primary);

    }

    ~IconTextButton(){
      Texture::destroy(textTexture);
      Texture::destroy(iconTexture);

      textTexture=NULL;
      iconTexture=NULL;
    }
   
    void update(){
      if(window::flags&UI_STARTED){
        onResize(static_cast<Widget*>(this),DIMENSION_X|DIMENSION_Y|DIMENSION_W|DIMENSION_H);
      }
    }
 
};


class TwoFacedIconButton : public Widget{
public:
  void* pointer=NULL;
  Texture* texture[2]={};
  TwoFacedIconButtonPressed onPressed=nullptr;
  TwoFacedIconButtonReleased onReleased=nullptr;
  unsigned int first;
  unsigned int second;
  vec4 textureCoords[2];
  unsigned int fontSize[2];
  uint32_t angle[2]={20,20};
  uint32_t borderThickness[2]={};
  uint32_t blur[2]={};

private:

  void alignFirst(){
    Texture** texture=this->texture,*ttex;
    int width=coords.w*window::width;
    int height=coords.h*window::height;
    if(!texture[0]){
      texture[0]=font::createIconTexture(first,fontSize[0],0xffffffff);
      if(!texture[0])
        std::exit(EXIT_FAILURE);
    }

    // button->textureCoords[0]=(vec4){width-texture[0]->w)}

    ttex=texture[0];

    float win=1.0/width;
    float hin=1.0/height;

    textureCoords[0].w=(float)(ttex->w)*win;
    textureCoords[0].h=(float)(ttex->h)*hin;
    textureCoords[0].y=((height-ttex->h)>>1)*hin;


    if(flags&ALIGN_TEXT_RIGHT){
      textureCoords[0].x=((width-ttex->w)*win)-0.1f;
    }else if(flags&ALIGN_TEXT_LEFT){
      textureCoords[0].x=0.1f;
    }else{
      textureCoords[0].x=((width-ttex->w)>>1)*win;
    }

  }

  void alignSecond(){
    Texture** texture=this->texture,*ttex1;
    int width=coords.w*window::width;
    int height=coords.h*window::height;

    if(!texture[1]){
      texture[1]=font::createIconTexture(second,fontSize[1],0xffffffff);
      if(!texture[1])
        std::exit(EXIT_FAILURE);
    }

    // button->textureCoords[0]=(vec4){width-texture[0]->w)}

    ttex1=texture[1];

    float win=1.0/width;
    float hin=1.0/height;


    textureCoords[1].w=(float)(ttex1->w)*win;
    textureCoords[1].h=(float)(ttex1->h)*hin;
    textureCoords[1].y=((height-ttex1->h)>>1)*hin;

    if(flags&ALIGN_TEXT_RIGHT){
      textureCoords[1].x=((width-ttex1->w)*win)-0.1f;
    }else if(flags&ALIGN_TEXT_LEFT){
      textureCoords[1].x=0.1f;
    }else{
      textureCoords[1].x=((width-ttex1->w)>>1)*win;
    }

  }


  void alignTextures(){
    Texture** texture=this->texture,*ttex,*ttex1;
    int width=coords.w*window::width;
    int height=coords.h*window::height;
    if(!texture[0]){
      texture[0]=font::createIconTexture(first,fontSize[0],0xffffffff);
      if(!texture[0])
        std::exit(EXIT_FAILURE);
    }
    if(!texture[1]){
      texture[1]=font::createIconTexture(second,fontSize[1],0xffffffff);
      if(!texture[1])
        std::exit(EXIT_FAILURE);
    }

    // button->textureCoords[0]=(vec4){width-texture[0]->w)}

    ttex=texture[0];
    ttex1=texture[1];

    float win=1.0/width;
    float hin=1.0/height;

    textureCoords[0].w=(float)(ttex->w)*win;
    textureCoords[0].h=(float)(ttex->h)*hin;
    textureCoords[0].y=((height-ttex->h)>>1)*hin;

    textureCoords[1].w=(float)(ttex1->w)*win;
    textureCoords[1].h=(float)(ttex1->h)*hin;
    textureCoords[1].y=((height-ttex1->h)>>1)*hin;

    if(flags&ALIGN_TEXT_RIGHT){
      textureCoords[0].x=((width-ttex->w)*win)-0.1f;
      textureCoords[1].x=((width-ttex1->w)*win)-0.1f;
    }else if(flags&ALIGN_TEXT_LEFT){
      textureCoords[0].x=0.1f;
      textureCoords[1].x=0.1f;
    }else{
      textureCoords[0].x=((width-ttex->w)>>1)*win;
      textureCoords[1].x=((width-ttex1->w)>>1)*win;
    }

  }

public:


  Widget_Render(twoFacedButton_render){
    TwoFacedIconButton* button=static_cast<TwoFacedIconButton*>(widget);
    unsigned int bit=(button->flags>>FACE_SHIFT)&1;
    RectangleDetails rd={
      .coords=button->coords,
      .textureCoords=button->textureCoords[bit],
      .textureFrameCoords=(vec4){.x=0.0,.y=0.0,.w=1.0,.h=1.0},
      .texture=button->texture[bit],
      .color=*color,
      .borderColor=color[1],
      .angle=button->angle[bit],
      .borderThickness=button->borderThickness[bit],
      .blur=button->blur[bit],
      .textureColorMultiplier=color[1]//button->fontColor[bit]
    };
    renderer::renderRectangle(rd);
  }

  Widget_Render(twoFacedButton_render_pressed){
    TwoFacedIconButton* button=static_cast<TwoFacedIconButton*>(widget);
    unsigned int bit=(button->flags>>FACE_SHIFT)&1;
    RectangleDetails rd={
      .coords=button->coords,
      .textureCoords=button->textureCoords[bit],
      .textureFrameCoords=(vec4){.x=0.0,.y=0.0,.w=1.0,.h=1.0},
      .texture=button->texture[bit],
      .color=*color,
      .borderColor=color[1],
      .angle=button->angle[bit],
      .borderThickness=button->borderThickness[bit],
      .blur=button->blur[bit],
      .textureColorMultiplier=color[1]//button->fontColor[bit]
    };
    renderer::renderRectangle(rd);

    renderer::setRectangleTextureDimensions(0.0,0.0,0.0,0.0);
    renderer::setRectangleColor((color[1]&(~0xff))|0x20);

    renderer::draw();
  }

  Widget_Touch(twoFacedIconButton_touchDown){
    if(touch.flags&POINT_IN_WIDGET){
      TwoFacedIconButton* but=static_cast<TwoFacedIconButton*>(widget);
      unsigned int bit;
      if(!(but->flags&MODE_DOWN)){
        bit=(but->flags>>FACE_SHIFT)&1;
        but->flags|=MODE_DOWN;
        but->setRenderFunc(twoFacedButton_render_pressed);
        if(but->onPressed)
          but->onPressed(but->pointer,bit);
      }else{
        bit=((but->flags>>FACE_SHIFT)&1)^1;
        but->flags^=(MODE_DOWN|FACE_2);
        but->setRenderFunc(twoFacedButton_render);
        if(but->onReleased)
          but->onReleased(but->pointer,bit);
      }
      callback::render();
    }
    return 0;
  }

  Widget_Touch(twoFacedIconButton_touchUp){
    TwoFacedIconButton* but=static_cast<TwoFacedIconButton*>(widget);
    if(but->flags&MODE_DOWN){
      unsigned int bit=(but->flags>>FACE_SHIFT)&1;
      but->flags^=(MODE_DOWN|FACE_2);
      but->setRenderFunc(twoFacedButton_render);
      if(but->onReleased){
        but->onReleased(but->pointer,bit^1);
      }

      callback::render();
    }
    return 0;
  }


  Widget_Resize(twoFacedButton_resized){
    if(mode&(DIMENSION_W^DIMENSION_H)){
      (static_cast<TwoFacedIconButton*>(widget))->alignTextures();
    }
  }

  ~TwoFacedIconButton(){
    Texture::destroy(texture[0]);
    Texture::destroy(texture[1]);

    texture[0]=NULL;
    texture[1]=NULL;
  }

  TwoFacedIconButton& setPointer(void* ptr){
    pointer=ptr;
    return *this;
  }

  TwoFacedIconButton& setOnPressedCallback(TwoFacedIconButtonPressed func){
    onPressed=func;
    return *this;
  }
  TwoFacedIconButton& setOnReleasedCallback(TwoFacedIconButtonReleased func){
    onReleased=func;
    return *this;
  }

  TwoFacedIconButton& alignTextRight(){
    flags=(flags&(~ALIGN_TEXT_MASK))^ALIGN_TEXT_RIGHT;
    return *this;
  }

  TwoFacedIconButton& alignTextLeft(){
    flags=(flags&(~ALIGN_TEXT_MASK))^ALIGN_TEXT_LEFT;
    return *this;
  }

  TwoFacedIconButton& alignTextCenter(){
    flags&=(~ALIGN_TEXT_MASK);
    return *this;
  }



  TwoFacedIconButton(unsigned int ficon,unsigned int sicon,unsigned int ffsize=GLOBAL_ICON_SIZE,unsigned int sfsize=GLOBAL_ICON_SIZE){
    onTouchDown=twoFacedIconButton_touchDown;
    onTouchUp=twoFacedIconButton_touchUp;
    onResize=twoFacedButton_resized;
    onRender=twoFacedButton_render;
    fontSize[0]=ffsize;
    fontSize[1]=sfsize;
    first=ficon;
    second=sicon;
    setColor(themes::primary);
  }

  void setFace(unsigned int face){
    flags=((flags&(~FACE_2))|(face&1));
  }


  void setIcon(unsigned int icon,int mode){
    if(mode){
      second=icon;
      Texture::destroy(texture[1]);
      texture[1]=NULL;
      flags|=TWO_FACED_BUTTONS_SECOND_ICON;
    }else{
      first=icon;
      Texture::destroy(texture[0]);
      texture[0]=NULL;
      flags|=TWO_FACED_BUTTONS_FIRST_ICON;
    }
  }

  void update(){
    if(window::flags&UI_STARTED){
      alignTextures();
    }
  }


};

#endif

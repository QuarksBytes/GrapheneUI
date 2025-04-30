#ifndef __UI__WIDGET_CONTAINER__
#define __UI__WIDGET_CONTAINER__

#include"Core/Widget.cpp"

class Container : public Widget{
  public:
    ivec4 backgroundColor=(ivec4){.r=0xdfdfdfff,.g=0xdfdfdfff,.b=0xdfdfdfff,.a=0xdfdfdfff};
    ivec4 borderColor;
    uint32_t angle=0.2;
    uint32_t borderThickness=0;
    uint32_t blur=0;

    Widget_Render(on_render){
      Container* ctn=static_cast<Container*>(widget);

      RectangleDetails rd=(RectangleDetails){
        .coords=ctn->coords,
        .textureCoords=(vec4){0},
        .textureFrameCoords=(vec4){0},
        .texture=nullptr,
        .color=*color,
        .borderColor=*outline,
        .angle=ctn->angle,
        .borderThickness=ctn->borderThickness,
        .blur=ctn->blur
      };
      renderer::renderRectangle(&rd);
    }

    Container(){
      onRender=on_render;
      flags=SETTING_RENDER|SETTING_TOUCH;
      setColor(themes::primary);
    }
    
};

#endif

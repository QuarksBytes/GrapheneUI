#ifndef __UI__WIDGET_POPUPWIDGET__
#define __UI__WIDGET_POPUPWIDGET__

#define POPUP_DISMISS_ON_EXTERNAL_TOUCH 1

class Popup : public Widget{
public:
  BasicDrawInfo;

  Widget_Render(popup_render){
    Popup* dd=static_cast<Popup*>(widget);
    RectangleDetails rd={
      .coords=dd->coords,
      .textureCoords=(vec4){0},
      .textureFrameCoords=(vec4){.x=0.0,.y=0.0,.w=1.0,.h=1.0},
      .color=*color,
      .borderColor=*outline,
      .angle=dd->angle,
      .borderThickness=dd->borderThickness,
      .blur=dd->blur
    };
    renderer::renderShadow(rd,dd->flags);
    renderer::renderRectangle(rd);
  }

  Widget_Touch(popup_touchDown){
    if(!(touch.flags&(POINT_IN_WIDGET|POINT_ALSO_IN_WIDGET)) && (widget->flags&(SETTING_RENDER|POPUP_DISMISS_ON_EXTERNAL_TOUCH))==(SETTING_RENDER|POPUP_DISMISS_ON_EXTERNAL_TOUCH)){
      widget->flags|=MODE_DOWN;
    }
    return 1;
  }

  Widget_Touch(popup_touchUp){
    if(widget->flags&MODE_DOWN){
      widget->flags&=(~(SETTING_RENDER|SETTING_TOUCH|MODE_DOWN));
      callback::render();
      return 1;
    }
    return 0;
  }

  Popup(){
    onTouchDown=popup_touchDown;
    onTouchUp=popup_touchUp;
    onRender=popup_render;
    angle=20;
    flags|=SETTING_SHADOW_ENABLE;
    flags&=(~(SETTING_RENDER|SETTING_TOUCH));
  }

  Popup& dismissOnExternalTouch(){
    flags|=POPUP_DISMISS_ON_EXTERNAL_TOUCH;
    return *this;
  }

  Popup& noDismissOnExternalTouch(){
    flags&=(~POPUP_DISMISS_ON_EXTERNAL_TOUCH);
    return *this;
  }

};
#endif

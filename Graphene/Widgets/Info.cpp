#ifndef __UI__WIDGET_INFO__
#define __UI__WIDGET_INFO__

#include"Widgets/Button.cpp"
#include"Widgets/Popup.cpp"
#include"Widgets/Label.cpp"

#define Info_Callback(name) Button_Released(name)

class Info : public Popup{
private:
  Label text;
public:
  IconButton close;
  IconButton ok;


  Info& setText(const std::string& ttext){
    text.reset();
    text.text.setText(ttext);
    text.updateTexture();
    return *this;
  }


  Info():
    close(CLOSE_THICK),
    ok(CHECK_BOLD)
  {
    add(&close);
    add(&ok);
    add(&text);

    close.setColor(themes::surface).setXY(0.0,0.0).setWH(0.5,0.1);
    ok.setColor(themes::surface).setXY(0.5,0.0).setWH(0.5,0.1);
    text.setXY(0.0,0.1).setWH(1.0,0.9);

  }
};

#endif

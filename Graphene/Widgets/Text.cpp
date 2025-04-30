#ifndef __UI_CORE_TEXT__
#define __UI_CORE_TEXT__



/*
 *
 * Use this only for const text which won't be changing and which dosn't contain things like new Lines
 *
 *
 * Incomplete ::
 *
 */
class Text : public Widget{
private:
  Texture texture=nullptr;
  std::string text;

  void refreshText(){
    if(!text.length()){
      return;
    }

    if(texture){
      Texture::destroy(texture);
      texture=nullptr;
    }

    texture=font::createTextTexture(text);
  }

public:

  void setText(const std::string& str){
    text=str;
    if(window::started()){
      refreshText();
    }
  }

  Text(const std::string& text){
    setText(text);
  }

  ~Text(){
    if(texture){
      Texture::destroy(texture);
      texture=nullptr;
    }
  }
}

#endif

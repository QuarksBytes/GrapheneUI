#ifndef __UI__WIDGET_FILEMANAGER__
#define __UI__WIDGET_FILEMANAGER__

#include<dirent.h>

#include "Lib/FileOperations.cpp"
#include"Lib/BitOperations.cpp"

#include"Widgets/Button.cpp"
#include"Widgets/RecycleView.cpp"
#include"Widgets/Popup.cpp"

#include"Widgets/SingleLineTextInput.cpp"
#include"Widgets/Label.cpp"


enum{
  _DOTS=0,
  _FOLDER,
  _FILE,
  _ZIP,
  _VIDEO,
  _AUDIO,
  _PDF,
  _CSS,
  _CPP,
  _C,
  _PY,
  _JS,
  _HTML,
  _IMAGE,
  _R,
  _RUST,
  _KOTLIN,
  _CSHARP,
  _GO,
  _HASKELL,
  _RUBY,
  _LUA,
  _PHP,
  _ERROR,
  TOTAL_FILEMANAGER_ICONS
};



#define DEFAULT_TEXT_FONT_SIZE 20
#define DEFAULT_ICON_FONT_SIZE 25

#define FILEMANAGER_DEFAULT_TEXT_COLOR 0x347270
#define FILEMANAGER_DEFAULT_ICON_COLOR 0x1f7a77
#define FILEMANAGER_DEFAULT_BACKGROUND_COLOR 0x145250ff
#define FILEMANAGER_DEFAULT_BOX_BACKGROUND_COLOR 0xd6f5f4ff


enum{
  FILEMANAGER_POPUP_SCREEN_CREATE=0,
  FILEMANAGER_POPUP_SCREEN_PROPERTIES,
  FILEMANAGER_TOTAL_SCREENS
};


typedef struct {
  uint64_t bits_1;
  uint32_t bits_2;
  uint32_t length;
  uint32_t index;
  uint32_t type;
}FileManagerBuffer;

typedef struct{
  unsigned int index;
  unsigned int bitsUnmatched;
}FileManagerSearchBuffer;

#define FileManager_onFileOpen(name) static void name(void* ptr,const char* const path)
#define FileManager_onClose(name) Button_Released(name)

class FileManager: public Widget{
private:
  Texture* iconTextures[TOTAL_FILEMANAGER_ICONS]={};
  vec4 iconCoords[TOTAL_FILEMANAGER_ICONS]={};
  vec4 textCoords={};
  struct{
    lib::string data;
    lib::vector<FileManagerBuffer> fmb;
  }dirData;

  struct{
    std::vector<FileManagerSearchBuffer> fsb;
  }searchData;
  std::string path;
  unsigned int rootPathLen=0;

  int textColumn;
public:
  RecycleView recycleView;

  void (*onFileOpen)(void* ,const char* const )=NULL;
  void* pointer=NULL;

  unsigned int textFontSize=GLOBAL_TEXT_SIZE;
  unsigned int iconFontSize=GLOBAL_ICON_SIZE;

  unsigned int iconFontColor=0xff;
  unsigned int textFontColor=0xff;

  struct{
    BasicDrawInfo;
  }button;

  BasicDrawInfo;

  Container topBar;

  IconButton closeButton;

  IconButton backButton;
  IconButton refreshButton;
  IconButton newFolderButton;
  IconButton newFileButton;
  TwoFacedIconButton searchButton;

  Popup popup;
  Screen popupScreen;
  IconButton popupCloseButton;

  SingleLineTextInput searchBar;
  Label properties;

private:
  void setIconsCoords(){ // done perfectly
    if(!recycleView.texture)
      return ;

    vec4* coord;
    Texture* texture;
    uint32_t iter;//=fm->scrollLayout.widget.texture->w;

    const float iconSize=0.1;

    float winverse;
    float hinverse;
    vec2 size=recycleView.getSize();
    int width=size.w*recycleView.texture->w;
    int height=size.h*recycleView.texture->h;
    int iw;

    iw=iconSize*width;
    winverse=1.0/(float)(width);
    hinverse=1.0/(float)(height);

    vec4* iconCoord=iconCoords;
    Texture** iconTexture=iconTextures;

    texture=iconTexture[_DOTS];
    iconCoord[_DOTS].w=(float)(texture->w)*winverse;
    iconCoord[_DOTS].h=(float)(texture->h)*hinverse;
    iconCoord[_DOTS].x=(width-((iw+texture->w)>>1))*winverse;
    iconCoord[_DOTS].y=((height-texture->h)>>1)*hinverse;




    iter=1;
    while(iter<TOTAL_FILEMANAGER_ICONS){
      texture=iconTexture[iter];
      coord=iconCoord+(iter++);
      if(texture){
        coord->w=(float)(texture->w)*winverse;
        coord->h=(float)(texture->h)*hinverse;
        coord->x=((iw-texture->w)>>1)*winverse;
        coord->y=((height-texture->h)>>1)*hinverse;
      }
    }
  }

  bool refreshTextCoords(){
    if(!textFontSize){
      textFontSize=GLOBAL_TEXT_SIZE;
    }
    if(!recycleView.texture)
      return false;

    int h=font::dpToPx(textFontSize);
    int w=font::bitmap->whRatio*h;
    vec2 size=recycleView.getSize();
    int width=size.w*recycleView.texture->w;
    int height=size.h*recycleView.texture->h;

    float winv=1.0f/recycleView.texture->w;
    float hinv=1.0f/recycleView.texture->h;

    int t32_1=((int)(textColumn=(0.8f*width)/w))*w;// 0.8 = text width

    textCoords.w=t32_1*winv;
    textCoords.h=h*hinv;

    textCoords.x=0.1;
    textCoords.y=((height-h)>>1)*hinv;
    return 1;
  }

  void destroyAllTextures(){
    Texture** textureBuffer=iconTextures;
    Texture** textureBufferEnd=textureBuffer+TOTAL_FILEMANAGER_ICONS;

    while(textureBuffer<textureBufferEnd){
      Texture::destroy(*textureBuffer);
      *(textureBuffer++)=NULL;
    }

  }

  bool createAllTextures(){
  printf("Size of textue %d\n",sizeof(Texture));
    Texture** textureBuffer=iconTextures;

    textureBuffer[_DOTS]=font::createIconTexture(DOTS_HORIZONTAL,iconFontSize,0xffffffu);
    if(!textureBuffer[_DOTS])
      goto __fileManagerCreateIcons_return;

    textureBuffer[_FOLDER]=font::createIconTexture(FOLDER,iconFontSize,0xffffffu);
    if(!textureBuffer[_FOLDER])
      goto __fileManagerCreateIcons_deleteDotsTexture;


    textureBuffer[_FILE]=font::createIconTexture(FILE_ICON,iconFontSize,0xffffffu);
    if(!textureBuffer[_FILE])
      goto __fileManagerCreateIcons_deleteFolderTexture;

    textureBuffer[_ZIP]=font::createIconTexture(FOLDER_ZIP,iconFontSize,0xffffffu);
    if(!textureBuffer[_ZIP])
      goto __fileManagerCreateIcons_deleteFileTexture;


    textureBuffer[_VIDEO]=font::createIconTexture(FILE_VIDEO,iconFontSize,0xffffffffu);
    if(!textureBuffer[_VIDEO])
      goto __fileManagerCreateIcons_deleteZipTexture;

    textureBuffer[_AUDIO]=font::createIconTexture(FILE_MUSIC,iconFontSize,0xffffffffu);
    if(!textureBuffer[_AUDIO])
      goto __fileManagerCreateIcons_deleteVideoTexture;

    textureBuffer[_PDF]=font::createIconTexture(FILE_PDF,iconFontSize,0xffffffffu);
    if(!textureBuffer[_PDF])
      goto __fileManagerCreateIcons_deleteAudioTexture;


    textureBuffer[_CSS]=font::createIconTexture(LANGUAGE_CSS3,iconFontSize,0xffffffffu);
    if(!textureBuffer[_CSS])
      goto __fileManagerCreateIcons_deletePdfTexture;


    textureBuffer[_CPP]=font::createIconTexture(LANGUAGE_CPP,iconFontSize,0xffffffffu);
    if(!textureBuffer[_CPP])
      goto __fileManagerCreateIcons_deleteCssTexture;


    textureBuffer[_C]=font::createIconTexture(LANGUAGE_C,iconFontSize,0xffffffffu);
    if(!textureBuffer[_C])
      goto __fileManagerCreateIcons_deleteCppTexture;

    textureBuffer[_PY]=font::createIconTexture(LANGUAGE_PYTHON,iconFontSize,0xffffffffu);
    if(!textureBuffer[_PY])
      goto __fileManagerCreateIcons_deleteCTexture;

    textureBuffer[_JS]=font::createIconTexture(LANGUAGE_JAVASCRIPT,iconFontSize,0xffffffffu);
    if(!textureBuffer[_JS])
      goto __fileManagerCreateIcons_deletePyTexture;

    textureBuffer[_HTML]=font::createIconTexture(LANGUAGE_HTML5,iconFontSize,0xffffffffu);
    if(!textureBuffer[_HTML])
      goto __fileManagerCreateIcons_deleteJsTexture;

    textureBuffer[_IMAGE]=font::createIconTexture(IMAGE,iconFontSize,0xffffffffu);
    if(!textureBuffer[_IMAGE])
      goto __fileManagerCreateIcons_deleteHtmlTexture;


    textureBuffer[_R]=font::createIconTexture(LANGUAGE_R,iconFontSize,0xffffffffu);
    if(!textureBuffer[_R])
      goto __fileManagerCreateIcons_deleteImageTexture;

    textureBuffer[_RUST]=font::createIconTexture(LANGUAGE_RUST,iconFontSize,0xffffffffu);
    if(!textureBuffer[_RUST])
      goto __fileManagerCreateIcons_deleteRTexture;

    textureBuffer[_KOTLIN]=font::createIconTexture(LANGUAGE_KOTLIN,iconFontSize,0xffffffffu);
    if(!textureBuffer[_KOTLIN])
      goto __fileManagerCreateIcons_deleteRustTexture;

    textureBuffer[_CSHARP]=font::createIconTexture(LANGUAGE_CSHARP,iconFontSize,0xffffffffu);
    if(!textureBuffer[_CSHARP])
      goto __fileManagerCreateIcons_deleteKotlinTexture;

    textureBuffer[_GO]=font::createIconTexture(LANGUAGE_GO,iconFontSize,0xffffffffu);
    if(!textureBuffer[_GO])
      goto __fileManagerCreateIcons_deleteCsharpTexture;

    textureBuffer[_HASKELL]=font::createIconTexture(LANGUAGE_HASKELL,iconFontSize,0xffffffffu);
    if(!textureBuffer[_HASKELL])
      goto __fileManagerCreateIcons_deleteGoTexture;

    textureBuffer[_RUBY]=font::createIconTexture(LANGUAGE_RUBY,iconFontSize,0xffffffffu);
    if(!textureBuffer[_RUBY])
      goto __fileManagerCreateIcons_deleteHaskellTexture;

    textureBuffer[_LUA]=font::createIconTexture(LANGUAGE_LUA,iconFontSize,0xffffffffu);
    if(!textureBuffer[_LUA])
      goto __fileManagerCreateIcons_deleteRubyTexture;

    textureBuffer[_PHP]=font::createIconTexture(LANGUAGE_PHP,iconFontSize,0xffffffffu);
    if(!textureBuffer[_PHP])
      goto __fileManagerCreateIcons_deleteLuaTexture;

    textureBuffer[_ERROR]=font::createIconTexture(ALERT,iconFontSize,0xffffffffu);
    if(!textureBuffer[_ERROR])
      goto __fileManagerCreateIcons_deletePhpTexture;

    return true;

    __fileManagerCreateIcons_deleteErrorTexture:
    Texture::destroy(textureBuffer[_ERROR]);
    textureBuffer[_ERROR]=(Texture*)0;
    __fileManagerCreateIcons_deletePhpTexture:
    Texture::destroy(textureBuffer[_PHP]);
    textureBuffer[_PHP]=(Texture*)0;
    __fileManagerCreateIcons_deleteLuaTexture:
    Texture::destroy(textureBuffer[_LUA]);
    textureBuffer[_LUA]=(Texture*)0;
    __fileManagerCreateIcons_deleteRubyTexture:
    Texture::destroy(textureBuffer[_RUBY]);
    textureBuffer[_RUBY]=(Texture*)0;
    __fileManagerCreateIcons_deleteHaskellTexture:
    Texture::destroy(textureBuffer[_HASKELL]);
    textureBuffer[_HASKELL]=(Texture*)0;
    __fileManagerCreateIcons_deleteGoTexture:
    Texture::destroy(textureBuffer[_GO]);
    textureBuffer[_GO]=(Texture*)0;
    __fileManagerCreateIcons_deleteCsharpTexture:
    Texture::destroy(textureBuffer[_CSHARP]);
    textureBuffer[_CSHARP]=(Texture*)0;
    __fileManagerCreateIcons_deleteKotlinTexture:
    Texture::destroy(textureBuffer[_KOTLIN]);
    textureBuffer[_KOTLIN]=(Texture*)0;
    __fileManagerCreateIcons_deleteRustTexture:
    Texture::destroy(textureBuffer[_RUST]);
    textureBuffer[_RUST]=(Texture*)0;
    __fileManagerCreateIcons_deleteRTexture:
    Texture::destroy(textureBuffer[_R]);
    textureBuffer[_R]=(Texture*)0;
    __fileManagerCreateIcons_deleteImageTexture:
    Texture::destroy(textureBuffer[_IMAGE]);
    textureBuffer[_IMAGE]=(Texture*)0;
    __fileManagerCreateIcons_deleteHtmlTexture:
    Texture::destroy(textureBuffer[_HTML]);
    textureBuffer[_HTML]=(Texture*)0;
    __fileManagerCreateIcons_deleteJsTexture:
    Texture::destroy(textureBuffer[_JS]);
    textureBuffer[_JS]=(Texture*)0;
    __fileManagerCreateIcons_deletePyTexture:
    Texture::destroy(textureBuffer[_PY]);
    textureBuffer[_PY]=(Texture*)0;
    __fileManagerCreateIcons_deleteCTexture:
    Texture::destroy(textureBuffer[_C]);
    textureBuffer[_C]=(Texture*)0;
    __fileManagerCreateIcons_deleteCppTexture:
    Texture::destroy(textureBuffer[_CPP]);
    textureBuffer[_CPP]=(Texture*)0;
    __fileManagerCreateIcons_deleteCssTexture:
    Texture::destroy(textureBuffer[_CSS]);
    textureBuffer[_CSS]=(Texture*)0;
    __fileManagerCreateIcons_deletePdfTexture:
    Texture::destroy(textureBuffer[_PDF]);
    textureBuffer[_PDF]=(Texture*)0;
    __fileManagerCreateIcons_deleteAudioTexture:
    Texture::destroy(textureBuffer[_AUDIO]);
    textureBuffer[_AUDIO]=(Texture*)0;
    __fileManagerCreateIcons_deleteVideoTexture:
    Texture::destroy(textureBuffer[_VIDEO]);
    textureBuffer[_VIDEO]=(Texture*)0;
    __fileManagerCreateIcons_deleteZipTexture:
    Texture::destroy(textureBuffer[_ZIP]);
    textureBuffer[_ZIP]=(Texture*)0;
    __fileManagerCreateIcons_deleteFileTexture:
    Texture::destroy(textureBuffer[_FILE]);
    textureBuffer[_FILE]=(Texture*)0;
    __fileManagerCreateIcons_deleteFolderTexture:
    Texture::destroy(textureBuffer[_FOLDER]);
    textureBuffer[_FOLDER]=(Texture*)0;
    __fileManagerCreateIcons_deleteDotsTexture:
    Texture::destroy(textureBuffer[_DOTS]);
    textureBuffer[_DOTS]=(Texture*)0;
    __fileManagerCreateIcons_return:
    return false;
  }


  void alignTextures(){
    refreshTextCoords();

    if(!(iconTextures[_ERROR])){

      if(!createAllTextures())
        return;
    }


    setIconsCoords();

  }

public:

  void setRootPath(const std::string& defaultFilePath){
    path.assign(defaultFilePath);
    unsigned int plen=path.length();
    #if defined(LINUX) || defined(ANDROID)
    if(plen>1 && path[plen-1]!='/'){
      path+='/';
    }
    #endif
    rootPathLen=path.length();
  }

private:
  unsigned int getFileType(char* name,uint64_t length,uint8_t d_type){ // done perfectly
    /*
    * .c - c
    * .r - r lang
    * .py - py
    * .js - javascript
    * .rs - rust
    * .kt - kotlin
    * .cs - csharp
    * .go - go
    * .hs - haskell
    * .rb - ruby
    * .cpp - cpp
    * .mp3 - audio
    * .mp4 - video
    * .zip - zip
    * .pdf - pdf
    * .cxx - cpp
    * .css - css
    * .lua - lua
    * .lhs - haskell
    * .php - php
    * .html - html
    * .java - java
    *
    */
    if(!name || !length)
      return 0;

    if(d_type==DT_LNK){
      long int earlyLength=path.length();
      path.append(name,length);

      if(lib::io::folder::exists(path)){
        d_type=DT_DIR;
      }else
        d_type=DT_REG;
      path.resize(earlyLength);
    }
    if(d_type==DT_REG){
      char* pointer=name+length;
      uint64_t bits_1=0;
      uint32_t bits_2=0;
      uint32_t len=0;
      char chr;
      while(--pointer>=name){
        if((chr=*pointer)=='.')
          break;
        ++len;

        if(chr>0x1f && chr<0x7f){
          if(chr<0x60){
            bits_1|=(((uint64_t)1)<<(chr-0x20));
          }else
            bits_2|=(((uint64_t)1)<<(chr-0x60));
        }

      }

      if(*pointer!='.' || len>4)
        return _FILE;

      if(len&1){
        if(len&2){ // len = 3
          if(!((bits_1^0) | (bits_2^0x10008))){ // cpp
            if(pointer[1]=='c' && pointer[2]=='p' && pointer[3]=='p'){
              return _CPP;
            }
          }else if(!((bits_1^0x80000) | (bits_2^0x12000) )){ // mp3
            if(pointer[1]=='m' && pointer[2]=='p' && pointer[3]=='3'){
              return _AUDIO;
            }
          }else if(!((bits_1^0x100000) | (bits_2^0x12000) )){ // mp4
            if(pointer[1]=='m' && pointer[2]=='p' && pointer[3]=='4'){
              return _VIDEO;
            }
          }else if(!((bits_1^0x0) | (bits_2^0x4010200) )){  // zip
            if(pointer[1]=='z' && pointer[2]=='i' && pointer[3]=='p'){
              return _ZIP;
            }
          }else if(!((bits_1^0x0) | (bits_2^0x10050) )){ // pdf
            if(pointer[1]=='p' && pointer[2]=='d' && pointer[3]=='f'){
              return _PDF;
            }
          }else if(!((bits_1^0x0) | (bits_2^0x1000008) )){ // cxx
            if(pointer[1]=='c' && pointer[2]=='x' && pointer[3]=='x'){
              return _CPP;
            }
          }else if(!((bits_1^0x0) | (bits_2^0x80008) )){ // css
            if(pointer[1]=='c' && pointer[2]=='s' && pointer[3]=='s'){
              return _CSS;
            }
          }else if(!((bits_1^0x0) | (bits_2^0x201002) )){ // lua
            if(pointer[1]=='l' && pointer[2]=='u' && pointer[3]=='a'){
              return _LUA;
            };
          }else if(!((bits_1^0x0) | (bits_2^0x81100) )){ // lhs
            if(pointer[1]=='l' && pointer[2]=='h' && pointer[3]=='s'){
              return _HASKELL;
            };
          }else if(!((bits_1^0x0) | (bits_2^0x10100) )){ // php
            if(pointer[1]=='p' && pointer[2]=='h' && pointer[3]=='p'){
              return _PHP;
            }
          }else if(!((bits_1^0x0) | (bits_2^0x10480) )){ // jpg
            if(pointer[1]=='j' && pointer[2]=='p' && pointer[3]=='g'){
              return _IMAGE;
            };
          }else if(!((bits_1^0x0) | (bits_2^0x14080) )){ // png
            if(pointer[1]=='p' && pointer[2]=='n' && pointer[3]=='g'){
              return _IMAGE;
            }
          }
          return _FILE;
        }else{ // len = 1
          if(pointer[1]=='c'){
            return _C;
          }else if(pointer[1]=='r'){
            return _R;
          }
        }
      }else if(len&2){
        if(!((bits_1^0) | (bits_2^0x2010000))){ // py
          if(pointer[1]=='p'){
            return _PY;
          }
        }else if(!((bits_1^0) | (bits_2^0x80400))){ // js
          if(pointer[1]=='j'){
            return _JS;
          }
        }else if(!((bits_1^0) | (bits_2^0xc0000))){ // rs
          if(pointer[1]=='r'){
            return _RUST;
          }
        }else if(!((bits_1^0) | (bits_2^0x100800))){ // kt
          if(pointer[1]=='k'){
            return _KOTLIN;
          }
        }else if(!((bits_1^0) | (bits_2^0x80008))){ // cs
          if(pointer[1]=='c'){
            return _CSHARP;
          }
        }else if(!((bits_1^0) | (bits_2^0x8080))){ // go
          if(pointer[1]=='g'){
            return _GO;
          }
        }else if(!((bits_1^0) | (bits_2^0x80100))){ // hs
          if(pointer[1]=='h'){
            return _HASKELL;
          }
        }else if(!((bits_1^0) | (bits_2^0x40004))){ // rb
          if(pointer[1]=='r'){
            return _RUBY;
          }
        }

      }else if(len&4){
        if(!((bits_1^0) | (bits_2^0x103100))){ // html
          if(pointer[1]=='h' && pointer[2]=='t' && pointer[3]=='m' && pointer[4]=='l'){
            return _HTML;
          }
        }
      }
    }else if(d_type==DT_DIR){
      return _FOLDER;
    }

    return _FILE;
  }

public:

  void reloadDir(){

    if(path.length()<=0){
#ifdef LINUX
      setRootPath("/");
#elif ANDROID
      setRootPath("/storage/emulated/0/");
#endif
    }
    lib::io::folder dir(path);
    // DIR* dir=opendir(path.c_str());
    struct dirent* dent;
    uint64_t length;
    uint64_t temp;
    uint64_t t64_1;
    void* tempAdd;

    lib::string& data=dirData.data;
    lib::vector<FileManagerBuffer>& fmb=dirData.fmb;

    FileManagerBuffer tfmb;

    fmb.resize(0);
    data.reset();

    uint32_t count=0;
    if(dir.opened()){
      while(dent=dir.next()){
        ++count;
        length=strlen(dent->d_name);

        if(length-1<2){
          if(length&2){
            if(*(dent->d_name)=='.' && *(((uint8_t*)(dent->d_name))+1)=='.')
              continue;
          }else if( *(dent->d_name)=='.' ){
              continue;
          }
        }

        tfmb.index=data.length;
        tfmb.type=getFileType(dent->d_name,length,dent->d_type);
        tfmb.length=length;
        lib::bits::stringToBits(dent->d_name,length,tfmb.bits_1,tfmb.bits_2);

        data.append(dent->d_name,length);

        fmb+=tfmb;
      }

      if(!fmb.size()){
        data.append("Folder Is Empty"); // 15

        tfmb.index=0;
        tfmb.type=_ERROR;
        tfmb.length=data.length;

        fmb+=tfmb;

      }
      // closedir(dir);
    }else{
      data.append("Cannot Enter "); // 13
      data.append(path); // 13

      tfmb.index=0;
      tfmb.type=_ERROR;
      tfmb.length=data.length;

      fmb+=tfmb;
    }
    recycleView.setScrollData(0,0,fmb.size(),1);

  }

  void updateProperties(FileManagerBuffer& fmb,const std::string& path){
    std::cout<<path.c_str()<<std::endl;

    Label& lb=properties;
    lib::string& text=properties.text;
    char* tpoint;
    uint32_t t32_1;

    text.reset();

    text.append("\nName : ");
    text.append(path);
    if(fmb.type==_FOLDER){
      text.append("\n\nType : Folder\n");
    }else{
      text.append("\n\nType : File\n" );
    }

    struct stat st;
    if(stat(path.c_str(),&st)==-1){
      return ;
    }

    // text.append(st.st_size);

    {
      char ita[40];
      t32_1=snprintf(ita,sizeof(ita),"Total Size : %.6f MB\n",st.st_size*0.000001f);

      text.append(ita,t32_1);
    }

    text.append("\nLast Accessed on : ");
    tpoint=ctime(&st.st_atime); // last access time
    text.append(tpoint,strlen(tpoint));

    text.append("Last Modified on : ");
    tpoint=ctime(&st.st_mtime); // modification time
    text.append(tpoint,strlen(tpoint));

    text.append("Last Change on   : ");
    tpoint=ctime(&st.st_ctime); // last status change
    text.append(tpoint,strlen(tpoint));

    if(st.st_mode&S_IRUSR){
      text.append("\n\nUser Permissions -\n\nRead    : Available\n");
    }else{
      text.append("\nUser Permissions -\n\nRead    : Unavailable\n");
    }

    if(st.st_mode&S_IWUSR){
      text.append("Write   : Available\n");
    }else{
      text.append("Write   : Unavailable\n");
    }

    if(st.st_mode&S_IXUSR){
      text.append("Execute : Available\n\nGroup Permissions -\n\n");
    }else{
      text.append("Execute : Unavailable\n\nGroup Permissions -\n\n");
    }

    if(st.st_mode&S_IRGRP){
      text.append("Read    : Available\n");
    }else{
      text.append("Read    : Unavailable\n");
    }

    if(st.st_mode&S_IWGRP){
      text.append("Write   : Available\n");
    }else{
      text.append("Write   : Unavailable\n");
    }

    if(st.st_mode&S_IXGRP){
      text.append("Execute : Available\n\nOther Permissions -\n\n");
    }else{
      text.append("Execute : Unavailable\n\nOther Permissions -\n\n");
    }

    if(st.st_mode&S_IROTH){
      text.append("Read    : Available\n");
    }else{
      text.append("Read    : Unavailable\n");
    }

    if(st.st_mode&S_IWOTH){
      text.append("Write   : Available\n");
    }else{
      text.append("Write   : Unavailable\n");
    }

    if(st.st_mode&S_IXOTH){
      text.append("Execute : Available\n");
    }else{
      text.append("Execute : Unavailable\n");
    }



    properties.updateTexture();

  }

  void updateTexture(){
    recycleView.updateTexture();
  }

// #define bitsCount(a,b) bmap[a&0xff]+bmap[(a>>8)&0xff]+bmap[(a>>16)&0xff]+bmap[(a>>24)&0xff]+bmap[(a>>32)&0xff]+bmap[(a>>40)&0xff]+bmap[(a>>48)&0xff]+bmap[(a>>56)&0xff]+bmap[b&0xff]+bmap[(b>>8)&0xff]+bmap[(b>>16)&0xff]+bmap[(b>>24)&0xff]

  void gotoParentDirectory(){
    int len=path.size();
    if(len && path[len-1]=='/'){
      len-=2;
      if(len<0)
        return;
    }
    long int pos=path.rfind('/',len);
    if(pos<rootPathLen){
      path.resize(rootPathLen);
    }else{
      path.resize(pos+1);
    }
    reloadDir();
  }

  void gotoFolderInCurrent(unsigned int index){
    if(index>=dirData.fmb.size())
      return;

    unsigned int plen=path.size();
    if(plen && path[plen-1]!='/')
      path+='/';

    FileManagerBuffer& fmb=dirData.fmb[index];
    if(fmb.type==_FOLDER){
      path.append(dirData.data.pointer+fmb.index,fmb.length);
      path+='/';
      reloadDir();
    }
  }

  RecycleView_Touch(fileManager_recycleView_onTouch){
    FileManager* fm=static_cast<FileManager*>(ptr);
    if(row>=fm->dirData.fmb.size())
      return;

    if(coords.x>0.9f){
      unsigned int plen=fm->path.size();
      if(plen && fm->path[plen-1]!='/')
        fm->path+='/';
      FileManagerBuffer& fmb=fm->dirData.fmb[row];

      if(fmb.type!=_ERROR){
        plen=fm->path.size();
        fm->path.append(fm->dirData.data.pointer+fmb.index,fmb.length);
        fm->updateProperties(fmb,fm->path);
        fm->path.resize(plen);

        fm->popup.show();
        fm->popupScreen.set(FILEMANAGER_POPUP_SCREEN_PROPERTIES);
        callback::render();
      }

        // show dropdown
        // popup_show(context,&(fm->popup));
        // fm->popup.widget.flags|=POPUP_DISMISS_ON_EXTERNAL_TOUCH;
        // screen_set(context,&(fm->popupScreen),FILEMANAGER_POPUP_SCREEN_PROPERTIES);
    }else{

      unsigned int plen=fm->path.size();
      if(plen && fm->path[plen-1]!='/')
        fm->path+='/';

      FileManagerBuffer& fmb=fm->dirData.fmb[row];
      if(fmb.type==_ERROR)
        return;
      if(fmb.type==_FOLDER){
        fm->path.append(fm->dirData.data.pointer+fmb.index,fmb.length);
        fm->path+='/';
        fm->reloadDir();
        fm->updateTexture();

      }else{
        if(fm->onFileOpen){
          plen=fm->path.size();
          fm->path.append(fm->dirData.data.pointer+fmb.index,fmb.length);
          fm->onFileOpen(fm->pointer,(const char* const)(fm->path.c_str()));
          fm->path.resize(plen);
        }

      }


      callback::render();
    }
  }

  RecycleView_Resize(fileManager_recycleView_onResize){
    if(mode&(DIMENSION_W|DIMENSION_H)){
      (static_cast<FileManager*>(ptr))->alignTextures();
    }
  }

  RecycleView_Render(fileManager_recycleView_onRender){
    FileManager* fm=static_cast<FileManager*>(ptr);
    FileManagerBuffer& fmb=fm->dirData.fmb[row];

      // printVec4(coords);
    RectangleDetails rd=(RectangleDetails){
      .coords=coords,
      .textureCoords=fm->iconCoords[fmb.type],
      .textureFrameCoords=(vec4){.x=0.0,.y=0.0,.w=1.0,.h=1.0},
      .texture=fm->iconTextures[fmb.type],
      .color=*color,
      .borderColor=*outline,
      .angle=fm->button.angle,
      .borderThickness=fm->button.borderThickness,
      .blur=fm->button.blur,
      .textureColorMultiplier=color[1],
    };
    renderer::renderRectangle(rd);

    rd.texture=fm->iconTextures[_DOTS];
    rd.textureCoords=fm->iconCoords[_DOTS];
    rd.color=0;
    rd.borderColor=0;

    renderer::renderRectangle(rd);

    TextDetails td=(TextDetails){
      .boundingBox=(vec4){fm->textCoords.x+coords.x,fm->textCoords.y+coords.y,fm->textCoords.w,fm->textCoords.h},
      .text=fm->dirData.data.pointer+fmb.index,
      .bitmap=(FontBitmap*)0,
      .textStart=0,
      .textEnd=fmb.length,
      .row=1,
      .column=(uint32_t)fm->textColumn,
      .currentRow=0,
      .currentColumn=0,
      .rowOffset=0,
      .columnOffset=0,
      .rowSize=1,
      .columnSize=(uint32_t)fm->textColumn,
      .fontColor=color[1]>>8,
      .tabSize=1,
    };

    renderer::renderText(td);

  }

  Widget_Render(fileManager_render){
    FileManager* fm=static_cast<FileManager*>(widget);
    RectangleDetails rd=(RectangleDetails){
      .coords=widget->coords,
      .textureCoords=(vec4){0},
      .textureFrameCoords=(vec4){0},
      .texture=(Texture*)0,
      .color=*color,
      .borderColor=*outline,
      .angle=fm->angle,
      .borderThickness=fm->borderThickness,
      .blur=fm->blur
    };
    renderer::renderRectangle(&rd);

  }

  // void fileManager_searchMode(FileManager* fm){
  //   widget_show(&(fm->searchBar.widget));
  //   fm->recycleView.functions.onRender=fileManager_recycleView_onRender_Search;
  //   fm->recycleView.functions.onTouch=fileManager_recycleView_onTouch_Search;
  //
  //   fileManager_updateSearchBuffer(fm);
  // }

  void normalMode(){
    // widget_hide(&(fm->searchBar.widget));
    recycleView.setComponentRenderFunction(fileManager_recycleView_onRender);
    recycleView.setComponentTouchFunction(fileManager_recycleView_onTouch);



  }



  void setCloseFunction(void (*func)(void* ),void* ptr){
    closeButton.onReleased=func;
    closeButton.pointer=ptr;
  }

  void setOpenFileFunction(void (*func)(void*,const char* const),void* ptr){
    onFileOpen=func;
    pointer=ptr;
  }

  Button_Released(fileManager_refreshDirButton){
    FileManager* fm=static_cast<FileManager*>(ptr);
    fm->reloadDir();
    fm->updateTexture();
  }

  Button_Released(fileManager_goBack){
    FileManager* fm=static_cast<FileManager*>(ptr);
    fm->gotoParentDirectory();
    fm->updateTexture();
  }

  Button_Released(fileManager_closePopup){
    (static_cast<FileManager*>(ptr))->popup.hide();
  }


  TwoFacedIconButton_Released(fileManager_showSearchBar){
    FileManager* fm=static_cast<FileManager*>(ptr);
    if(mode){
      fm->searchBar.show();
    }else{
      fm->searchBar.hide();
    }

  }
  ~FileManager(){
    destroyAllTextures();
  }

  FileManager(const std::string& defaultFilePath) :
    recycleView(12,1),
    closeButton(CLOSE_THICK),
    backButton(ARROW_LEFT_BOLD),
    refreshButton(FOLDER_REFRESH),
    newFolderButton(FOLDER_PLUS),
    newFileButton(FILE_PLUS),
    searchButton(FILE_SEARCH,CLOSE),
    popupScreen(FILEMANAGER_TOTAL_SCREENS),
    popupCloseButton(CLOSE_THICK)
  {
    setRootPath(defaultFilePath);

    topBar.setXY(0.0,0.95).setWH(1.0,0.05);
    closeButton.setXY(0.02f,0.1f).setWH(0.1f,0.8f);
    backButton.setXY(0.12f,0.1f).setWH(0.1f,0.8f);
    refreshButton.setXY(0.78f,0.1f).setWH(0.1f,0.8f);
    searchButton.setXY(0.88f,0.1f).setWH(0.1f,0.8f);
    newFileButton.setXY(0.68f,0.1f).setWH(0.1f,0.8f);
    newFolderButton.setXY(0.58f,0.1f).setWH(0.1f,0.8f);
    searchBar.setXY(0.25f,0.1f).setWH(0.63f,0.8f);
    searchBar.angle=0.2f;

    popup.setXY(0.1f,0.1f).setWH(0.8f,0.8f);
    popupScreen.setXY(0.0f,0.1f).setWH(1.0f,0.9f);
    popupCloseButton.setXY(0.0f,0.0f).setWH(1.0f,0.1f).setColor(themes::surface);

    properties.setXY(0.0f,0.0f).setWH(1.0f,1.0f);

    searchBar.hide();

    topBar.setColor(themes::primary);

    recycleView.setComponentPointer(static_cast<void*>(this));

    recycleView.setComponentRenderFunction(fileManager_recycleView_onRender);
    recycleView.setComponentTouchFunction(fileManager_recycleView_onTouch);
    recycleView.setComponentResizeFunction(fileManager_recycleView_onResize);

    recycleView.setXY(0.0f,0.0f).setWH(1.0f,0.95f);

    onRender=fileManager_render;
    flags=SETTING_RENDER|SETTING_TOUCH;

    add(&recycleView);
    topBar.add(&closeButton);
    topBar.add(&backButton);
    topBar.add(&refreshButton);
    topBar.add(&searchButton);
    topBar.add(&newFileButton);
    topBar.add(&newFolderButton);
    topBar.add(&searchBar);

    add(&popup);
    add(&topBar);

    popup.add(&popupScreen);
    popup.add(&popupCloseButton);
    popupScreen.add(FILEMANAGER_POPUP_SCREEN_PROPERTIES,&properties);


    searchButton.onReleased=fileManager_showSearchBar;
    searchButton.pointer=static_cast<void*>(this);

    backButton.onReleased=fileManager_goBack;
    backButton.pointer=static_cast<void*>(this);

    refreshButton.onReleased=fileManager_refreshDirButton;
    refreshButton.pointer=static_cast<void*>(this);

    popupCloseButton.onReleased=fileManager_closePopup;
    popupCloseButton.pointer=static_cast<void*>(this);

    reloadDir();
  }
};
#endif

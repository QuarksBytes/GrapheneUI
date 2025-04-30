#ifndef __UI__WIDGET_LIST_BUTTON__
#define __UI__WIDGET_LIST_BUTTON__

#include"Widgets/RecycleView.cpp"

#define ListButton_Pressed(name) static void name(Data data)

#define LISTBUTTON_SCROLL_ROW 1
#define LISTBUTTON_SCROLL_COLUMN 0

struct ____ListButton_InternalStorage{
  unsigned int index;
  unsigned int length;
  void (*onTouch)(Data data);
  Data data;
};

class ListButtons : public RecycleView{
private:
  lib::string data;
  vec4 textCoords;
  lib::vector<____ListButton_InternalStorage> list;
  unsigned int textColumn=0;
public:

  struct{
    BasicDrawInfo;
  }button={0};

  unsigned int fontSize;
  unsigned int fontColor;

private:

  void refreshCoords(){
    if(!fontSize){
      fontSize=GLOBAL_TEXT_SIZE;
    }
    if(!texture)
      return ;

    int h=font::dpToPx(fontSize);
    int w=font::bitmap->whRatio*h;
    vec2 size=getSize();
    int width=size.w*texture->w;
    int height=size.h*texture->h;

    float winv=1.0f/texture->w;
    float hinv=1.0f/texture->h;

    int t32_1=((int)(textColumn=(0.95f*width)/w))*w;

    textCoords.w=t32_1*winv;
    textCoords.h=h*hinv;

    textCoords.x=((width-t32_1)>>1)*winv;
    textCoords.y=((height-h)>>1)*hinv;
  }

  void updateScrollData(){
    unsigned int len=list.size(),rc;
    float tfloat;
    if(flags&1){
      rc=getCount().column;
      tfloat=len/(float)rc;
      len=((int)tfloat);
      if(tfloat>len){
        len+=1;
      }
      setScrollData(0,0,len,rc);
    }else{
      rc=getCount().row;
      tfloat=len/(float)rc;
      len=((int)tfloat);
      if(tfloat>len){
        len+=1;
      }
      setScrollData(0,0,rc,len);
    }
  }
public:
  void appendButton(const std::string& text,void (*touch)(Data ),Data d1){
    if(!touch)
      return;
    ____ListButton_InternalStorage buf=(____ListButton_InternalStorage){.index=(unsigned int)data.length,.length=(unsigned int)text.length(),.onTouch=touch,.data=d1};

    data.append(text);

    list+=buf;
    updateScrollData();

  }

  void addButton(unsigned int index,const std::string& text,void (*touch)(Data),Data d1){
    if(index>=list.size() || !touch)
      return;
    ____ListButton_InternalStorage buf=(____ListButton_InternalStorage){.index=(unsigned int)data.length,.length=(unsigned int)text.length(),.onTouch=touch,.data=d1};

    data.append(text);

    list.add(index,buf);

    updateScrollData();

  }

  void reset(){
    list.resize(0);
    setScrollData(0,0,0,0);
  }

  void update(){
    if(window::flags&UI_STARTED)
      updateTexture();
  }

  RecycleView_Touch(listButton_touch){
    ListButtons * but=static_cast<ListButtons*>(ptr);

    unsigned int index=but->getCount().column;
    index=(index*row)+column;

    if(index>=but->list.size())
      return;

    printf("Index %d\n",column);

    ____ListButton_InternalStorage& buf=but->list[index];

    if(buf.onTouch)
      buf.onTouch(buf.data);

  }

  RecycleView_Render(listButton_render){
    ListButtons * but=static_cast<ListButtons*>(ptr);

    unsigned int index=but->getCount().column;
    index=(index*row)+column;

    if(index>=but->list.size())
      return;

    ____ListButton_InternalStorage& buf=but->list[index];
    RectangleDetails rd=(RectangleDetails){
      .coords=coords,
      .textureCoords=(vec4){0.0},
      .textureFrameCoords=(vec4){0.0},
      .texture=nullptr,
      .color=*color,
      .borderColor=*outline,
      .angle=but->button.angle,
      .borderThickness=but->button.borderThickness,
      .blur=but->button.blur,
      .textureColorMultiplier=0xffffffffu,
    };
    renderer::renderRectangle(rd);

    TextDetails td=(TextDetails){
      .boundingBox=(vec4){but->textCoords.x+coords.x,but->textCoords.y+coords.y,but->textCoords.w,but->textCoords.h},
      .text=but->data.pointer+buf.index,
      .bitmap=(FontBitmap*)0,
      .textStart=0,
      .textEnd=buf.length,
      .row=1,
      .column=(uint32_t)but->textColumn,
      .currentRow=0,
      .currentColumn=0,
      .rowOffset=0,
      .columnOffset=0,
      .rowSize=1,
      .columnSize=(uint32_t)but->textColumn,
      .fontColor=color[1],
      .tabSize=1,
    };

    if(buf.length<td.column){
      td.columnOffset=(td.column-buf.length)>>1;
    }

    renderer::renderText(td);
  }

  RecycleView_Resize(listButton_resize){
    if(mode&(DIMENSION_W|DIMENSION_H)){
      ListButtons * but=static_cast<ListButtons*>(ptr);
      but->refreshCoords();
    }
  }

  ListButtons(unsigned int row,unsigned int column,unsigned int mode=LISTBUTTON_SCROLL_ROW,unsigned int fsize=GLOBAL_TEXT_SIZE):
    RecycleView(row,column)
  {
    fontSize=fsize;
    setComponentPointer(static_cast<void*>(this));

    setComponentTouchFunction(listButton_touch);
    setComponentRenderFunction(listButton_render);
    setComponentResizeFunction(listButton_resize);

    flags|=(mode&1);
  }

};

#endif

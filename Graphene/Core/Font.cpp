#ifndef __UI_CORE_FONT__
#define __UI_CORE_FONT__

#include<ft2build.h>
#include FT_FREETYPE_H

#include"Core/Renderer.cpp"

namespace font{


  FT_Library library;
  FT_Face iconFace;
  FontBitmap* bitmap;


  int reloadGlobalIcon(char* path){
    ReturnOnError(path) Error;
    FT_Face newFace;
    if(FT_New_Face(library,path,0,&(newFace))){
      uiLog("[ ERROR ] : Icon font file opening error , Falling back to current font\n");
      return Error;
    }
    FT_Done_Face(iconFace);
    iconFace=newFace;
    return Success;
  }

  int dpToPx(int dp){
    return dp*window::dpiRatio;
  }

  FontBitmap* createBitmap(std::string fontFile){
    FT_Face face;
    if(FT_New_Face( library,fontFile.c_str(),0,&face)){
      uiLog("[ ERROR ] : Font file opening error\n");
      return (FontBitmap*)0;
    }
    FontBitmap* tb=new(std::nothrow) FontBitmap;
    if(!tb){
      FT_Done_Face(face);
      return 0;
    }

    FT_Set_Pixel_Sizes(face, 0,40);

    float yscaleFactor=(face->size->metrics.y_ppem/(float)(face->units_per_EM));

    int descend=(face->bbox.yMin*-1)*yscaleFactor;
    int bbox_y=(face->bbox.yMax-face->bbox.yMin);//*yscaleFactor;//>>6;
    int ascend;
    int h=32;

    tb->whRatio=(float)((face->bbox.xMax)>>6)*(1.0/(float)(bbox_y>>6));
    int w=(int)(h*tb->whRatio);
    bbox_y*=yscaleFactor;
    ascend=bbox_y-descend;

    float hConverter=(float)h/(float)bbox_y;
    float win=1.0/(float)w;
    float hin;

    int width=w*0x6;
    int height=(h*0x10);

    uint8_t *buffer=(uint8_t*)calloc(sizeof(uint8_t),width*height),*tempBuf,*t2buf,*bitmapBuf,*bitmapTemp;
    if(!buffer){
      delete tb;
      return 0;
    }
    tempBuf=buffer;

    int row=0x10;
    int column;
    int rows;
    int rtc;
    int bitmapWidth;
    int iter;
    int t32_1;
    char chr;
    while(row--){
      column=0;
      while(column<6){
        chr=(((++column)+1)<<4)^row;
        if(FT_Load_Char(face,chr, FT_LOAD_RENDER)){
          delete tb;
          delete[] buffer;
          uiLog("[ ERROR ] : FREETYTPE : Failed to load Glyph\n");
          return NULL;
        }
        if(FT_Render_Glyph(face->glyph,FT_RENDER_MODE_SDF)){
          delete tb;
          delete[] buffer;
          uiLog("[ ERROR ] : FREETYTPE : Failed to load Glyph\n");
          return NULL;
        }

        if(face->glyph->bitmap.buffer){
          rows=(face->glyph->bitmap.rows-(face->glyph->metrics.height>>6));
          t32_1=(face->glyph->metrics.horiBearingY>>6)+(rows>>1);
          if(t32_1>ascend)
            t32_1=ascend;
          t32_1=(descend+t32_1);
          t2buf=tempBuf+((((int)(t32_1*hConverter))-1)*width);
          rows+=(face->glyph->metrics.height>>6);
          if(t32_1<rows)
            rows=t32_1;
          rtc=((float)(rows))*hConverter;
          bitmapBuf=face->glyph->bitmap.buffer;
          bitmapWidth=face->glyph->bitmap.width;
          rows=0;
          hin=(float)(face->glyph->bitmap.rows)/(float)rtc;
          while(rows<rtc){
            bitmapTemp=bitmapBuf+(((int)((rows++)*hin))*bitmapWidth);
            for(iter=0;iter<w;++iter){
              t2buf[iter]=bitmapTemp[(int)(iter*win*bitmapWidth)];
            }
            t2buf-=width;
          }
        }
        tempBuf+=w;
      }
      tempBuf+=((h-1)*width);
    }
    glGenTextures(1, &(tb->textureID));
    if(!(tb->textureID)){
      uiLog("[ ERROR ] : Texture Creation Error\n");
      delete tb;
      delete[] buffer;
      return 0;
    }
    glBindTexture(GL_TEXTURE_2D, tb->textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_ALPHA,
        width,
        height,
        0,
        GL_ALPHA,
        GL_UNSIGNED_BYTE,
        buffer
    );
    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    free(buffer);
    return tb;
  }

  void destroyBitmap(FontBitmap* bitmap){
    if(bitmap){
      glDeleteTextures(1,&bitmap->textureID);
      delete bitmap;
    }
  }


  void init(){
    if(FT_Init_FreeType(&library))
      std::exit(EXIT_FAILURE);
    if(FT_New_Face(library,GLOBAL_ICON_FONT,0,&iconFace)){
      uiLog("[ ERROR ] : Icon font file opening error\n");
      FT_Done_FreeType(library);
      std::exit(EXIT_FAILURE);
    }
    bitmap=createBitmap(GLOBAL_TEXT_FONT);
    if(!bitmap){
      FT_Done_Face(iconFace);
      FT_Done_FreeType(library);
      std::exit(EXIT_FAILURE);
    }
  }

  void fini(){
    destroyBitmap(bitmap);
    FT_Done_Face(iconFace);
    FT_Done_FreeType(library);
  }


  Texture* createIconTexture(uint32_t uniCode,uint32_t fontSize,uint32_t color){
    fontSize*=window::dpiRatio;
    FT_Set_Pixel_Sizes(iconFace, 0, fontSize);
    FT_UInt glyph_index = FT_Get_Char_Index(iconFace, uniCode);
    if (FT_Load_Glyph(iconFace, glyph_index, FT_LOAD_DEFAULT)) {
      uiLog("Error in loading icon glyph\n");
      return (Texture*)Error;
        // Handle error
    }
    if (FT_Render_Glyph(iconFace->glyph, FT_RENDER_MODE_NORMAL)){
      printf("Error in getting bitmap of icon glyph\n");
      return 0;
        // Handle error
    }
    Texture* tex=new(std::nothrow) Texture;
    if(!tex){
      uiLog("Icon texture memory allocation error\n");
      return NULL;
    }
    tex->w=iconFace->glyph->bitmap.width;
    tex->h=iconFace->glyph->bitmap.rows;

    uint32_t *buffer=new(std::nothrow) uint32_t[tex->w*tex->h];
    if(!buffer){
      delete tex;
      return 0;
    }
    uint32_t rows=iconFace->glyph->bitmap.rows;
    uint32_t width=iconFace->glyph->bitmap.width;
    uint32_t column;
    uint8_t* srcRow=((uint8_t*)iconFace->glyph->bitmap.buffer)+((rows-1)*width);
    uint8_t* trow;
    uint32_t* destRow=buffer;
    color>>=8;
    color=((color&0xff)<<16)^(color&0xff00)^(color&0xff0000)>>16;
    while(rows--){
      column=0;
      trow=srcRow;
      while(column++<width){
        *(destRow++)=color^((*(trow++))<<24);
      }
      srcRow-=width;
    }


    glGenTextures(1, &(tex->id));
    if(!(tex->id)){
      delete[] buffer;
      delete tex;
    }
    glBindTexture(GL_TEXTURE_2D, tex->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        tex->w,
        tex->h,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        buffer
    );
    glBindTexture(GL_TEXTURE_2D, 0);
    delete[] buffer;

    return tex;

  }

  Texture* createTextTexture(FontBitmap* tb,const char* text,uint32_t fontColor,uint32_t maxWidth,uint32_t fontSize){
    if(!text)
      return 0;
    if(!tb)
      tb=bitmap;
    fontSize*=window::dpiRatio;
    unsigned int length=strlen(text),cr=0,cc=0,width,column,row;
    int rheight=fontSize;
    int rwidth=tb->whRatio*rheight;
    width=length*rwidth;
    if(width>maxWidth){
      column=maxWidth/rwidth;
      row=(length/column)+1;
      width=column*rwidth;
    }else{
      column=length;
      row=1;
    }
    Texture* texture=Texture::create(width,rheight*row);
    ReturnOnError(texture) NULL;

    TextDetails td=(TextDetails){
                                                .boundingBox=(vec4){0.0,0.0,1.0,1.0},
                                                .text=text,
                                                .bitmap=tb,
                                                .textStart=0,
                                                .textEnd=length,
                                                .row=row,
                                                .column=column,
                                                .currentRow=0,
                                                .currentColumn=0,
                                                .rowOffset=0,
                                                .columnOffset=0,
                                                .rowSize=row,
                                                .columnSize=column,
                                                .fontColor=fontColor,
                                                .tabSize=1,
                                              };

    const vec4& clearColor=themes::getClearColor();
    renderer::framebuffer::start(texture);
    renderer::clear(0.0,0.0,0.0,0.0);
    renderer::__renderTextInternal(td);
    renderer::framebuffer::stop();
    return texture;
  }

  Texture* createTextTexture(FontBitmap* tb,const char* text,int64_t length,uint32_t fontColor,uint32_t maxWidth,uint32_t fontSize){
    if(!text)
      return 0;
    if(!tb)
      tb=bitmap;
    fontSize*=window::dpiRatio;
    unsigned int cr=0,cc=0,width,column,row;
    int rheight=fontSize;
    int rwidth=tb->whRatio*rheight;
    width=length*rwidth;
    if(width>maxWidth){
      column=maxWidth/rwidth;
      row=(length/column)+1;
      width=column*rwidth;
    }else{
      column=length;
      row=1;
    }
    Texture* texture=Texture::create(width,rheight*row);
    ReturnOnError(texture) NULL;

    TextDetails td=(TextDetails){
      .boundingBox=(vec4){0.0,0.0,1.0,1.0},
      .text=text,
      .bitmap=tb,
      .textStart=0,
      .textEnd=length,
      .row=row,
      .column=column,
      .currentRow=0,
      .currentColumn=0,
      .rowOffset=0,
      .columnOffset=0,
      .rowSize=row,
      .columnSize=column,
      .fontColor=fontColor,
      .tabSize=1,
    };

    renderer::framebuffer::start(texture);
    renderer::clear(0.0,0.0,0.0,0.0);
    renderer::__renderTextInternal(td);
    renderer::framebuffer::stop();
    return texture;
  }


  FontBitmap* getGlobalBitmap(){
    return bitmap;
  }

  int getWidth(FontBitmap* bitmap,int height){
    ReturnOnError(bitmap) Error;
    return bitmap->whRatio*height;
  }

  int getWidthFromDefault(int height){
    return getWidth(bitmap,height);
  }


}

namespace renderer{

  void setTextBitmap(FontBitmap*  bitmap){
    shader::use(text.shaderProgram);
    glBindTexture(GL_TEXTURE_2D,0);
    if(bitmap)
      glBindTexture(GL_TEXTURE_2D,bitmap->textureID);
    else
      glBindTexture(GL_TEXTURE_2D,font::bitmap->textureID);
  }


  void renderText(TextDetails& details){
    if(!(details.bitmap))
      details.bitmap=font::bitmap;
    __renderTextInternal(details);
  }



}

#endif

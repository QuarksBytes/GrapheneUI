#ifndef __UI_CORE_RENDERER__
#define __UI_CORE_RENDERER__


#include<GLES3/gl3.h>
#include<SDL_opengles2.h>

#include"Core/Shaders.h"
#include"Core/Window.cpp"

#define TEXT_SHADER_DEFAULT_BUFFER_SIZE 0x20


#define VBO_STATIC GL_STATIC_DRAW
#define VBO_DYNAMIC GL_DYNAMIC_DRAW
#define VBO_STREAM GL_STREAM_DRAW
#define RENDERER_DATATYPE_FLOAT GL_FLOAT
#define RENDERER_DATATYPE_INT GL_INT


struct RectangleShader{
  unsigned int shaderProgram;
  int angleIDX;
  int dimensionIDX;
  int colorIDX;
  int textureIDX;
  int frameIDX;
  int widthHeightIDX;
  int blurIDX;
  int borderColorIDX;
  int borderThicknessIDX;
  int dpiIDX;
  int colorManipulationIDX;
};

struct TextShader{
    unsigned int shaderProgram;
    int totalRowColumnIDX;
    int currentRowColumnIDX;
    int backgroundColorIDX;
    int fontColorIDX;
    int characterIDX;
    int rcOffsetIDX;
    int rcSizeIDX;
    int dimensionsIDX;
};

#define COMPILE_MODE_VERTEX_SHADER 1
#define COMPILE_MODE_FRAGMENT_SHADER 2

namespace shader{


  GLuint compile(const char* text,int mode){
    ReturnOnError(text) Error;
    GLuint shader;
    if(mode&COMPILE_MODE_VERTEX_SHADER)
      shader=glCreateShader(GL_VERTEX_SHADER);
    else
      shader=glCreateShader(GL_FRAGMENT_SHADER);
    ReturnOnError(shader) Error;
    glShaderSource(shader,1,&text,NULL);
    glCompileShader(shader);
    GLint compileSucceded=GL_FALSE;
    glGetShaderiv(shader,GL_COMPILE_STATUS,&compileSucceded);
    if(!compileSucceded){
      GLint logLength=0;
      glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&logLength);
      GLchar* errLog=new (std::nothrow) GLchar[logLength];//(GLchar*)qmalloc(logLength);
      if(errLog){
        glGetShaderInfoLog(shader,logLength,&logLength,errLog);
        log("[ Error ] : Shader compilation error\n[ Error Log ] :%s\n",errLog);
        delete[] errLog;
      }else{
        log("[ Error ] : Couldn't get shader error log\n");
      }
      glDeleteShader(shader);
      return 0;
    }
    return shader;
  }

  GLuint link(GLuint vshader,GLuint fshader){
    if(!vshader || !fshader)
      return Error;

    GLuint shaderProgram=glCreateProgram();
    if(!shaderProgram){
      log("[ Error ] : Couldn't create shader program\n");
      return 0;
    }
    glAttachShader(shaderProgram,vshader);
    glAttachShader(shaderProgram,fshader);
    glLinkProgram(shaderProgram);
    GLint compileSucceded=GL_FALSE;
    glGetProgramiv(shaderProgram,GL_LINK_STATUS,&compileSucceded);
    if(!compileSucceded){
        log("[ Error ] : Linking shader failed , Couldn't Create Shader Program\n");
        glDeleteProgram(shaderProgram);
        return 0;
    }
    return shaderProgram;
  }


  GLuint create(const char* vertexShader,const char* fragmentShader){

    if(!vertexShader || !fragmentShader)
      return Error;
    GLuint vShader=compile(vertexShader,COMPILE_MODE_VERTEX_SHADER);
    if(!vShader){
      log("[ Error ] : Compilation of vertex shader failed\n");
      return Error;
    }
    GLuint fShader=compile(fragmentShader,COMPILE_MODE_FRAGMENT_SHADER);
    if(!fShader){
      glDeleteShader(vShader);
      log("[ Error ] : Compilation of fragment shader failed\n");
      return Error;
    }
    GLuint shaderProgram=link(vShader,fShader);
    glDeleteShader(vShader);
    glDeleteShader(fShader);
    return shaderProgram;
  }

  void destroy(GLuint shaderProgram){
    glDeleteProgram(shaderProgram);
  }

  int use(GLuint shaderProgram){
    static GLuint current=0;
    if(shaderProgram==current)
      return Success;
    while(glGetError()!=GL_NO_ERROR);
    glUseProgram(shaderProgram);
    if(glGetError()==GL_NO_ERROR){
      current=shaderProgram;
      return Success;
    }
    return Error;
  }

  GLuint generateVBO(int location,void* vertices,int totalData,unsigned int pairs,int dataSize,unsigned int dataType,unsigned int usage){
    ReturnOnError(vertices) Error;
    GLuint vbo;
    glGenBuffers(1,&vbo);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER, dataSize*totalData,vertices,usage);
    glVertexAttribPointer(location,pairs,dataType,GL_FALSE,dataSize*pairs,(const GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    GLenum err=glGetError();
    if(err!=GL_NO_ERROR){
      log("[ ERROR ] : Creating VBO failed , Error code 0x%x\n",err);
      glDeleteBuffers(1,&vbo);
      return Error;
    }
    return vbo;
  }

  void enableVBO(GLuint vbo){
    glEnableVertexAttribArray(vbo);
  }

  void deleteVBO(GLuint* vbo){
    ReturnOnError(vbo);
    glDeleteBuffers(1,vbo);
    *vbo=0;
  }


}

struct Texture{
  unsigned int id;
  int32_t w,h;

  static int flipSurfaceVertically(SDL_Surface* surface){
    uint32_t length=surface->pitch;
    char* temp=new (std::nothrow) char[length];
    if(!temp)
      return Error;
    int h=surface->h>>1;
    char* pixels=(char*)(surface->pixels),*tpixels=0;
    SDL_LockSurface(surface);
    for(int i=0,j=0,k=0;i<h;++i){
      j=i*length;
      k=((h<<1)-(i+1))*length;
      memcpy(temp,pixels+j,length);
      memcpy(pixels+j,pixels+k,length);
      memcpy(pixels+k,temp,length);
    }
    SDL_UnlockSurface(surface);
    delete[] temp;
    return Success;
  }


  static uint8_t swizzle(GLenum channel,uint32_t mask){
    GLint swizzle;
    switch(mask){
      case 0x000000FF:
        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        swizzle=GL_ALPHA;
        #else
        swizzle=GL_RED;
        #endif
        break;
      case 0x0000FF00:
        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        swizzle=GL_BLUE;
        #else
        swizzle=GL_GREEN;
        #endif
        break;
      case 0x00FF0000:
        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        swizzle=GL_GREEN;
        #else
        swizzle=GL_BLUE;
        #endif
        break;
      case 0xFF000000:
        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        swizzle=GL_RED;
        #else
        swizzle=GL_ALPHA;
        #endif
        break;
      default:
        log("Unrecoganized mask 0x%08x",mask);
        return 0;
    }
    glTexParameteri(GL_TEXTURE_2D,channel,swizzle);
    return 1;
  }

  static Texture* createFromSurface(SDL_Surface *surface){
    GLenum format=0;
    if(surface->format->BytesPerPixel==3){
      format=GL_RGB;
    }else if(surface->format->BytesPerPixel==4){
      format=GL_RGBA;
    }else{
      log("ERROR : Cannot open image , Image is not true color\n");
      return 0;
    }
    if(!flipSurfaceVertically(surface))
      return (Texture*)Error;
    Texture* texture=new(std::nothrow) Texture;
    if(!texture)
      return (Texture*)0;
    glGenTextures(1,&(texture->id));
    if(!(texture->id)){
      delete texture;
      log("Texture Generation Error\n");
      return (Texture*)0;
    }
    texture->w=surface->w;
    texture->h=surface->h;
    glBindTexture(GL_TEXTURE_2D,texture->id);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w,surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
    uint8_t bl=swizzle(GL_TEXTURE_SWIZZLE_R,surface->format->Rmask);
    bl&=swizzle(GL_TEXTURE_SWIZZLE_G,surface->format->Gmask);
    bl&=swizzle(GL_TEXTURE_SWIZZLE_B,surface->format->Bmask);
    if(format==GL_RGBA){
      bl&=swizzle(GL_TEXTURE_SWIZZLE_A,surface->format->Amask);
    }
    if(!bl){
      log("Error!! in Swizzling texture\n");
      glDeleteBuffers(1,&texture->id);
      delete texture;
      return (Texture*)0;
    }
    glBindTexture(GL_TEXTURE_2D,0);
    return texture;
  }


  static Texture* create(uint32_t w,uint32_t h){
    Texture* texture=new(std::nothrow) Texture;
    if(!texture)
      return (Texture*)Error;
    glGenTextures(1,&(texture->id));
    if(!(texture->id)){
      log("[ Error ] : Texture Creation Error\n");
      delete texture;
      return (Texture*)Error;
    }
    glBindTexture(GL_TEXTURE_2D,texture->id);
    texture->w=w;
    texture->h=h;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,texture->w ,texture->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D,0);
    return texture;
  }


  static void destroy(Texture* texture){
    if(texture){
      glDeleteTextures(1,&(texture->id));
      delete texture;
    }
  }

  void resize(int nw,int nh){

    glBindTexture(GL_TEXTURE_2D,id);
    w=nw;
    h=nh;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,nw ,nh, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D,0);

  }

  void bind(){
    glBindTexture(GL_TEXTURE_2D,0);
    glBindTexture(GL_TEXTURE_2D,id);
  }

};


namespace renderer{
  struct RectangleShader rectangle=(struct RectangleShader){0};
  struct TextShader text=(struct TextShader){0};
  struct{
    unsigned int v0;
  }vbo;

  vec4 screenBackground=(vec4){1.0,1.0,1.0,1.0};

  ivec4 viewport;

  unsigned int viewportFlags=0;

  void setViewport(int x,int y,int width,int height){
    glViewport(x,y,width,height);
    viewport=(ivec4){.x=x,.y=y,.w=width,.h=height};
  }

  namespace framebuffer{
    GLuint frameBuffer;

    void init(){
      glGenFramebuffers(1,&frameBuffer);
    }

    void fini(){
      glDeleteFramebuffers(1,&frameBuffer);
    }

    void start(Texture* texture){
      ReturnOnError(texture);
      glBindFramebuffer(GL_FRAMEBUFFER,frameBuffer);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->id, 0);
      renderer::setViewport(0,0,texture->w,texture->h);
      renderer::viewportFlags=1;
    }

    void stop(){
      GLenum drawBuffer=GL_COLOR_ATTACHMENT0;
      glDrawBuffers(1,&drawBuffer);
      glBindFramebuffer(GL_FRAMEBUFFER,0);
      renderer::setViewport(0,0,window::width,window::height);
      renderer::viewportFlags=0;
    }

  }


  int initRectangleShader(){
    rectangle.shaderProgram=shader::create(rectangleVertexShader.c_str(),rectangleFragmentShader.c_str());
    ReturnOnError(rectangle.shaderProgram) Error;
    shader::use(rectangle.shaderProgram);
    rectangle.angleIDX=glGetUniformLocation(rectangle.shaderProgram,"cornerRadii");
    rectangle.dimensionIDX=glGetUniformLocation(rectangle.shaderProgram,"rect");
    rectangle.widthHeightIDX=glGetUniformLocation(rectangle.shaderProgram,"resolution");
    rectangle.colorIDX=glGetUniformLocation(rectangle.shaderProgram,"color");
    rectangle.borderColorIDX=glGetUniformLocation(rectangle.shaderProgram,"borderColor");
    rectangle.dpiIDX=glGetUniformLocation(rectangle.shaderProgram,"dpi");
    rectangle.borderThicknessIDX=glGetUniformLocation(rectangle.shaderProgram,"borderThickness");
    rectangle.blurIDX=glGetUniformLocation(rectangle.shaderProgram,"blur");

    rectangle.textureIDX=glGetUniformLocation(rectangle.shaderProgram,"texDimensions");
    rectangle.frameIDX=glGetUniformLocation(rectangle.shaderProgram,"texFrame");
    rectangle.colorManipulationIDX=glGetUniformLocation(rectangle.shaderProgram,"colorManipulation");

    glUniform1f(rectangle.dpiIDX,window::dpiRatio);
    shader::use(0);
    GLenum err=glGetError();
    if(err!=GL_NO_ERROR){
      log("Error Code %d\n",err);
    }
    return Success;
  }

   int initTextShader(){
    text.shaderProgram=shader::create(textVertexShader,textFragmentShader);
    ReturnOnError(text.shaderProgram) Error;
    shader::use(text.shaderProgram);
    text.totalRowColumnIDX=glGetUniformLocation(text.shaderProgram,"totalRowColumn");
    text.currentRowColumnIDX=glGetUniformLocation(text.shaderProgram,"currentRowColumn");
    text.characterIDX=glGetUniformLocation(text.shaderProgram,"character");
    text.backgroundColorIDX=glGetUniformLocation(text.shaderProgram,"backgroundColor");
    text.fontColorIDX=glGetUniformLocation(text.shaderProgram,"fontColor");
    text.rcOffsetIDX=glGetUniformLocation(text.shaderProgram,"rcOffset");
    text.rcSizeIDX=glGetUniformLocation(text.shaderProgram,"rcSize");
    text.dimensionsIDX=glGetUniformLocation(text.shaderProgram,"dimensions");
    shader::use(0);
    GLenum err=glGetError();
    if(err!=GL_NO_ERROR){
      log("Error Code %d\n",err);
    }

    return Success;
  }

  int generateAllRequiredVBO(){
    GLfloat vertices[8]={0.0f,0.0f,0.0f,1.0f,1.0f,0.0f,1.0f,1.0f};
    vbo.v0=shader::generateVBO(0,vertices,8,2,sizeof(GLfloat),GL_FLOAT,GL_STATIC_DRAW);
    ReturnOnError(vbo.v0) Error;
    return Success;
  }



  void clear(float c1,float c2,float c3,float c4){
    glClearColor(c1,c2,c3,c4);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  void clear(vec4 color){
    glClearColor(color.r,color.g,color.b,color.a);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  void clear(ivec4 color){
    glClearColor(color.r*0.00392156862745098,color.g*0.00392156862745098,color.b*0.00392156862745098,color.a*0.00392156862745098);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  void clear(unsigned int color){
    glClearColor(((float)(color>>24))*0.00392156862745098,((float)((color>>16)&0xff))*0.00392156862745098,((float)((color>>8)&0xff))*0.00392156862745098,((float)(color&0xff))*0.00392156862745098);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  void clearScreen(){
    const vec4* cc;
    if(themes::current){
      cc=&themes::current->clearColor[themes::mode];
    }else{
      cc=&themes::material::blue.clearColor[themes::mode];
    }
    glClearColor(cc->r,cc->g,cc->b,cc->a);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  void setScreenBackground(const unsigned int color){
    screenBackground=(vec4){
        .r=(float)((color>>24)*0.00392156862745098),
        .g=(float)(((color>>16)&0xff)*0.00392156862745098),
        .b=(float)(((color>>8)&0xff)*0.00392156862745098),
        .a=(float)((color&0xff)*0.00392156862745098)
    };
  }

  void setScreenBackground(const unsigned int colorr,const unsigned int colorg,const unsigned int colorb,const unsigned int colora){
    screenBackground=(vec4){
      .r=(float)((colorr&0xff)*0.00392156862745098),
      .g=(float)((colorg&0xff)*0.00392156862745098),
      .b=(float)((colorb&0xff)*0.00392156862745098),
      .a=(float)((colora&0xff)*0.00392156862745098)
    };
  }

  void setScreenBackground(float r,float g,float b,float a){
    screenBackground=(vec4){
        .r=r,
        .g=g,
        .b=b,
        .a=a
    };
  }

  void draw(){
    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
  }

  void drawInstanced(unsigned int instance){
    glDrawArraysInstanced(GL_TRIANGLE_STRIP,0,4,instance);
  }

  void rectangleViewPort(int width,int height){
    shader::use(rectangle.shaderProgram);
    glUniform2f(rectangle.widthHeightIDX,(float)(width),(float)(height));
  }

  void resized(){
    if(!viewportFlags)
      setViewport(0,0,window::width,window::height);
  }

  void init(){
    framebuffer::init();
    GLenum error=glGetError();
    if(error!=GL_NO_ERROR){
      log("[ Error ] : Framebuffer creation error\n");
      std::exit(EXIT_FAILURE);
    }

    if(!initRectangleShader()){
      framebuffer::fini();
      log("[ Error ] : Renderer : Could not compile rectangle shader\n");
      std::exit(EXIT_FAILURE);
    }
    if(!initTextShader()){
      framebuffer::fini();
      shader::destroy(rectangle.shaderProgram);
      rectangle=(struct RectangleShader){0};
      log("[ Error ] : Renderer : Could not compile text shader\n");
      std::exit(EXIT_FAILURE);
    }
    if(!generateAllRequiredVBO()){
      framebuffer::fini();
      shader::destroy(rectangle.shaderProgram);
      rectangle=(struct RectangleShader){0};
      shader::destroy(text.shaderProgram);
      text=(struct TextShader){0};
      std::exit(EXIT_FAILURE);
    }
    shader::enableVBO(0);
    setViewport(0,0,window::width,window::height);

    // glDisable(GL_BLEND);
  }

  void fini(){
    shader::deleteVBO(&(vbo.v0));
    framebuffer::fini();
    shader::destroy(rectangle.shaderProgram);
    rectangle=(struct RectangleShader){0};
    shader::destroy(text.shaderProgram);
    text=(struct TextShader){0};
  }

  void startTextRendering(){
    shader::use(text.shaderProgram);
  }

  void startRectangleRendering(){
    shader::use(rectangle.shaderProgram);
  }

  void rectangleDimensions(float x,float y,float w,float h){
    shader::use(rectangle.shaderProgram);
    glUniform4f(rectangle.dimensionIDX,x,y,w,h);
  }

  void setRectangleTextureDimensions(float x,float y,float w,float h){
    shader::use(rectangle.shaderProgram);
    glUniform4f(rectangle.textureIDX,x,y,w,h);
  }

  void setRectangleTextureFrameDimensions(float x,float y,float w,float h){
    shader::use(rectangle.shaderProgram);
    glUniform4f(rectangle.frameIDX,x,y,w,h);
  }

  void setRectangleColor(uint32_t r){
    shader::use(rectangle.shaderProgram);
    glUniform1ui(rectangle.colorIDX,r);
  }

  void setRectangleBorderColor(uint32_t r){
    shader::use(rectangle.shaderProgram);
    glUniform1ui(rectangle.borderColorIDX,r);
  }

  void setRectangleBorderThickness(float thickness){
    shader::use(rectangle.shaderProgram);
    glUniform1i(rectangle.borderThicknessIDX,thickness);
  }

  void setRectangleAngle(float angle){
    shader::use(rectangle.shaderProgram);
    glUniform1i(rectangle.angleIDX,angle);
  }

  void setRectangleBlur(float blur){
    shader::use(rectangle.shaderProgram);
    glUniform1i(rectangle.blurIDX,blur);
  }


  void renderRectangle(vec4& coords,uint32_t color,uint32_t borderColor=0,uint32_t angle=0,uint32_t borderThickness=0,uint32_t blur=0){
    shader::use(rectangle.shaderProgram);
    glUniform4f(rectangle.dimensionIDX,coords.x,coords.y,coords.w,coords.h);
    glUniform4f(rectangle.textureIDX,0.0,0.0,0.0,0.0);
    glUniform1ui(rectangle.colorIDX,color);
    glUniform1ui(rectangle.borderColorIDX,borderColor);
    glUniform1i(rectangle.angleIDX,angle);
    glUniform1i(rectangle.borderThicknessIDX,borderThickness);
    glUniform1i(rectangle.blurIDX,blur);
    glUniform2f(rectangle.widthHeightIDX,(float)(viewport.w),(float)(viewport.h));
    draw();
  }

  void renderRectangle(const Texture* texture,const vec4& coords,const vec4& textureCoords,const vec4& textureFrameCoords,uint32_t texMultiply,uint32_t color,uint32_t borderColor=0,uint32_t angle=0,uint32_t borderThickness=0,uint32_t blur=0){
    shader::use(rectangle.shaderProgram);
    glUniform4f(rectangle.dimensionIDX,coords.x,coords.y,coords.w,coords.h);
    glUniform4f(rectangle.textureIDX,textureCoords.x,textureCoords.y,textureCoords.w,textureCoords.h);
    glUniform4f(rectangle.frameIDX,textureFrameCoords.x,textureFrameCoords.y,textureFrameCoords.w,textureFrameCoords.h);
    glUniform1ui(rectangle.colorIDX,color);
    glUniform1ui(rectangle.borderColorIDX,borderColor);
    glUniform1i(rectangle.angleIDX,angle);
    glUniform1i(rectangle.borderThicknessIDX,borderThickness);
    glUniform1i(rectangle.blurIDX,blur);
    glUniform2f(rectangle.widthHeightIDX,(float)(viewport.w),(float)(viewport.h));
    glUniform1ui(rectangle.colorManipulationIDX,texMultiply);

    glBindTexture(GL_TEXTURE_2D,0);
    if(texture){
      glBindTexture(GL_TEXTURE_2D,texture->id);
    }
    draw();
  }

  void renderRectangle(RectangleDetails& rd){
    shader::use(rectangle.shaderProgram);
    glUniform4f(rectangle.dimensionIDX,rd.coords.x,rd.coords.y,rd.coords.w,rd.coords.h);
    glUniform4f(rectangle.textureIDX,rd.textureCoords.x,rd.textureCoords.y,rd.textureCoords.w,rd.textureCoords.h);
    glUniform4f(rectangle.frameIDX,rd.textureFrameCoords.x,rd.textureFrameCoords.y,rd.textureFrameCoords.w,rd.textureFrameCoords.h);
    glUniform1ui(rectangle.colorIDX,rd.color);
    glUniform1ui(rectangle.borderColorIDX,rd.borderColor);
    glUniform1i(rectangle.angleIDX,rd.angle);
    glUniform1i(rectangle.borderThicknessIDX,rd.borderThickness);
    glUniform1i(rectangle.blurIDX,rd.blur);
    glUniform2f(rectangle.widthHeightIDX,(float)(viewport.w),(float)(viewport.h));
    glUniform1ui(rectangle.colorManipulationIDX,rd.textureColorMultiplier);

    if(rd.texture){
      glBindTexture(GL_TEXTURE_2D,0);
      glBindTexture(GL_TEXTURE_2D,rd.texture->id);
    }
    draw();
  }

  void renderRectangle(RectangleDetails* rd){
    ReturnOnError(rd);
    renderRectangle(*rd);
  }


  void setTextTotalRowColumn(unsigned int row,unsigned int column){
    shader::use(text.shaderProgram);
    glUniform2i(text.totalRowColumnIDX,row,column);
  }

  void setTextRowColumnOffset(unsigned int row,unsigned int column){
    shader::use(text.shaderProgram);
    glUniform2i(text.rcOffsetIDX,row,column);
  }

  void setTextRowColumnSize(unsigned int row,unsigned int column){
    shader::use(text.shaderProgram);
    glUniform2i(text.rcSizeIDX,row,column);
  }

  void setTextCurrentRowColumn(unsigned int row,unsigned int column){
    shader::use(text.shaderProgram);
    glUniform2i(text.currentRowColumnIDX,row,column);
  }

  void setTextRenderingDimensions(vec4 vertex){
    shader::use(text.shaderProgram);
    glUniform4f(text.dimensionsIDX,vertex.x,vertex.y,vertex.w,vertex.h);
  }

  void setTextFontColor(unsigned int color){
    shader::use(text.shaderProgram);
    glUniform1i(text.fontColorIDX,color);
  }

  void setTextBackgroundColor(unsigned int color){
    shader::use(text.shaderProgram);
    glUniform1ui(text.backgroundColorIDX,color);
  }

  void uploadText(GLint* mem,unsigned int size){
    shader::use(text.shaderProgram);
    if(mem && size<=0x20){
      glUniform1iv(text.characterIDX,size,(const GLint*)mem);
    }
  }

  void uploadAndDrawText(GLint* mem,unsigned int size){
    shader::use(text.shaderProgram);
    if(mem && size<=0x20){
      glUniform1iv(text.characterIDX,size,(const GLint*)mem);
      drawInstanced(size);
    }
  }


  void renderShadow(RectangleDetails& rectangleDetails,uint64_t mode){
    if(mode&SETTING_SHADOW_ENABLE){
      RectangleDetails rd;
      rd.borderThickness=0;
      rd.textureCoords=(vec4){0};
      rd.color=0x80;
      rd.texture=0;
      if(mode&SETTING_SHADOW_MASK && mode&SETTING_RENDER){
        rd.angle=rectangleDetails.angle;
        rd.blur=20;
        rd.coords=rectangleDetails.coords;
        if(mode&SETTING_SHADOW_EXTEND_RIGHT)
          rd.coords.w+=0.008;
        if(mode&SETTING_SHADOW_EXTEND_LEFT){
          rd.coords.w+=0.008;
          rd.coords.x-=0.008;
        }
        if(mode&SETTING_SHADOW_EXTEND_TOP)
          rd.coords.h+=0.008;
        if(mode&SETTING_SHADOW_EXTEND_BOTTOM){
          rd.coords.h+=0.008;
          rd.coords.y-=0.008;
        }
      }else{
        rd.coords=(vec4){.x=0,.y=0,.w=1.0,.h=1.0};
        rd.angle=0;
        rd.blur=0;
      }
      renderRectangle(rd);
    }
  }


  void __renderTextInternal(TextDetails& details){

    shader::use(text.shaderProgram);

    void* returnAddress;
    const char* str=details.text+details.textStart;
    const char* strEnd=details.text+details.textEnd;
    uint32_t cr=details.currentRow,cc=details.currentColumn;
    uint32_t trow=details.rowSize,tcolumn=details.columnSize;
    uint32_t ctcolumn;
    uint32_t textBufferIter=0;
    uint32_t t32_1;
    char chr;

    GLint textBuffer[32];


    glUniform1i(text.fontColorIDX,details.fontColor>>8);
    glUniform1ui(text.backgroundColorIDX,0);
    glUniform2i(text.totalRowColumnIDX,details.row,details.column);
    glUniform2i(text.currentRowColumnIDX,details.currentRow,details.currentColumn);
    glUniform2i(text.rcSizeIDX,details.rowSize,details.columnSize);
    glUniform2i(text.rcOffsetIDX,details.rowOffset,details.columnOffset);
    glUniform4f(text.dimensionsIDX,details.boundingBox.x,details.boundingBox.y,details.boundingBox.w,details.boundingBox.h);

    glBindTexture(GL_TEXTURE_2D,0);
    glBindTexture(GL_TEXTURE_2D,details.bitmap->textureID);

    while(cr<trow && str<strEnd){
      chr=*(str++);
      if(chr<' '){
        if(chr=='\n'){
          returnAddress=&&____UI_RENDER_TEXT_UPLOADED_AND_DRAWN_TEXT_AT_NEW_LINE;
          goto ____UI_RENDER_TEXT_UPLOAD_AND_DRAW_TEXT;
          ____UI_RENDER_TEXT_UPLOADED_AND_DRAWN_TEXT_AT_NEW_LINE:
          ++cr;
          cc=0;
          glUniform2i(text.currentRowColumnIDX,cr,0);
        }else if(chr=='\t'){
          t32_1=0;
          while(t32_1++<details.tabSize){
            textBuffer[textBufferIter++]=' ';
            if(textBufferIter&0x20){
              returnAddress=&&____UI_RENDER_TEXT_UPLOADED_AND_DRAWN_TAB;
              goto ____UI_RENDER_TEXT_UPLOAD_AND_DRAW_TEXT;
____UI_RENDER_TEXT_UPLOADED_AND_DRAWN_TAB:;
            }
          }
        }
      }else{
        textBuffer[textBufferIter++]=chr;
        if(textBufferIter&0x20){
          returnAddress=&&____UI_RENDER_TEXT_UPLOADED_AND_DRAWN_TEXT_NORMALLY;
          goto ____UI_RENDER_TEXT_UPLOAD_AND_DRAW_TEXT;
____UI_RENDER_TEXT_UPLOADED_AND_DRAWN_TEXT_NORMALLY:;
        }
      }
    }

    returnAddress=&&____UI_RENDER_TEXT_UPLOADED_AND_DRAWN_TEXT_LAST;
    goto ____UI_RENDER_TEXT_UPLOAD_AND_DRAW_TEXT;
____UI_RENDER_TEXT_UPLOADED_AND_DRAWN_TEXT_LAST:;
    return;

____UI_RENDER_TEXT_UPLOAD_AND_DRAW_TEXT:
    if(textBufferIter){
      glUniform1iv(text.characterIDX,textBufferIter,textBuffer);
      drawInstanced(textBufferIter);
      cc+=textBufferIter;
      while(cc>=tcolumn){
        cc-=tcolumn;
        ++cr;
      }
      glUniform2i(text.currentRowColumnIDX,cr,cc);
      textBufferIter=0;
    }
    goto *returnAddress;


  }

}


  
  

#endif

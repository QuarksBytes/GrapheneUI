#ifndef __UI_CORE_WINDOW__
#define __UI_CORE_WINDOW__

#include<GLES3/gl3.h>
#include<SDL_opengles2.h>



namespace window{
  SDL_Window* window;
  SDL_GLContext glContext;
  long int flags=0;
  int width,height;
  float dpi=0.0;
  float dpiRatio=0.0;
  float widthInverse,heightInverse;

  uint64_t tick=0;

  void swap(){
    SDL_GL_SwapWindow(window);
  }

  void resized(){
    SDL_GetWindowSize(window,&width,&height);
    widthInverse=1.0/width;
    heightInverse=1.0/height;
  }

  bool started(){
    if(flags&UI_STARTED){
      return true;
    }
    return false;
  }

  void init(std::string name,ivec4 dimensions){
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    #ifdef ANDROID
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_ES);
    #else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
    #endif

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL,1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,4);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1");
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED,"0");

    window=SDL_CreateWindow(name.c_str(),dimensions.x,dimensions.y,dimensions.w,dimensions.h,SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE|SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI);
    if(!window){
      log("[ Error ] : Window creation error\n");
      std::exit(EXIT_FAILURE);
    }
    SDL_SetWindowMinimumSize(window, 400,300);
    glContext=SDL_GL_CreateContext(window);
    if(!glContext){
      log("[ Error ] : Window context creation error\n");
      SDL_DestroyWindow(window);
      std::exit(EXIT_FAILURE);
    }

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_BLEND);
    glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
    GLenum error=glGetError();
    if(error!=GL_NO_ERROR){
      SDL_GL_DeleteContext(glContext);
      SDL_DestroyWindow(window);
      log("[ Error ] : Window creation error\n");
      std::exit(EXIT_FAILURE);
    }

    SDL_GL_SetSwapInterval(0);
    SDL_GetWindowSize(window,&width,&height);
    widthInverse=1.0f/width;
    heightInverse=1.0f/height;

    float hdpi,vdpi;
    SDL_GetDisplayDPI(SDL_GetWindowDisplayIndex(window), &dpi, &hdpi, &vdpi);
    dpiRatio=dpi*0.00625f;

  }

  void fini(){
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }

}



#endif

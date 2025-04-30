#ifndef __UI_CORE_DATATYPES__
#define __UI_CORE_DATATYPES__


#define Error 0
#define Success 1

#ifndef ReturnOnError
#define ReturnOnError(x) if(!(x))return 
#endif

#ifndef ReturnOnSuccess
#define ReturnOnSuccess(x) if(x)return 
#endif


union vec2{
  struct{
    float x;
    float y;
  };
  struct{
    float w;
    float h;
  };
  struct{
    float r;
    float g;
  };
  struct{
    float column;
    float row;
  };

  void print(){
    std::cout<<" x "<<x<<" y "<<y<<std::endl;
  }
};

union vec3{
  struct{
    float x;
    float y;
    float z;
  };
  struct{
    float r;
    float g;
    float b;
  };
  void print(){
    std::cout<<" x "<<x<<" y "<<y<<" z "<<z<<std::endl;
  }
};

union vec4{
  struct{
    float x;
    float y;
    float w;
    float h;
  };
  struct{
    float r;
    float g;
    float b;
    float a;
  };
  void print(){
    std::cout<<" x "<<x<<" y "<<y<<" w "<<w<<" h "<<h<<std::endl;
  }
};

union ivec2{
  struct{
    int32_t x;
    int32_t y;
  };
  struct{
    int32_t w;
    int32_t h;
  };
  struct{
    int32_t column;
    int32_t row;
  };
  struct{
    uint32_t r;
    uint32_t g;
  };
  void print(){
    std::cout<<" x "<<x<<" y "<<y<<std::endl;
  }
};

union ivec3{
  struct{
    int32_t x;
    int32_t y;
    int32_t z;
  };
  struct{
    uint32_t r;
    uint32_t g;
    uint32_t b;
  };
  void print(){
    std::cout<<" x "<<x<<" y "<<y<<" z "<<z<<std::endl;
  }
};

union ivec4{
  struct{
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
  };
  struct{
    uint32_t r;
    uint32_t g;
    uint32_t b;
    uint32_t a;
  };
  void print(){
    std::cout<<" x "<<x<<" y "<<y<<" w "<<w<<" h "<<h<<std::endl;
  }
};

union Data{
  int64_t r;
  void* a;
  struct{
    float v1;
    float v2;
  }f;
  struct{
    uint32_t v1;
    uint32_t v2;
  }ud;
  struct{
    int32_t v1;
    int32_t v2;
  }d;
};

class Widget;
class Touch;

class Texture;

struct Callback{
  void (*function)(Callback* callback);
  Data d1,d2;
};


struct Key{
  char* text;
  int64_t keycode;
};

struct FontBitmap{
  uint32_t textureID;
  float whRatio;
};



struct RectangleDetails{
  vec4 coords;
  vec4 textureCoords;
  vec4 textureFrameCoords;
  Texture* texture;
  uint32_t color;
  uint32_t borderColor;
  uint32_t angle;
  uint32_t borderThickness;
  uint32_t blur;
  uint32_t textureColorMultiplier;
};

struct TextDetails{
  vec4 boundingBox;
  const char* text;
  FontBitmap* bitmap;
  int64_t textStart;
  int64_t textEnd;
  uint32_t row,column;
  uint32_t currentRow;
  uint32_t currentColumn;
  uint32_t rowOffset;
  uint32_t columnOffset;
  uint32_t rowSize;
  uint32_t columnSize;
  uint32_t fontColor;
  uint32_t tabSize;
};



// ui setting start
#define set_v4(v) (vec4){.x=v,.y=v,.w=v,.h=v}
#define set_v3(v) (vec3){.x=v,.y=v,.z=v}
#define set_v2(v) (vec2){.x=v,.y=v}

#define set_iv4u(v) (ivec4){.r=v,.g=v,.b=v,.a=v}
#define set_iv4(v) (ivec4){.x=v,.y=v,.w=v,.h=v}
#define set_iv3(v) (ivec3){.x=v,.y=v,.z=v}
#define set_iv2(v) (ivec2){.x=v,.y=v}

#define UI_STARTED 0x1

#define DIMENSION_X 8
#define DIMENSION_Y 4
#define DIMENSION_W 2
#define DIMENSION_H 1

#define UI_DONT_CALL_RESIZE_FUNCTION 0x10

// ui setting end

// Widget setting start

#define SETTING_RENDER (((uint64_t)0x1)<<32)
#define SETTING_TOUCH  (((uint64_t)0x2)<<32)

#define MODE_DOWN   (((uint64_t)0x4)<<32)
#define MODE_MOTION (((uint64_t)0x8)<<32)

#define SETTING_SHADOW_ENABLE (((uint64_t)0x100)<<32)
#define SETTING_SHADOW_EXTEND_BOTTOM (((uint64_t)0x10)<<32)
#define SETTING_SHADOW_EXTEND_RIGHT (((uint64_t)0x20)<<32)
#define SETTING_SHADOW_EXTEND_LEFT (((uint64_t)0x40)<<32)
#define SETTING_SHADOW_EXTEND_TOP (((uint64_t)0x80)<<32)
#define SETTING_SHADOW_MASK (((uint64_t)0xf0)<<32)

// 0x11

#define SETTING_DIMENSION_MASK (((uint64_t)0x1e00)<<32)
#define SETTING_DIMENSION_SHIFT 41

#define SETTING_THEME_SHIFT 45
#define SETTING_THEME_MASK (((uint64_t)0xf)<<SETTING_THEME_SHIFT)


// Widget setting end

#endif

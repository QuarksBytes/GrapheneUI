#ifndef __UI_CORE_THEMES__
#define __UI_CORE_THEMES__



namespace themes{
  enum Color: uint32_t{
    surface=0,    //surface onSurface
    surfaceBright,    //surface onSurface
    surfaceDim,    //surface onSurface
    primary, // primary , onPrimary
    primaryContainer, // primary , onPrimary
    secondary, // secondary onSecondary
    secondaryContainer, // secondary onSecondary
    tertiary, // tertiary,onTertiary
    tertiaryContainer, // tertiary,onTertiary
    error, // error , onError
    errorContainer, // error , onError
    outline, // outline outlineVariant
    TOTAL_PALETTE_TYPES
  };

  enum Mode{
    light=0,
    dark=1
  };
  uint32_t mode=0;

  struct Theme{
    const uint32_t array[2][TOTAL_PALETTE_TYPES][2]; // 2 for light or dark , 2 for normal and on
    const vec4 clearColor[2];

    const uint32_t* at(enum Color color){
      return array[mode&1][color];
    }

    const uint32_t* at(uint64_t color){
      return array[mode&1][color>>SETTING_THEME_SHIFT];
    }


    const uint32_t* operator[](enum Color color){
      return at(color);
    }

    const uint32_t* operator[](uint64_t color){
      return at(color);
    }

    // const uint32_t* primary(){
    //   return array[mode&1][primary];
    // }
    //
    // const uint32_t* primaryContainer(){
    //   return array[mode&1][primaryContainer];
    // }
    //
    // const uint32_t* secondary(){
    //   return array[mode&1][secondary];
    // }
    //
    // const uint32_t* secondaryContainer(){
    //   return array[mode&1][secondaryContainer];
    // }
    //
    // const uint32_t* tertiary(){
    //   return array[mode&1][tertiary];
    // }
    //
    // const uint32_t* tertiaryContainer(){
    //   return array[mode&1][tertiaryContainer];
    // }

  }* current=nullptr;


  namespace material{

    Theme blue=(Theme){
      .array={
        { // light
          {0xf9f9ffffu,0x44474effu},
          {0xf9f9ffffu,0x44474effu},
          {0xd9d9e0ffu,0x44474effu},
          {0x415f91ffu,0xffffffffu},
          {0xd6e3ffffu,0x284777ffu},
          {0x565f71ffu,0xffffffffu},
          {0xdae2f9ffu,0x3e4759ffu},
          {0x705575ffu,0xffffffffu},
          {0xfad8fdffu,0x573e5cffu},
          {0xba1a1affu,0xffffffffu},
          {0xffdad6ffu,0x93000affu},
          {0x74777fffu,0xc4c6d0ffu},
        },
        { // dark
          {0x111318ffu,0xe2e2e9ffu},
          {0x37393effu,0xc4c6d0ffu},
          {0x111318ffu,0xc4c6d0ffu},
          {0xaac7ffffu,0x0a305fffu},
          {0x284777ffu,0xd6e3ffffu},
          {0xbec6dcffu,0x283141ffu},
          {0x3e4759ffu,0xdae2f9ffu},
          {0xddbce0ffu,0x3f2844ffu},
          {0x573e5cffu,0xfad8fdffu},
          {0xffb4abffu,0x690005ffu},
          {0x93000affu,0xffdad6ffu},
          {0x8e9099ffu,0x44474effu},
        }
      },
      .clearColor={(vec4){0.976,0.976,1.0,1.0},(vec4){0.067,0.074,0.094,1.0}}
    };

  }


  enum SyntaxColors{
    ColorDefault=0,
    ColorError,
    ColorPunctuations,
    ColorNumbers,
    ColorConstants,
    ColorVariables,
    ColorFunctions,
    ColorDatatype,
    ColorComment,
    ColorString,
    ColorKeyword,
    ColorComparator,
    ColorLoops,
    ColorImport,
    ColorMacro,
    ColorRegex,
    ColorExtra0,
    ColorExtra1,
    ColorExtra2,
    ColorExtra3,
    ColorExtra4,
    ColorExtra5,
    ColorExtra6,
    ColorExtra7,
    ColorExtra8,
    ColorExtra9,
    ColorExtra10,
    TOTAL_SYNTAX_COLORS
  };



  void setDarkMode(){
    mode=dark;
  }

  void setLightMode(){
    mode=light;
  }

  const vec4& getClearColor(){
    themes::Theme* ctheme;
    if(themes::current){
      ctheme=themes::current;
    }else{
      ctheme=&(themes::material::blue);
    }

    return ctheme->clearColor[themes::mode];
  }

}
#endif







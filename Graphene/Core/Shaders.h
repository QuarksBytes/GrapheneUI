#ifndef __UI_CORE_SHADERS__
#define __UI_CORE_SHADERS__

const std::string rectangleVertexShader=R"(
#version 330 core

layout(location = 0) in vec2 index; // {0,0}, {0,1}, {1,0}, {1,1}
out vec2 uv;
out vec2 coord;
out vec4 colorRect;
out vec4 colorBorder;
out vec4 colorManip;
out vec4 rippleColor;

uniform vec4 rect; // x, y, width, height
uniform vec4 texDimensions;

uniform uint color;
uniform uint borderColor;

uniform uint colorManipulation;

vec4 hexToVec4(uint hex){
  return vec4(float((hex>>24u)&0xffu),float((hex>>16u)&0xffu),float((hex>>8u)&0xffu), float(hex&0xffu))*0.00392156862745098f;
}


void main() {
    // Map index into rect position (in normalized space)
    uv = rect.xy + (index * rect.zw);
    coord.xy=((index-texDimensions.xy)/texDimensions.ba);

    // Convert to clip space [-1, 1]
    vec2 pos = uv * 2.0 - 1.0;
    gl_Position = vec4(pos, 0.0, 1.0);
    colorRect=hexToVec4(uint(color));
    colorBorder=hexToVec4(uint(borderColor));
    colorManip=hexToVec4(colorManipulation);
}

)";

const std::string rectangleFragmentShader=R"(
#version 330 core

in vec2 uv;
in vec2 coord;
in vec4 colorRect;
in vec4 colorBorder;
in vec4 colorManip;
in vec4 rippleColor;

out vec4 fragColor;

uniform vec2 resolution ;
uniform vec4 rect;
uniform vec4 texDimensions;


uniform int cornerRadii;
uniform int borderThickness;
uniform int blur;


// Colors

uniform float dpi;

uniform vec4 texFrame;
uniform sampler2D texSampler;

// ------------------------
// Rounded Box SDF
// ------------------------


float getPxFromDp(int px){
  return float(px)*dpi;
}

float roundedBoxSDF(vec2 p, vec2 halfSize, vec4 radius) {
  vec2 eq=step(vec2(0.0),p);
  radius.xy = (eq.x* radius.xy) + ((1.0-eq.x)* radius.zw);
  radius.x  = (eq.y * radius.x) +((1.0-eq.y) * radius.y);

  vec2 q = abs(p) - halfSize + radius.x;
  return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - radius.x;
}


void main() {

  vec2 bValue=step(vec2(0.0),coord.xy)*step(coord.xy,vec2(1.0));
  vec4 tColor=texture(texSampler,texFrame.xy+(coord.xy*texFrame.ba))*bValue.r*bValue.g*colorManip;
  // tColor=mix(colorRect,tColor,tColor.a);
  tColor=(1.0 - tColor.a) * colorRect + tColor.a * tColor; //mix(colorRect,tColor,tColor.a);

  vec2 rectSizePx = rect.zw * resolution;

  float dist = roundedBoxSDF((uv * resolution) - ((rect.xy * resolution) + rectSizePx * 0.5), rectSizePx * 0.5, vec4(getPxFromDp(cornerRadii)));
  rectSizePx.x= 1.0 - smoothstep(float(blur)*(-dpi),0.0,dist);
  rectSizePx.y=getPxFromDp(borderThickness);

  fragColor = mix(
                    vec4(tColor.rgb,min(tColor.a, rectSizePx.x)),
                    colorBorder,
                    min(
                      colorBorder.a,
                      (1.0-step(rectSizePx.y,0.0))*
                      smoothstep(
                        rectSizePx.y+0.5,
                        rectSizePx.y-0.5,
                        abs(dist)
                      ) * rectSizePx.x
                    )
                  );
  fragColor.rgb*=fragColor.a;

}
)";



const GLchar* textVertexShader=
"  #version 300 es\n"
"  precision lowp float;"
"  layout(location=0)in vec2 index;"
"  uniform ivec2 totalRowColumn;"
"  uniform ivec2 currentRowColumn;"
"  uniform vec4 dimensions;"
"  uniform int character[32];"
"  flat out int text;"
"  out vec2 coord;"
"  out vec2 pixelInv;"
"  "
"  void main(){"
"    ivec2 rc=clamp(ivec2(0),totalRowColumn,currentRowColumn);"
"    rc.g+=gl_InstanceID;"
"    int temp=rc.g/totalRowColumn.g;"
"    rc.g-=(temp*totalRowColumn.g);"
"    rc.r=totalRowColumn.r-(rc.r+temp+1);"
"    text=character[gl_InstanceID];"
"    coord=index;"
"    gl_Position=vec4(((dimensions.rg+(((vec2(rc).gr+index)/vec2(totalRowColumn.gr))*dimensions.ba))*2.0)-1.0,0.0,1.0);"
"  }";

const GLchar* textFragmentShader=R"(
  #version 300 es
  precision lowp float;

  flat in int text;
  in vec2 coord;

  uniform sampler2D bitmap;
  uniform uint backgroundColor;
  uniform int fontColor;
  out vec4 FragColor;

  vec3 hexToVec3(int hex){
    return vec3(float((hex>>16u)&0xff),float((hex>>8u)&0xff), float(hex&0xff))*0.00392156862745098f;
  }

  float linearStep(float edge0, float edge1, float x) {
    return clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
  }
  void main(){
    int chr=clamp(text,0x20,0x7f);
// float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, sdf);

    float alpha=texture(bitmap,(vec2(float(((chr>>4)&0xf)-0x2),float(0xf-(chr&0xff)))+coord)*vec2(0.16666666666666666,0.0625)).a;
    // float smoothing=;
    // smoothing=smoothing-fract(smoothing);
    vec4 tColor=vec4(hexToVec3(fontColor),clamp(smoothstep(0.5 - alpha, 0.5 + alpha, alpha)*2.0,0.0,1.0));

    FragColor=vec4(tColor.rgb*tColor.a,tColor.a);
    // FragColor.rgb*=FragColor.a;

  }
)";

#endif

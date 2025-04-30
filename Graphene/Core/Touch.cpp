#ifndef __UI_CORE_TOUCH__
#define __UI_CORE_TOUCH__

#include"Core/DataTypes.cpp"
#include"Core/Widget.cpp"

#define POINT_IN_WIDGET 0x1
#define POINT_ALSO_IN_WIDGET 0x2

class Touch{
  public:
    struct{
      Widget* controller=NULL;
      void (*function)(Widget*,Touch&)=NULL;
    }motion={0};

    vec2 coords;
    ivec2 coords_int;
    ivec2 coords_rel;
    long unsigned int flags;
    
    int pointInWidget(Widget& widget){
      return (coords.x>widget.coords.x && coords.x<widget.coordEnd.x && coords.y>widget.coords.y && coords.y<widget.coordEnd.y);
    }

    int pointNotInWidget(Widget& widget){
      return (coords.x<widget.coords.x || coords.x>widget.coordEnd.x || coords.y<widget.coords.y || coords.y>widget.coordEnd.y);
    }

    void iterateDown(Widget* root){
      ReturnOnError(root);

      Widget* rootParent=root->parent;
      
      flags=0;
      unsigned int state=POINT_IN_WIDGET;
      int rvalue;
      if(root){
        while(root!=rootParent){
          if(root->flags&SETTING_TOUCH){
            if(root->child){
              root=root->child;
              while(root->next)
                root=root->next;
              continue;
            }
__UI_TOUCH_CHECK_PARENT:
            if(coords.x>root->coords.x && coords.x<root->coordEnd.x && coords.y>root->coords.y && coords.y<root->coordEnd.y){
              flags|=state;
              if(root->onTouchDown(root,*this)&1)
                return;
              flags^=state;
              state=POINT_ALSO_IN_WIDGET;
            }else if(root->onTouchDown(root,*this)&1)
              return;
          }
          if(root!=rootParent && !(root->previous)){
            root=root->parent;
            if(root!=rootParent)
              goto __UI_TOUCH_CHECK_PARENT;
          }
          if(root!=rootParent && root->previous)
            root=root->previous;
        }
      }
    }

    void iterateUp(Widget* root){
      ReturnOnError(root);

      Widget* rootParent=root->parent;
      
      flags=0;
      int rvalue;
      if(root){
        while(root!=rootParent){
          if(root->flags&SETTING_TOUCH){
            if(root->child){
              root=root->child;
              while(root->next)
                root=root->next;
              continue;
            }
            if(root->onTouchUp(root,*this)&1)
              return;
          }
          while(root!=rootParent && !(root->previous)){
            root=root->parent;
            if(root!=rootParent){
              if(root->onTouchUp(root,*this)&1)
                return;
            }
          }
          if(root!=rootParent && root->previous)
            root=root->previous;
        }
      }
    }

};

#endif

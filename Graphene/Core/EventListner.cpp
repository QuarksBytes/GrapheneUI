#ifndef __UI_CORE_EVENT_LISTNER__
#define __UI_CORE_EVENT_LISTNER__

namespace Listner{
  using ListnerFunction=void (*)(void*)

  struct ____internals____listner____EventFired____{
    uint32_t id;
    uint32_t signal;
  };

  struct ____internals____listner____callbacks____{
    ListnerFunction function;
    void* ptr;
    EventFired info;
  };


  lib::vector<____internals____listner____callbacks____> callbacks;
  lib::vector<____internals____listner____EventFired____> eventFired;



  enum Signal{
    None=0,
    TouchDown,
    TouchUp,
    Resize,
    KeyDown,
    KeyUp,
    MouseMotion,
    UserSpaceSignal
  };

  uint32_t getJunction(uint32_t widgetId){
    ____internals____listner____callbacks____* ptr=callbacks.getPointer();
    ____internals____listner____callbacks____* tptr;

    uint32_t start=0,end=callbacks.size(),center;
    while(start<end){
      center=start+((end-start)>>1);
      tptr=ptr+center;

      if(tptr->info.id>widgetId){
        end=center;
      }else if(tptr->info.id<widgetId){
        start=center+1;
      }else{
        return center;
      }
    }
    return start;
  }


  void fire(uint32_t widgetId,uint32_t signal){
    ____internals____listner____EventFired____ info=(____internals____listner____EventFired____){
      .id=widgetId,
      .signal=signal
    };
    eventFired+=info;
  }

  void receive(uint32_t widgetId,uint32_t event,ListnerFunction func,void* ptr){
    if(!func){
      return;
    }
    ____internals____listner____callbacks____ cback=(____internals____listner____callbacks____){
      .function=func,
      .ptr=ptr,
      .info=(____internals____listner____EventFired____){
        .id=widgetId,
        .signal=signal
      }
    };
    callbacks.add(getJunction(widgetId),cback);
  }

  void iterate(){
    ____internals____listner____callbacks____* ptr=callbacks.getPointer();
    ____internals____listner____EventFired____* eptr=eventFired.getPointer();

    ____internals____listner____EventFired____* cptr;
    ____internals____listner____callbacks____* jptr;
    ____internals____listner____callbacks____* jptrend=ptr+callbacks.end();


    uint32_t start=0,end=eventFired.end();

    uint32_t junc;

    while(start<end){
      cptr=eptr+i;
      jptr=ptr+getJunction(cptr->id);
      while(jptr<jptrend && jptr->info.id==cptr->id){
        if(jptr->info.signal==cptr->signal){
          jptr->function(jptr->ptr);
        }
      }
    }


    for(uint32_t ls,ef=0,end=eventFired.end(),lsend=callbacks.end();i<end;++i){
      cptr=ptr+i;
      getJunction(cptr->id);
      for(ls=0;ls<lsend;++ls)

      if(cptr->)
    }
  }


  void reset(){
    eventFired.resize(0);
  }

};

#endif

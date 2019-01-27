#ifndef _CGSIGNAL_H_
#define _CGSIGNAL_H_

#include <DS/DSSignal.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGSignal : public DSSignal
{
  public:    
    CGSignal(DSObject* = NULL,
             DSString* = NULL,
             DSSortRefList* = NULL);
    CGSignal(DSObject*,
             DSString&,
             DSSortRefList* = NULL);
    CGSignal(DSObject*,
             const char *,
             DSSortRefList* = NULL);
   ~CGSignal(void);

    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  protected:
  
    DSResult WriteDefaultConstructor(CGWriter *) const;
    DSResult WriteCopyConstructor(CGWriter *) const;
    DSResult WriteClone(CGWriter *) const;
    DSResult WriteAssign(CGWriter *) const;
    DSResult WriteCompare(CGWriter *) const;
    DSResult WriteEqual(CGWriter *) const;
    DSResult WriteDisplay(CGWriter *) const;
    DSResult WriteStreamOperator(CGWriter *) const;
    DSResult WriteSave(CGWriter *) const;
    DSResult WriteRestore(CGWriter *) const;
    DSResult WriteSize(CGWriter *) const;
    DSResult WriteTypeEnum(CGWriter *) const; 
                                           // CG_WRITE_ENUM
    DSResult WriteParamStruct(CGWriter *) const;
                                           // CG_WRITE_PARAM_STRUCT
    DSResult WriteParam(CGWriter *) const; // CG_WRITE_PARAM
    DSResult WriteTypeHeader(CGWriter *) const; 
                                           // CG_WRITE_TYPE_H
    DSResult WriteTypeImplementation(CGWriter *) const; 
                                           // CG_WRITE_TYPE_CC
    DSResult WriteTypeInit(CGWriter *) const; 
                                           // CG_WRITE_TYPE_INIT
    DSResult WriteTypeFinish(CGWriter *) const; 
                                           // CG_WRITE_TYPE_FINISH
    DSResult WriteCreate(CGWriter *) const;
                                           // CG_WRITE_CREATE
};

#endif

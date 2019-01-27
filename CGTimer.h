#ifndef _CGTIMER_H_
#define _CGTIMER_H_

#include <DS/DSTimer.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGTimer : public DSTimer
{
  public:    
    CGTimer(DSObject* = NULL,
            DSString* = NULL,
            DSSortRefList* = NULL);
    CGTimer(DSObject*,
            DSString&,
            DSSortRefList* = NULL);
    CGTimer(DSObject*,
            const char *,
            DSSortRefList* = NULL);
   ~CGTimer(void);

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
    DSResult WriteTypeEnum(CGWriter *) const; // CG_WRITE_ENUM
    DSResult WriteParamStruct(CGWriter *) const; // CG_WRITE_PARAM_STRUCT
    DSResult WriteParam(CGWriter *) const; // CG_WRITE_PARAM
    DSResult WriteTypeHeader(CGWriter *) const; // CG_WRITE_TYPE_H
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

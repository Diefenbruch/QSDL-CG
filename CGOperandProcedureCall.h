#ifndef _CGOPERANDPROCEDURECALL_H_
#define _CGOPERANDPROCEDURECALL_H_

#include <DS/DSOperandProcedureCall.h>
 
#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGOperandProcedureCall : public DSOperandProcedureCall
{                           
  public:
    CGOperandProcedureCall(DSObject* = NULL,
                           DSCall* = NULL);
   ~CGOperandProcedureCall(void);

    DSObject *New(DSObject * = NULL) const;        // ueberladener Konstruktor

    DSResult  Write(DSWriter *, DSCardinal = 0) const;
};

#endif

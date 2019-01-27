#ifndef _CGOPERANDSTATE_H_
#define _CGOPERANDSTATE_H_

#include <DS/DSOperandState.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGOperandState : public DSOperandState
{                           
  public:
    CGOperandState(DSObject* = NULL,
                   DSStateRef = NULL);
   ~CGOperandState(void);

    DSObject *New(DSObject * = NULL) const;   // virtueller Konstruktor

    DSResult  Write(DSWriter *, DSCardinal = 0) const;

  private:
    DSResult  WriteImplementation(CGWriter *) const;
};

#endif

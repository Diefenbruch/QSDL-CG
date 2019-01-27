#ifndef _CGOPERANDSIGNAL_H_
#define _CGOPERANDSIGNAL_H_

#include <DS/DSOperandSignal.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGOperandSignal : public DSOperandSignal
{                           
  public:
    CGOperandSignal(DSObject* = NULL,
                    DSSignalRef = NULL);
   ~CGOperandSignal(void);

    DSObject *New(DSObject * = NULL) const;   // virtueller Konstruktor

    DSResult  Write(DSWriter *, DSCardinal = 0) const;

  private:
    DSResult  WriteImplementation(CGWriter *) const;
};

#endif

#ifndef _CGOPERANDSENSOR_H_
#define _CGOPERANDSENSOR_H_

#include <DS/DSOperandSensor.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGOperandSensor : public DSOperandSensor
{                           
  public:
    CGOperandSensor(DSObject* = NULL,
                    DSSensorRef = NULL);
   ~CGOperandSensor(void);

    DSObject *New(DSObject * = NULL) const;   // virtueller Konstruktor

    DSResult  Write(DSWriter *, DSCardinal = 0) const;

  private:
    DSResult  WriteImplementation(CGWriter *) const;
};

#endif

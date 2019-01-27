#ifndef _CGOPERANDPROCESSACCESS_H_
#define _CGOPERANDPROCESSACCESS_H_

#include <DS/DSOperandProcessAccess.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGOperandProcessAccess : public DSOperandProcessAccess
{                           
  public:
    CGOperandProcessAccess(DSObject* = NULL,
                           class DSProcessAccess * = NULL);
   ~CGOperandProcessAccess(void);

    DSObject *New(DSObject * = NULL) const;   // virtueller Konstruktor

    DSResult  Write(DSWriter *, DSCardinal = 0) const;

  private:
    DSResult  WriteImplementation(CGWriter *) const;
};

#endif

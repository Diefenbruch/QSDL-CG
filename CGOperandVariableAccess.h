#ifndef _CGOPERANDVARIABLEACCESS_H_
#define _CGOPERANDVARIABLEACCESS_H_

#include <DS/DSOperandVariableAccess.h>
 
#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGOperandVariableAccess : public DSOperandVariableAccess
{                           
  public:
    CGOperandVariableAccess(DSObject* = NULL,
                            DSVariableAccess* = NULL);
   ~CGOperandVariableAccess(void);

    DSObject *New(DSObject * = NULL) const;        // ueberladener Konstruktor

    DSResult  Write(DSWriter *, DSCardinal = 0) const;

  private:
    DSResult  WriteImplementation(CGWriter *) const;
};

#endif

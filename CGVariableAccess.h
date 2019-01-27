#ifndef _CGVARIABLEACCESS_H_
#define _CGVARIABLEACCESS_H_

#include <DS/DSVariableAccess.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGVariableAccess : public DSVariableAccess
{
  public:
    CGVariableAccess(DSObject* = NULL,          // Vaterobjekt
                     DSVariableRef = NULL,      // Referenz auf Deklaration
                     DSVariableAccessElemList* = NULL);
   ~CGVariableAccess(void);

    DSObject *New(DSObject * = NULL) const;        // ueberladener Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;
  private:
    DSResult WriteVariableAccess(CGWriter *) const;
};

#endif

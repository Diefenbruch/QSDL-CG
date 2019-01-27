#ifndef _CGPROCESSACCESS_H_
#define _CGPROCESSACCESS_H_

#include <DS/DSProcessAccess.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGProcessAccess : public DSProcessAccess
{
  public:
    CGProcessAccess(DSObject* = NULL,          // Vaterobjekt
                    DSProcessRef = NULL,       // Referenz auf Prozess
                    DSExpression* = NULL,      // Identifizierungsnummer
                    DSVariableAccess* = NULL);     // Referenz auf Variable
    CGProcessAccess(DSObject*,                 // Vaterobjekt
                    DSProcessRef,              // Referenz auf Prozess
                    DSExpression*,             // Identifizierungsnummer
                    DSSensorRef);              // Referenz auf Sensor
   ~CGProcessAccess(void);

    DSObject *New(DSObject * = NULL) const;        // ueberladener Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;
  private:
    DSResult WriteProcessAccess(CGWriter *) const;
};

#endif

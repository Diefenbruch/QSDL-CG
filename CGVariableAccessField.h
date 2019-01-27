#ifndef _CGVARIABLEACCESSFIELD_H_
#define _CGVARIABLEACCESSFIELD_H_

#include <DS/DSVariableAccessField.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGVariableAccessField : public DSVariableAccessField
{
  public:
    CGVariableAccessField(DSObject* = NULL,       // Vaterobjekt
                          DSNameSortRef = NULL); // Feldzugriff
   ~CGVariableAccessField(void);

    DSObject *New(DSObject * = NULL) const;        // ueberladener Konstruktor

    DSResult  Write(DSWriter *, DSCardinal) const;

  private:
    DSResult  WriteVariableAccessField(CGWriter *) const;
};

#endif

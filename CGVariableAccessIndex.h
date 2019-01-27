#ifndef _CGVARIABLEACCESSINDEX_H_
#define _CGVARIABLEACCESSINDEX_H_

#include <DS/DSVariableAccessIndex.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGVariableAccessIndex : public DSVariableAccessIndex
{
  public:
    CGVariableAccessIndex(DSObject* = NULL,      // Vaterobjekt
                          DSExpression* = NULL); // Index
   ~CGVariableAccessIndex(void);

    DSObject *New(DSObject * = NULL) const;        // ueberladener Konstruktor

    DSResult  Write(DSWriter *, DSCardinal) const;

  private:
    DSResult  WriteVariableAccessIndex(CGWriter *) const;
};

#endif

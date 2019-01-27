#ifndef _CGOPERANDLITERAL_H_
#define _CGOPERANDLITERAL_H_

#include <DS/DSOperandLiteral.h>
 
#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGOperandLiteral : public DSOperandLiteral
{                           
  public:
    CGOperandLiteral(DSObject* = NULL,      // Vaterobjekt, wird an DSOperand
                                            // weitergegeben
                     DSString* = NULL,      // Literalname
                     DSSortRef = NULL);     // Lietralsorte (DSSortExtended)
    CGOperandLiteral(DSObject*,             // Vaterobjekt, wird an DSOperand
                                             // weitergegeben
                     DSString&,
                     DSSortRef = NULL);     // Lietralsorte (DSSortExtended)
    CGOperandLiteral(DSObject*,             // Vaterobjekt, wird an DSOperand
                                             // weitergegeben
                     const char *,
                     DSSortRef = NULL);     // Lietralsorte (DSSortExtended)
   ~CGOperandLiteral(void);

    DSObject *New(DSObject * = NULL) const;              // ueberladener Konstruktor

    DSResult  Write(DSWriter *, DSCardinal = 0) const;

  private:
    DSResult  WriteImplementation(CGWriter *) const;
};

#endif

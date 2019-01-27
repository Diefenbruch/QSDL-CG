#ifndef _CGOPERANDCONSTANT_H_
#define _CGOPERANDCONSTANT_H_

#include <DS/DSOperandConstant.h>
 
#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGOperandConstant : public DSOperandConstant
{                           
  public:
    CGOperandConstant(DSObject* = NULL,      // Vaterobjekt, wird an DSOperand
                                             // weitergegeben
                      DSString* = NULL,      // Konstante
                      DSBoolean = DS_TRUE);  // numerisch (ja/nein)
    CGOperandConstant(DSObject*,             // Vaterobjekt, wird an DSOperand
                                             // weitergegeben
                      DSString&,
                      DSBoolean = DS_TRUE);
    CGOperandConstant(DSObject*,             // Vaterobjekt, wird an DSOperand
                                             // weitergegeben
                      const char *,
                      DSBoolean = DS_TRUE);
   ~CGOperandConstant(void);

    DSObject *New(DSObject * = NULL) const;               // ueberladener Konstruktor

    DSResult  Write(DSWriter *, DSCardinal = 0) const;

  private:
    DSResult  WriteImplementation(CGWriter *) const;
};

#endif

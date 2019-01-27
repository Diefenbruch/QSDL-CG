#ifndef _CGOPERANDEXPRESSION_H_
#define _CGOPERANDEXPRESSION_H_

#include <DS/DSOperandExpression.h>
 
#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGOperandExpression : public DSOperandExpression
{                           
  public:
    CGOperandExpression(DSObject* = NULL,
                        DSExpression* = NULL);
   ~CGOperandExpression(void);

    DSObject *New(DSObject * = NULL) const;        // ueberladener Konstruktor

    DSResult  Write(DSWriter *, DSCardinal = 0) const;

  private:
    DSResult  WriteImplementation(CGWriter *) const;
};

#endif

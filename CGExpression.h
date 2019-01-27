#ifndef _CGEXPRESSION_H_
#define _CGEXPRESSION_H_

#include <DS/DSExpression.h>
 
#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGExpression : public DSExpression
{                           
  public:
    CGExpression(DSObject* = NULL,     // Vaterobjekt
                 DSOperand* = NULL,
                 DSExpressionOperator = DS_OPT_NONE,
                 DSOperand* = NULL,
                 DSSortRef  = NULL);
    CGExpression(CGExpression&);      // Copy-Konstruktor

   ~CGExpression(void);

    DSObject *New(DSObject * = NULL) const;        // ueberladener Konstruktor

    DSResult  Write(DSWriter *, DSCardinal) const;

  private:

    DSResult  WriteOperator(CGWriter *, DSExpressionOperator) const;
    DSResult  WriteImplementation(CGWriter *, DSCardinal) const;
};

#endif

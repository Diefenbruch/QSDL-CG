#ifndef _CGOPERANDOPERATOR_H_
#define _CGOPERANDOPERATOR_H_

#include <DS/DSOperandOperator.h>
 
#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGOperandOperator : public DSOperandOperator
{                           
  public:
    CGOperandOperator(DSObject* = NULL,
                      DSOperatorRef = NULL,       // Operator
                      DSActualParamList* = NULL); // aktuelle Parameter
   ~CGOperandOperator(void);

    DSObject *New(DSObject * = NULL) const;        // ueberladener Konstruktor

    DSResult  Write(DSWriter *, DSCardinal = 0) const;

  private:
    DSResult  WriteImplementation(CGWriter *) const;
};

#endif

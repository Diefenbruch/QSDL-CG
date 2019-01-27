#ifndef _CGTASK_H_
#define _CGTASK_H_

#include <DS/DSTask.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGTask : public DSTask
{                           
  public:
    CGTask(DSObject* = NULL,        // Konstruktoren fuer Variablenzuweisung
           DSString* =     NULL,
           DSVariableAccess* = NULL,
           DSExpression* = NULL);
    CGTask(DSObject*,
           DSString&,
           DSVariableAccess* = NULL,
           DSExpression* = NULL);
    CGTask(DSObject*,
           const char *,
           DSVariableAccess* = NULL,
           DSExpression* = NULL);
   ~CGTask(void);
  
    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  private:
    DSResult WriteImplementation(CGWriter *) const;
    DSResult WriteLabel(CGWriter *) const;
};

#endif

#ifndef _CGSET_H_
#define _CGSET_H_

#include <DS/DSSet.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGSet : public DSSet
{                           
  public:
    CGSet(DSObject* = NULL,           // Parent
          DSString* = NULL,           // Label
          DSTimerRef = NULL,          // Timer
          DSExpressionList* = NULL,   // Indizes
          DSExpression* = NULL);      // Zeit
    CGSet(DSObject*,
          DSString&,
          DSTimerRef = NULL,
          DSExpressionList* = NULL,
          DSExpression* = NULL);
    CGSet(DSObject*,
          const char *,
          DSTimerRef = NULL,
          DSExpressionList * = NULL,
          DSExpression* = NULL);
   ~CGSet(void);
  
    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  private:
    DSResult WriteImplementation(CGWriter *) const;
    DSResult WriteLabel(CGWriter *) const;
};

#endif

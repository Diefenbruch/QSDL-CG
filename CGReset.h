#ifndef _CGRESET_H_
#define _CGRESET_H_

#include <DS/DSReset.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGReset : public DSReset
{                           
  public:
    CGReset(DSObject*         = NULL,
            DSString*         = NULL,     // Label
            DSTimerRef        = NULL,     // Timer-Objekt
            DSExpressionList* = NULL);    // Indizes
    CGReset(DSObject*,
            DSString&,
            DSTimerRef        = NULL,
            DSExpressionList* = NULL);
    CGReset(DSObject*,
            const char *,
            DSTimerRef        = NULL,
            DSExpressionList* = NULL);
   ~CGReset(void);
  
    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  private:
    DSResult WriteImplementation(CGWriter *) const;
    DSResult WriteLabel(CGWriter *) const;
};

#endif

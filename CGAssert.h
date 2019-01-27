#ifndef _CGASSERT_H_
#define _CGASSERT_H_

#include <DS/DSAssert.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGAssert : public DSAssert
{                           
  public:
    CGAssert(DSObject*   = NULL,     // Vater-Objekt
             DSString*   = NULL,     // Label
             DSExpression * = NULL); // Expression-Objekt
    CGAssert(DSObject*,
             DSString&,
             DSExpression * = NULL);
    CGAssert(DSObject*,
             const char *,
             DSExpression * = NULL);
   ~CGAssert(void);
  
    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  private:
    DSResult WriteImplementation(CGWriter *) const;
    DSResult WriteLabel(CGWriter *) const;
};

#endif

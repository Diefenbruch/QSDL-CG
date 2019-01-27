#ifndef _CGRETURN_H_
#define _CGRETURN_H_

#include <DS/DSReturn.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGReturn : public DSReturn
{                           
  public:
    CGReturn(DSObject* = NULL,             // Vaterobjekt
             DSExpression* = NULL,         // Rueckgabewert
             DSString* = NULL);            // Label
    CGReturn(DSObject*,                    // Vaterobjekt
             DSExpression*,                // Rueckgabewert
             DSString&);
    CGReturn(DSObject*,                    // Vaterobjekt
             DSExpression*,                // Rueckgabewert
             const char *);
   ~CGReturn(void);

    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  private:
    DSResult WriteImplementation(CGWriter *) const;
    DSResult WriteLabel(CGWriter *) const;
};

#endif

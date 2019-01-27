#ifndef _CGSTOP_H_
#define _CGSTOP_H_

#include <DS/DSStop.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGStop : public DSStop
{                           
  public:
    CGStop(DSObject* = NULL,             // Vaterobjekt
           DSString* = NULL);            // Label
    CGStop(DSObject*,                    // Vaterobjekt
           DSString&);
    CGStop(DSObject*,                    // Vaterobjekt
           const char *);
   ~CGStop(void);
  
    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  private:
    DSResult WriteImplementation(CGWriter *) const;
    DSResult WriteLabel(CGWriter *) const;
};

#endif

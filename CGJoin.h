#ifndef _CGJOIN_H_
#define _CGJOIN_H_

#include <DS/DSJoin.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGJoin : public DSJoin
{                           
  public:
    CGJoin(DSObject* = NULL,             // Vaterobjekt
           DSString* = NULL,             // Connector
           DSString* = NULL);            // Label
    CGJoin(DSObject*,                    // Vaterobjekt
           DSString&,
           DSString&);
    CGJoin(DSObject*,                    // Vaterobjekt
           const char *,
           const char *);
   ~CGJoin(void);
  
    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  private:
    DSResult WriteImplementation(CGWriter *) const;
    DSResult WriteLabel(CGWriter *) const;
};

#endif

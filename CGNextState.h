#ifndef _CGNEXTSTATE_H_
#define _CGNEXTSTATE_H_

#include <DS/DSNextState.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGNextState : public DSNextState
{                           
  public:
    CGNextState(DSObject* = NULL,   // Vaterobjekt
                DSString* = NULL,   // optionales Label
                DSStateRef = NULL); // naechster Zustand
    CGNextState(DSObject*,
                DSString&,
                DSStateRef =    NULL);
    CGNextState(DSObject*,
                const char *,
                DSStateRef =    NULL);
   ~CGNextState(void);
  
    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  private:
    DSResult WriteImplementation(CGWriter *) const;
    DSResult WriteLabel(CGWriter *) const;
};

#endif

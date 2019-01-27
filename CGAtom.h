#ifndef _CGATOM_H_
#define _CGATOM_H_

#include <DS/DSAtom.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGAtom : public DSAtom
{
  public:
    CGAtom(DSObject* = NULL,           // Vaterobjekt
               DSString* = NULL,       // Name des Atoms
               DSExpression* = NULL);  // Proposition
    CGAtom(DSObject*,                  // Vaterobjekt
               DSString&,              // Name der Atoms
               DSExpression*);         // Proposition
    CGAtom(DSObject*,                  // Vaterobjekt
               const char *,           // Name der Atomn
               DSExpression*);         // Proposition
   ~CGAtom(void);

    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  private:
    DSResult WriteHeader(CGWriter *) const; // CG_WRITE_H
    DSResult WriteImplementation(CGWriter *) const; //CG_WRITE_CC 
    DSResult WriteInitialize(CGWriter *) const; //CG_WRITE_INIT
};

#endif

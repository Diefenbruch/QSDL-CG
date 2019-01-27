#ifndef _CGSYNONYM_H_
#define _CGSYNONYM_H_

#include <DS/DSSynonym.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGSynonym : public DSSynonym
{
  public:
    CGSynonym(DSObject* = NULL,                     // Vater-Objekt
              DSString* = NULL,                     // Name,
              DSExpression* = NULL,                 // Default-Wert,
              DSSortRef = NULL);                    // Sorte
    CGSynonym(DSObject*,                            // Vater-Objekt
              DSString&,                            // Name,
              DSExpression* = NULL,                 // Default-Wert,
              DSSortRef = NULL);                    // Sorte
    CGSynonym(DSObject*,                            // Vater-Objekt
              const char *,                         // Name,
              DSExpression* = NULL,                 // Default-Wert,
              DSSortRef = NULL);                    // Sorte
   ~CGSynonym(void);

    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  private:
    DSResult WriteHeader(CGWriter *) const; // CG_WRITE_H
    DSResult WriteImplementation(CGWriter *) const; // CG_WRITE_CC
    DSResult WriteInit(CGWriter *) const; // CG_WRITE_INIT
    DSResult WriteIdentifier(CGWriter *) const; // CG_WRITE_IDENTIFIER
};

#endif

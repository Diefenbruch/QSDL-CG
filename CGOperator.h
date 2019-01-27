#ifndef _CGOPERATOR_H_
#define _CGOPERATOR_H_

#include <DS/DSOperator.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGOperator : public DSOperator
{
  public:
    CGOperator(DSObject* = NULL,                /* Vaterobjekt               */
               DSString* = NULL,                /* Name des Operators        */
               DSSortRefList* = NULL,           /* Parametertypen            */
               DSSortRef = NULL);               /* Ergebnistyp               */
    CGOperator(DSObject*,                       /* Vaterobjekt               */
               DSString&,                       /* Name des Operators        */
               DSSortRefList* = NULL,           /* Parametertypen            */
               DSSortRef = NULL);               /* Ergebnistyp               */
    CGOperator(DSObject*,                       /* Vaterobjekt               */
               const char *,                    /* Name des Operators        */
               DSSortRefList* = NULL,           /* Parametertypen            */
               DSSortRef = NULL);               /* Ergebnistyp               */
   ~CGOperator(void);

    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;
  private:
    DSResult WriteImplementation(CGWriter *) const;
};

#endif

#ifndef _CGTRANSITION_H_
#define _CGTRANSITION_H_

#include <DS/DSTransition.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGTransition : public DSTransition
{                           
  public:
    CGTransition(DSObject* = NULL,           // Vaterobjekt
                 DSActionList* = NULL);
   ~CGTransition(void);
  
    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult         Write(DSWriter *, DSCardinal) const;

    // Die folgenden Methoden muessen redefinert werden
    // da dynamisch Objekte kreiert werden:
    virtual DSResult BuildSpontaneousInput(DSState *, DSState *,
                                           DSTransition *);
    virtual DSResult BuildContinuousSignal(DSState *, DSState *,
                                           DSTransition *,
                                           DSExpression *, DSRange *);
    virtual DSResult DissolveDecision(DSDecision *,
                                      DSObject *,
                                      DSString *,
                                      DSCardinal &);

  private:
    DSResult         WriteInit(CGWriter *) const;
    DSResult         WriteImplementation(CGWriter *) const;

};

#endif

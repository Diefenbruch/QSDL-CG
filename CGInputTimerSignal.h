#ifndef _CGINPUTTIMERSIGNAL_H_
#define _CGINPUTTIMERSIGNAL_H_

#include <DS/DSInputTimerSignal.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

/******************************************************************************
 * Klasse CGInputTimerSignal
 * Zweck: Steuert die Codegenerierung fuer die SDL-Inputs: signalkonsumierende
 *        und spontane Transitonen
 ******************************************************************************/

class CGInputTimerSignal : public DSInputTimerSignal
{
  public:
    CGInputTimerSignal(DSObject*             = NULL,     // Vaterobjekt
                       DSTimerRef            = NULL,
                       DSVariableAccessList* = NULL);
   ~CGInputTimerSignal(void);

    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  private:
    DSResult WriteImplementation(CGWriter *) const;
};

#endif

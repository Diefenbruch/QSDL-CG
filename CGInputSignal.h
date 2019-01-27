#ifndef _CGINPUTSIGNAL_H_
#define _CGINPUTSIGNAL_H_

#include <DS/DSInputSignal.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

/******************************************************************************
 * Klasse CGInputSignal
 * Zweck: Steuert die Codegenerierung fuer die SDL-Inputs: signalkonsumierende
 *        und spontane Transitonen
 ******************************************************************************/

class CGInputSignal : public DSInputSignal
{
  public:
    CGInputSignal(DSObject* = NULL,                 // Vaterobjekt
                  DSSignalRef           = NULL,
                  DSVariableAccessList* = NULL);
   ~CGInputSignal(void);

    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  private:
    DSResult WriteImplementation(CGWriter *) const;
};

#endif

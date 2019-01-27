#ifndef _CGINPUT_H_
#define _CGINPUT_H_

#include <DS/DSInput.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

/******************************************************************************
 * Klasse CGInput
 * Zweck: Steuert die Codegenerierung fuer die SDL-Inputs: signalkonsumierende
 *        und spontane Transitonen
 ******************************************************************************/

class CGInput : public DSInput
{
  public:
    CGInput(DSObject* = NULL,          // 1. Parameter: Vaterobjekt
            DSBoolean = DS_FALSE,      // 2. Parameter: Prioritaets-Input (j/n)
            DSBoolean = DS_FALSE,      // 3. Parameter: Asterisk-Input (j/n)
            DSBoolean = DS_FALSE,      // 4. Parameter: Spontanous-Input (j/n)
            DSInputSignalList* = NULL, // 5. Parameter: Signalliste
            DSInputTimerSignalList* = NULL, // 6. Parameter: Timer-Liste
            DSExpression* = NULL,      // 7. Parameter: Enabling-Condition
            DSTransition* = NULL);     // 8. Parameter: Transition
   ~CGInput(void);

    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  private:
    DSResult WriteInputSetHeader(CGWriter *) const; // CG_WRITE_H
    DSResult WriteInputSetImplementation(CGWriter *) const; // CG_WRITE_CC
    DSResult WriteImplementation(CGWriter *) const;
};

#endif

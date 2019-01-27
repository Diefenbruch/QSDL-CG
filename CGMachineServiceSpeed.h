#ifndef _CGMACHINESERVICESPEED_H_
#define _CGMACHINESERVICESPEED_H_

#include <DS/DSMachineServiceSpeed.h>
 
#include "CGBasicTypes.h"
#include "CGWriter.h"

/******************************************************************************
 * Klasse CGMachineServiceSpeed
 * Zweck: Steuert die Codegenerierung fuer verschiedenen Maschinentypen, die
 *        von der SCL angeboten werden
 ******************************************************************************/

class CGMachineServiceSpeed : public DSMachineServiceSpeed
{
  public:
    CGMachineServiceSpeed(DSObject* = NULL,
                           DSMachineServiceRef = NULL,
                           DSExpression* = NULL);
   ~CGMachineServiceSpeed(void);

    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  private:
    DSResult WriteImplementation(CGWriter *) const;
};

#endif  // ifndef

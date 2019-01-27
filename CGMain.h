#ifndef _CGMAIN_H_
#define _CGMAIN_H_

#include <DS/DSSystem.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"
 
/******************************************************************************
 * Klasse CGMain
 * Zweck: Schreibt das Main-Programm der Simulation raus. Das erzeugte Main-
 *        Programm kann ggf. Laufzeitparameter wie Ausgabedatei oder Simula-
 *        tionszeit parsen und verarbeiten 
 ******************************************************************************/

class CGMain 
{                           
  public:
    CGMain(void);
   ~CGMain(void);

    DSResult CGWriteMain(CGWriter *) const;
    
  private:
    DSResult CGWriteParseOptions(CGWriter *) const;
    DSResult CGWriteMessageFunction(CGWriter *) const;
    DSResult CGWriteHelpFunction(CGWriter *) const;
};

#endif

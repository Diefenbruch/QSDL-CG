#ifndef _CGMACHINESERVICE_H_
#define _CGMACHINESERVICE_H_

#include <DS/DSMachineService.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

/******************************************************************************
 * Klasse CGMachineService
 * Zweck: Steuert die Codegenerierung fuer Maschinendienste
 ******************************************************************************/

class CGMachineService : public DSMachineService
{
  public:
    CGMachineService(DSObject* = NULL,
                     DSString* = NULL,
                     DSSortRefList* = NULL);  // Discipline
    CGMachineService(DSObject*,
                     DSString&,
                     DSSortRefList* = NULL);  // Discipline
    CGMachineService(DSObject*,
                     const char*,
                     DSSortRefList* = NULL);  // Discipline
   ~CGMachineService(void);

    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;
    
  protected:
    DSResult WriteEnum(CGWriter *) const; // CG_WRITE_ENUM
    DSResult WriteImplementation(CGWriter *) const; // CG_WRITE_CC 
    DSResult WriteInit(CGWriter *) const; // CG_WRITE_INIT
    DSResult WriteFinish(CGWriter *) const; // CG_WRITE_FINISH
    DSResult WriteHeader(CGWriter *) const; // CG_WRITE_H 
};

#endif  // ifndef

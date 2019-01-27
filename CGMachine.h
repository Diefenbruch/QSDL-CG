#ifndef _CGMACHINE_H_
#define _CGMACHINE_H_

#include <DS/DSMachine.h>
 
#include "CGBasicTypes.h"
#include "CGWriter.h"

/******************************************************************************
 * Klasse CGMachine
 * Zweck: Steuert die Codegenerierung fuer verschiedenen Maschinentypen, die
 *        von der SCL angeboten werden
 ******************************************************************************/

class CGMachine : public DSMachine
{
  public:
    CGMachine(DSObject* = NULL,
              DSString* = NULL,
              DSExpression* = NULL,   // Server
              DSExpression* = NULL);  // Discipline
    CGMachine(DSObject*,
              DSString&,
              DSExpression* = NULL,   // Server
              DSExpression* = NULL);  // Discipline
    CGMachine(DSObject*,
              const char*,
              DSExpression* = NULL,   // Server
              DSExpression* = NULL);  // Discipline
   ~CGMachine(void);

    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;
    
    DSResult WriteIdentifier(CGWriter *) const; // CG_WRITE_IDENTIFIER

    DSString & GetIdentifier(void) const;

  protected:
    DSResult WriteInclude(CGWriter *) const; // CG_WRITE_INCLUDE
    DSResult WriteHeader(CGWriter *) const; // CG_WRITE_H
    DSResult WriteImplementation(CGWriter *) const; // CG_WRITE_CC
    DSResult WriteEnum(CGWriter *) const; // CG_WRITE_ENUM
    DSResult WriteSpeedHeader(CGWriter *) const; // CG_WRITE_MACHINE_SPEED_H
    DSResult WriteSpeedImplementation(CGWriter *) const; 
                                                // CG_WRITE_MACHINE_SPEED_CC
    DSResult WriteInit(CGWriter *) const; // CG_WRITE_INIT
    DSResult WriteCreate(CGWriter *) const; // CG_WRITE_CC
    DSResult WriteDefinition(CGWriter *) const; // CG_WRITE_H
    DSResult WriteSchedule(CGWriter *) const; // CG_WRITE_MACHINE_SCHEDULE
    DSResult WriteDisplay(CGWriter *) const;
    DSResult WriteStreamOperator(CGWriter *) const;
    DSResult WriteSave(CGWriter *) const; // CG_WRITE_SAVE
    DSResult WriteRestore(CGWriter *) const; // CG_WRITE_RESTORE
    DSResult WriteSize(CGWriter *) const; // CG_WRITE_SIZE
    DSResult WriteSensorUpdate(CGWriter *) const; // CG_WRITE_UPDATE
};

#endif  // ifndef

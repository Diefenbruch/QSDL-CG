#ifndef CG_SENSORSTATE_H_
#define CG_SENSORSTATE_H_

#include "CGBasicTypes.h"
#include "CGWriter.h"
#include <DS/DSSensorState.h>

/************************************************************************
 * Classname: CGSensorState
 * Purpose:   Definition eines zustandsbezogenen Sensors
 ************************************************************************/

class CGSensorState : public DSSensorState
{
  public:
              CGSensorState (DSObject* = NULL,
                             DSSensorType = DS_SENSOR_ILLEGAL,
                             DSString* = NULL,
                             DSStateRef pcs = NULL);
              CGSensorState (DSObject*,
                             DSSensorType,
                             DSString&,
                             DSStateRef = NULL);
              CGSensorState (DSObject*,
                             DSSensorType,
                             const char *,
                             DSStateRef = NULL);
             ~CGSensorState(void);

    DSObject *New(DSObject * = NULL) const;

    DSResult  Write(DSWriter *, DSCardinal) const;

    DSResult WriteIdentifier(CGWriter *) const;   // CG_WRITE_IDENTIFIER

  private:
    DSResult  WriteSensor(CGWriter *) const;      // CG_WRITE_H
    DSResult  WriteInit(CGWriter *) const;        // CG_WRITE_CC 
    DSResult  WriteSave(CGWriter *) const;        // CG_WRITE_SAVE 
    DSResult  WriteRestore(CGWriter *) const;     // CG_WRITE_RESTORE 
    DSResult  WriteSize(CGWriter *) const;        // CG_WRITE_SIZE
    DSResult  WriteHistorySize(CGWriter *) const; // CG_WRITE_HISTORY_SIZE
    DSResult  WriteUpdate(CGWriter *) const;      // CG_WRITE_UPDATE
    DSResult  WriteCreate(CGWriter *) const;      // CG_WRITE_CREATE
    DSResult  WriteDestroy(CGWriter *) const;     // CG_WRITE_DESTROY
};

#endif

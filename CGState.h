#ifndef _CGSTATE_H_
#define _CGSTATE_H_

#include <DS/DSState.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGState : public DSState
{
  public:
    CGState(DSObject* = NULL,         // 1. Parameter: Vaterobjekt
            DSStringKeyList* = NULL,  // 2. Parameter: Namen des Zustands
            DSBoolean = DS_FALSE);    // 3. Parameter: Zwischenzustand ?
    CGState(DSObject*,                // 1. Parameter: Vaterobjekt
            DSString*,                // 2. Parameter: 1. Name des Zustands
            DSBoolean = DS_FALSE);    // 3. Parameter: Zwischenzustand ?
    CGState(DSObject*,                // 1. Parameter: Vaterobjekt
            DSString&,                // 2. Parameter: 1. Name des Zustands
            DSBoolean = DS_FALSE);    // 3. Parameter: Zwischenzustand ?
    CGState(DSObject*,                // 1. Parameter: Vaterobjekt
            const char *,             // 2. Parameter: 1. Name des Zustands
            DSBoolean = DS_FALSE);    // 3. Parameter: Zwischenzustand ?
   ~CGState(void);

    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  private:
    DSResult WriteSaveSetHeader(CGWriter *) const; // CG_WRITE_STATE_SAVE_SET_H
    DSResult WriteSaveSetImplementation(CGWriter *) const; // CG_WRITE_STATE_SAVE_SET_CC
    DSResult WriteSetEnablingConditions(CGWriter *) const;
                                      //CG_WRITE_STATE_SET_ENABLING_CONDITION
    DSResult WriteClearEnablingConditions(CGWriter *) const;
                                      //CG_WRITE_STATE_SET_ENABLING_CONDITION
    DSResult WriteImplementation(CGWriter *) const; // CG_WRITE_CC
    DSResult WriteTypeHeader(CGWriter *) const; // CG_WRITE_TYPE_H
    DSResult WriteTypeImplementation(CGWriter *) const;
                                                // CG_WRITE_TYPE_CC
    DSResult WriteTypeInit(CGWriter *) const;  // CG_WRITE_TYPE_INIT
    DSResult WriteTypeFinish(CGWriter *) const;  // CG_WRITE_TYPE_FINISH
    DSResult WriteTypeEnum(CGWriter *) const; // CG_WRITE_STATE_TYPE_ENUM
    DSResult WriteInputImplementation(CGWriter *) const;
    DSResult WriteTransitionImplementation(CGWriter *) const;

    // die folgenden Funktionen laufen nur Listen durch:
    DSResult WriteInputList(CGWriter *, DSCardinal) const;
    DSResult WriteTransitionList(CGWriter *, DSCardinal) const;
};

#endif

#ifndef _CGPROCESS_H_
#define _CGPROCESS_H_

#include <DS/DSString.h> 
#include <DS/DSProcess.h>
 
#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGProcess : public DSProcess 
{
  public:
    CGProcess(DSObject* = NULL,
              DSString* = NULL,
              DSExpression* = NULL,  // initiale Anzahl von Instanzen
              DSExpression* = NULL,  // maximale Anzahl von Instanzen
              DSExpression* = NULL); // maximale Queue-Laenge);
    CGProcess(DSObject*,
              DSString&,
              DSExpression* = NULL,  // initiale Anzahl von Instanzen
              DSExpression* = NULL,  // maximale Anzahl von Instanzen
              DSExpression* = NULL); // maximale Queue-Laenge);
    CGProcess(DSObject*,
              const char *,
              DSExpression* = NULL,  // initiale Anzahl von Instanzen
              DSExpression* = NULL,  // maximale Anzahl von Instanzen
              DSExpression* = NULL); // maximale Queue-Laenge);
   ~CGProcess(void);

    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;
    
    DSResult WriteIdentifier(CGWriter *) const; // CG_WRITE_IDENTIFIER

    DSString & GetIdentifier(void) const;

  private:
    DSResult WriteInitialize(CGWriter *) const; // CG_WRITE_INIT
    DSResult WriteInitializeImplementation(CGWriter *) const;
    DSResult WriteFinish(CGWriter *) const; // CG_WRITE_FINISH
    DSResult WriteFinishImplementation(CGWriter *) const; // CG_WRITE_FINISH
    DSResult WriteInclude(CGWriter *) const; // CG_WRITE_INCLUDE
    DSResult WriteEnum(CGWriter *) const; // CG_WRITE_PROCESS_ENUM
    DSResult WriteParamStruct(CGWriter *) const; // CG_WRITE_PARAM_STRUCT
    DSResult WriteParam(CGWriter *) const; // CG_WRITE_PARAM
    DSResult WriteTypeHeader(CGWriter *) const; // CG_WRITE_TYPE_H
    DSResult WriteTypeImplementation(CGWriter *) const; // CG_WRITE_TYPE_CC:
    DSResult WriteTypeInit(CGWriter *) const; // CG_WRITE_TYPE_INIT:
    DSResult WriteTypeFinish(CGWriter *) const; // CG_WRITE_TYPE_FINISH:
    DSResult WriteHeader(CGWriter *) const; // CG_WRITE_H
    DSResult WriteImplementation(CGWriter *) const; // CG_WRITE_CC
    DSResult WriteParamDefaultConstructor(CGWriter *) const;
    DSResult WriteParamCopyConstructor(CGWriter *) const;
    DSResult WriteParamClone(CGWriter *) const;
    DSResult WriteParamAssign(CGWriter *) const;
    DSResult WriteParamCompare(CGWriter *) const;
    DSResult WriteParamEqual(CGWriter *) const;
    DSResult WriteParamDisplay(CGWriter *) const;
    DSResult WriteParamStreamOperator(CGWriter *) const;
    DSResult WriteParamSave(CGWriter *) const;
    DSResult WriteParamRestore(CGWriter *) const;
    DSResult WriteDisplay(CGWriter *) const;
    DSResult WriteStreamOperator(CGWriter *) const;
    DSResult WriteSave(CGWriter *) const; // CG_WRITE_SAVE
    DSResult WriteRestore(CGWriter *) const; // CG_WRITE_RESTORE
    DSResult WriteSize(CGWriter *) const; // CG_WRITE_SIZE
    DSResult WriteSensorUpdate(CGWriter *) const;
    DSResult WriteConstructor(CGWriter *) const;
    DSResult WriteDestructor(CGWriter *) const;
};

#endif    

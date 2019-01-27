#ifndef _CGVARIABLE_H_
#define _CGVARIABLE_H_

#include <DS/DSVariable.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGVariable : public DSVariable
{
  public:
    CGVariable(DSObject* = NULL,       // Vaterobjekt
               DSString* = NULL,       // Name der Variablen
               DSSortRef = NULL,       // Datentyp
               DSExpression* = NULL,   // Initialisierung
               DSBoolean = DS_FALSE,   // Revealed (j/n)
               DSBoolean = DS_FALSE,   // Exported (j/n)
               DSString* = NULL);      // Exported as (Name)
    CGVariable(DSObject*,              // Vaterobjekt
               DSString&,              // Name der Variablen
               DSSortRef,              // Datentyp
               DSExpression*,          // Initialisierung
               DSBoolean,              // Revealed (j/n)
               DSBoolean,              // Exported (j/n)
               DSString&);             // Exported as (Name)
    CGVariable(DSObject*,              // Vaterobjekt
               const char *,           // Name der Variablen
               DSSortRef,              // Datentyp
               DSExpression*,          // Initialisierung
               DSBoolean,              // Revealed (j/n)
               DSBoolean,              // Exported (j/n)
               const char* = NULL);    // Exported as (Name)
    // spezielle Konstruktoren fuer formale Parameter:
    CGVariable(DSObject*,              // Vaterobjekt
               DSString*,              // Name des Parameters
               DSSortRef = NULL,       // Datentyp
               DSBoolean = DS_FALSE);  // IN/OUT-Parameter (j/n)
    CGVariable(DSObject*,              // Vaterobjekt
               DSString&,              // Name des Parameters
               DSSortRef = NULL,       // Datentyp
               DSBoolean = DS_FALSE);  // IN/OUT-Parameter (j/n)
    CGVariable(DSObject*,              // Vaterobjekt
               const char *,           // Name des Parameters
               DSSortRef = NULL,       // Datentyp
               DSBoolean = DS_FALSE);  // IN/OUT-Parameter (j/n)
   ~CGVariable(void);

    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  private:
    DSResult WriteVariable(CGWriter *) const; // CG_WRITE_H
    DSResult WriteInit(CGWriter *) const; //CG_WRITE_CC 
    DSResult WriteProcessFormalParamImplementation(CGWriter *) const;
                                         //CG_WRITE_PROCESS_FORMAL_PARAM_CC 
    DSResult WriteProcessFormalParamHeader(CGWriter *) const;
                                         // CG_WRITE_PROCESS_FORMAL_PARAM_H
    DSResult WriteProcedureFormalParamImplementation(CGWriter *) const;
                                         // CG_WRITE_PROCEDURE_FORMAL_PARAM_CC
    DSResult WriteProcedureFormalParamHeader(CGWriter *) const;
                                        //CG_WRITE_PROCEDURE_FORMAL_PARAM_H 
    DSResult WriteSave(CGWriter *) const;
                                        //CG_WRITE_SAVE 
    DSResult WriteRestore(CGWriter *) const;
                                        //CG_WRITE_RESTORE 
    DSResult WriteSize(CGWriter *) const;
                                        //CG_WRITE_SIZE
    DSResult WriteHistorySize(CGWriter *) const;
                                        //CG_WRITE_HISTORY_SIZE
};

#endif

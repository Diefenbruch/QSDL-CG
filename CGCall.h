#ifndef _CGCALL_H_
#define _CGCALL_H_

#include <DS/DSCall.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGCall : public DSCall
{                           
  public:
    CGCall(DSObject*          = NULL,
           DSString*          = NULL,                   // Label
           DSProcedureRef     = NULL,
           DSActualParamList* = NULL);
    CGCall(DSObject*,
           DSString&,
           DSProcedureRef     = NULL,
           DSActualParamList* = NULL);
    CGCall(DSObject*,
           const char *,
           DSProcedureRef     = NULL,
           DSActualParamList* = NULL);
   ~CGCall(void);

    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  private:
    DSResult WriteImplementation(CGWriter *) const;
    DSResult WriteLabel(CGWriter *) const;
};

#endif

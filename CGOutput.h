#ifndef _CGOUTPUT_H_
#define _CGOUTPUT_H_

#include <DS/DSOutput.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGOutput : public DSOutput
{                           
  public:
    CGOutput(DSObject*          = NULL,    // Vaterobjekt
             DSString*          = NULL,    // Label
             DSSignalRef        = NULL,    // Output-Signal
             DSActualParamList* = NULL);   // Output-Parameter
    CGOutput(DSObject*,
             DSString&,
             DSSignalRef        = NULL,
             DSActualParamList* = NULL);
    CGOutput(DSObject*,
             const char *,
             DSSignalRef        = NULL,
             DSActualParamList* = NULL);
   ~CGOutput(void);
  
    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter*, DSCardinal) const;

  private:
    DSResult WriteImplementation(CGWriter *) const;
    DSResult WriteLabel(CGWriter *) const;
};

#endif

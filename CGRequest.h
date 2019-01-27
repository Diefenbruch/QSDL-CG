#ifndef _CGREQUEST_H_
#define _CGREQUEST_H_

#include <DS/DSRequest.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGRequest : public DSRequest
{                           
  public:
    CGRequest(DSObject* = NULL,            // Parent
              DSString* = NULL,            // Label
              DSMachineServiceRef = NULL,  // angeforderter Dienst
              DSExpression* = NULL,        // Bedienwunsch
              DSExpression* = NULL,        // Prioritaet
              DSLinkRefList* = NULL,       // VIA-Links
              DSPipeRefList* = NULL);      // VIA-Pipes
    CGRequest(DSObject*,
              DSString&,                   // Label
              DSMachineServiceRef = NULL,  // angeforderter Dienst
              DSExpression* = NULL,        // Bedienwunsch
              DSExpression* = NULL,        // Prioritaet
              DSLinkRefList* = NULL,       // VIA-Links
              DSPipeRefList* = NULL);      // VIA-Pipes
    CGRequest(DSObject*,
              const char *,                // Label
              DSMachineServiceRef = NULL,  // angeforderter Dienst
              DSExpression* = NULL,        // Bedienwunsch
              DSExpression* = NULL,        // Prioritaet
              DSLinkRefList* = NULL,       // VIA-Links
              DSPipeRefList* = NULL);      // VIA-Pipes
   ~CGRequest(void);
  
    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  private:
    DSResult WriteImplementation(CGWriter *) const;
    DSResult WriteLabel(CGWriter *) const;
};

#endif

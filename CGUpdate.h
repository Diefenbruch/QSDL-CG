#ifndef _CGUPDATE_H_
#define _CGUPDATE_H_

#include <DS/DSUpdate.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGUpdate : public DSUpdate
{                           
  public:
    CGUpdate(DSObject* = NULL,           // Parent
             DSString* = NULL,           // Label
             DSSensorRef = NULL,         // Sensor
             DSExpression* = NULL,       // Sample
             DSExpression* = NULL);      // Value (nur fuer Frequency)
    CGUpdate(DSObject*,
             DSString&,
             DSSensorRef = NULL,
             DSExpression* = NULL,
             DSExpression* = NULL);      // Value (nur fuer Frequency)
    CGUpdate(DSObject*,
             const char *,
             DSSensorRef = NULL,
             DSExpression* = NULL,
             DSExpression* = NULL);      // Value (nur fuer Frequency)
   ~CGUpdate(void);
  
    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  private:
    DSResult WriteImplementation(CGWriter *) const;
    DSResult WriteLabel(CGWriter *) const;
};

#endif

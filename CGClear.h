#ifndef _CGCLEAR_H_
#define _CGCLEAR_H_

#include <DS/DSClear.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGClear : public DSClear
{                           
  public:
    CGClear(DSObject*   = NULL,     // Vater-Objekt
            DSString*   = NULL,     // Label
            DSSensorRef = NULL);    // Sensor-Objekt
    CGClear(DSObject*,
            DSString&,
            DSSensorRef = NULL);
    CGClear(DSObject*,
            const char *,
            DSSensorRef = NULL);
   ~CGClear(void);
  
    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  private:
    DSResult WriteImplementation(CGWriter *) const;
    DSResult WriteLabel(CGWriter *) const;
};

#endif

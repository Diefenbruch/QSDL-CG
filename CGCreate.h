#ifndef _CGCREATE_H_
#define _CGCREATE_H_

#include <DS/DSCreate.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGCreate : public DSCreate
{                           
  public:
    CGCreate(DSObject*          = NULL,          // parent
             DSString*          = NULL,          // Label
             DSProcessRef       = NULL,
             DSActualParamList* = NULL);
    CGCreate(DSObject*,
             DSString&,
             DSProcessRef       = NULL,
             DSActualParamList* = NULL);
    CGCreate(DSObject*,
             const char *,
             DSProcessRef       = NULL,
             DSActualParamList* = NULL);
   ~CGCreate(void);
  
    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  private:
    DSResult WriteImplementation(CGWriter *) const;
    DSResult WriteLabel(CGWriter *) const;
};

#endif

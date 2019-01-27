#ifndef _CGOPERANDSYNONYM_H_
#define _CGOPERANDSYNONYM_H_

#include <DS/DSOperandSynonym.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGOperandSynonym : public DSOperandSynonym
{                           
  public:
    CGOperandSynonym(DSObject* = NULL,
                     DSSynonymRef = NULL);
   ~CGOperandSynonym(void);

    DSObject *New(DSObject * = NULL) const;        // ueberladener Konstruktor

    DSResult  Write(DSWriter *, DSCardinal = 0) const;

  private:
    DSResult  WriteImplementation(CGWriter *) const;
};

#endif

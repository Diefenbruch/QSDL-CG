#ifndef _CGMAKE_H_
#define _CGMAKE_H_

#include <DS/DSBasicTypes.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGMake
{
  public:
    CGMake(void);
   ~CGMake(void);

    DSResult CGMakefile(CGWriter *);

  private:
    DSResult CGWriteBlockFiles(CGWriter *, DSBlockKeyList*, const char *);
    DSResult CGWriteProcessFiles(CGWriter *, DSProcessKeyList*, const char *);
    DSResult CGWriteProcedureFiles(CGWriter *, DSProcedureKeyList*, const char *);
    DSResult CGWriteMachineFiles(CGWriter *, DSMachineKeyList*, const char *);
};

#endif

#ifndef _CGOPERANDMACHINESERVICE_H_
#define _CGOPERANDMACHINESERVICE_H_

#include <DS/DSOperandMachineService.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGOperandMachineService : public DSOperandMachineService
{                           
  public:
    CGOperandMachineService(DSObject* = NULL,
                            DSMachineServiceRef = NULL);
   ~CGOperandMachineService(void);

    DSObject *New(DSObject * = NULL) const;   // virtueller Konstruktor

    DSResult  Write(DSWriter *, DSCardinal = 0) const;

  private:
    DSResult  WriteImplementation(CGWriter *) const;
};

#endif

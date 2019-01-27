#ifndef _CGSYSTEM_H_
#define _CGSYSTEM_H_

#include <DS/DSString.h>
#include <DS/DSSystem.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGSystem : public DSSystem
{                           
  public:
    CGSystem(DSString* = NULL);
    CGSystem(DSString&);
    CGSystem(const char *);
   ~CGSystem(void);

    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult InitializeBasicSorts(void);
                                                     // legt die Sorten "Real",
                                                     // "Boolean", etc. an
    DSResult InitializeQSDLSorts(void); 
                                                     // legt die Sorten "discipline",
                                                     // "exponential", etc. an
    DSResult Write(DSWriter *, DSCardinal) const;
    DSResult Close(DSStream &, DSBoolean);       // schliesst offenes System
                                                // Param: Verbose-Flag

    DSResult WriteIdentifier(CGWriter *) const; // CG_WRITE_IDENTIFIER

    DSString & GetIdentifier(void) const;

    DSResult WriteProcessIncludes(CGWriter *) const;
    DSResult WriteMachineIncludes(CGWriter *) const;
    DSResult WriteBlockIncludes(CGWriter *) const;
    
  private:
    DSResult WriteHeader(CGWriter *) const; // CG_WRITE_H
    DSResult WriteImplementation(CGWriter *) const; // CG_WRITE_CC
    DSResult WriteInclude(CGWriter *) const; // CG_WRITE_INCLUDE
    DSResult WriteInitializeImplementation(CGWriter *) const;
    DSResult WriteFinishImplementation(CGWriter *) const;
    DSResult WriteSaveImplementation(CGWriter *) const;
    DSResult WriteRestoreImplementation(CGWriter *) const;
    DSResult WriteSizeImplementation(CGWriter *) const;
};

#endif

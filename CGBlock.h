#ifndef _CGBLOCK_H_
#define _CGBLOCK_H_

#include <DS/DSString.h> 
#include <DS/DSBlock.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

/******************************************************************************
 * Klasse CGBlock
 * Zweck: Steuert die Codegenerierung fuer die .cc und .h- Dateien des
          SDL-Blocks 
 ******************************************************************************/

class CGBlock : public DSBlock
{                           
  public:
    CGBlock(DSObject* = NULL,
            DSString* = NULL);
    CGBlock(DSObject*,
            DSString&);
    CGBlock(DSObject*,
            const char *);
    ~CGBlock(void);

    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;
  
    DSResult WriteIdentifier(CGWriter *) const; // CG_WRITE_IDENTIFIER

    DSString & GetIdentifier(void) const;

    DSResult WriteInclude(CGWriter *) const; // CG_WRITE_INCLUDE
    DSResult WriteProcessIncludes(CGWriter *) const;
    DSResult WriteMachineIncludes(CGWriter *) const;
    DSResult WriteBlockIncludes(CGWriter *) const;

  private:
    DSResult WriteHeader(CGWriter *) const; // CG_WRITE_H
    DSResult WriteImplementation(CGWriter *) const; //CG_WRITE_CC
    DSResult WriteConstructor(CGWriter *) const;
    DSResult WriteDestructor(CGWriter *) const;
    DSResult WriteInitialize(CGWriter *) const; // CG_WRITE_INIT
    DSResult WriteInitializeImplementation(CGWriter *) const;
    DSResult WriteFinish(CGWriter *) const; // CG_WRITE_FINISH
    DSResult WriteFinishImplementation(CGWriter *) const;
    DSResult WriteSave(CGWriter *) const;
    DSResult WriteRestore(CGWriter *) const;
    DSResult WriteSize(CGWriter *) const;
    DSResult WriteSaveImplementation(CGWriter *) const;
    DSResult WriteRestoreImplementation(CGWriter *) const;
    DSResult WriteSizeImplementation(CGWriter *) const;
};

#endif

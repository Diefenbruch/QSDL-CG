#ifndef _CGBLOCKSUBSTRUCTURE_H_
#define _CGBLOCKSUBSTRUCTURE_H_

#include <DS/DSString.h>
#include <DS/DSBlockSubstructure.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGBlockSubstructure : public DSBlockSubstructure
{                           
  public:
    CGBlockSubstructure(DSObject* = NULL, DSString* = NULL);
    CGBlockSubstructure(DSObject*, DSString&);
    CGBlockSubstructure(DSObject*, const char *);
   ~CGBlockSubstructure(void);

    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

    DSResult WriteIdentifier(CGWriter *) const; // CG_WRITE_IDENTIFIER

    DSString & GetIdentifier(void) const;
    
  private:
    DSResult WriteHeader(CGWriter *) const; // CG_WRITE_H
    DSResult WriteImplementation(CGWriter *) const; // CG_WRITE_CC
    DSResult WriteInclude(CGWriter *) const; // CG_WRITE_INCLUDE
    DSResult WriteInitialize(CGWriter *) const; // CG_WRITE_INIT
    DSResult WriteInitializeImplementation(CGWriter *) const;
    DSResult WriteFinish(CGWriter *) const; // CG_WRITE_FINISH
    DSResult WriteFinishImplementation(CGWriter *) const;
    DSResult WriteSaveImplementation(CGWriter *) const;
    DSResult WriteRestoreImplementation(CGWriter *) const;
    DSResult WriteSizeImplementation(CGWriter *) const;
    DSResult WriteSave(CGWriter *) const;
    DSResult WriteRestore(CGWriter *) const;
    DSResult WriteSize(CGWriter *) const;
};

#endif

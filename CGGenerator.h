#ifndef _CGGENERATOR_H_
#define _CGGENERATOR_H_

#include <DS/DSGenerator.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGGenerator : public DSGenerator
{
  public:
    CGGenerator(DSObject* = NULL,                /* Vaterobjekt               */
                DSString* = NULL,                /* Name des DSSort           */
                DSString* = NULL,                /* Name des DSGenerator      */
                DSSortRefList* = NULL,           /* Typen                     */
                class DSOperandLiteral* = NULL); /* Emptystring               */
    CGGenerator(DSObject*,                       /* Vaterobjekt               */
                DSString&,                       /* Name des DSSort           */
                DSString&,                       /* Name des DSGenerator      */
                DSSortRefList* = NULL,           /* Typen                     */
                class DSOperandLiteral* = NULL); /* Emptystring               */
    CGGenerator(DSObject*,                       /* Vaterobjekt               */
                const char*,                     /* Name des DSSort           */
                const char* = NULL,              /* Name des DSGenerator      */
                DSSortRefList* = NULL,           /* Typen                     */
                class DSOperandLiteral* = NULL); /* Emptystring               */
   ~CGGenerator(void);

    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

    DSResult WriteInitializeHeader(CGWriter *) const;
    DSResult WriteInitializeImplementation(CGWriter *) const;
    DSResult WriteGenerator(CGWriter *) const;
    DSResult WriteIdentifier(CGWriter *) const;  // CG_WRITE_IDENTIFIER
    DSResult WriteSave(CGWriter *) const;
    DSResult WriteHistorySave(CGWriter *) const;
    DSResult WriteRestore(CGWriter *) const;
    DSResult WriteHistoryRestore(CGWriter *) const;
    DSResult WriteSize(CGWriter *) const;
    DSResult WriteHistorySize(CGWriter *) const;
    DSResult WriteAssign(CGWriter *) const;
};

#endif

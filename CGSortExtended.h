#ifndef _CGSORTEXTENDED_H_
#define _CGSORTEXTENDED_H_

#include "CGSort.h"
#include <DS/DSSortExtended.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGSortExtended : public DSSortExtended
{
  public:
    CGSortExtended(DSObject* = NULL,                /* Vaterobjekt            */
                   DSString* = NULL,                /* Name des DSSort        */
                   DSSortTag = DS_SORT_EXTENDED,
                   DSStringList* = NULL,            /* Literale               */
                   DSOperatorKeyList* = NULL,       /* Operatorenliste        */
                   DSBoolean = DS_FALSE,            /* vordefinierter Typ?    */
                   DSBoolean = DS_FALSE);           /* komplexer Typ?         */
    CGSortExtended(DSObject*,                       /* Vaterobjekt            */
                   DSString&,                       /* Name des DSSort        */
                   DSSortTag = DS_SORT_EXTENDED,
                   DSStringList* = NULL,            /* Literale               */
                   DSOperatorKeyList* = NULL,       /* Operatorenliste        */
                   DSBoolean = DS_FALSE,            /* vordefinierter Typ?    */
                   DSBoolean = DS_FALSE);           /* komplexer Typ?         */
    CGSortExtended(DSObject*,                       /* Vaterobjekt            */
                   const char*,                     /* Name des DSSort        */
                   DSSortTag = DS_SORT_EXTENDED,
                   DSStringList* = NULL,            /* Literale               */
                   DSOperatorKeyList* = NULL,       /* Operatorenliste        */
                   DSBoolean = DS_FALSE,            /* vordefinierter Typ?    */
                   DSBoolean = DS_FALSE);           /* komplexer Typ?         */
   ~CGSortExtended(void);

    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal = 0) const;

  protected:
    DSResult WriteLiteralList(CGWriter *) const; 
                            // CG_WRITE_H und CG_WRITE_SORT_LITERAL_LIST
    DSResult WriteOperatorList(CGWriter *) const;
                            // CG_WRITE_H und CG_WRITE_SORT_OPERATOR_LIST 
    DSResult WriteIdentifier(CGWriter *) const; // CG_WRITE_IDENTIFIER
};

#endif

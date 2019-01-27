#ifndef _CGSORT_H_
#define _CGSORT_H_

#include <DS/DSSort.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGSort : public DSSort
{
  public:
    CGSort(DSObject* = NULL,               /* Vaterobjekt          */
           DSString* = NULL,               /* Name                 */
           DSSortTag = DS_SORT_BASIC,      /* Type der Sorte       */
           DSBoolean = DS_FALSE,           /* vordefinierter Typ ? */
           DSBoolean = DS_FALSE,           /* komplexer Typ ?      */
           DSExpression* = NULL);
    CGSort(DSObject*,                      /* Vaterobjekt          */
           DSString&,                      /* Name                 */
           DSSortTag = DS_SORT_BASIC,      /* Type der Sorte       */
           DSBoolean = DS_FALSE,           /* vordefinierter Typ ? */
           DSBoolean = DS_FALSE,           /* komplexer Typ ?      */
           DSExpression* = NULL);
    CGSort(DSObject*,                      /* Vaterobjekt          */
           const char *,                   /* Name                 */
           DSSortTag = DS_SORT_BASIC,      /* Type der Sorte       */
           DSBoolean = DS_FALSE,           /* vordefinierter Typ ? */
           DSBoolean = DS_FALSE,           /* komplexer Typ ?      */
           DSExpression* = NULL);
   ~CGSort(void);

    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

    DSResult WriteIdentifier(CGWriter *) const;  // CG_WRITE_IDENTIFIER
};

#endif

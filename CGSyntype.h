#ifndef _CGSYNTYPE_H_
#define _CGSYNTYPE_H_

#include <DS/DSSyntype.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"

class CGSyntype : public DSSyntype
{
  public:
    CGSyntype(DSObject* = NULL,     // Vaterobjekt, wird an DSSort weiter-
              DSString* = NULL,     // gegeben
              DSSortRef = NULL,
              DSRangeList*  = NULL);
    CGSyntype(DSObject*,
              DSString&,
              DSSortRef = NULL,
              DSRangeList*  = NULL);
    CGSyntype(DSObject*,
              const char *,
              DSSortRef = NULL,
              DSRangeList*  = NULL);           /* zwecks Initialisierung des */
   ~CGSyntype(void);

    DSBoolean GetIsComplex(void) const;      // redefiniert virtuelle Funktion
                                             // in DSSort !

    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  private:
    DSResult WriteSyntype(CGWriter *) const;
    DSResult WriteIdentifier(CGWriter *) const; // CG_WRITE_IDENTIFIER
    DSResult WriteSave(CGWriter *) const;
    DSResult WriteHistorySave(CGWriter *) const;
    DSResult WriteRestore(CGWriter *) const;
    DSResult WriteHistoryRestore(CGWriter *) const;
    DSResult WriteSize(CGWriter *) const;
    DSResult WriteHistorySize(CGWriter *) const;
};

#endif

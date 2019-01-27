#ifndef _CGSTRUCT_H_
#define _CGSTRUCT_H_

#include <DS/DSStruct.h>

#include "CGBasicTypes.h"
#include "CGWriter.h"
#include "CGSortExtended.h"

class CGStruct : public DSStruct
{
  public:
    CGStruct(DSObject* = NULL,
             DSString * = NULL,
             DSNameSortKeyList* = NULL);
    CGStruct(DSObject*,
             DSString& ,
             DSNameSortKeyList* = NULL);
    CGStruct(DSObject*,
             const char *,
             DSNameSortKeyList* = NULL);
   ~CGStruct(void);

    DSObject *New(DSObject * = NULL) const;  // virtueller Konstruktor

    DSResult Write(DSWriter *, DSCardinal) const;

  private:
    DSResult WriteDefaultConstructor(CGWriter *) const;
    DSResult WriteCopyConstructor(CGWriter *) const;
    DSResult WriteClone(CGWriter *) const;
    DSResult WriteAssign(CGWriter *) const;
    DSResult WriteCompare(CGWriter *) const;
    DSResult WriteEqual(CGWriter *) const;
    DSResult WriteDisplay(CGWriter *) const;
    DSResult WriteStreamOperator(CGWriter *) const;
    DSResult WriteSave(CGWriter *) const;
    DSResult WriteHistorySave(CGWriter *) const;
    DSResult WriteRestore(CGWriter *) const;
    DSResult WriteHistoryRestore(CGWriter *) const;
    DSResult WriteSize(CGWriter *) const;
    DSResult WriteHistorySize(CGWriter *) const;
    DSResult WriteFields(CGWriter *) const;
    DSResult WriteStruct(CGWriter *) const;
    DSResult WriteLiteralList(CGWriter *) const; 
                            // CG_WRITE_H und CG_WRITE_SORT_LITERAL_LIST
    DSResult WriteOperatorList(CGWriter *) const;
                            // CG_WRITE_H und CG_WRITE_SORT_OPERATOR_LIST 
    DSResult WriteIdentifier(CGWriter *) const; // CG_WRITE_IDENTIFIER
};

#endif

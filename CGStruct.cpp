/****************************************************************************
 *                 Modul: $RCSfile: CGStruct.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/02/09 11:09:56 $
 * $Revision: 1.3 $
 *
 * Aufgabe: Dieser Modul schreibt SDL-Datentypen in entsprechende SCL-Typen um
 * Funktionen:
 ****************************************************************************/
 
#ifdef USE_PRAGMA
#pragma interface
#endif
 
/****************************************************************************
 * Konstanten
 ****************************************************************************/
 
/****************************************************************************
 * Include-Anweisungen
 ****************************************************************************/

#include <DS/DSString.h>
#include <DS/DSNameSort.h>
#include <DS/DSExpression.h>
 
#include "CGFile.h"
#include "CGStruct.h"

#ifdef DEBUG
#include <dmalloc.h>
#endif
 
/****************************************************************************
 * Externe Variablen
 ****************************************************************************/
 
/****************************************************************************
 * Globale Variablen
 ****************************************************************************/
 
/****************************************************************************
 * Konstruktoren
 ****************************************************************************/
 
CGStruct::CGStruct(DSObject *father,
                   DSString *sort_name,
                   DSNameSortKeyList *list) :
  DSStruct(father, sort_name, list)
{
}

CGStruct::CGStruct(DSObject *father,
                   DSString& sort_name,
                   DSNameSortKeyList *list) :
  DSStruct(father, sort_name, list)
{
}

CGStruct::CGStruct(DSObject *father,
                   const char* sort_name,
                   DSNameSortKeyList *list) :
  DSStruct(father, sort_name, list)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGStruct::~CGStruct(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGStruct::New(DSObject *father) const
{
  return new CGStruct(father);
}
 
/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer einen Syntype
 *                   -> writer
 *                   -> what
 *                   Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                             DS_ERROR
 *                   Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/
 
DSResult CGStruct::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  switch (what)
  {
    case CG_WRITE_LITERAL_LIST:
      result = WriteLiteralList((CGWriter *)writer);
      break;

    case CG_WRITE_OPERATOR_LIST:
      result = WriteOperatorList((CGWriter *)writer);
      break;

    case CG_WRITE_IDENTIFIER:
      result = WriteIdentifier((CGWriter *)writer);
      break;

    case CG_WRITE_H:
      result = WriteStruct((CGWriter *)writer);
      break;

    default:
      return DS_OK;
  }
  return result;
}
 
DSResult CGStruct::WriteDefaultConstructor(CGWriter *writer) const
{ 
  DSResult     result;
  DSNameSort  *name_sort;
  DSSortRef    sort;
  CGFile      *out;
  CGPos        pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  WriteIdentifier(writer);
  InsertString(*out, "(void)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  for (name_sort = GetFirstField();
       name_sort;
       name_sort = GetNextField())
  {
    sort = name_sort->GetSortRef();
    assert(sort);

    // Gibt es einen in SDL spezifizierten Default-Wert?
    if (sort->GetDefaultInitialization())
    {
      InsertString(*out, PREFIX_FIELD, pos + 2, CG_NO_EOL);
      InsertString(*out, name_sort->GetName(), 0, CG_NO_EOL);
      InsertString(*out, " = ", 0, CG_NO_EOL);
      result = sort->GetDefaultInitialization()->Write(writer);
      if (result != DS_OK)
      {
        return DS_ERROR;
      }
      InsertString(*out, ";", 0, CG_WITH_EOL);
    }
    // Ist ein neutrales Element vorhanden?
    else if (sort->GetNeutralElement())
    {
      InsertString(*out, PREFIX_FIELD, pos + 2, CG_NO_EOL);
      InsertString(*out, name_sort->GetName(), 0, CG_NO_EOL);
      InsertString(*out, " = ", 0, CG_NO_EOL);
      InsertString(*out, sort->GetNeutralElement(), 0, CG_NO_EOL);
      InsertString(*out, ";", 0, CG_WITH_EOL);
    }
    // Ist es ein vordefinierter Datentyp?
    else if (!sort->GetIsComplex())
    {
      InsertString(*out, "memset(&", pos + 2, CG_NO_EOL);
      InsertString(*out, PREFIX_FIELD, 0, CG_NO_EOL);
      InsertString(*out, name_sort->GetName(), 0, CG_NO_EOL);
      InsertString(*out, ", 0, sizeof(", 0, CG_NO_EOL);
      writer->CGSetIndentPos(0);
      sort->Write(writer, CG_WRITE_IDENTIFIER);
      writer->CGSetIndentPos(pos);
      InsertString(*out, "));", 0, CG_WITH_EOL);
    }
  }
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGStruct::WriteCopyConstructor(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  WriteIdentifier(writer);
  InsertString(*out, "(const ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  writer->CGSetIndentPos(pos);
  InsertString(*out, "& orig)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "*this = orig;", pos + 2, CG_WITH_EOL);
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGStruct::WriteClone(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCDataType *Clone(void) const", pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "return new ", pos + 2, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  writer->CGSetIndentPos(pos);
  InsertString(*out, "(*this);", 0, CG_WITH_EOL);
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGStruct::WriteEqual(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean Equal(const SCDataType &second) const",
               pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "return (*this == *(", pos + 2, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  writer->CGSetIndentPos(pos);
  InsertString(*out, " *)&second);", 0, CG_WITH_EOL);
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGStruct::WriteCompare(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSNameSort  *name_sort;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean operator== (const ", pos, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  writer->CGSetIndentPos(pos);
  InsertString(*out, " &second) const", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "return (", pos + 2, CG_NO_EOL);

  for (name_sort = GetFirstField();
       name_sort;
       name_sort = GetNextField())
  {
    InsertString(*out, PREFIX_FIELD, 0, CG_NO_EOL);
    InsertString(*out, name_sort->GetName(), 0, CG_NO_EOL);
    InsertString(*out, " == second.", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_FIELD, 0, CG_NO_EOL);
    InsertString(*out, name_sort->GetName(), 0, CG_NO_EOL);
    if (!GetFieldList()->AtEndOfList())
    {
      InsertString(*out, " && ", 0, CG_NO_EOL);
		}
	}
  InsertString(*out, ");", 0, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGStruct::WriteAssign(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSNameSort  *name_sort;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  WriteIdentifier(writer);
  InsertString(*out, "& operator= (const ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  writer->CGSetIndentPos(pos);
  InsertString(*out, " &orig)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  for (name_sort = GetFirstField();
       name_sort;
       name_sort = GetNextField())
  {
    InsertString(*out, PREFIX_FIELD, pos + 2, CG_NO_EOL);
    InsertString(*out, name_sort->GetName(), 0, CG_NO_EOL);
    InsertString(*out, " = orig.", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_FIELD, 0, CG_NO_EOL);
    InsertString(*out, name_sort->GetName(), 0, CG_NO_EOL);
    InsertString(*out, ";", 0, CG_WITH_EOL);
  }
  InsertString(*out, "return *this;", pos + 2, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGStruct::WriteDisplay(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCStream& Display(SCStream &out) const", pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  InsertString(*out, "return (out << *this);", pos + 2, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGStruct::WriteStreamOperator(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSNameSort  *name_sort;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "friend SCStream& operator<<(SCStream &out, const ", pos, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  writer->CGSetIndentPos(pos);
  InsertString(*out, " &me)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  if (GetFieldList())
  {
    if (GetFieldList()->NumberOfElements() > 1)
    {
      InsertString(*out, "out << \"[\";", pos + 2, CG_WITH_EOL);
    }
  }
  for (name_sort = GetFirstField();
       name_sort;
       name_sort = GetNextField())
  {
    InsertString(*out, "out << me.", pos + 2, CG_NO_EOL);
    InsertString(*out, PREFIX_FIELD, 0, CG_NO_EOL);
    InsertString(*out, name_sort->GetName(), 0, CG_NO_EOL);
    if (!GetFieldList()->AtEndOfList())
    {
      InsertString(*out, " << \", \"", 0, CG_NO_EOL);
    }
    InsertString(*out, ";", 0, CG_WITH_EOL);
  }
  if (GetFieldList())
  {
    if (GetFieldList()->NumberOfElements() > 1)
    {
      InsertString(*out, "out << \"]\";", pos + 2, CG_WITH_EOL);
    }
  }
  InsertString(*out, "return out;", pos + 2, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGStruct::WriteSave(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSNameSort  *name_sort;
  DSSortRef    sort;
  DSString     str;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean Save(SCMem &save_area) const",
               pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  for (name_sort = GetFirstField();
       name_sort;
       name_sort = GetNextField())
  {
    sort = name_sort->GetSortRef();
    assert(sort);

    if (!sort->GetIsComplex())
    {
      str.Format("save_area.Store(&%s%s, sizeof(",
                 PREFIX_FIELD,
                 name_sort->GetName()->GetString());
      InsertString(*out, str, pos + 2, CG_NO_EOL);
      writer->CGSetIndentPos(0);
      sort->Write(writer, CG_WRITE_IDENTIFIER);
      writer->CGSetIndentPos(pos);
      InsertString(*out, "));", 0, CG_WITH_EOL);
    }
    else // komplexer Datentyp mit eigener Save-Methode!
    {
      str.Format("%s%s.Save(save_area);",
                 PREFIX_FIELD,
                 name_sort->GetName()->GetString());
      InsertString(*out, str, pos + 2, CG_WITH_EOL);
    }
  }
  InsertString(*out, "return true;", pos + 2, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGStruct::WriteHistorySave(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSNameSort  *name_sort;
  DSSortRef    sort;
  DSString     str;
//  char        *area;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean HistorySave(SCMem &save_area) const",
               pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  for (name_sort = GetFirstField();
       name_sort;
       name_sort = GetNextField())
  {
    sort = name_sort->GetSortRef();
    assert(sort);

    if (!sort->GetIsComplex())
    {
      str.Format("save_area.HistoryStore(&%s%s, sizeof(",
                 PREFIX_FIELD,
                 name_sort->GetName()->GetString());
      InsertString(*out, str, pos + 2, CG_NO_EOL);
      writer->CGSetIndentPos(0);
      sort->Write(writer, CG_WRITE_IDENTIFIER);
      writer->CGSetIndentPos(pos);
      InsertString(*out, "));", 0, CG_WITH_EOL);
    }
    else // komplexer Datentyp mit eigener HistorySave-Methode!
    {
      str.Format("%s%s.HistorySave(save_area);",
                 PREFIX_FIELD,
                 name_sort->GetName()->GetString());
      InsertString(*out, str, pos + 2, CG_WITH_EOL);
    }
  }
  InsertString(*out, "return true;", pos + 2, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGStruct::WriteRestore(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSNameSort  *name_sort;
  DSSortRef    sort;
  DSString     str;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean Restore(SCMem &save_area)",
               pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  for (name_sort = GetFirstField();
       name_sort;
       name_sort = GetNextField())
  {
    sort = name_sort->GetSortRef();
    assert(sort);

    if (!sort->GetIsComplex())
    {
      str.Format("save_area.Restore(&%s%s, sizeof(",
                 PREFIX_FIELD,
                 name_sort->GetName()->GetString());
      InsertString(*out, str, pos + 2, CG_NO_EOL);
      writer->CGSetIndentPos(0);
      sort->Write(writer, CG_WRITE_IDENTIFIER);
      writer->CGSetIndentPos(pos);
      InsertString(*out, "));", 0, CG_WITH_EOL);
    }
    else // komplexer Datentyp mit eigener Restore-Methode!
    {
      str.Format("%s%s.Restore(save_area);",
                 PREFIX_FIELD,
                 name_sort->GetName()->GetString());
      InsertString(*out, str, pos + 2, CG_WITH_EOL);
    }
  }
  InsertString(*out, "return true;", pos + 2, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGStruct::WriteHistoryRestore(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSNameSort  *name_sort;
  DSSortRef    sort;
  DSString     str;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean HistoryRestore(SCMem &save_area)",
               pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  for (name_sort = GetFirstField();
       name_sort;
       name_sort = GetNextField())
  {
    sort = name_sort->GetSortRef();
    assert(sort);

    if (!sort->GetIsComplex())
    {
      str.Format("save_area.HistoryRestore(&%s%s, sizeof(",
                 PREFIX_FIELD,
                 name_sort->GetName()->GetString());
      InsertString(*out, str, pos + 2, CG_NO_EOL);
      writer->CGSetIndentPos(0);
      sort->Write(writer, CG_WRITE_IDENTIFIER);
      writer->CGSetIndentPos(pos);
      InsertString(*out, "));", 0, CG_WITH_EOL);
    }
    else // komplexer Datentyp mit eigener Restore-Methode!
    {
      str.Format("%s%s.HistoryRestore(save_area);",
                 PREFIX_FIELD,
                 name_sort->GetName()->GetString());
      InsertString(*out, str, pos + 2, CG_WITH_EOL);
    }
  }
  InsertString(*out, "return true;", pos + 2, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGStruct::WriteSize(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSNameSort  *name_sort;
  DSSortRef    sort;
  DSString     str;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "void Size(SCSize *cur_size) const",
               pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  for (name_sort = GetFirstField();
       name_sort;
       name_sort = GetNextField())
  {
    sort = name_sort->GetSortRef();
    assert(sort);

    if (!sort->GetIsComplex())
    {
      InsertString(*out, "cur_size->size += sizeof(", pos + 2, CG_NO_EOL);
      writer->CGSetIndentPos(0);
      sort->Write(writer, CG_WRITE_IDENTIFIER);
      writer->CGSetIndentPos(pos);
      InsertString(*out, ");", 0, CG_WITH_EOL);
    }
    else // komplexer Datentyp mit eigener Size-Methode!
    {
      str.Format("%s%s.Size(cur_size);",
                 PREFIX_FIELD,
                 name_sort->GetName()->GetString());
      InsertString(*out, str, pos + 2, CG_WITH_EOL);
    }
  }
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGStruct::WriteHistorySize(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSNameSort  *name_sort;
  DSSortRef    sort;
  DSString     str;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "void HistorySize(SCSize *cur_size) const",
               pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  for (name_sort = GetFirstField();
       name_sort;
       name_sort = GetNextField())
  {
    sort = name_sort->GetSortRef();
    assert(sort);

    if (!sort->GetIsComplex())
    {
      InsertString(*out, "cur_size->historySize += sizeof(", pos + 2, CG_NO_EOL);
      writer->CGSetIndentPos(0);
      sort->Write(writer, CG_WRITE_IDENTIFIER);
      writer->CGSetIndentPos(pos);
      InsertString(*out, ");", 0, CG_WITH_EOL);
    }
    else // komplexer Datentyp mit eigener Size-Methode!
    {
      str.Format("%s%s.HistorySize(cur_size);",
                 PREFIX_FIELD,
                 name_sort->GetName()->GetString());
      InsertString(*out, str, pos + 2, CG_WITH_EOL);
    }
  }
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGStruct::WriteFields(CGWriter *writer) const
{ 
  DSResult     result;
  DSNameSort  *name_sort;
  DSSortRef    sort;
  CGFile      *out;
  CGPos        pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  for (name_sort = GetFirstField();
       name_sort;
       name_sort = GetNextField())
  {
    sort = name_sort->GetSortRef();
    assert(sort);

    sort->Write(writer, CG_WRITE_IDENTIFIER);
    InsertString(*out, " ", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_FIELD, 0, CG_NO_EOL);
    InsertString(*out, name_sort->GetName(), 0, CG_NO_EOL);
    InsertString(*out, ";", 0, CG_WITH_EOL);
  }
  
  return DS_OK;
}

/****************************************************************************
 * WriteStruct(): schreibt eine SCL-Structure
 *                -> writer
 *                Ergebnis: CG_OK, falls alles geklappt hat, sonst DS_ERROR
 *                Seiteneffekte: Anlegen und Fuellen der Files
 ****************************************************************************/
 
DSResult CGStruct::WriteStruct(CGWriter *writer) const
{ 
  DSResult result;
  CGFile  *out;
  CGPos    pos;
  DSString str;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  str.Format("SDL-Newtype: Struct '%s':", DSStruct::GetName()->GetString());
  InsertHead(*out, str, pos);
  InsertString(*out, "struct ", pos, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  writer->CGSetIndentPos(pos);
  InsertString(*out, " : public SCDataType", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  writer->CGSetIndentPos(pos + 2);

  // Default-Konstruktor:
  InsertString(*out, "// Default Konstruktor:", pos + 2, CG_WITH_EOL);
  result = WriteDefaultConstructor(writer);
  if (result != DS_OK) return result;

  // Copy-Konstruktor:
  InsertString(*out, "// Copy Konstruktor:", pos + 2, CG_WITH_EOL);
  result = WriteCopyConstructor(writer);
  if (result != DS_OK) return result;
  
  // Felder:
  InsertString(*out, "// Felder:", pos + 2, CG_WITH_EOL);
  result = WriteFields(writer);
  if (result != DS_OK) return result;

  // Clone-Operator:
  InsertString(*out, "// Virtueller Clone-Operator:", pos + 2, CG_WITH_EOL);
  result = WriteClone(writer);
  if (result != DS_OK) return result;

  // Assign-Operator:
  InsertString(*out, "// Zuweisungsoperator:", pos + 2, CG_WITH_EOL);
  result = WriteAssign(writer);
  if (result != DS_OK) return result;

  // Compare-Operator:
  InsertString(*out, "// Vergleichsoperator:", pos + 2, CG_WITH_EOL);
  result = WriteCompare(writer);
  if (result != DS_OK) return result;
  
  // Equal-Operator:
  InsertString(*out, "// Virtueller Vergleichssoperator:", pos + 2, CG_WITH_EOL);
  result = WriteEqual(writer);
  if (result != DS_OK) return result;

  // <<-Operator:
  InsertString(*out, "// Ausgabeoperator:", pos + 2, CG_WITH_EOL);
  result = WriteStreamOperator(writer);
  if (result != DS_OK) return result;

  // Display-Operator:
  InsertString(*out, "// Virtueller Ausgabeoperator:", pos + 2, CG_WITH_EOL);
  result = WriteDisplay(writer);
  if (result != DS_OK) return result;

  // Save-Methode:
  InsertString(*out, "// Virtuelle Save-Methode:", pos + 2, CG_WITH_EOL);
  result = WriteSave(writer);
  if (result != DS_OK) return result;

  // HistorySave-Methode: (used for history variables)
  InsertString(*out, "// Virtuelle HistorySave-Methode (fuer History-Variablen):", pos + 2, CG_WITH_EOL);
  result = WriteHistorySave(writer);
  if (result != DS_OK) return result;

  // Restore-Methode:
  InsertString(*out, "// Virtuelle Restore-Methode:", pos + 2, CG_WITH_EOL);
  result = WriteRestore(writer);
  if (result != DS_OK) return result;

  // HistoryRestore-Methode:
  InsertString(*out, "// Virtuelle HistoryRestore-Methode (fuer History-Variablen):", pos + 2, CG_WITH_EOL);
  result = WriteHistoryRestore(writer);
  if (result != DS_OK) return result;

  // Size-Methode:
  InsertString(*out, "// Virtuelle Size-Methode:", pos + 2, CG_WITH_EOL);
  result = WriteSize(writer);
  if (result != DS_OK) return result;

  // HistorySize-Methode:
  InsertString(*out, "// Virtuelle HistorySize-Methode (fuer History-Variablen):", pos + 2, CG_WITH_EOL);
  result = WriteHistorySize(writer);
  if (result != DS_OK) return result;

  // Literale:  
  result = Write(writer, CG_WRITE_LITERAL_LIST);
  if (result != DS_OK) return result;

  // Operatoren:  
  result = Write(writer, CG_WRITE_OPERATOR_LIST);
  if (result != DS_OK) return result;
  
  writer->CGSetIndentPos(pos);

  InsertString(*out, "};", pos, CG_WITH_EOL);
  InsertLine(*out);

  return DS_OK;
}

/****************************************************************************
 * WriteLiteralList(): schreibt die Literalliste eines SDL-Datentyps
 *                     als C-enum.
 *                     -> writer
 *                     Ergebnis: CG_OK, falls alles geklappt hat, sonst DS_ERROR
 *                     Seiteneffekte: Anlegen und Fuellen der Files
 ****************************************************************************/
 
DSResult CGStruct::WriteLiteralList(CGWriter *writer) const
{ 
  DSResult   result;
  DSCardinal i = 0;
  DSString  *literal;
  CGFile    *out;
  CGPos      pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  if (GetLiteralList() == NULL) return DS_OK;
  
  if ((*GetName() == DS_SORT_NAME_BOOLEAN) || 
      (*GetName() == DS_SORT_NAME_PID) ||
      (*GetName() == DS_SORT_NAME_CHARACTER) ||
      (*GetName() == DS_SORT_NAME_CHARSTRING) ||
      (*GetName() == DS_SORT_NAME_DISCIPLINE))
  {
    return DS_OK;
  }

  InsertString(*out, "enum ", pos, CG_NO_EOL); 

  // Testen ob die Enum-Umgebung einen
  // Namen benoetigt:
  switch (GetTag())
  {
    case DS_SORT_STRUCT:
    case DS_SORT_SYNTYPE:
    case DS_SORT_SYNONYM:
    case DS_SORT_BASIC:
    case DS_SORT_GENERATOR:
      InsertString(*out, "", 0, CG_WITH_EOL);
      break;
          
    case DS_SORT_EXTENDED:
      WriteIdentifier(writer);
      break;

    default:
      assert(DS_FALSE);
      break;
  }
  
  InsertString(*out, "{", pos, CG_WITH_EOL);
  for (literal = GetFirstLiteral();
       literal;
       literal = GetNextLiteral())
  {
    if (i > 0) InsertString(*out, ", ", 0, CG_WITH_EOL);
    InsertString(*out, literal, pos + 2, CG_NO_EOL);
    i++;
  }
  InsertString(*out, "", 0, CG_WITH_EOL); 
  InsertString(*out, "};", pos, CG_WITH_EOL); 

  return DS_OK;
}

/****************************************************************************
 * WriteOperatorList(): schreibt die Operatorliste eines SDL-Datentyps
 *                      als C-enum.
 *                      -> writer
 *                      Ergebnis: CG_OK, falls alles geklappt hat, sonst
 *                                DS_ERROR
 *                      Seiteneffekte: Anlegen und Fuellen der Files
 ****************************************************************************/
 
DSResult CGStruct::WriteOperatorList(CGWriter *writer) const
{ 
  DSResult result;

  result = Run(writer, DS_OPERATOR);

  return result;
}

DSResult CGStruct::WriteIdentifier(CGWriter *writer) const
{ 
  DSResult   result;
  CGFile    *out;
  CGPos      pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  assert (GetName());

  if (GetIsPredefined())
  {
    DSString tmp(*GetName());

    InsertString(*out, "SC", pos, CG_NO_EOL);
    tmp.ToUpper(1);
    InsertString(*out, tmp, 0, CG_NO_EOL);
  }
  else
  {
    InsertString(*out, PREFIX_DATA_TYPE, pos, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
  }

  return DS_OK;
}

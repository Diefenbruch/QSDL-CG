/****************************************************************************
 *                 Modul: $RCSfile: CGGenerator.cpp,v $
 *
 * $Author: md $
 * $Date: 1997/10/30 09:56:14 $
 * $Revision: 1.4 $
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
#include <DS/DSSyntype.h>
#include <DS/DSSystem.h>
#include <DS/DSBlock.h>
#include <DS/DSProcess.h>
#include <DS/DSRange.h>
#include <DS/DSExpression.h>

#include "CGFile.h"
#include "CGGenerator.h"

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
 
CGGenerator::CGGenerator(DSObject *father,
                         DSString *sort_name,
                         DSString *gen_name,
                         DSSortRefList *sorts,
                         DSOperandLiteral *emptystring) :
  DSGenerator(father, sort_name, gen_name, sorts, emptystring)
{
}

CGGenerator::CGGenerator(DSObject *father,
                         DSString& sort_name,
                         DSString& gen_name,
                         DSSortRefList *sorts,
                         DSOperandLiteral *emptystring) :
  DSGenerator(father, sort_name, gen_name, sorts, emptystring)
{
}

CGGenerator::CGGenerator(DSObject *father,
                         const char *sort_name,
                         const char *gen_name,
                         DSSortRefList *sorts,
                         DSOperandLiteral *emptystring) :
  DSGenerator(father, sort_name, gen_name, sorts, emptystring)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGGenerator::~CGGenerator(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGGenerator::New(DSObject *father) const
{
  return new CGGenerator(father);
}

/****************************************************************************
 * Write(): schreibt eine SCL-Generator
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK, falls alles geklappt hat, sonst enthaelt
 *          DS_RESULT den aufgetretenen Fehler
 *          Seiteneffekte: Anlegen und Fuellen der Files
 ****************************************************************************/
 
DSResult CGGenerator::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  switch (what)
  {
    case CG_WRITE_IDENTIFIER:
      result = WriteIdentifier((CGWriter *)writer);
      break;

    case CG_WRITE_H:
      result = WriteGenerator((CGWriter *)writer);
      break;

    default:
      return DS_OK;
  }
  return result;
}
 
DSResult CGGenerator::WriteGenerator(CGWriter *writer) const
{ 
  DSResult   result;
  DSSyntype *syntype;
  DSSort    *elem_sort;
  CGFile    *out;
  CGPos      pos;
  DSObject  *father;
  DSString   str;
  const char      *gen_name = NULL;
  
  if (GetIsPredefined())
  {
    return DS_OK;
  }

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  if (*(GetGeneratorName()) == DS_GENERATOR_NAME_ARRAY)
  {
    gen_name = "Array";

    // 1. Sorte: Indexbereich
    // 2. Sorte: Elementstyp
    syntype = (DSSyntype *)GetFirstSortRef();
    assert(syntype);
    elem_sort = (DSSort*)GetNextSortRef();
    assert(elem_sort);
  }
  else if (*(GetGeneratorName()) == DS_GENERATOR_NAME_STRING)
  {
    gen_name = "String";

    // 1. Sorte: Elementstyp
    elem_sort = (DSSort*)GetFirstSortRef();
    assert(elem_sort);
    syntype = NULL;
  }
  else
  {
    return DS_OK;
  }
  
  str.Format("SDL-Newtype: %s '%s':", gen_name, DSSort::GetName()->GetString());
  InsertHead(*out, str, pos);
  InsertString(*out, "struct ", pos, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  WriteIdentifier((CGWriter *)writer);
  writer->CGSetIndentPos(pos);
  str.Format(" : public SC%s<", gen_name);
  InsertString(*out, str, 0, CG_NO_EOL);

  father = DSSort::GetParent();
  assert (father);

  if (father->GetType() == DS_PROCEDURE)
  {
    while(father->GetType() == DS_PROCEDURE)
    {
      father = father->GetParent();
    }
    ((CGWriter *)writer)->CGSetIndentPos(0);
    result = father->Write(writer, CG_WRITE_IDENTIFIER);
    ((CGWriter *)writer)->CGSetIndentPos(pos);
    if (result != DS_OK) return result;
    InsertString(*out, "::", 0, CG_NO_EOL);
  }

  writer->CGSetIndentPos(0);
  elem_sort->Write(writer, CG_WRITE_IDENTIFIER);
  writer->CGSetIndentPos(pos);
  InsertString(*out, ">", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "// Konstruktor:", pos + 2, CG_WITH_EOL);
  writer->CGSetIndentPos(pos + 2);
  WriteIdentifier(writer);
  str.Format("(void) : SC%s<", gen_name);
  InsertString(*out, str, 0, CG_NO_EOL);
  if (DSSort::GetParent()->GetType() == DS_PROCEDURE)
  {
    ((CGWriter *)writer)->CGSetIndentPos(0);
    result = father->Write(writer, CG_WRITE_IDENTIFIER);
    ((CGWriter *)writer)->CGSetIndentPos(pos);
    if (result != DS_OK) return result;
    InsertString(*out, "::", 0, CG_NO_EOL);
  }

  writer->CGSetIndentPos(0);
  elem_sort->Write(writer, CG_WRITE_IDENTIFIER);
  writer->CGSetIndentPos(pos);
  InsertString(*out, "> (", 0, CG_NO_EOL);
  
  if (syntype) // Array ?
  {
    if (!syntype->GetFirstRange())
    {
      std::cerr << "Array without range of variable: " << DSSort::GetName() << std::endl;
      return DS_ERROR;
    }
    if (!syntype->GetFirstRange()->GetLowerBound()->IsConstant() &&
        DSSort::GetParent()->GetType() == DS_PROCEDURE)
    {
      ((CGWriter *)writer)->CGSetIndentPos(0);
      result = father->Write(writer, CG_WRITE_IDENTIFIER);
      ((CGWriter *)writer)->CGSetIndentPos(pos);
      if (result != DS_OK) return result;
      InsertString(*out, "::", 0, CG_NO_EOL);
    }
    ((CGWriter *)writer)->CGSetIndentPos(0);
    result = syntype->GetFirstRange()->GetLowerBound()->Write(writer);
    ((CGWriter *)writer)->CGSetIndentPos(pos);
    if (result != DS_OK) return result;
    InsertString(*out, ", ", 0, CG_NO_EOL);
    if (!syntype->GetFirstRange()->GetUpperBound()->IsConstant() &&
        DSSort::GetParent()->GetType() == DS_PROCEDURE)
    {
      ((CGWriter *)writer)->CGSetIndentPos(0);
      result = father->Write(writer, CG_WRITE_IDENTIFIER);
      ((CGWriter *)writer)->CGSetIndentPos(pos);
      if (result != DS_OK) return result;
      InsertString(*out, "::", 0, CG_NO_EOL);
    }
    ((CGWriter *)writer)->CGSetIndentPos(0);
    result = syntype->GetFirstRange()->GetUpperBound()->Write(writer);
    ((CGWriter *)writer)->CGSetIndentPos(pos);
    if (result != DS_OK) return result;
    if (!elem_sort->GetIsComplex())  
    {
      InsertString(*out, ", ", 0, CG_NO_EOL); // Komma zum Einleiten des Initialwertes
    }
  } // if (syntype)
  
  if (!elem_sort->GetIsComplex())  // T ohne Konstruktor (d.h. Elemente werden NICHT
  {                                // automatisch initialisiert) ?
    InsertString(*out, "(", 0, CG_NO_EOL);  
    ((CGWriter *)writer)->CGSetIndentPos(0);
    elem_sort->Write(writer, CG_WRITE_IDENTIFIER);
    ((CGWriter *)writer)->CGSetIndentPos(pos);
    InsertString(*out, ")", 0, CG_NO_EOL);  

    // Initialisierungswert des Arrays bestimmen (fuer Validation SEHR wichtig!)
    if (elem_sort->GetDefaultInitialization())
    {
      ((CGWriter *)writer)->CGSetIndentPos(0);
      result = elem_sort->GetDefaultInitialization()->Write(writer);
      ((CGWriter *)writer)->CGSetIndentPos(pos);
      if (result != DS_OK) return result;
    }
    else if (elem_sort->GetNeutralElement())
    {
      InsertString(*out, elem_sort->GetNeutralElement(), 0, CG_NO_EOL);  
    }
    else if (elem_sort->GetTag() == DS_SORT_EXTENDED &&
             ((DSSortExtended *)elem_sort)->GetFirstLiteral())
    {
      InsertString(*out, ((DSSortExtended *)elem_sort)->GetFirstLiteral(), 0, CG_NO_EOL);  
    }
    else
    {
      InsertString(*out, "NULL", 0, CG_NO_EOL);  
    }
  }

  InsertString(*out, ") {};", 0, CG_WITH_EOL);

  if (syntype) // Array-Generator
  {
    // Save-Methode:
    InsertString(*out, "// Virtuelle Save-Methode:", pos + 2, CG_WITH_EOL);
    result = WriteSave(writer);
    if (result != DS_OK) return result;

    // HistorySave-Methode:
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
  }
  else // String-Generator
  {
    // Operator=-Methode:
    InsertString(*out, "// Zuweisungssoperator:", pos + 2, CG_WITH_EOL);
    result = WriteAssign(writer);
    if (result != DS_OK) return result;
  }
  
  InsertString(*out, "};", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGGenerator::WriteIdentifier(CGWriter *writer) const
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

DSResult CGGenerator::WriteSave(CGWriter *writer) const
{
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSSyntype *syntype;
  DSSortRef    elem_sort;
  DSString     str;

  if (*(GetGeneratorName()) != DS_GENERATOR_NAME_ARRAY)
  {
    return DS_OK;
  }
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  // 1. Sorte: Indexbereich
  // 2. Sorte: Elementstyp
  syntype = (DSSyntype *)GetFirstSortRef();
  assert(syntype);
  elem_sort = (DSSort*)GetNextSortRef();
  
  InsertString(*out, "SCBoolean Save(SCMem &save_area) const",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "{", pos + 2, CG_WITH_EOL);
  if (!elem_sort->GetIsComplex())
  {
    InsertString(*out, "save_area.Store(array, (highBound - lowBound + 1) * sizeof(",
                 pos + 4, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    elem_sort->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);
    InsertString(*out, "));", 0, CG_WITH_EOL);
  }
  else
  {
    InsertString(*out, "SCInteger i;", pos + 4, CG_WITH_EOL);
    InsertLine(*out);
    InsertString(*out, "for (i = lowBound; i <= highBound; i++)", pos + 4, CG_WITH_EOL);
    InsertString(*out, "{", pos + 4, CG_WITH_EOL);
    InsertString(*out, "(*this)[i].Save(save_area);", pos + 6, CG_WITH_EOL);
    InsertString(*out, "}", pos + 4, CG_WITH_EOL);
  }
  InsertLine(*out);
  InsertString(*out, "return true;", pos + 4, CG_WITH_EOL); 
  InsertString(*out, "}", pos + 2, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGGenerator::WriteHistorySave(CGWriter *writer) const
{
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSSyntype *syntype;
  DSSortRef    elem_sort;
  DSString     str;

  if (*(GetGeneratorName()) != DS_GENERATOR_NAME_ARRAY)
  {
    return DS_OK;
  }

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  // 1. Sorte: Indexbereich
  // 2. Sorte: Elementstyp
  syntype = (DSSyntype *)GetFirstSortRef();
  assert(syntype);
  elem_sort = (DSSort*)GetNextSortRef();
  assert(elem_sort);
  
  InsertString(*out, "SCBoolean HistorySave(SCMem &save_area) const",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "{", pos + 2, CG_WITH_EOL);
  if (!elem_sort->GetIsComplex())
  {
    InsertString(*out, "save_area.HistoryStore(array, (highBound - lowBound + 1) * sizeof(",
                 pos + 4, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    elem_sort->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);
    InsertString(*out, "));", 0, CG_WITH_EOL);
  }
  else
  {
    InsertString(*out, "SCInteger i;", pos + 4, CG_WITH_EOL);
    InsertLine(*out);
    InsertString(*out, "for (i = lowBound; i <= highBound; i++)", pos + 4, CG_WITH_EOL);
    InsertString(*out, "{", pos + 4, CG_WITH_EOL);
    InsertString(*out, "(*this)[i].HistorySave(save_area);", pos + 6, CG_WITH_EOL);
    InsertString(*out, "}", pos + 4, CG_WITH_EOL);
  }
  InsertLine(*out);
  InsertString(*out, "return true;", pos + 4, CG_WITH_EOL); 
  InsertString(*out, "}", pos + 2, CG_WITH_EOL);

  return DS_OK;
}


DSResult CGGenerator::WriteRestore(CGWriter *writer) const
{
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSSyntype *syntype;
  DSSortRef    elem_sort;
  DSString     str;

  if (*(GetGeneratorName()) != DS_GENERATOR_NAME_ARRAY)
  {
    return DS_OK;
  }

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  // 1. Sorte: Indexbereich
  // 2. Sorte: Elementstyp
  syntype = (DSSyntype *)GetFirstSortRef();
  assert(syntype);
  elem_sort = (DSSort*)GetNextSortRef();
  assert(elem_sort);
  
  InsertString(*out, "SCBoolean Restore(SCMem &save_area)",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "{", pos + 2, CG_WITH_EOL);
  if (!elem_sort->GetIsComplex())
  {
    InsertString(*out, "save_area.Restore(array, (highBound - lowBound + 1) * sizeof(",
                 pos + 4, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    elem_sort->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);
    InsertString(*out, "));", 0, CG_WITH_EOL);
  }
  else
  {
    InsertString(*out, "SCInteger i;", pos + 4, CG_WITH_EOL);
    InsertLine(*out);
    InsertString(*out, "for (i = lowBound; i <= highBound; i++)", pos + 4, CG_WITH_EOL);
    InsertString(*out, "{", pos + 4, CG_WITH_EOL);
    InsertString(*out, "(*this)[i].Restore(save_area);", pos + 6, CG_WITH_EOL);
    InsertString(*out, "}", pos + 4, CG_WITH_EOL);
  }
  InsertLine(*out);
  InsertString(*out, "return true;", pos + 4, CG_WITH_EOL); 
  InsertString(*out, "}", pos + 2, CG_WITH_EOL);

  return DS_OK;
}


DSResult CGGenerator::WriteHistoryRestore(CGWriter *writer) const
{
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSSyntype *syntype;
  DSSortRef    elem_sort;
  DSString     str;

  if (*(GetGeneratorName()) != DS_GENERATOR_NAME_ARRAY)
  {
    return DS_OK;
  }

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  // 1. Sorte: Indexbereich
  // 2. Sorte: Elementstyp
  syntype = (DSSyntype *)GetFirstSortRef();
  assert(syntype);
  elem_sort = GetNextSortRef();
  assert(elem_sort);
  
  InsertString(*out, "SCBoolean HistoryRestore(SCMem &save_area)",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "{", pos + 2, CG_WITH_EOL);
  if (!elem_sort->GetIsComplex()) // flacher Datentyp ?
  {
    InsertString(*out, "save_area.HistoryRestore(array, (highBound - lowBound + 1) * sizeof(",
                 pos + 4, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    elem_sort->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);
    InsertString(*out, "));", 0, CG_WITH_EOL);
  }
  else
  {
    InsertString(*out, "SCInteger i;", pos + 4, CG_WITH_EOL);
    InsertLine(*out);
    InsertString(*out, "for (i = lowBound; i <= highBound; i++)", pos + 4, CG_WITH_EOL);
    InsertString(*out, "{", pos + 4, CG_WITH_EOL);
    InsertString(*out, "(*this)[i].HistoryRestore(save_area);", pos + 6, CG_WITH_EOL);
    InsertString(*out, "}", pos + 4, CG_WITH_EOL);
  }
  InsertLine(*out);
  InsertString(*out, "return true;", pos + 4, CG_WITH_EOL); 
  InsertString(*out, "}", pos + 2, CG_WITH_EOL);

  return DS_OK;
}


DSResult CGGenerator::WriteSize(CGWriter *writer) const
{
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSSyntype *syntype;
  DSSortRef    elem_sort;
  DSString     str;

  if (*(GetGeneratorName()) != DS_GENERATOR_NAME_ARRAY)
  {
    return DS_OK;
  }

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  // 1. Sorte: Indexbereich
  // 2. Sorte: Elementstyp
  syntype = (DSSyntype *)GetFirstSortRef();
  assert(syntype);
  elem_sort = (DSSort*)GetNextSortRef();
  assert(elem_sort);
  
  InsertString(*out, "void Size(SCSize *cur_size) const",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "{", pos + 2, CG_WITH_EOL);
  if (!elem_sort->GetIsComplex())
  {
    InsertString(*out, "cur_size->size += ((highBound - lowBound + 1) * sizeof(",
                 pos + 4, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    elem_sort->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);
    InsertString(*out, "));", 0, CG_WITH_EOL);
  }
  else
  {
    InsertString(*out, "SCSize elemSize;", pos + 4, CG_WITH_EOL);
    InsertLine(*out);
    InsertString(*out, "(*this)[lowBound].Size(&elemSize);", pos + 4, CG_WITH_EOL);
    InsertString(*out, "cur_size->size += ((highBound - lowBound + 1) * elemSize.size);",
                 pos + 4, CG_WITH_EOL);
  }
  InsertString(*out, "}", pos + 2, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGGenerator::WriteHistorySize(CGWriter *writer) const
{
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSSyntype *syntype;
  DSSortRef    elem_sort;
  DSString     str;

  if (*(GetGeneratorName()) != DS_GENERATOR_NAME_ARRAY)
  {
    return DS_OK;
  }

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  // 1. Sorte: Indexbereich
  // 2. Sorte: Elementstyp
  syntype = (DSSyntype *)GetFirstSortRef();
  assert(syntype);
  elem_sort = (DSSort*)GetNextSortRef();
  assert(elem_sort);
  
  InsertString(*out, "void HistorySize(SCSize *cur_size) const",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "{", pos + 2, CG_WITH_EOL);
  if (!elem_sort->GetIsComplex())
  {
    InsertString(*out, "cur_size->historySize += ((highBound - lowBound + 1) * sizeof(",
                 pos + 4, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    elem_sort->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);
    InsertString(*out, "));", 0, CG_WITH_EOL);
  }
  else
  {
    InsertString(*out, "SCSize elemSize;", pos + 4, CG_WITH_EOL);
    InsertLine(*out);
    InsertString(*out, "(*this)[lowBound].HistorySize(&elemSize);", pos + 4, CG_WITH_EOL);
    InsertString(*out, "cur_size->historySize += ((highBound - lowBound + 1) * elemSize.historySize);",
                 pos + 4, CG_WITH_EOL);
  }
  InsertString(*out, "}", pos + 2, CG_WITH_EOL);

  return DS_OK;
}


DSResult CGGenerator::WriteAssign(CGWriter *writer) const
{
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSSortRef    elem_sort;
  DSString     str;

  if (*(GetGeneratorName()) != DS_GENERATOR_NAME_STRING)
  {
    return DS_OK;
  }

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  elem_sort = (DSSyntype *)GetFirstSortRef();
  assert(elem_sort);
  
  ((CGWriter *)writer)->CGSetIndentPos(pos + 2);
  WriteIdentifier(writer);
  InsertString(*out, "& operator= (const SCString<", 0, CG_NO_EOL);
  ((CGWriter *)writer)->CGSetIndentPos(0);
  elem_sort->Write(writer, CG_WRITE_IDENTIFIER);
  InsertString(*out, "> & orig)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos + 2, CG_WITH_EOL);
  InsertString(*out, "*this = *((", pos + 4, CG_NO_EOL);
  WriteIdentifier(writer);
  ((CGWriter *)writer)->CGSetIndentPos(pos);
  InsertString(*out, " *)&orig);", 0, CG_WITH_EOL);
  InsertLine(*out);
  InsertString(*out, "return *this;", pos + 4, CG_WITH_EOL);
  InsertString(*out, "}", pos + 2, CG_WITH_EOL);

  return DS_OK;
}

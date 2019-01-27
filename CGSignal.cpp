/****************************************************************************
 *                 Modul: $RCSfile: CGSignal.cpp,v $
 *
 * $Author: md $
 * $Date: 1997/12/09 14:17:24 $
 * $Revision: 1.4 $
 *
 * Aufgabe: Dieses Modul enthaelt die Funktionen, mit denen der C++-Code aus
 *          der Datenstruktur generiert wird
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
#include <DS/DSSystem.h>
#include <DS/DSBlock.h>
#include <DS/DSProcess.h>
#include <DS/DSSort.h>
#include <DS/DSExpression.h>

#include "CGFile.h"
#include "CGSignal.h"

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

CGSignal::CGSignal(DSObject* father,
                   DSString *signal_name,
                   DSSortRefList* sort) :
  DSSignal(father, signal_name, sort)
{
}

CGSignal::CGSignal(DSObject* father,
                   DSString &signal_name,
                   DSSortRefList* sort) :
  DSSignal(father, signal_name, sort)
{
}

CGSignal::CGSignal(DSObject* father,
                   const char *signal_name,
                   DSSortRefList* sort) :
  DSSignal(father, signal_name, sort)
{
}

/****************************************************************************
 * Destruktoren
 ****************************************************************************/

CGSignal::~CGSignal(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGSignal::New(DSObject *father) const
{
  return new CGSignal(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer Signale
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/

DSResult CGSignal::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;
  
  switch (what)
  {
    case CG_WRITE_PARAM_STRUCT:
      result = WriteParamStruct((CGWriter *)writer);
      if (result != DS_OK) return result;
      break;
    case CG_WRITE_PARAM:
      result = WriteParam((CGWriter *)writer);
      if (result != DS_OK) return result;
      break;
    case CG_WRITE_ENUM:
      result = WriteTypeEnum((CGWriter *)writer);
      if (result != DS_OK) return result;
      break;
    case CG_WRITE_TYPE_H:
      result = WriteTypeHeader((CGWriter *)writer);
      if (result != DS_OK) return result;
      break;
    case CG_WRITE_TYPE_CC:
      result = WriteTypeImplementation((CGWriter *)writer);
      if (result != DS_OK) return result;
      break;
    case CG_WRITE_TYPE_INIT:
      result = WriteTypeInit((CGWriter *)writer);
      if (result != DS_OK) return result;
      break;
    case CG_WRITE_TYPE_FINISH:
      result = WriteTypeFinish((CGWriter *)writer);
      if (result != DS_OK) return result;
      break;
    case CG_WRITE_CREATE:
      result = WriteCreate((CGWriter *)writer);
      if (result != DS_OK) return result;
      break;
    default:
      assert(DS_FALSE);
      return DS_ERROR;
  }
  return DS_OK;
}

DSResult CGSignal::WriteDefaultConstructor(CGWriter *writer) const
{ 
  DSResult     result;
  DSSortRef    sort;
  CGFile      *out;
  CGPos        pos;
  DSCardinal   i = 1;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, PREFIX_SIGNAL_PARAM_TYPE, pos, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "(void)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  for (sort = GetFirstSortRef();
       sort; 
       sort = GetNextSortRef())
  {
    // Gibt es einen in SDL spezifizierten Default-Wert?
    if (sort->GetDefaultInitialization())
    {
      InsertString(*out, PREFIX_SIGNAL_FIELD, pos + 2, CG_NO_EOL);
      InsertCardinal(*out, i, 0, CG_NO_EOL);
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
      InsertString(*out, PREFIX_SIGNAL_FIELD, pos + 2, CG_NO_EOL);
      InsertCardinal(*out, i, 0, CG_NO_EOL);
      InsertString(*out, " = ", 0, CG_NO_EOL);
      InsertString(*out, sort->GetNeutralElement(), 0, CG_NO_EOL);
      InsertString(*out, ";", 0, CG_WITH_EOL);
    }
    // Ist es ein vordefinierter Datentyp?
    else if (!sort->GetIsComplex())
    {
      InsertString(*out, "memset(&", pos + 2, CG_NO_EOL);
      InsertString(*out, PREFIX_SIGNAL_FIELD, 0, CG_NO_EOL);
      InsertCardinal(*out, i, 0, CG_NO_EOL);
      InsertString(*out, ", 0, sizeof(", 0, CG_NO_EOL);
      writer->CGSetIndentPos(0);
      sort->Write(writer, CG_WRITE_IDENTIFIER);
      writer->CGSetIndentPos(pos);
      InsertString(*out, "));", 0, CG_WITH_EOL);
    }
    i++;
  }
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGSignal::WriteCopyConstructor(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, PREFIX_SIGNAL_PARAM_TYPE, pos, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "(const ", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_SIGNAL_PARAM_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "& orig)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "*this = orig;", pos + 2, CG_WITH_EOL);
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGSignal::WriteClone(CGWriter *writer) const
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
  InsertString(*out, PREFIX_SIGNAL_PARAM_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "(*this);", 0, CG_WITH_EOL);
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGSignal::WriteEqual(CGWriter *writer) const
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
  InsertString(*out, PREFIX_SIGNAL_PARAM_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " *)&second);", 0, CG_WITH_EOL);
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGSignal::WriteCompare(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSSortRef    sort;
  DSCardinal   i = 1;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean operator== (const ", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_SIGNAL_PARAM_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " &second) const", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "return (", pos + 2, CG_NO_EOL);

  for (sort = GetFirstSortRef();
       sort; 
       sort = GetNextSortRef())
  {
    InsertString(*out, PREFIX_SIGNAL_FIELD, 0, CG_NO_EOL);
    InsertCardinal(*out, i, 0, CG_NO_EOL);
    InsertString(*out, " == second.", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_SIGNAL_FIELD, 0, CG_NO_EOL);
    InsertCardinal(*out, i, 0, CG_NO_EOL);
    if (!GetSortRefList()->AtEndOfList())
    {
      InsertString(*out, " && ", 0, CG_NO_EOL);
    }
    i++;
  }
  InsertString(*out, ");", 0, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGSignal::WriteAssign(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSCardinal   i = 1;
  DSSortRef    sort;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, PREFIX_SIGNAL_PARAM_TYPE, pos, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "& operator= (const ", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_SIGNAL_PARAM_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " &orig)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  for (sort = GetFirstSortRef();
       sort; 
       sort = GetNextSortRef())
  {
    InsertString(*out, PREFIX_SIGNAL_FIELD, pos + 2, CG_NO_EOL);
    InsertCardinal(*out, i, 0, CG_NO_EOL);
    InsertString(*out, " = orig.", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_SIGNAL_FIELD, 0, CG_NO_EOL);
    InsertCardinal(*out, i, 0, CG_NO_EOL);
    InsertString(*out, ";", 0, CG_WITH_EOL);
    i++;
  }
  InsertString(*out, "return *this;", pos + 2, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGSignal::WriteDisplay(CGWriter *writer) const
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

DSResult CGSignal::WriteStreamOperator(CGWriter *writer) const
{ 
  DSResult   result;
  CGFile    *out;
  CGPos      pos;
  DSSortRef  sort;
  DSCardinal i = 1;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "friend SCStream& operator<<(SCStream &out, const ", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_SIGNAL_PARAM_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " &me)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  if (GetSortRefList())
  {
    if (GetSortRefList()->NumberOfElements() > 1)
    {
      InsertString(*out, "out << \"[\";", pos + 2, CG_WITH_EOL);
    }
  }
  for (sort = GetFirstSortRef();
       sort;
       sort = GetNextSortRef())
  {
    InsertString(*out, "out << me.", pos + 2, CG_NO_EOL);
    InsertString(*out, PREFIX_SIGNAL_FIELD, 0, CG_NO_EOL);
    InsertCardinal(*out, i, 0, CG_NO_EOL);
    if (!GetSortRefList()->AtEndOfList())
    {
      InsertString(*out, " << \", \"", 0, CG_NO_EOL);
    }
    InsertString(*out, ";", 0, CG_WITH_EOL);
    i++;
  }
  if (GetSortRefList())
  {
    if (GetSortRefList()->NumberOfElements() > 1)
    {
      InsertString(*out, "out << \"]\";", pos + 2, CG_WITH_EOL);
    }
  }
  InsertString(*out, "return out;", pos + 2, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGSignal::WriteSave(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSSortRef    sort_ref;
  DSString     str;
  DSCardinal   i;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean Save(SCMem &save_area) const",
               pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  i = 1;
  for (sort_ref = GetFirstSortRef();
       sort_ref;
       sort_ref = GetNextSortRef())
  {
    if (!sort_ref->GetIsComplex()) // kein komplexer Datentyp ?
    {
       str.Format("save_area.Store(&%s%d, sizeof(",
                  PREFIX_SIGNAL_FIELD, i);
       InsertString(*out, str, pos + 2, CG_NO_EOL);
       writer->CGSetIndentPos(0);
       sort_ref->Write(writer, CG_WRITE_IDENTIFIER);
       writer->CGSetIndentPos(pos);
       InsertString(*out, "));", 0, CG_WITH_EOL);
    }
    else
    {
      str.Format("%s%d.Save(save_area);",
                 PREFIX_SIGNAL_FIELD, i);
      InsertString(*out, str, pos + 2, CG_WITH_EOL);
    }
    i++;
  }
  InsertString(*out, "return true;", pos + 2, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGSignal::WriteRestore(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSSortRef    sort_ref;
  DSString     str;
  DSCardinal   i;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean Restore(SCMem &save_area)",
               pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  i = 1;
  for (sort_ref = GetFirstSortRef();
       sort_ref;
       sort_ref = GetNextSortRef())
  {
    if (!sort_ref->GetIsComplex())
    {
       str.Format("save_area.Restore(&%s%d, sizeof(",
                  PREFIX_SIGNAL_FIELD, i);
       InsertString(*out, str, pos + 2, CG_NO_EOL);
       writer->CGSetIndentPos(0);
       sort_ref->Write(writer, CG_WRITE_IDENTIFIER);
       writer->CGSetIndentPos(pos);
       InsertString(*out, "));", 0, CG_WITH_EOL);
    }
    else
    {
      str.Format("%s%d.Restore(save_area);",
                 PREFIX_SIGNAL_FIELD, i);
      InsertString(*out, str, pos + 2, CG_WITH_EOL);
    }
    i++;
  }
  InsertString(*out, "return true;", pos + 2, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGSignal::WriteSize(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSSortRef    sort_ref;
  DSString     str;
  DSCardinal   i;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "void Size(SCSize *cur_size) const",
               pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  i = 1;
  for (sort_ref = GetFirstSortRef();
       sort_ref;
       sort_ref = GetNextSortRef())
  {
    if (!sort_ref->GetIsComplex())
    {
       InsertString(*out, "cur_size->size += sizeof(", pos + 2, CG_NO_EOL);
       writer->CGSetIndentPos(0);
       sort_ref->Write(writer, CG_WRITE_IDENTIFIER);
       writer->CGSetIndentPos(pos);
       InsertString(*out, ");", 0, CG_WITH_EOL);
    }
    else
    {
      str.Format("%s%d.Size(cur_size);",
                 PREFIX_SIGNAL_FIELD, i);
      InsertString(*out, str, pos + 2, CG_WITH_EOL);
    }
    i++;
  }
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * WriteParamStruct(): Schreiben der Signalparameter in Structure
 *                     -> writer
 *                     Ergebnis: DS_OK oder DS_ERROR
 *                     Seiteneffekte: Anlegen und Fuellen der .h-Files
 ****************************************************************************/
 
DSResult CGSignal::WriteParamStruct(CGWriter *writer) const
{
  DSResult   result;
  DSCardinal i = 0;
  DSSortRef  sort;
  CGFile    *out;
  CGPos      pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  if (GetSortRefList()) 
  {
    InsertString(*out, "struct ", pos, CG_NO_EOL);
    InsertString(*out, PREFIX_SIGNAL_PARAM_TYPE, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
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
    writer->CGSetIndentPos(pos);
 
    InsertString(*out, "// Signalparameter:", pos + 2, CG_WITH_EOL);
    i = 0;
    for (sort = GetFirstSortRef();
         sort; 
         sort = GetNextSortRef())
    {
      writer->CGSetIndentPos(pos + 2);
      sort->Write(writer, CG_WRITE_IDENTIFIER);
      writer->CGSetIndentPos(pos);
      InsertString(*out, " ", 0, CG_NO_EOL);
      InsertString(*out, PREFIX_SIGNAL_FIELD, 0, CG_NO_EOL);
      InsertCardinal(*out, ++i, 0, CG_NO_EOL);
      InsertString(*out, ";", 0, CG_WITH_EOL);
    }
    writer->CGSetIndentPos(pos + 2);
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

    // Virtueller Equal-Operator:
    InsertString(*out, "// Virtueller Vergleichssoperator:", pos + 2, CG_WITH_EOL);
    result = WriteEqual(writer);
    if (result != DS_OK) return result;

    // <<-Operator:
    InsertString(*out, "// Ausgabeoperator:", pos + 2, CG_WITH_EOL);
    result = WriteStreamOperator(writer);
    if (result != DS_OK) return result;

    // Virtueller Display-Operator:
    InsertString(*out, "// Virtueller Ausgabeoperator:", pos + 2, CG_WITH_EOL);
    result = WriteDisplay(writer);
    if (result != DS_OK) return result;

    // Save-Methode:
    InsertString(*out, "// Virtuelle Save-Methode:", pos + 2, CG_WITH_EOL);
    result = WriteSave(writer);
    if (result != DS_OK) return result;

    // Restore-Methode:
    InsertString(*out, "// Virtuelle Restore-Methode:", pos + 2, CG_WITH_EOL);
    result = WriteRestore(writer);
    if (result != DS_OK) return result;

    // Size-Methode:
    InsertString(*out, "// Virtuelle Size-Methode:", pos + 2, CG_WITH_EOL);
    result = WriteSize(writer);
    if (result != DS_OK) return result;

    writer->CGSetIndentPos(pos);

    InsertString(*out, "};", pos, CG_WITH_EOL);
  }

  return DS_OK;
}

/****************************************************************************
 * WriteParam(): Schreiben der Signalparametervariablen (Zeiger auf struct)
 *               -> writer
 *               Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *               Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGSignal::WriteParam(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  if (GetSortRefList() != NULL) 
  {
    InsertString(*out, PREFIX_SIGNAL_PARAM_TYPE, pos, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, " *", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_SIGNAL_PARAM_VARIABLE, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, ";", 0, CG_WITH_EOL);
  }

  return DS_OK;
}

/****************************************************************************
 * WriteTypeImplementation(): schreibt Initialisierung der Signaltypen im C-File
 *                -> writer
 *                Ergebnis: DS_OK, falls alles geklappt hat,sonst DS_ERROR
 *                Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGSignal::WriteTypeImplementation(CGWriter *writer) const
{
  DSResult  result;
  DSObject *father;
  CGFile   *out;
  CGPos     pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  // Scope feststellen:
  father = GetParent();
  assert(father);

  writer->CGSetIndentPos(0);

  if (GetSortRefList())  // Signal MIT Parametern?
  {
    result = father->Write(writer, CG_WRITE_IDENTIFIER);
    if (result != DS_OK) return result;
    InsertString(*out, "::", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_SIGNAL_TYPE, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, " * ", 0, CG_NO_EOL);
    result = father->Write(writer, CG_WRITE_IDENTIFIER);
    if (result != DS_OK) return result;
  }
  else                  // Signal OHNE Parameter!
  {
    InsertString(*out, "SCSignalType * ", pos, CG_NO_EOL);
    result =father->Write(writer, CG_WRITE_IDENTIFIER);
    if (result != DS_OK) return result;
  }

  writer->CGSetIndentPos(pos);

  if (father->GetType() == DS_PROCESS)
  {
    InsertString(*out, "_data", 0, CG_NO_EOL);
  }
  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_SIGNAL_TYPE_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " = NULL;", 0, CG_WITH_EOL);

  return DS_OK; 
}

/****************************************************************************
 * WriteTypeInit(): schreibt Initialisierung der Signaltypen im C-File
 *                -> writer
 *                Ergebnis: DS_OK, falls alles geklappt hat,sonst DS_ERROR
 *                Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGSignal::WriteTypeInit(CGWriter *writer) const
{
  DSResult  result;
  CGFile   *out;
  CGPos     pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, PREFIX_SIGNAL_TYPE_VARIABLE, pos, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  if (GetSortRefList())
  {
    InsertString(*out, " = new ", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_SIGNAL_TYPE, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, "(", 0, CG_NO_EOL);
  }
  else
  {
    InsertString(*out, " = new SCSignalType(", 0, CG_NO_EOL);
  }
  InsertString(*out, PREFIX_SIGNAL_TYPE_ID, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, ", \"", 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "\");", 0, CG_WITH_EOL);

  return DS_OK; 
}

/****************************************************************************
 * WriteTypeFinish(): schreibt Initialisierung der Signaltypen im C-File
 *                -> writer
 *                Ergebnis: DS_OK, falls alles geklappt hat,sonst DS_ERROR
 *                Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGSignal::WriteTypeFinish(CGWriter *writer) const
{
  DSResult  result;
  CGFile   *out;
  CGPos     pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "delete ", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_SIGNAL_TYPE_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, ";", 0, CG_WITH_EOL);

  return DS_OK; 
}

/****************************************************************************
 * WriteTypeHeader(): schreibt Signaltypdefinitionen in eine Header-Datei
 *                    -> writer
 *                    Ergebnis: DS_OK, falls alles geklappt hat,sonst DS_ERROR
 *                    Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGSignal::WriteTypeHeader(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  if (GetSortRefList()) // Parameter vorhanden ?
  {
    InsertString(*out, "struct ", pos, CG_NO_EOL);
    InsertString(*out, PREFIX_SIGNAL_TYPE, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, ": public SCSignalType", 0, CG_WITH_EOL);
    InsertString(*out, "{", pos, CG_WITH_EOL);
    InsertString(*out, PREFIX_SIGNAL_TYPE, pos + 2, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, "(SCNatural type_key, const char *type_name) :", 0, CG_WITH_EOL);
    InsertString(*out, "SCSignalType(type_key, type_name)", pos + 4, CG_WITH_EOL);
    InsertString(*out, "{", pos + 2, CG_WITH_EOL);
    InsertString(*out, "}", pos + 2, CG_WITH_EOL);
    InsertString(*out, "SCDataType *NewData(void) const", pos + 2, CG_WITH_EOL);
    InsertString(*out, "{", pos + 2, CG_WITH_EOL);
    InsertString(*out, "return new ", pos + 4, CG_NO_EOL);
    InsertString(*out, PREFIX_SIGNAL_PARAM_TYPE, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, ";", 0, CG_WITH_EOL);
    InsertString(*out, "}", pos + 2, CG_WITH_EOL);
    InsertString(*out, "};", pos, CG_WITH_EOL);
    InsertString(*out, "static ", pos, CG_NO_EOL);
    InsertString(*out, PREFIX_SIGNAL_TYPE, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);        
    InsertString(*out, " * ", 0, CG_NO_EOL);
  }
  else
  {
    InsertString(*out, "static SCSignalType * ", pos, CG_NO_EOL);
  }
  InsertString(*out, PREFIX_SIGNAL_TYPE_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, ";", 0, CG_WITH_EOL);

  return DS_OK; 
}
    
/****************************************************************************
 * WriteTypeEnum(): Schreiben der SignalIDs
 *                  -> writer
 *                  Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                  Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGSignal::WriteTypeEnum(CGWriter *writer) const
{
  DSResult  result;
  DSBoolean first = DS_FALSE, last = DS_FALSE;
  DSObject *father;
  CGFile   *out;
  CGPos     pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  father = GetParent();

  switch(father->GetType())
  {
    case DS_PROCESS:
      if (((DSProcess *)father)->DSSignalComponent::GetSignalList()->AtStartOfList())
      {
        first = DS_TRUE;
      }
      if (((DSProcess *)father)->DSSignalComponent::GetSignalList()->AtEndOfList())
      {
        last = DS_TRUE;
      }
      break;
    case DS_BLOCK:
      if (((DSBlock *)father)->DSSignalComponent::GetSignalList()->AtStartOfList())
      {
        first = DS_TRUE;
      }
      if (((DSBlock *)father)->DSSignalComponent::GetSignalList()->AtEndOfList())
      {
        last = DS_TRUE;
      }
      break;
    case DS_BLOCKSUBSTRUCTURE:
      if (((DSBlockSubstructure *)father)->DSSignalComponent::GetSignalList()->AtStartOfList())
      {
        first = DS_TRUE;
      }
      if (((DSBlockSubstructure *)father)->DSSignalComponent::GetSignalList()->AtEndOfList())
      {
        last = DS_TRUE;
      }
      break;
    case DS_SYSTEM:
      if (((DSSystem *)father)->DSSignalComponent::GetSignalList()->AtStartOfList())
      {
        first = DS_TRUE;
      }
      if (((DSSystem *)father)->DSSignalComponent::GetSignalList()->AtEndOfList())
      {
        last = DS_TRUE;
      }
      break;
    default:
      assert(DS_FALSE);
      return DS_ERROR;
  }

  if (first)
  {
    InsertString(*out, "enum", pos, CG_WITH_EOL);
    InsertString(*out, "{", pos, CG_WITH_EOL);
  }
  InsertString(*out, PREFIX_SIGNAL_TYPE_ID, pos + 2, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  if (first)
  {
    InsertString(*out, " = kSCSignalTypeIDBase + ", 0, CG_NO_EOL);
    InsertCardinal(*out, writer->CGGetLastSignalID(), 0, CG_NO_EOL);
  }
  writer->CGSetLastSignalID(writer->CGGetLastSignalID() + 1);

  if (!last)
  {
    InsertString(*out, ",", 0, CG_WITH_EOL);
  }
  else
  {
    InsertString(*out, "", 0, CG_WITH_EOL);
    InsertString(*out, "};", pos, CG_WITH_EOL);
  }

  return DS_OK;
}

/****************************************************************************
 * WriteCreate(): Schreiben der einer Anweisung zum dynamischen Erzeugen
 *                der Signalparameter
 *                  -> writer
 *                  Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                  Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGSignal::WriteCreate(CGWriter *writer) const
{
  DSResult  result;
  CGFile   *out;
  CGPos     pos;

  if (GetSortRefList() != NULL) 
  {
    out = writer->CGGetFilePointer();
    assert(out);
    pos = writer->CGGetIndentPos();

    InsertString(*out, "case ", pos, CG_NO_EOL);
    InsertString(*out, PREFIX_SIGNAL_TYPE_ID, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, ":", 0, CG_WITH_EOL);
    InsertString(*out, "data_type = new ", pos + 2, CG_NO_EOL);
    InsertString(*out, PREFIX_SIGNAL_PARAM_TYPE, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, ";", 0, CG_WITH_EOL);
    InsertString(*out, "assert(data_type);", pos + 2, CG_WITH_EOL);
    InsertString(*out, "break;", pos + 2, CG_WITH_EOL);
  }

  return DS_OK;
}

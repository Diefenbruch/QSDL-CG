/****************************************************************************
 *                 Modul: $RCSfile: CGTimer.cpp,v $
 *
 * $Author: md $
 * $Date: 1997/12/09 14:17:24 $
 * $Revision: 1.3 $
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
#include <DS/DSProcess.h>
#include <DS/DSSort.h>
#include <DS/DSExpression.h>

#include "CGFile.h"
#include "CGTimer.h"

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

CGTimer::CGTimer(DSObject* father,
                 DSString *timer_name,
                 DSSortRefList* sort) :
  DSTimer(father, timer_name, sort)
{
}

CGTimer::CGTimer(DSObject* father,
                 DSString &timer_name,
                 DSSortRefList* sort) :
  DSTimer(father, timer_name, sort)
{
}

CGTimer::CGTimer(DSObject* father,
                 const char *timer_name,
                 DSSortRefList* sort) :
  DSTimer(father, timer_name, sort)
{
}

/****************************************************************************
 * Destruktoren
 ****************************************************************************/

CGTimer::~CGTimer(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGTimer::New(DSObject *father) const
{
  return new CGTimer(father);
}

/****************************************************************************
 * Write(): Schreiben der Timer
 *             -> writer
 *             -> what
 *             Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/

DSResult CGTimer::Write(DSWriter *writer, DSCardinal what) const
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
    case CG_WRITE_ENUM:
      result = WriteTypeEnum((CGWriter *)writer);
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


DSResult CGTimer::WriteDefaultConstructor(CGWriter *writer) const
{ 
  DSResult     result;
  DSSortRef    sort;
  CGFile      *out;
  CGPos        pos;
  DSCardinal   i = 1;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, PREFIX_TIMER_PARAM_TYPE, pos, CG_NO_EOL);
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
      InsertString(*out, PREFIX_TIMER_FIELD, pos + 2, CG_NO_EOL);
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
      InsertString(*out, PREFIX_TIMER_FIELD, pos + 2, CG_NO_EOL);
      InsertCardinal(*out, i, 0, CG_NO_EOL);
      InsertString(*out, " = ", 0, CG_NO_EOL);
      InsertString(*out, sort->GetNeutralElement(), 0, CG_NO_EOL);
      InsertString(*out, ";", 0, CG_WITH_EOL);
    }
    // Ist es ein flacher (nicht komplexer) Datentyp?
    else if (!sort->GetIsComplex())
    {
      InsertString(*out, "memset(&", pos + 2, CG_NO_EOL);
      InsertString(*out, PREFIX_TIMER_FIELD, 0, CG_NO_EOL);
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

DSResult CGTimer::WriteCopyConstructor(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, PREFIX_TIMER_PARAM_TYPE, pos, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "(const ", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_TIMER_PARAM_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "& orig)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "*this = orig;", pos + 2, CG_WITH_EOL);
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGTimer::WriteClone(CGWriter *writer) const
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
  InsertString(*out, PREFIX_TIMER_PARAM_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "(*this);", 0, CG_WITH_EOL);
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGTimer::WriteEqual(CGWriter *writer) const
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
  InsertString(*out, PREFIX_TIMER_PARAM_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " *)&second);", 0, CG_WITH_EOL);
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGTimer::WriteCompare(CGWriter *writer) const
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
  InsertString(*out, PREFIX_TIMER_PARAM_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " &second) const", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "return (", pos + 2, CG_NO_EOL);

  for (sort = GetFirstSortRef();
       sort; 
       sort = GetNextSortRef())
  {
    InsertString(*out, PREFIX_TIMER_FIELD, 0, CG_NO_EOL);
    InsertCardinal(*out, i, 0, CG_NO_EOL);
    InsertString(*out, " == second.", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_TIMER_FIELD, 0, CG_NO_EOL);
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

DSResult CGTimer::WriteAssign(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSCardinal   i = 1;
  DSSortRef    sort;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, PREFIX_TIMER_PARAM_TYPE, pos, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "& operator= (const ", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_TIMER_PARAM_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " &orig)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  for (sort = GetFirstSortRef();
       sort; 
       sort = GetNextSortRef())
  {
    InsertString(*out, PREFIX_TIMER_FIELD, pos + 2, CG_NO_EOL);
    InsertCardinal(*out, i, 0, CG_NO_EOL);
    InsertString(*out, " = orig.", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_TIMER_FIELD, 0, CG_NO_EOL);
    InsertCardinal(*out, i, 0, CG_NO_EOL);
    InsertString(*out, ";", 0, CG_WITH_EOL);
    i++;
  }
  InsertString(*out, "return *this;", pos + 2, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGTimer::WriteDisplay(CGWriter *writer) const
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

DSResult CGTimer::WriteStreamOperator(CGWriter *writer) const
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
  InsertString(*out, PREFIX_TIMER_PARAM_TYPE, 0, CG_NO_EOL);
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
    InsertString(*out, PREFIX_TIMER_FIELD, 0, CG_NO_EOL);
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

DSResult CGTimer::WriteSave(CGWriter *writer) const
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
    if (!sort_ref->GetIsComplex())
    {
       str.Format("save_area.Store(&%s%d, sizeof(",
                  PREFIX_TIMER_FIELD, i);
       InsertString(*out, str, pos + 2, CG_NO_EOL);
       writer->CGSetIndentPos(0);
       sort_ref->Write(writer, CG_WRITE_IDENTIFIER);
       writer->CGSetIndentPos(pos);
       InsertString(*out, "));", 0, CG_WITH_EOL);
    }
    else
    {
      str.Format("%s%d.Save(save_area);",
                 PREFIX_TIMER_FIELD, i);
      InsertString(*out, str, pos + 2, CG_WITH_EOL);
    }
    i++;
  }
  InsertString(*out, "return true;", pos + 2, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGTimer::WriteRestore(CGWriter *writer) const
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
                  PREFIX_TIMER_FIELD, i);
       InsertString(*out, str, pos + 2, CG_NO_EOL);
       writer->CGSetIndentPos(0);
       sort_ref->Write(writer, CG_WRITE_IDENTIFIER);
       writer->CGSetIndentPos(pos);
       InsertString(*out, "));", 0, CG_WITH_EOL);
    }
    else
    {
      str.Format("%s%d.Restore(save_area);",
                 PREFIX_TIMER_FIELD, i);
      InsertString(*out, str, pos + 2, CG_WITH_EOL);
    }
    i++;
  }
  InsertString(*out, "return true;", pos + 2, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGTimer::WriteSize(CGWriter *writer) const
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
      str.Format("%s.Size(cur_size);",
                 PREFIX_TIMER_FIELD, i);
      InsertString(*out, str, pos + 2, CG_WITH_EOL);
    }
    i++;
  }
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * WriteParamStruct(): Schreiben der Timerprameter in Structure
 *             -> writer
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *             Seiteneffekte: Anlegen und Fuellen der .h-Files
 ****************************************************************************/
 
DSResult CGTimer::WriteParamStruct(CGWriter *writer) const
{
  DSResult   result;
  DSCardinal i = 0;
  DSSortRef  sort;
  DSString  *tmp_str;
  CGFile    *out;
  CGPos      pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  if (GetSortRefList()) 
  {
    InsertString(*out, "struct ", pos, CG_NO_EOL);
    InsertString(*out, PREFIX_TIMER_PARAM_TYPE, 0, CG_NO_EOL);
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

    i = 0;
    for (sort = GetFirstSortRef();
         sort; 
         sort = GetNextSortRef())
    {
      InsertString(*out, sort->GetName(), pos + 2, CG_NO_EOL);
      InsertString(*out, " ", 0, CG_NO_EOL);
      InsertString(*out, PREFIX_TIMER_FIELD, 0, CG_NO_EOL);
      tmp_str = new DSString(++i);
      assert(tmp_str);
      InsertString(*out, tmp_str, 0, CG_NO_EOL);
      delete tmp_str;
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
 * WriteParam(): Schreiben der Timerparameter-Variablen
 *               -> writer
 *               Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *               Seiteneffekte: Anlegen und Fuellen der .h-Files
 ****************************************************************************/
 
DSResult CGTimer::WriteParam(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  if (GetSortRefList() != NULL) 
  {
    InsertString(*out, PREFIX_TIMER_PARAM_TYPE, pos, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, " *", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_TIMER_PARAM_VARIABLE, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, ";", 0, CG_WITH_EOL);
  }

  return DS_OK;
}

/****************************************************************************
 * WriteTypeImplementation(): schreibt Initialisierung der Timertypen im C-File
 *                -> writer
 *                Ergebnis: DS_OK, falls alles geklappt hat,sonst DS_ERROR
 *                Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGTimer::WriteTypeImplementation(CGWriter *writer) const
{
  DSResult   result;
  DSProcess *process;
  CGFile    *out;
  CGPos      pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  assert(GetParent());
  assert(GetParent()->GetType() == DS_PROCESS);
  process = (DSProcess *)GetParent();

  if (GetSortRefList())  // Timer MIT Parametern?
  {
    writer->CGSetIndentPos(0);
    result = process->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK) return result;
    InsertString(*out, "::", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_TIMER_TYPE, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, " * ", 0, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    result = process->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK) return result;
  }
  else                   // Timer OHNE Parameter!
  {
    InsertString(*out, "SCTimerType * ", pos, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    result = process->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK) return result;
  }
  
  InsertString(*out, "_data::", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_TIMER_TYPE_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " = NULL;", 0, CG_WITH_EOL);

  return DS_OK; 
}

/****************************************************************************
 * WriteTypeInit(): schreibt Initialisierung der Timertypen im C-File
 *                -> writer
 *                Ergebnis: DS_OK, falls alles geklappt hat,sonst DS_ERROR
 *                Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGTimer::WriteTypeInit(CGWriter *writer) const
{
  DSResult   result;
  CGFile    *out;
  CGPos      pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, PREFIX_TIMER_TYPE_VARIABLE, pos, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  if (GetSortRefList())
  {
    InsertString(*out, " = new ", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_TIMER_TYPE, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, "(", 0, CG_NO_EOL);
  }
  else
  {
    InsertString(*out, " = new SCTimerType(", 0, CG_NO_EOL);
  }
  InsertString(*out, PREFIX_TIMER_TYPE_ID, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, ", \"", 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "\");", 0, CG_WITH_EOL);

  return DS_OK; 
}


/****************************************************************************
 * WriteTypeFinish(): schreibt Initialisierung der Timertypen im C-File
 *                -> writer
 *                Ergebnis: DS_OK, falls alles geklappt hat,sonst DS_ERROR
 *                Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGTimer::WriteTypeFinish(CGWriter *writer) const
{
  DSResult   result;
  CGFile    *out;
  CGPos      pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "delete ", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_TIMER_TYPE_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, ";", 0, CG_WITH_EOL);

  return DS_OK; 
}

/****************************************************************************
 * WriteTypeHeader(): schreibt Timertypdefinitionen in eine Header-Datei
 *                    -> writer
 *                    Ergebnis: DS_OK, falls alles geklappt hat,sonst DS_ERROR
 *                    Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGTimer::WriteTypeHeader(CGWriter *writer) const
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
    InsertString(*out, PREFIX_TIMER_TYPE, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, ": public SCTimerType", 0, CG_WITH_EOL);
    InsertString(*out, "{", pos, CG_WITH_EOL);
    InsertString(*out, PREFIX_TIMER_TYPE, pos + 2, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, "(SCNatural type_key, const char *type_name) :", 0, CG_WITH_EOL);
    InsertString(*out, "SCTimerType(type_key, type_name)", pos + 4, CG_WITH_EOL);
    InsertString(*out, "{", pos + 2, CG_WITH_EOL);
    InsertString(*out, "}", pos + 2, CG_WITH_EOL);
    InsertString(*out, "SCDataType *NewData(void) const", pos + 2, CG_WITH_EOL);
    InsertString(*out, "{", pos + 2, CG_WITH_EOL);
    InsertString(*out, "return new ", pos + 4, CG_NO_EOL);
    InsertString(*out, PREFIX_TIMER_PARAM_TYPE, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, ";", 0, CG_WITH_EOL);
    InsertString(*out, "}", pos + 2, CG_WITH_EOL);
    InsertString(*out, "};", pos, CG_WITH_EOL);
    InsertString(*out, "static ", pos, CG_NO_EOL);
    InsertString(*out, PREFIX_TIMER_TYPE, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);        
    InsertString(*out, " * ", 0, CG_NO_EOL);
  }
  else
  {
    InsertString(*out, "static SCTimerType * ", pos, CG_NO_EOL);
  }
  InsertString(*out, PREFIX_TIMER_TYPE_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, ";", 0, CG_WITH_EOL);

  return DS_OK; 
}
    
/****************************************************************************
 * WriteTypeEnum(): Schreiben der TimerIDs
 *                  -> writer
 *                  Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                  Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGTimer::WriteTypeEnum(CGWriter *writer) const
{
  DSResult  result;
  CGFile   *out;
  CGPos     pos;
  DSBoolean first = DS_FALSE, last = DS_FALSE;
  DSObject *father;
  DSString *tmp_str;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  father = GetParent();
  assert(father);
  assert(father->GetType() == DS_PROCESS);

  if (((DSProcess *)father)->GetTimerList()->AtStartOfList())
  {
    first = DS_TRUE;
  }
  if (((DSProcess *)father)->GetTimerList()->AtEndOfList())
  {
    last = DS_TRUE;
  }

  if (first)
  {
    InsertString(*out, "enum", pos, CG_WITH_EOL);
    InsertString(*out, "{", pos, CG_WITH_EOL);
  }
  InsertString(*out, PREFIX_TIMER_TYPE_ID, pos + 2, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  if (first)
  {
    InsertString(*out, " = kSCSignalTypeIDBase + ", 0, CG_NO_EOL);
    tmp_str = new DSString(writer->CGGetLastSignalID());
    assert(tmp_str);
    InsertString(*out, tmp_str, 0, CG_NO_EOL);
    delete tmp_str;
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
 *                der Timerparameter
 *                  -> writer
 *                  Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                  Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGTimer::WriteCreate(CGWriter *writer) const
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
    InsertString(*out, PREFIX_TIMER_TYPE_ID, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, ":", 0, CG_WITH_EOL);
    InsertString(*out, "data_type = new ", pos + 2, CG_NO_EOL);
    InsertString(*out, PREFIX_TIMER_PARAM_TYPE, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, ";", 0, CG_WITH_EOL);
    InsertString(*out, "assert(data_type);", pos + 2, CG_WITH_EOL);
    InsertString(*out, "break;", pos + 2, CG_WITH_EOL);
  }

  return DS_OK;
}

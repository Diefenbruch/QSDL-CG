/****************************************************************************
 *                 Modul: $RCSfile: CGSyntype.cpp,v $
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
#include <DS/DSSystem.h>
#include <DS/DSBlock.h>
#include <DS/DSProcess.h>
#include <DS/DSRange.h>
#include <DS/DSExpression.h>
 
#include "CGFile.h"
#include "CGSyntype.h"
#include "CGSort.h"

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
 
CGSyntype::CGSyntype(DSObject    *father,
                     DSString    *sort_name,
                     DSSortRef    father_sort,
                     DSRangeList *r_list) :
  DSSyntype(father, sort_name, father_sort, r_list)
{
}

CGSyntype::CGSyntype(DSObject    *father,
                     DSString&    sort_name,
                     DSSortRef    father_sort,
                     DSRangeList *r_list) :
  DSSyntype(father, sort_name, father_sort, r_list)
{
}

CGSyntype::CGSyntype(DSObject    *father,
                     const char  *sort_name,
                     DSSortRef    father_sort,
                     DSRangeList *r_list) :
  DSSyntype(father, sort_name, father_sort, r_list)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGSyntype::~CGSyntype(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGSyntype::New(DSObject *father) const
{
  return new CGSyntype(father);
}
 
/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer einen Syntype
 *                   -> writer
 *                   -> what
 *                   Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                             DS_ERROR
 *                   Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/
 
DSResult CGSyntype::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  switch (what)
  {
    case CG_WRITE_IDENTIFIER:
      result = WriteIdentifier((CGWriter *)writer);
      break;

    case CG_WRITE_H:
      result = WriteSyntype((CGWriter *)writer);
      break;

    default:
      return DS_OK;
  }
  return result;
}

/****************************************************************************
 * WriteSyntype(): schreibt eine SCL-Syntype
 *                 -> writer
 *                 Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                 Seiteneffekte: Anlegen und Fuellen der Files
 ****************************************************************************/
 
DSResult CGSyntype::WriteSyntype(CGWriter *writer) const
{ 
  DSResult  result;
  DSRange  *range;
  CGFile   *out;
  CGPos     pos;
  DSObject *father;
  DSString  str;
  DSSortRef sort;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  str.Format("SDL-Syntype: '%s':", GetName()->GetString());
  InsertHead(*out, str, pos);
  
  range = GetFirstRange();
  if (!range)                                   /* SYNTYPE ohne Wertebereichs*/
  {                                             /* einschraenkung: typedef...*/
    InsertString(*out, "typedef ", pos, CG_NO_EOL);
    InsertString(*out, GetParentSortRef()->GetName(), 0, CG_NO_EOL);
    InsertString(*out, " ", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_DATA_TYPE, 0, CG_NO_EOL);
    InsertString(*out, GetName(), 0, CG_NO_EOL);
    InsertString(*out, ";", 0, CG_WITH_EOL);

    return DS_OK;
  }

  InsertString(*out, "struct ", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_DATA_TYPE, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL); 
  InsertString(*out, " : public SCSyntype<", 0, CG_NO_EOL);

  father = GetParent();
  assert (father);

  if (father->GetType() == DS_PROCEDURE)
  {
    while(father->GetType() == DS_PROCEDURE)
    {
      father = father->GetParent();
      assert(father);
    }
    father->Write(writer, CG_WRITE_IDENTIFIER);
    InsertString(*out, "::", 0, CG_NO_EOL);
  }

  sort = GetParentSortRef();
  writer->CGSetIndentPos(0);
  sort->Write(writer, CG_WRITE_IDENTIFIER);
  writer->CGSetIndentPos(pos);
  InsertString(*out, ">", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  // Konstruktor OHNE Initialisierungsparameter:
  InsertString(*out, PREFIX_DATA_TYPE, pos + 2, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL); 
  InsertString(*out, "(void) : SCSyntype<", 0, CG_NO_EOL);
  if (GetParent()->GetType() == DS_PROCEDURE)
  {
    father->Write(writer, CG_WRITE_IDENTIFIER);
    InsertString(*out, "::", 0, CG_NO_EOL);
  }
  writer->CGSetIndentPos(0);
  sort->Write(writer, CG_WRITE_IDENTIFIER);
  writer->CGSetIndentPos(pos);
  InsertString(*out, "> (", 0, CG_NO_EOL);
  if (!GetFirstRange()->GetLowerBound()->IsConstant() &&
      DSSort::GetParent()->GetType() == DS_PROCEDURE)
  {
    father->Write(writer, CG_WRITE_IDENTIFIER);
    InsertString(*out, "::", 0, CG_NO_EOL);
  }
  writer->CGSetIndentPos(0);
  result = GetFirstRange()->GetLowerBound()->Write(writer);
  writer->CGSetIndentPos(pos);
  if (result != DS_OK) return result;
  InsertString(*out, ", ", 0, CG_NO_EOL);
  if (!GetFirstRange()->GetUpperBound()->IsConstant() &&
      DSSort::GetParent()->GetType() == DS_PROCEDURE)
  {
    father->Write(writer, CG_WRITE_IDENTIFIER);
    InsertString(*out, "::", 0, CG_NO_EOL);
  }
  writer->CGSetIndentPos(0);
  result = GetFirstRange()->GetUpperBound()->Write(writer);
  writer->CGSetIndentPos(pos);
  if (result != DS_OK) return result;

  // Lower-Bound als Default Initialwert nutzen:
  InsertString(*out, ", ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = GetFirstRange()->GetLowerBound()->Write(writer);
  writer->CGSetIndentPos(pos);
  if (result != DS_OK) return result;
  InsertString(*out, ", NULL) {}", 0, CG_WITH_EOL);

  // Konstruktor MIT Initialisierungsparameter:
  InsertString(*out, PREFIX_DATA_TYPE, pos + 2, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL); 
  InsertString(*out, "(", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  sort->Write(writer, CG_WRITE_IDENTIFIER);
  writer->CGSetIndentPos(pos);
  InsertString(*out, " init) : SCSyntype<", 0, CG_NO_EOL);
  if (GetParent()->GetType() == DS_PROCEDURE)
  {
    father->Write(writer, CG_WRITE_IDENTIFIER);
    InsertString(*out, "::", 0, CG_NO_EOL);
  }
  writer->CGSetIndentPos(0);
  sort->Write(writer, CG_WRITE_IDENTIFIER);
  writer->CGSetIndentPos(pos);
  InsertString(*out, "> (", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = GetFirstRange()->GetLowerBound()->Write(writer);
  writer->CGSetIndentPos(pos);
  if (result != DS_OK) return result;
  InsertString(*out, ", ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = GetFirstRange()->GetUpperBound()->Write(writer);
  writer->CGSetIndentPos(pos);
  if (result != DS_OK) return result;
  InsertString(*out, ", init, NULL) {}", 0, CG_WITH_EOL);  

  // =-Operator (kann nicht geerbt werden!)
  InsertString(*out, "// Zuweisungsoperator von Basistyp auf Syntype:", pos + 2, CG_WITH_EOL);
  InsertString(*out, PREFIX_DATA_TYPE, pos + 2, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL); 
  InsertString(*out, "& operator=(", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  sort->Write(writer, CG_WRITE_IDENTIFIER);
  writer->CGSetIndentPos(pos);
  InsertString(*out, " new_value)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos + 2, CG_WITH_EOL);
  InsertString(*out, "SetValue(new_value);", pos + 4, CG_WITH_EOL);
  InsertString(*out, "return *this;", pos + 4, CG_WITH_EOL);
  InsertString(*out, "}", pos + 2, CG_WITH_EOL);

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

  InsertString(*out, "};", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGSyntype::WriteSave(CGWriter *writer) const
{
  DSResult result;
  CGFile  *out;
  CGPos    pos;
  DSString str;
  DSSortRef sort = GetParentSortRef();

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean Save(SCMem &save_area) const",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "{", pos + 2, CG_WITH_EOL);
  if (!GetParentSortRef()->GetIsComplex())
  {
    InsertString(*out, "save_area.Store(&value, sizeof(", pos + 4, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    sort->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);
    InsertString(*out, "));", 0, CG_WITH_EOL);
  }
  else // komplexer Datentyp mit eigener Save()-Methode !
  {
    InsertString(*out, "value.Save(save_area);", pos + 4, CG_WITH_EOL);
  }
  InsertLine(*out);
  InsertString(*out, "return true;", pos + 4, CG_WITH_EOL); 
  InsertString(*out, "}", pos + 2, CG_WITH_EOL);

  return DS_OK;
}


DSResult CGSyntype::WriteHistorySave(CGWriter *writer) const
{
  DSResult result;
  CGFile  *out;
  CGPos    pos;
  DSString str;
  DSSortRef sort = GetParentSortRef();

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean HistorySave(SCMem &save_area) const",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "{", pos + 2, CG_WITH_EOL);
  if (!GetParentSortRef()->GetIsComplex())
  {
    InsertString(*out, "save_area.HistoryStore(&value, sizeof(", pos + 4, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    sort->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);
    InsertString(*out, "));", 0, CG_WITH_EOL);
  }
  else // komplexer Datentyp mit eigener Save()-Methode !
  {
    InsertString(*out, "value.HistorySave(save_area);", pos + 4, CG_WITH_EOL);
  }
  InsertLine(*out);
  InsertString(*out, "return true;", pos + 4, CG_WITH_EOL); 
  InsertString(*out, "}", pos + 2, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGSyntype::WriteRestore(CGWriter *writer) const
{
  DSResult result;
  CGFile  *out;
  CGPos    pos;
  DSString str;
  DSSortRef sort = GetParentSortRef();

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean Restore(SCMem &save_area)",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "{", pos + 2, CG_WITH_EOL);
  if (!GetParentSortRef()->GetIsComplex())
  {
    InsertString(*out, "save_area.Restore(&value, sizeof(", pos + 4, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    sort->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);
    InsertString(*out, "));", 0, CG_WITH_EOL);
  }
  else // komplexer Datentyp mit eigener Restore()-Methode !
  {
    InsertString(*out, "value.Restore(save_area);", pos + 6, CG_WITH_EOL);
  }
  InsertLine(*out);
  InsertString(*out, "return true;", pos + 4, CG_WITH_EOL); 
  InsertString(*out, "}", pos + 2, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGSyntype::WriteHistoryRestore(CGWriter *writer) const
{
  DSResult result;
  CGFile  *out;
  CGPos    pos;
  DSString str;
  DSSortRef sort = GetParentSortRef();

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean HistoryRestore(SCMem &save_area)",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "{", pos + 2, CG_WITH_EOL);
  if (!GetParentSortRef()->GetIsComplex())
  {
    InsertString(*out, "save_area.HistoryRestore(&value, sizeof(", pos + 4, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    sort->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);
    InsertString(*out, "));", 0, CG_WITH_EOL);
  }
  else // komplexer Datentyp mit eigener Restore()-Methode !
  {
    InsertString(*out, "value.HistoryRestore(save_area);", pos + 6, CG_WITH_EOL);
  }
  InsertLine(*out);
  InsertString(*out, "return true;", pos + 4, CG_WITH_EOL); 
  InsertString(*out, "}", pos + 2, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGSyntype::WriteSize(CGWriter *writer) const
{
  DSResult result;
  CGFile  *out;
  CGPos    pos;
  DSString str;
  DSSortRef sort = GetParentSortRef();

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "void Size(SCSize *cur_size) const",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "{", pos + 2, CG_WITH_EOL);
  if (!GetParentSortRef()->GetIsComplex())
  {
    InsertString(*out, "cur_size->size += sizeof(", pos + 4, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    sort->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);
    InsertString(*out, ");", 0, CG_WITH_EOL);
  }
  else // komplexer Datentyp mit eigener Size()-Methode !
  {
    InsertString(*out, "value.Size(cur_size);", pos + 4, CG_WITH_EOL);
  }
  InsertString(*out, "}", pos + 2, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGSyntype::WriteHistorySize(CGWriter *writer) const
{
  DSResult result;
  CGFile  *out;
  CGPos    pos;
  DSString str;
  DSSortRef sort = GetParentSortRef();

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "void HistorySize(SCSize *cur_size) const",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "{", pos + 2, CG_WITH_EOL);
  if (!GetParentSortRef()->GetIsComplex())
  {
    InsertString(*out, "cur_size->historySize += sizeof(", pos + 4, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    sort->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);
    InsertString(*out, ");", 0, CG_WITH_EOL);
  }
  else // komplexer Datentyp mit eigener HistorySize()-Methode !
  {
    InsertString(*out, "value.HistorySize(cur_size);", pos + 4, CG_WITH_EOL);
  }
  InsertString(*out, "}", pos + 2, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGSyntype::WriteIdentifier(CGWriter *writer) const
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


/****************************************************************************
 * GetIsComplex(): Liefert boolesches Flag ob es sich um einen komplexen
 *                 d.h. zusammengesetzten Datentyp handelt.
 *                 Ergebnis: Zeiger auf einen Enum-Wert
 *                 Seiteneffekte: keine
 ****************************************************************************/

DSBoolean CGSyntype::GetIsComplex(void) const
{
  if (!DSSort::GetIsComplex())
    return DS_FALSE;
    
  if (GetFirstRange()) // Syntypes mit Range werden als eigene
    return DS_TRUE;    // Klassen umgesetzt!
    
  if (!GetParentSortRef())
    return DS_FALSE;
    
  return GetParentSortRef()->GetIsComplex();
}


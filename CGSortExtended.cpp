/****************************************************************************
 *                 Modul: $RCSfile: CGSortExtended.cpp,v $
 *
 * $Author: hirche $
 * $Date: 1997/08/29 13:35:30 $
 * $Revision: 1.1 $
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

#include "CGFile.h"
#include "CGSortExtended.h"

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
 
CGSortExtended::CGSortExtended(DSObject          *father,
                               DSString          *sort_name,
                               DSSortTag          t,
                               DSStringList      *l_list,
                               DSOperatorKeyList *o_list,
                               DSBoolean          pre,
                               DSBoolean          comp) :
  DSSortExtended(father, sort_name, t, l_list, o_list, pre, comp)
{
}
 
CGSortExtended::CGSortExtended(DSObject          *father,
                               DSString&          sort_name,
                               DSSortTag          t,
                               DSStringList      *l_list,
                               DSOperatorKeyList *o_list,
                               DSBoolean          pre,
                               DSBoolean          comp) :
  DSSortExtended(father, sort_name, t, l_list, o_list, pre, comp)
{
}
 
CGSortExtended::CGSortExtended(DSObject          *father,
                               const char        *sort_name,
                               DSSortTag          t,
                               DSStringList      *l_list,
                               DSOperatorKeyList *o_list,
                               DSBoolean          pre,
                               DSBoolean          comp) :
  DSSortExtended(father, sort_name, t, l_list, o_list, pre, comp)
{
}
 
/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGSortExtended::~CGSortExtended(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGSortExtended::New(DSObject *father) const
{
  return new CGSortExtended(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer SortExtended
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst DS_ERROR
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/
 
DSResult CGSortExtended::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result = DS_OK;

  switch (what)
  {
    case CG_WRITE_H:
      if (GetIsPredefined()) return DS_OK;

      result = WriteLiteralList((CGWriter *)writer);
      result = WriteOperatorList((CGWriter *)writer);
      break;
    case CG_WRITE_CC:
      break;
    case CG_WRITE_INIT:
      break;
    case CG_WRITE_LITERAL_LIST:
      if (GetIsPredefined()) return DS_OK;

      result = WriteLiteralList((CGWriter *)writer);
      break;
    case CG_WRITE_OPERATOR_LIST:
      if (GetIsPredefined()) return DS_OK;

      result = WriteOperatorList((CGWriter *)writer);
      break;
    case CG_WRITE_IDENTIFIER:
      result = WriteIdentifier((CGWriter *)writer);
      break;
    default:
      assert(DS_FALSE);
      return DS_ERROR;
  }

  return result;
}

/****************************************************************************
 * WriteLiteralList(): schreibt die Literalliste eines SDL-Datentyps
 *                     als C-enum.
 *                     -> writer
 *                     Ergebnis: CG_OK, falls alles geklappt hat, sonst DS_ERROR
 *                     Seiteneffekte: Anlegen und Fuellen der Files
 ****************************************************************************/
 
DSResult CGSortExtended::WriteLiteralList(CGWriter *writer) const
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
      InsertString(*out, "", 0, CG_WITH_EOL);
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
 
DSResult CGSortExtended::WriteOperatorList(CGWriter *writer) const
{ 
  DSResult result;

  result = Run(writer, DS_OPERATOR);

  return result;
}

DSResult CGSortExtended::WriteIdentifier(CGWriter *writer) const
{ 
  DSResult   result;
  CGFile    *out;
  CGPos      pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

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

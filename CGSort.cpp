/****************************************************************************
 *                 Modul: $RCSfile: CGSort.cpp,v $
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
 
CGSort::CGSort(DSObject *father,
               DSString *sort_name,
               DSSortTag t,
               DSBoolean pre,
               DSBoolean comp,
               DSExpression *init) :
  DSSort(father, sort_name, t, pre, comp, init)
{
}
 
CGSort::CGSort(DSObject *father,
               DSString& sort_name,
               DSSortTag t,
               DSBoolean pre,
               DSBoolean comp,
               DSExpression *init) :
  DSSort(father, sort_name, t, pre, comp, init)
{
}
 
CGSort::CGSort(DSObject   *father,
               const char *sort_name,
               DSSortTag   t,
               DSBoolean   pre,
               DSBoolean   comp,
               DSExpression *init) :
  DSSort(father, sort_name, t, pre, comp, init)
{
}
 
/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGSort::~CGSort(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGSort::New(DSObject *father) const
{
  return new CGSort(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer Sort
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst DS_ERROR
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/
 
DSResult CGSort::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result = DS_OK;

  switch (what)
  {
    case CG_WRITE_IDENTIFIER:
      result = WriteIdentifier((CGWriter *)writer);
      break;
    default:
      if (GetIsPredefined()) return DS_OK;

      assert(DS_FALSE);
      return DS_ERROR;
  }

  return result;
}

DSResult CGSort::WriteIdentifier(CGWriter *writer) const
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

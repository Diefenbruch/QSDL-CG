/****************************************************************************
 *                 Modul: $RCSfile: CGOperator.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/01/14 10:59:58 $
 * $Revision: 1.2 $
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

#include <DS/DSSort.h>
 
#include "CGFile.h"
#include "CGOperator.h"

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
 
CGOperator::CGOperator(DSObject      *father,
                       DSString      *n,
                       DSSortRefList *sort_list,
                       DSSortRef      result) :
  DSOperator(father, n, sort_list, result)
{
}
 
CGOperator::CGOperator(DSObject      *father,
                       const char    *n,
                       DSSortRefList *sort_list,
                       DSSortRef      result) :
  DSOperator(father, n, sort_list, result)
{
}
 
/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGOperator::~CGOperator(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGOperator::New(DSObject *father) const
{
  return new CGOperator(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer Operator
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGOperator::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;
 
  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;
 
  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt einen selbstdefinierten Operator
 *                       -> writer
 *                       Ergebnis: CG_OK, falls alles geklappt hat, sonst 
 *                                 DS_ERROR
 *                      Seiteneffekte: Anlegen und Fuellen der Files
 ****************************************************************************/
 
DSResult CGOperator::WriteImplementation(CGWriter *writer) const
{ 
  DSResult    result;
  DSCardinal  i = 0;
  DSSortRef   sort;
  CGFile     *out;
  CGPos       pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  if (GetResultSortRef())
  {
    InsertString(*out, GetResultSortRef()->GetName(),
                 pos, CG_NO_EOL);
  }
  else
  {
    InsertString(*out, "void", pos, CG_NO_EOL);
  }
  InsertString(*out, GetName(), 1, CG_NO_EOL);
  InsertString(*out, "(", 0, CG_NO_EOL);
  for (sort = GetFirstSortRef();
       sort;
       sort = GetNextSortRef())
  {
    if (i > 0)  InsertString(*out, ", ", 0, CG_NO_EOL);
    InsertString(*out, sort->GetName(), 0, CG_NO_EOL);
    i++;
  }
  InsertString(*out, ");", 0, CG_WITH_EOL);

  return DS_OK;
}

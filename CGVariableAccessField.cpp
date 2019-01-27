/****************************************************************************
 *                 Modul: $RCSfile: CGVariableAccessField.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/01/14 10:59:58 $
 * $Revision: 1.2 $
 *
 * Aufgabe: Dieses Modul enthaelt Hilfsfunktionen
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

#include <DS/DSNameSort.h>

#include "CGFile.h"
#include "CGVariableAccessField.h"

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

CGVariableAccessField::CGVariableAccessField(DSObject      *father,
                                             DSNameSortRef  field) :
  DSVariableAccessField(father, field)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGVariableAccessField::~CGVariableAccessField(void)
{
}

DSObject *CGVariableAccessField::New(DSObject *father) const
{
  return new CGVariableAccessField(father);
}
 
/****************************************************************************
 * Write(): schreibt den Zugriff auf eine SCArray-Variable
 *                   -> writer
 *                   -> what
 *                   Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                             DS_ERROR
 *                   Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/
 
DSResult CGVariableAccessField::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteVariableAccessField((CGWriter *)writer);

  return result;
}

/****************************************************************************
 * Write(): schreibt Zugriff auf ein Variablenfeld
 *                   -> writer
 *                   -> what
 *                   Ergebnis: CG_OK,falls Aktion erfolgreich war, sonst 
 *                             DS_OK
 *                   Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGVariableAccessField::WriteVariableAccessField(CGWriter *writer) const
{
  DSResult result;
  DSNameSortRef name_sort;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, ".", 0, CG_NO_EOL);
  name_sort = GetField();
  assert(name_sort);
  InsertString(*out, PREFIX_FIELD, 0, CG_NO_EOL);
  InsertString(*out, name_sort->GetName(), 0, CG_NO_EOL);

  return DS_OK;
}

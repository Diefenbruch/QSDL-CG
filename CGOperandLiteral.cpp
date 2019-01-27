/****************************************************************************
 *                 Modul: $RCSfile: CGOperandLiteral.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/01/14 10:59:58 $
 * $Revision: 1.3 $
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

#include <ctype.h>  // fuer toupper()

#include <DS/DSString.h>

#include "CGFile.h"
#include "CGOperandLiteral.h"

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

CGOperandLiteral::CGOperandLiteral(DSObject *father,
                                   DSString *n, DSSortRef s) :
  DSOperandLiteral(father, n, s)
{
}

CGOperandLiteral::CGOperandLiteral(DSObject *father,
                                   DSString &n, DSSortRef s) :
  DSOperandLiteral(father, n, s)
{
}

CGOperandLiteral::CGOperandLiteral(DSObject *father,
                                   const char *n, DSSortRef s) :
  DSOperandLiteral(father, n, s)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGOperandLiteral::~CGOperandLiteral(void)
{
}

DSObject *CGOperandLiteral::New(DSObject *father) const
{
  return new CGOperandLiteral(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer OperandLiteral
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGOperandLiteral::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;

  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt einen als Literal gespeicherten Operanden
 *          -> writer
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst  DS_ERROR
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGOperandLiteral::WriteImplementation(CGWriter *writer) const
{ 
  DSString s_literal1(DS_SORT_LITERAL_NAME_FCFS);
  DSString s_literal2(DS_SORT_LITERAL_NAME_FCFSPNP);
  DSString s_literal3(DS_SORT_LITERAL_NAME_FCFSPP);
  DSString s_literal4(DS_SORT_LITERAL_NAME_IS);
  DSString s_literal5(DS_SORT_LITERAL_NAME_LCFSPP);
  DSString s_literal6(DS_SORT_LITERAL_NAME_PS);
  DSString s_literal7(DS_SORT_LITERAL_NAME_RANDOM);
  DSString s_literal8(DS_SORT_LITERAL_NAME_NULL);
  DSResult result;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  assert(GetName());
  
  if (*GetName() == s_literal1)
  {
    s_literal1.ToUpper();
    InsertString(*out, s_literal1, pos, CG_NO_EOL);
    return DS_OK;
  }
  if (*GetName() == s_literal2)
  {
    s_literal2.ToUpper();
    InsertString(*out, s_literal2, pos, CG_NO_EOL);
    return DS_OK;
  }
  if (*GetName() == s_literal3)
  {
    s_literal3.ToUpper();
    InsertString(*out, s_literal3, pos, CG_NO_EOL);
    return DS_OK;
  }
  if (*GetName() == s_literal4)
  {
    s_literal4.ToUpper();
    InsertString(*out, s_literal4, pos, CG_NO_EOL);
    return DS_OK;
  }
  if (*GetName() == s_literal5)
  {
    s_literal5.ToUpper();
    InsertString(*out, s_literal5, pos, CG_NO_EOL);
    return DS_OK;
  }
  if (*GetName() == s_literal6)
  {
    s_literal6.ToUpper();
    InsertString(*out, s_literal6, pos, CG_NO_EOL);
    return DS_OK;
  }
  if (*GetName() == s_literal7) // bei Random nur 1. Buchstaben gross!
  {
    s_literal7[0] = toupper(s_literal7[0]);
    InsertString(*out, s_literal7, pos, CG_NO_EOL);
    return DS_OK;
  }
  if (*GetName() == s_literal8)
  {
    InsertString(*out, "(SCProcessID)NULL", pos, CG_NO_EOL);
    return DS_OK;
  }
  InsertString(*out, GetName(), pos, CG_NO_EOL);
  return DS_OK;
}

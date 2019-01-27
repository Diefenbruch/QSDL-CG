/****************************************************************************
 *                 Modul: $RCSfile: CGOperandExpression.cpp,v $
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

#include <DS/DSExpression.h>

#include "CGFile.h"
#include "CGOperandExpression.h"

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

CGOperandExpression::CGOperandExpression(DSObject *father,
                                         DSExpression *exp) :
  DSOperandExpression(father, exp)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGOperandExpression::~CGOperandExpression(void)
{
}

DSObject *CGOperandExpression::New(DSObject *father) const
{
  return new CGOperandExpression(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer OperandExpression
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGOperandExpression::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;

  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt einen als Ausdruck gespeicherten Operanden
 *          -> writer
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst DS_ERROR
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGOperandExpression::WriteImplementation(CGWriter *writer) const
{                                                
  DSResult result;
  
  result = GetExpression()->Write(writer);
  if (result != DS_OK)
  { 
    return result;
  }
  return DS_OK;
}

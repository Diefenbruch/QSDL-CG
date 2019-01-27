/****************************************************************************
 *                 Modul: $RCSfile: CGOperandVariableAccess.cpp,v $
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

#include <DS/DSVariableAccess.h>

#include "CGFile.h"
#include "CGOperandVariableAccess.h"

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

CGOperandVariableAccess::CGOperandVariableAccess(DSObject         *father,
                                                 DSVariableAccess *access) :
  DSOperandVariableAccess(father, access)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGOperandVariableAccess::~CGOperandVariableAccess(void)
{
}

DSObject *CGOperandVariableAccess::New(DSObject *father) const
{
  return new CGOperandVariableAccess(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer OperandVariable
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGOperandVariableAccess::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;

  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt einen als Variable gespeicherten Operanden 
 *                        -> writer
 *                        -> what
 *                        Ergebnis: DS_OK,falls Aktion erfolgreich war,
 *                                  sonst DS_ERROR
 *                        Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGOperandVariableAccess::WriteImplementation(CGWriter *writer) const
{
  DSVariableAccess *var_access;
  DSResult result;

  var_access = GetVariableAccess();
  assert(var_access);
  result = var_access->Write(writer);

  return result;
}

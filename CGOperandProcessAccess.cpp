/****************************************************************************
 *                 Modul: $RCSfile: CGOperandProcessAccess.cpp,v $
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

#include <DS/DSProcessAccess.h>

#include "CGFile.h"
#include "CGOperandProcessAccess.h"

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

CGOperandProcessAccess::CGOperandProcessAccess(DSObject *father,
                                               DSProcessAccess * p_access) :
  DSOperandProcessAccess(father, p_access)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGOperandProcessAccess::~CGOperandProcessAccess(void)
{
}

DSObject *CGOperandProcessAccess::New(DSObject *father) const
{
  return new CGOperandProcessAccess(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer OperandProcessAccess
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGOperandProcessAccess::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;

  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt einen als ProcessAccess gespeicherten Operanden
 *                        -> writer
 *                        -> what
 *                        Ergebnis: DS_OK,falls Aktion erfolgreich war,
                                    sonst DS_ERROR
 *                        Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGOperandProcessAccess::WriteImplementation(CGWriter *writer) const
{
  DSProcessAccess * p_access;
  DSResult result;
  CGFile *out;

  out = writer->CGGetFilePointer();
  assert(out);

  p_access = GetProcessAccess();
  result = p_access->Write(writer, CG_WRITE_IDENTIFIER);

  return result;
}

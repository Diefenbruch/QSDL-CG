/****************************************************************************
 *                 Modul: $RCSfile: CGOperandState.cpp,v $
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

#include <DS/DSState.h>

#include "CGFile.h"
#include "CGOperandState.h"

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

CGOperandState::CGOperandState(DSObject *father,
                                 DSStateRef sig_ref) :
  DSOperandState(father, sig_ref)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGOperandState::~CGOperandState(void)
{
}

DSObject *CGOperandState::New(DSObject *father) const
{
  return new CGOperandState(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer OperandState
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGOperandState::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;

  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt einen als State gespeicherten Operanden
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst DS_ERROR
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGOperandState::WriteImplementation(CGWriter *writer) const
{    
  DSStateRef state;
  DSResult result;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  state = GetStateRef();
  assert(state);
  assert(state->GetParent());
  
  writer->CGSetIndentPos(0);
  state->GetParent()->Write(writer, CG_WRITE_IDENTIFIER);
  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_STATE_TYPE_ID, 0, CG_NO_EOL);
  InsertString(*out, state->GetFirstName(), 0, CG_NO_EOL);
  writer->CGSetIndentPos(pos);

  return result;
}

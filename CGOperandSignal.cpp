/****************************************************************************
 *                 Modul: $RCSfile: CGOperandSignal.cpp,v $
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

#include <DS/DSSignal.h>

#include "CGFile.h"
#include "CGOperandSignal.h"

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

CGOperandSignal::CGOperandSignal(DSObject *father,
                                 DSSignalRef sig_ref) :
  DSOperandSignal(father, sig_ref)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGOperandSignal::~CGOperandSignal(void)
{
}

DSObject *CGOperandSignal::New(DSObject *father) const
{
  return new CGOperandSignal(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer OperandSignal
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGOperandSignal::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;

  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt einen als Signal gespeicherten Operanden
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst DS_ERROR
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGOperandSignal::WriteImplementation(CGWriter *writer) const
{    
  DSSignalRef sig_ref;
  DSResult result;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  sig_ref = GetSignalRef();
  assert(sig_ref);
  assert(sig_ref->GetParent());
  
  writer->CGSetIndentPos(0);
  sig_ref->GetParent()->Write(writer, CG_WRITE_IDENTIFIER);
  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_SIGNAL_TYPE_ID, 0, CG_NO_EOL);
  InsertString(*out, sig_ref->GetName(), 0, CG_NO_EOL);
  writer->CGSetIndentPos(pos);

  return result;
}

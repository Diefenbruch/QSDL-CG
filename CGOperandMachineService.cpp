/****************************************************************************
 *                 Modul: $RCSfile: CGOperandMachineService.cpp,v $
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

#include <DS/DSMachineService.h>

#include "CGFile.h"
#include "CGOperandMachineService.h"

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

CGOperandMachineService::CGOperandMachineService(DSObject *father,
                                 DSMachineServiceRef sen_ref) :
  DSOperandMachineService(father, sen_ref)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGOperandMachineService::~CGOperandMachineService(void)
{
}

DSObject *CGOperandMachineService::New(DSObject *father) const
{
  return new CGOperandMachineService(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer OperandMachineService
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGOperandMachineService::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;

  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt einen als MachineService gespeicherten Operanden
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst DS_ERROR
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGOperandMachineService::WriteImplementation(CGWriter *writer) const
{    
  DSMachineServiceRef ms_ref;
  DSResult result;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  ms_ref = GetMachineServiceRef();
  assert(ms_ref);
  assert(ms_ref->GetParent());

  writer->CGSetIndentPos(0);
  ms_ref->GetParent()->Write(writer, CG_WRITE_IDENTIFIER);
  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_MACHINE_SERVICE_TYPE_ID, 0, CG_NO_EOL);
  InsertString(*out, ms_ref->GetName(), 0, CG_NO_EOL);
  writer->CGSetIndentPos(pos);

  return result;
}

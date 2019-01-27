/****************************************************************************
 *                 Modul: $RCSfile: CGMachineServiceSpeed.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/01/14 10:59:58 $
 * $Revision: 1.2 $
 *
 * Aufgabe: Dieses Modul enthaelt die Funktionen, mit denen der C++-Code aus
 *          der Datenstruktur generiert wird
 * Funktionen: Write(): Oberfunktion der Codegenerierung 
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
#include <DS/DSMachine.h>
#include <DS/DSExpression.h>

#include "CGFile.h"
#include "CGMachineServiceSpeed.h"

#ifdef DEBUG
#include <dmalloc.h>
#endif

/****************************************************************************
 * Konstruktoren
 ****************************************************************************/

CGMachineServiceSpeed::CGMachineServiceSpeed(DSObject            *father,
                                             DSMachineServiceRef service,
                                             DSExpression        *spd) :
  DSMachineServiceSpeed(father, service, spd)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGMachineServiceSpeed::~CGMachineServiceSpeed(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGMachineServiceSpeed::New(DSObject *father) const
{
  return new CGMachineServiceSpeed(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer Set
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGMachineServiceSpeed::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;
 
  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;
 
  return DS_OK;
}
 
/****************************************************************************
 * WriteImplementation(): Oberfunktion der Codegenerierung
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *          Seiteneffekte: Fuellen der Files
 ****************************************************************************/

DSResult CGMachineServiceSpeed::WriteImplementation(CGWriter *writer) const
{
  DSResult result, status;
  DSMachine *machine;
  CGFile *out;
  CGPos pos;
  DSString str;
  DSCardinal counter = 0;
  DSMachineServiceRefList *machine_service_list;
  DSMachineServiceRef service_global;
  DSMachineServiceRef service_local;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  assert(GetParent()->GetType() == DS_MACHINE);
  machine = (DSMachine *)GetParent();

  machine_service_list = writer->CGGetServiceList();
  assert(machine_service_list);

  service_local = GetMachineServiceRef();
  assert(service_local);
  
  // Nachschauen, welchen Index dieser Dienst hat:
  for (status = machine_service_list->MoveFirst();
       status == DS_OK;
       status = machine_service_list->MoveNext())
  {
    service_global = machine_service_list->GetCurrentData();
    assert(service_global);
    if (service_local == service_global) // Dienst gefunden?
    {
      break;
    }
    counter++;
  }
  assert(status == DS_OK);

  InsertString(*out, PREFIX_MACHINE_SPEED, pos, CG_NO_EOL);
  InsertString(*out, machine->GetName(), 0, CG_NO_EOL);
  InsertString(*out, "[", 0, CG_NO_EOL);
  str.Format("%d", counter);
  InsertString(*out, str, 0, CG_NO_EOL);
  InsertString(*out, "] = ", 0, CG_NO_EOL);

  writer->CGSetIndentPos(0);
  if ((result = GetSpeed()->Write(writer)) != DS_OK)
  {
    return result;
  }
  writer->CGSetIndentPos(pos);
  InsertString(*out, ";", 0, CG_WITH_EOL);

  return DS_OK;
}

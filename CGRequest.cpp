/****************************************************************************
 *                 Modul: $RCSfile: CGRequest.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/01/14 10:59:58 $
 * $Revision: 1.5 $
 *
 * Aufgabe: Dieses Modul enthaelt die Generierung des C++-Codes fuer 
 *          entsprechende SDL-Aktionen
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
#include <DS/DSBlock.h>
#include <DS/DSExpression.h>

#include "CGFile.h"
#include "CGRequest.h"

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

CGRequest::CGRequest(DSObject           *father,
                     DSString           *label_name,
                     DSMachineServiceRef req_service,
                     DSExpression       *amount,
                     DSExpression       *prio,
                     DSLinkRefList      *via_links,
                     DSPipeRefList      *via_pipes) :
  DSRequest(father, label_name, req_service, amount, prio, via_links,
            via_pipes)
{
}

CGRequest::CGRequest(DSObject           *father,
                     DSString&           label_name,
                     DSMachineServiceRef req_service,
                     DSExpression       *amount,
                     DSExpression       *prio,
                     DSLinkRefList      *via_links,
                     DSPipeRefList      *via_pipes) :
  DSRequest(father, label_name, req_service, amount, prio, via_links,
            via_pipes)
{
}

CGRequest::CGRequest(DSObject           *father,
                     const char         *label_name,
                     DSMachineServiceRef req_service,
                     DSExpression       *amount,
                     DSExpression       *prio,
                     DSLinkRefList      *via_links,
                     DSPipeRefList      *via_pipes) :
  DSRequest(father, label_name, req_service, amount, prio, via_links,
            via_pipes)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGRequest::~CGRequest(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGRequest::New(DSObject *father) const
{
  return (DSRequest *)new CGRequest(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer Request
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGRequest::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;

  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt einen DSRequest-Typ
 *            -> writer
 *            -> what
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst DS_ERROR 
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGRequest::WriteImplementation(CGWriter *writer) const
{
  DSResult result;
  DSMachineServiceRef service;
  DSMachineRef machine;
  DSMachineRefList *destination_machine_list;
  DSString str;
  DSBlockRef block;
  CGFile *out;
  CGPos pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  result = WriteLabel(writer);
  if (result != DS_OK) return result;
  
  service = GetMachineServiceRef();
  assert(service);

  destination_machine_list = GetMachineServiceDestination();

  if (!GetMachineRef()) // keine FROM-Syntax sondern VIA-Syntax?
  {
    if (!destination_machine_list || destination_machine_list->IsEmpty())
    {
      std::cerr << "Can't determine destination for service "
           << GetMachineServiceRef()->GetName() << std::endl;
      if (destination_machine_list) delete destination_machine_list;
      return DS_OK;
    }

    if (destination_machine_list->NumberOfElements() > 1)
    {
      std::cerr << "Warning: More than one destination for service '"
           << (GetMachineServiceRef())->GetName() << "', using first." << std::endl;
    }
    assert(destination_machine_list->MoveFirst() == DS_OK);
    machine = destination_machine_list->GetCurrentData();
    assert(machine);
  }
  else
  {
    machine = GetMachineRef();
  }
  
  str.Format("%scur_state = %srequest_%d;", PREFIX_VARIABLE,
             PREFIX_STATE_TYPE_ID,
             writer->CGGetLastRequestID());
  InsertString(*out, str, pos, CG_WITH_EOL);
  InsertString(*out, CG_NAME_SCL_REQUEST, pos, CG_NO_EOL);
  InsertString(*out, "(", 0, CG_NO_EOL);
  block = machine->GetThisBlock();
  assert(block);
  writer->CGSetIndentPos(0);
  block->Write(writer, CG_WRITE_IDENTIFIER);
  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_MACHINE, 0, CG_NO_EOL);  // hier NICHT
                                                     // WriteIdentifier() verwenden!
  InsertString(*out, machine->GetName(), 0, CG_NO_EOL);
  writer->CGSetIndentPos(pos);
  InsertString(*out, ", ", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_MACHINE_SERVICE_TYPE_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, GetMachineServiceRef()->GetName(), 0, CG_NO_EOL);
  InsertString(*out, ", ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = GetMachineServiceAmount()->Write(writer);
  writer->CGSetIndentPos(pos);
  if (result != DS_OK) return result;
  if (GetPriority())
  {
    InsertString(*out, ", ", 0, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    result = GetPriority()->Write(writer);
    writer->CGSetIndentPos(pos);
  }
  InsertString(*out, ");", 0, CG_WITH_EOL);

  //
  // Backtracking-Behandlung:
  //
    
  str.Format("if (%scur_state != %srequest_%d)",
             PREFIX_VARIABLE, PREFIX_STATE_TYPE_ID, writer->CGGetLastRequestID());
  InsertStringComment(*out, str.GetString(), "Backtracked ?", pos);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  str.Format("if (IsInState())",
             PREFIX_VARIABLE, PREFIX_STATE_TYPE_ID);
  InsertStringComment(*out, str.GetString(), "Backtracked to normal state ?", pos + 2);
  InsertString(*out, "{", pos + 2, CG_WITH_EOL);
  str.Format("%stransition_id = Transition(&%ssignal_read);",
             PREFIX_VARIABLE, PREFIX_VARIABLE);
  InsertStringComment(*out, str.GetString(), "Choose next transition !", pos + 4);
  InsertString(*out, "}", pos + 2, CG_WITH_EOL);
  InsertStringComment(*out, "goto backtracking;", "Jump to current state !", pos + 2);
  InsertString(*out, "}", pos, CG_WITH_EOL);
  str.Format("label_request_%d:", writer->CGGetLastRequestID());
  InsertString(*out, str, pos, CG_WITH_EOL);
  InsertStringComment(*out, "SetCallScheduler(true);", "Important if process was recreated !", pos);
  writer->CGSetLastRequestID(writer->CGGetLastRequestID() + 1);

  delete destination_machine_list;

  return DS_OK;
}     

/****************************************************************************
 * WriteLabel(): Schreibt das optionale Label der SDL-Aktion
 *               -> writer
 *               Ergebnis: DS_OK, falls Aktion erfolgreich war, sonst 
 *                         enthaelt DSResult das Ergebnis
 *               Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGRequest::WriteLabel(CGWriter *writer) const
{
  DSResult result;
  DSStringKeyList *label_list;
  DSString *new_label;
  CGFile *out;
  CGPos pos;

  if (GetLabel() == NULL) return DS_OK;

  out = writer->CGGetFilePointer();
  assert(out);

  pos = writer->CGGetIndentPos();

  label_list = writer->CGGetLabelList();

  if (label_list == NULL ||  // kann innerhalb einer Starttransition passieren
      !label_list->IsKeyInList(GetLabel()))
  {
    InsertString(*out, PREFIX_LABEL, pos, CG_NO_EOL);
    InsertString(*out, GetLabel(), 0, CG_NO_EOL);
    InsertString(*out, ":", 0, CG_WITH_EOL);
    if (label_list != NULL)
    {
      new_label = new DSString(*GetLabel());
      assert(new_label);
      assert(label_list->Append(new DSString(*new_label), new_label) == DS_OK);
    }
  }
  
  return DS_OK;
}     

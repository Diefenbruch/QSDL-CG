/****************************************************************************
 *                 Modul: $RCSfile: CGOutput.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/01/14 10:59:58 $
 * $Revision: 1.3 $
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
#include <DS/DSBlock.h>
#include <DS/DSProcess.h>
#include <DS/DSSignal.h>
#include <DS/DSExpression.h>

#include "CGFile.h"
#include "CGOutput.h"

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

CGOutput::CGOutput(DSObject* father, DSString* name,
                   DSSignalRef sig, DSActualParamList* param) :
  DSOutput(father, name, sig, param)
{
}

CGOutput::CGOutput(DSObject* father, DSString& name,
                   DSSignalRef sig, DSActualParamList* param) :
  DSOutput(father, name, sig, param)
{
}

CGOutput::CGOutput(DSObject* father, const char* name,
                   DSSignalRef sig, DSActualParamList* param) :
  DSOutput(father, name, sig, param)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGOutput::~CGOutput(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGOutput::New(DSObject *father) const
{
  return (DSOutput *)new CGOutput(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer Output
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGOutput::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;

  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt einen Output
 *          -> writer
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst DS_ERROR
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGOutput::WriteImplementation(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;
  DSSignalRef signal;
  DSProcessRef process_ref;
  DSProcessRefList *destination_process_list;
  DSActualParam *actual_param;
  DSCardinal i = 1;
  DSBlockRef block;
  DSSortRef sort;
  DSString str;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  result = WriteLabel(writer);
  if (result != DS_OK) return result;

  assert(GetSignalRef());

  signal = GetSignalRef();
  assert(signal);

  // Signalparameter vorhanden?
  if (GetActualParamList())
  {
    InsertString(*out, PREFIX_SIGNAL_PARAM_VARIABLE, pos, CG_NO_EOL);
    InsertString(*out, signal->GetName(), 0, CG_NO_EOL);
    InsertString(*out, " = new ", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_SIGNAL_PARAM_TYPE, 0, CG_NO_EOL);
    InsertString(*out, signal->GetName(), 0, CG_NO_EOL);
    InsertString(*out, ";", 0, CG_WITH_EOL);
    InsertString(*out, "assert(", pos, CG_NO_EOL);
    InsertString(*out, PREFIX_SIGNAL_PARAM_VARIABLE, 0, CG_NO_EOL);
    InsertString(*out, signal->GetName(), 0, CG_NO_EOL);
    InsertString(*out, ");", 0, CG_WITH_EOL);
    for (actual_param = GetFirstActualParam(),
         sort = signal->GetFirstSortRef();
         actual_param;
         actual_param = GetNextActualParam(),
         sort = signal->GetNextSortRef())
    {
      InsertString(*out, PREFIX_SIGNAL_PARAM_VARIABLE, pos, CG_NO_EOL);
      InsertString(*out, signal->GetName(), 0,
                   CG_NO_EOL);
      InsertString(*out, "->", 0, CG_NO_EOL);
      str.Format("%s%d", PREFIX_SIGNAL_FIELD, i);
      InsertString(*out, str, 0, CG_NO_EOL);
      InsertString(*out, " = ", 0, CG_NO_EOL);
      writer->CGSetIndentPos(0);
      if ((result = actual_param->Write(writer) ) != DS_OK)
      {
        return result;
      }
      InsertString(*out, ";", 0, CG_WITH_EOL);
      writer->CGSetIndentPos(pos);

      i++;
    }
  }

  // Empfaenger ueber TO spezifiziert?
  if (GetProcessID())
  {
    InsertString(*out, CG_NAME_SCL_OUTPUT, pos, CG_NO_EOL);
    InsertString(*out, "(", 0, CG_NO_EOL);

    writer->CGSetIndentPos(0);
    if ((result = GetProcessID()->Write(writer) ) != DS_OK)
    {
      return result;
    }
    writer->CGSetIndentPos(pos);
  }
  else  // Empfaenger NICHT ueber TO spezifiziert!
  {
    // Signalziel(e) aus statischer Systemstruktur bestimmen:
    destination_process_list = GetSignalDestination();
    if (!destination_process_list || destination_process_list->IsEmpty())
    {
      writer->cgCout << "Can't determine destination for signal '"
                     << (GetSignalRef())->GetName() << "'" << std::endl;
      if (destination_process_list) delete destination_process_list;
      return DS_OK;
	  }

    if (destination_process_list->NumberOfElements() > 1)
    {
      writer->cgCout << "Warning: More than one destination for signal '"
                     << (GetSignalRef())->GetName() << "', using first." << std::endl;
    }

    InsertString(*out, CG_NAME_SCL_OUTPUT, pos, CG_NO_EOL);
    InsertString(*out, "(", 0, CG_NO_EOL);

    assert(destination_process_list->MoveFirst() == DS_OK);
    process_ref = destination_process_list->GetCurrentData();
    assert(process_ref);
    block = process_ref->GetThisBlock();
    assert(block);
    writer->CGSetIndentPos(0);
    block->Write(writer, CG_WRITE_IDENTIFIER);
    writer->CGSetIndentPos(pos);
    str.Format("::%s%s",
               PREFIX_PROCESS_TYPE_VARIABLE,
               process_ref->GetName()->GetString());
    InsertString(*out, str, 0, CG_NO_EOL);
    delete destination_process_list;
  }

  str.Format(", %s%s", PREFIX_SIGNAL_TYPE_VARIABLE,
             signal->GetName()->GetString());
  InsertString(*out, str, 0, CG_NO_EOL);
  if (GetActualParamList())
  {
    str.Format(", %s%s",
               PREFIX_SIGNAL_PARAM_VARIABLE,
               signal->GetName()->GetString());
    InsertString(*out, str, 0, CG_NO_EOL);
  }
  else if (GetDelay())
  {
    InsertString(*out, ", NULL", 0, CG_NO_EOL);
  }

  if (GetDelay())
  {
    InsertString(*out, ", ", 0, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    result = GetDelay()->Write(writer);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK)
    {
      return result;
    }
  }
  InsertString(*out, ");", 0, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * WriteLabel(): Schreibt das optionale Label der SDL-Aktion
 *               -> writer
 *               Ergebnis: DS_OK, falls Aktion erfolgreich war, sonst 
 *                         enthaelt DSResult das Ergebnis
 *               Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGOutput::WriteLabel(CGWriter *writer) const
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

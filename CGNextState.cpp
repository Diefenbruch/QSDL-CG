/****************************************************************************
 *                 Modul: $RCSfile: CGNextState.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/01/14 10:59:58 $
 * $Revision: 1.2 $
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
#include <DS/DSState.h>

#include "CGFile.h"
#include "CGNextState.h"

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

CGNextState::CGNextState(DSObject *father,
                         DSString *label_name,
                         DSStateRef state) :
  DSNextState(father, label_name, state)
{
}

CGNextState::CGNextState(DSObject *father,
                         DSString& label_name,
                         DSStateRef state) :
  DSNextState(father, label_name, state)
{
}

CGNextState::CGNextState(DSObject *father,
                         const char *label_name,
                         DSStateRef state) :
  DSNextState(father, label_name, state)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGNextState::~CGNextState(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGNextState::New(DSObject *father) const
{
  return (DSNextState *)new CGNextState(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer NextState
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGNextState::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;

  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt einen DSNextState-Typ
 *          -> writer
 *          Ergebnis: CG_OK,falls Aktion erfolgreich war, sonst 
 *                    DS_ERROR
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGNextState::WriteImplementation(CGWriter *writer) const
{
  DSResult result;
  DSString str;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  result = WriteLabel(writer); // Label
  if (result != DS_OK) return result;
  
  assert(GetNextStateRef());
  assert(GetNextStateRef()->GetFirstName());
  str.Format("%scur_state = %s%s;",
             PREFIX_VARIABLE, PREFIX_STATE_TYPE_ID,
             GetNextStateRef()->GetFirstName()->GetString());
  InsertString(*out, str, pos, CG_WITH_EOL);

  InsertString(*out, "goto nextstate", pos, CG_NO_EOL);
  InsertString(*out, ";", 0, CG_WITH_EOL);

  return DS_OK;
}     

/****************************************************************************
 * WriteLabel(): Schreibt das optionale Label der SDL-Aktion
 *               -> writer
 *               Ergebnis: DS_OK, falls Aktion erfolgreich war, sonst 
 *                         enthaelt DSResult das Ergebnis
 *               Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGNextState::WriteLabel(CGWriter *writer) const
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
  else // Doppeltes Label!
  {
    if (writer->CGVerbose())
    {
      std::cerr << "Duplicate label '" << GetLabel() << "'." << std::endl;
    }
  }
  
  return DS_OK;
}     

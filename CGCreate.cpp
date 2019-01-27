/****************************************************************************
 *                 Modul: $RCSfile: CGCreate.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/01/29 15:34:28 $
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

#include "CGFile.h"
#include "CGProcess.h"
#include "CGCreate.h"
#include "CGFile.h"
#include "CGVariable.h"
#include "CGExpression.h"

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

CGCreate::CGCreate(DSObject *father,
                   DSString *label_name,
                   DSProcessRef proc,
                   DSActualParamList *p_list) :
  DSCreate(father, label_name, proc, p_list)
{
}

CGCreate::CGCreate(DSObject *father,
                   DSString& label_name,
                   DSProcessRef proc,
                   DSActualParamList *p_list) :
  DSCreate(father, label_name, proc, p_list)
{
}

CGCreate::CGCreate(DSObject *father,
                   const char *label_name,
                   DSProcessRef proc,
                   DSActualParamList *p_list) :
  DSCreate(father, label_name, proc, p_list)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGCreate::~CGCreate(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGCreate::New(DSObject *father) const
{
  return (DSCreate *)new CGCreate(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer Create
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGCreate::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;

  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt einen DSAction-Typ
 *          -> writer
 *          Ergebnis: CG_OK,falls Aktion erfolgreich war, sonst DS_ERROR
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGCreate::WriteImplementation(CGWriter *writer) const
{
  DSResult result;
  DSProcessRef process;
  DSActualParam *actual_param;
  DSFormalParam *formal_param;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  pos = writer->CGGetIndentPos();

  result = WriteLabel(writer);
  if (result != DS_OK) return result;

  process = DSCreate::GetProcessRef();
  assert(process);

  if (GetActualParamList())
  {
    InsertString(*out, PREFIX_PROCESS_PARAM_VARIABLE, pos, CG_NO_EOL);
    InsertString(*out, process->GetName(), 0, CG_NO_EOL);
    InsertString(*out, " = new ", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_PROCESS_PARAM_TYPE, 0, CG_NO_EOL);
    InsertString(*out, process->GetName(), 0, CG_NO_EOL);
    InsertString(*out, ";", 0, CG_WITH_EOL);
    InsertString(*out, "assert(", pos, CG_NO_EOL);
    InsertString(*out, PREFIX_PROCESS_PARAM_VARIABLE, 0, CG_NO_EOL);
    InsertString(*out, process->GetName(), 0, CG_NO_EOL);
    InsertString(*out, ");", 0, CG_WITH_EOL);
  }

  for (actual_param = GetFirstActualParam(),
       formal_param = process->GetFirstFormalParam(); 
       actual_param && formal_param;
       actual_param = GetNextActualParam(),
       formal_param = process->GetNextFormalParam())
  {
    InsertString(*out, PREFIX_PROCESS_PARAM_VARIABLE, pos, CG_NO_EOL);     
    InsertString(*out, process->GetName(),
                 0, CG_NO_EOL); 
    InsertString(*out, "->", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_VARIABLE, 0, CG_NO_EOL);
    InsertString(*out, formal_param->GetName(), 0, CG_NO_EOL);
    InsertString(*out, " = ", 0, CG_NO_EOL);
    writer->CGSetIndentPos(0);
    result = actual_param->Write(writer);
    writer->CGSetIndentPos(pos);
    if (result != DS_OK) return result;
    InsertString(*out, ";", 0, CG_WITH_EOL);
  }

  InsertString(*out, CG_NAME_SCL_CREATE, pos, CG_NO_EOL);
  InsertString(*out, "(", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_PROCESS_TYPE_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, process->GetName(),
               0, CG_NO_EOL); 
  if (process->GetFormalParamList())
  {
    InsertString(*out, ", ", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_PROCESS_PARAM_VARIABLE, 0, CG_NO_EOL);
    InsertString(*out, process->GetName(),
                 0, CG_NO_EOL);
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

DSResult CGCreate::WriteLabel(CGWriter *writer) const
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

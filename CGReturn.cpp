/****************************************************************************
 *                 Modul: $RCSfile: CGReturn.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/01/14 11:31:39 $
 * $Revision: 1.4 $
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
#include <DS/DSVariable.h>

#include "CGFile.h"
#include "CGReturn.h"
#include "CGProcedure.h"

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

CGReturn::CGReturn(DSObject* father, DSExpression* ret, DSString* name) :
  DSReturn(father, ret, name)
{
}

CGReturn::CGReturn(DSObject* father, DSExpression* ret, DSString& name) :
  DSReturn(father, ret, name)
{
}

CGReturn::CGReturn(DSObject* father, DSExpression* ret, const char* name) :
  DSReturn(father, ret, name)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGReturn::~CGReturn(void)
{
}
 
/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGReturn::New(DSObject *father) const
{
  return (DSReturn *)new CGReturn(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer Return
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/

DSResult CGReturn::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult ret;

  (void)what;

  ret = WriteImplementation((CGWriter *)writer);
  if (ret != DS_OK) return ret;

  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt einen Prozedur-Return
 *            -> writer
 *            Ergebnis: CG_OK,falls Aktion erfolgreich war, DS_ERROR
 *            Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGReturn::WriteImplementation(CGWriter *writer) const
{
  DSResult result;
  DSProcedureRef proc;
  DSString str;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  result = WriteLabel(writer);
  if (result != DS_OK) return result;

  proc = GetThisProcedure();
  assert(proc);

  if (proc->GetIsComplex())
  {
    InsertString(*out, "Return()", pos , CG_NO_EOL);
  }
  else
  {  
    InsertString(*out, "return", pos , CG_NO_EOL);
    if (proc->GetReturnParamRef())
    {
      InsertString(*out, " ", 0, CG_NO_EOL);
      InsertString(*out, PREFIX_VARIABLE, 0, CG_NO_EOL);
      InsertString(*out, proc->GetReturnParamRef()->GetName(), 0, CG_NO_EOL);
    }
  }
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

DSResult CGReturn::WriteLabel(CGWriter *writer) const
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

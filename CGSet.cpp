/****************************************************************************
 *                 Modul: $RCSfile: CGSet.cpp,v $
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
#include <DS/DSTimer.h>
#include <DS/DSExpression.h>

#include "CGFile.h"
#include "CGSet.h"

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

CGSet::CGSet(DSObject         *father,
             DSString         *label_name,
             DSTimerRef        t_ref,
             DSExpressionList *i_list,
             DSExpression     *time_exp) :
  DSSet(father, label_name, t_ref, i_list, time_exp)
{
}

CGSet::CGSet(DSObject         *father,
             DSString         &label_name,
             DSTimerRef        t_ref,
             DSExpressionList *i_list,
             DSExpression     *time_exp) :
  DSSet(father, label_name, t_ref, i_list, time_exp)
{
}

CGSet::CGSet(DSObject         *father,
             const char       *label_name,
             DSTimerRef        t_ref,
             DSExpressionList *i_list,
             DSExpression     *time_exp) :
  DSSet(father, label_name, t_ref, i_list, time_exp)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGSet::~CGSet(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGSet::New(DSObject *father) const
{
  return (DSSet *)new CGSet(father);
}


/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer Set
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGSet::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;

  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt Timer-Set-Befehl
 *                        -> writer
 *                        Ergebnis: CG_OK,falls Aktion erfolgreich war, sonst 
 *                                  DS_ERROR
 *                        Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGSet::WriteImplementation(CGWriter *writer) const
{
  DSResult       result;
  DSActualParam *actual_param;
  DSCardinal     i = 1;
  DSTimerRef     the_timer;
  DSString      *tmp_str;
  CGFile        *out;
  CGPos          pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  result = WriteLabel(writer);
  if (result != DS_OK) return result;
  
  assert(GetTime());

  the_timer = GetTimerRef();
  assert(the_timer);

  /******* Initialisieren von Timerparametern (falls vorhanden) ***************/

  if (GetActualParamList())
  {
    InsertString(*out, PREFIX_TIMER_PARAM_VARIABLE, pos, CG_NO_EOL);
    InsertString(*out, the_timer->GetName(), 0, CG_NO_EOL);
    InsertString(*out, " = new ", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_TIMER_PARAM_TYPE, 0, CG_NO_EOL);
    InsertString(*out, the_timer->GetName(), 0, CG_NO_EOL);
    InsertString(*out, ";", 0, CG_WITH_EOL);
    InsertString(*out, "assert(", pos, CG_NO_EOL);
    InsertString(*out, PREFIX_TIMER_PARAM_VARIABLE, 0, CG_NO_EOL);
    InsertString(*out, the_timer->GetName(), 0, CG_NO_EOL);
    InsertString(*out, ");", 0, CG_WITH_EOL);

    for (actual_param = GetFirstActualParam(); 
         actual_param;
         actual_param = GetNextActualParam())
    {
      InsertString(*out, PREFIX_TIMER_PARAM_VARIABLE, pos, CG_NO_EOL);
      InsertString(*out, the_timer->GetName(),
                   0, CG_NO_EOL);
      InsertString(*out, "->", 0, CG_NO_EOL);
      InsertString(*out, PREFIX_TIMER_FIELD, 0, CG_NO_EOL);
      tmp_str = new DSString(i++);
      assert(tmp_str);
      InsertString(*out, tmp_str, 0, CG_NO_EOL);
      delete tmp_str;
      InsertString(*out, " = ", 0, CG_NO_EOL);
      writer->CGSetIndentPos(0);
      result = actual_param->Write(writer);
      writer->CGSetIndentPos(pos);
      if (result != DS_OK) return result;
      InsertString(*out, ";", 0, CG_WITH_EOL);
    }
  }
  
  InsertString(*out, CG_NAME_SCL_SETTIMER, pos, CG_NO_EOL);
  InsertString(*out, "(", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = GetTime()->Write(writer);
  writer->CGSetIndentPos(pos);
  if (result != DS_OK) return result;
  InsertString(*out, ", ", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_TIMER_TYPE_VARIABLE, 0, CG_NO_EOL);
  InsertString(*out, the_timer->GetName(), 0, CG_NO_EOL);
  if (GetActualParamList())
  {
    InsertString(*out, ", ", 0, CG_NO_EOL); 
    InsertString(*out, PREFIX_TIMER_PARAM_VARIABLE, 0, CG_NO_EOL);
    InsertString(*out, the_timer->GetName(),
                 0, CG_NO_EOL);
    InsertString(*out, ");", 0, CG_WITH_EOL); 
  }
  else 
  {
    InsertString(*out, ");", 0, CG_WITH_EOL);
  } 

  return DS_OK;
}     

/****************************************************************************
 * WriteLabel(): Schreibt das optionale Label der SDL-Aktion
 *               -> writer
 *               Ergebnis: DS_OK, falls Aktion erfolgreich war, sonst 
 *                         enthaelt DSResult das Ergebnis
 *               Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGSet::WriteLabel(CGWriter *writer) const
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

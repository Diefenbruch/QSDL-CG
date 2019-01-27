/****************************************************************************
 *                 Modul: $RCSfile: CGTask.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/01/14 10:59:58 $
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
#include <DS/DSVariableAccess.h>
#include <DS/DSVariableAccessField.h>
#include <DS/DSVariableAccessElem.h>
#include <DS/DSVariable.h>
#include <DS/DSSort.h>
#include <DS/DSExpression.h>
#include <DS/DSGenerator.h>
#include <DS/DSNameSort.h>

#include "CGFile.h"
#include "CGTask.h"

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

CGTask::CGTask(DSObject         *father,
               DSString         *label_name,
               DSVariableAccess *access,
               DSExpression     *expr) :
  DSTask(father, label_name, access, expr)
{
}

CGTask::CGTask(DSObject         *father,
               DSString&         label_name,
               DSVariableAccess *access,
               DSExpression     *expr) :
  DSTask(father, label_name, access, expr)
{
}

CGTask::CGTask(DSObject         *father,
               const char       *label_name,
               DSVariableAccess *access,
               DSExpression     *expr) :
  DSTask(father, label_name, access, expr)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGTask::~CGTask(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGTask::New(DSObject *father) const
{
  return (DSTask *)new CGTask(father);
}

/****************************************************************************
 * Write(): schreibt einen Task, Oberfunktion
 *                   -> writer
 *                   -> what
 *                   Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst
 *                             DS_ERROR
 *                   Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGTask::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;

  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt einen DSTask-Typ
 *                        Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst
 *                                  DS_ERROR
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGTask::WriteImplementation(CGWriter *writer) const
{
  DSResult result;
  DSVariableAccess *var_access;
  DSVariableAccessElem *var_access_elem;
  DSVariableRef     variable_ref;
  DSSortRef         sort_ref;
  DSString         *variable_name, *sort_name, *inform_text;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  result = WriteLabel(writer);
  if (result != DS_OK) return result;

  if (GetInformalText())
  {
    if (writer->CGGetWriteInformalText())
    {
//      InsertString(*out, "if (IsVerbose())", pos, CG_WITH_EOL);
//      InsertString(*out, "{", pos, CG_WITH_EOL);
      inform_text = new DSString(*GetInformalText()->SubString(1));
      assert(inform_text);
      (*inform_text)[inform_text->Length() - 1] = '\0';

      InsertString(*out, inform_text, pos, CG_WITH_EOL);
//      InsertString(*out, "}", pos, CG_WITH_EOL);
      delete inform_text;
    }
    return DS_OK;
  }

  var_access = GetVariableAccess();
  assert(var_access);
  variable_ref = var_access->GetVariableRef();
  assert(variable_ref);
  variable_name = variable_ref->GetName();
  assert(variable_name);
  sort_ref = variable_ref->GetSortRef();
  assert(sort_ref);
  for (var_access_elem = var_access->GetFirstElem();
       var_access_elem;
       var_access_elem = var_access->GetNextElem())
  {
    switch (var_access_elem->GetTag())
    {
      case DS_ACCESS_FIELD:
        sort_ref = ((DSVariableAccessField *)var_access_elem)->GetField()->GetSortRef();
        break;
      case DS_ACCESS_INDEX:
        if (sort_ref->GetTag() != DS_SORT_GENERATOR)
        {
          assert(DS_FALSE);
          return DS_ERROR;
        }
        assert(sort_ref);
        ((DSGenerator *)sort_ref)->GetFirstSortRef(); // Index-Sorte
        sort_ref = ((DSGenerator *)sort_ref)->GetNextSortRef();  // Element-Sorte
        assert(sort_ref);
        break;
      default:
        assert(DS_FALSE);
        return DS_ERROR;
    }
  }
  
  sort_name = sort_ref->GetName();
  assert(sort_name);

  if (*sort_name == DS_SORT_NAME_DETERMINISTIC ||
      *sort_name == DS_SORT_NAME_EMPIRICAL ||
      *sort_name == DS_SORT_NAME_ERLANG ||
      *sort_name == DS_SORT_NAME_NEGEXP ||
      *sort_name == DS_SORT_NAME_NORMAL ||
      *sort_name == DS_SORT_NAME_UNIFORM ||
      *sort_name == DS_SORT_NAME_BERNOULLI ||
      *sort_name == DS_SORT_NAME_POISSON ||
      *sort_name == DS_SORT_NAME_RANDINT )
  {
    InsertString(*out, "", pos, CG_NO_EOL);
  }
  else
  {
    result = var_access->Write(writer);
    if (result != DS_OK) return result;
    InsertString(*out, " = ", 0, CG_NO_EOL);
  }
  writer->CGSetIndentPos(0);
  result = GetExpression()->Write(writer);
  writer->CGSetIndentPos(pos);
  if (result != DS_OK) return result;
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

DSResult CGTask::WriteLabel(CGWriter *writer) const
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

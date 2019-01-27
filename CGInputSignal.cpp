/****************************************************************************
 *                 Modul: $RCSfile: CGInputSignal.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/01/14 10:59:58 $
 * $Revision: 1.2 $
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

#include <DS/DSString.h>
#include <DS/DSSignal.h>
#include <DS/DSVariable.h>
#include <DS/DSVariableAccess.h>

#include "CGFile.h"
#include "CGInputSignal.h"

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

CGInputSignal::CGInputSignal(DSObject             *father,
                             DSSignalRef           sig_ref,
                             DSVariableAccessList *access_list) :
  DSInputSignal(father, sig_ref, access_list)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGInputSignal::~CGInputSignal(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGInputSignal::New(DSObject *father) const
{
  return new CGInputSignal(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer InputSignal
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGInputSignal::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;
 
  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;
 
  return DS_OK;
}
 
/****************************************************************************
 * WriteImplementation(): schreibt ein Inputsignal mit Parametern 
 *                        -> writer
 *                        Ergebnis: DS_OK, falls alles geklappt hat, sonst 
 *                                  DS_ERROR
 *                        Seiteneffekte: Anlegen und Fuellen der Files
 ****************************************************************************/
 
DSResult CGInputSignal::WriteImplementation(CGWriter *writer) const
{
  DSSignalRef       signal;
  DSResult          result;
  DSVariableAccess *var_access;
  DSCardinal        i;
  DSString         *tmp_str;
  CGFile           *out;
  CGPos             pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  signal = GetSignalRef();
  if (signal->GetFirstSortRef() &&            /* Signal mit Parametern? */
      GetVariableAccessList())  /* werden sie abgerufen? */
  {
    InsertString(*out, "case ", pos, CG_NO_EOL);
    InsertString(*out, PREFIX_SIGNAL_TYPE_ID, 0, CG_NO_EOL);
    InsertString(*out, signal->GetName(), 0, CG_NO_EOL);
    InsertString(*out, ":", 0, CG_WITH_EOL); 

    InsertString(*out, PREFIX_SIGNAL_PARAM_VARIABLE, pos + 2, CG_NO_EOL);
    InsertString(*out, signal->GetName(), 0, CG_NO_EOL);
    InsertString(*out, " = (", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_SIGNAL_PARAM_TYPE, 0, CG_NO_EOL);
    InsertString(*out, signal->GetName(), 0, CG_NO_EOL);
    InsertString(*out, " *)RetrieveInputData();", 0, CG_WITH_EOL);
    InsertString(*out, "assert(", pos + 2, CG_NO_EOL);
    InsertString(*out, PREFIX_SIGNAL_PARAM_VARIABLE, 0, CG_NO_EOL);
    InsertString(*out, signal->GetName(), 0, CG_NO_EOL);
    InsertString(*out, ");", 0, CG_WITH_EOL);
    i = 1;

    for (var_access = GetFirstVariableAccess();
         var_access;
         var_access = GetNextVariableAccess())
    {
      if (!var_access->GetVariableRef()) // Parameter ausgelassen,
      {                                  // d.h. keine Variable
        i++;                             // angegeben ?
        continue;
      }

      InsertString(*out, PREFIX_VARIABLE, pos + 2, CG_NO_EOL);
      InsertString(*out,
                   var_access->GetVariableRef()->GetName(),
                   0, CG_NO_EOL);
      InsertString(*out, " = ", 0, CG_NO_EOL);
      InsertString(*out, PREFIX_SIGNAL_PARAM_VARIABLE, 0, CG_NO_EOL);
      InsertString(*out, signal->GetName(), 0, CG_NO_EOL);
      InsertString(*out, "->", 0, CG_NO_EOL); 
      InsertString(*out, PREFIX_SIGNAL_FIELD, 0, CG_NO_EOL);
      tmp_str = new DSString(i++);
      assert(tmp_str);
      InsertString(*out, tmp_str, 0, CG_NO_EOL);
      delete tmp_str;
      InsertString(*out, ";", 0, CG_WITH_EOL); 
    }

    InsertString(*out, "delete ", pos + 2, CG_NO_EOL);
    InsertString(*out, PREFIX_SIGNAL_PARAM_VARIABLE, 0, CG_NO_EOL);
    InsertString(*out, signal->GetName(), 0, CG_NO_EOL);
    InsertString(*out, ";", 0, CG_WITH_EOL);
    InsertString(*out, "break;", pos + 2, CG_WITH_EOL);
    InsertLine(*out);
  }

  return DS_OK;
}

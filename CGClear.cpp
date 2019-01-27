/****************************************************************************
 *                 Modul: $RCSfile: CGClear.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/01/14 10:59:58 $
 * $Revision: 1.2 $
 *
 * Aufgabe: Dieses Modul enthaelt die Generierung des C++-Codes fuer 
 *          entsprechende QSDL-Aktionen
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
#include "CGClear.h"
#include "CGSensor.h"

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

CGClear::CGClear(DSObject* father, DSString* name,
                 DSSensorRef sensor) :
  DSClear(father, name, sensor)
{
}
CGClear::CGClear(DSObject* father, DSString& name,
                 DSSensorRef sensor) :
  DSClear(father, name, sensor)
{
}

CGClear::CGClear(DSObject* father, const char* name,
                 DSSensorRef sensor) :
  DSClear(father, name, sensor)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGClear::~CGClear(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGClear::New(DSObject *father) const
{
  return (DSClear *)new CGClear(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer Clear
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGClear::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;

  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt einen DSClear-Typ
 *            -> writer
 *            -> what
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst DS_ERROR 
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGClear::WriteImplementation(CGWriter *writer) const
{
  DSResult       result;
  DSSensorRef    the_sensor;
  CGFile        *out;

  out = writer->CGGetFilePointer();
  assert(out);

  result = WriteLabel(writer);
  if (result != DS_OK) return result;

  assert(GetSensorRef());
  assert(out);

  the_sensor = GetSensorRef();
  assert(the_sensor);

  the_sensor->Write(writer, CG_WRITE_IDENTIFIER);
  InsertString(*out, ".Clear();", 0, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * WriteLabel(): Schreibt das optionale Label der SDL-Aktion
 *               -> writer
 *               Ergebnis: DS_OK, falls Aktion erfolgreich war, sonst 
 *                         enthaelt DSResult das Ergebnis
 *               Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGClear::WriteLabel(CGWriter *writer) const
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

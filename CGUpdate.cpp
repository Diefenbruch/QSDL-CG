/****************************************************************************
 *                 Modul: $RCSfile: CGUpdate.cpp,v $
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
#include <DS/DSSensor.h>
#include <DS/DSExpression.h>

#include "CGFile.h"
#include "CGUpdate.h"

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

CGUpdate::CGUpdate(DSObject     *father,
                   DSString     *label_name,
                   DSSensorRef   s_ref,
                   DSExpression *sample_exp,
                   DSExpression *value_exp) :
  DSUpdate(father, label_name, s_ref, sample_exp, value_exp)
{
}

CGUpdate::CGUpdate(DSObject     *father,
                   DSString     &label_name,
                   DSSensorRef   s_ref,
                   DSExpression *sample_exp,
                   DSExpression *value_exp) :
  DSUpdate(father, label_name, s_ref, sample_exp, value_exp)
{
}

CGUpdate::CGUpdate(DSObject     *father,
                   const char   *label_name,
                   DSSensorRef   s_ref,
                   DSExpression *sample_exp,
                   DSExpression *value_exp) :
  DSUpdate(father, label_name, s_ref, sample_exp, value_exp)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGUpdate::~CGUpdate(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGUpdate::New(DSObject *father) const
{
  return (DSUpdate *)new CGUpdate(father);
}


/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer Update
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGUpdate::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;

  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt Sensor-Update-Befehl
 *                        -> writer
 *                        Ergebnis: CG_OK,falls Aktion erfolgreich war, sonst 
 *                                  DS_ERROR
 *                        Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGUpdate::WriteImplementation(CGWriter *writer) const
{
  DSResult       result;
  DSSensorRef    the_sensor;
  CGFile        *out;
  CGPos          pos;
  DSObject *father;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  result = WriteLabel(writer);
  if (result != DS_OK) return result;
  
  the_sensor = GetSensorRef();
  assert(the_sensor);
  
  father = the_sensor->GetParent();
  assert(father);
  
  the_sensor->Write(writer, CG_WRITE_IDENTIFIER);
  
  switch (father->GetType())
  {
    case DS_SYSTEM:
    case DS_BLOCK:
      InsertString(*out, "->Update(", 0, CG_NO_EOL);
      break;
      
    case DS_PROCESS:
    case DS_MACHINE:
      InsertString(*out, ".Update(", 0, CG_NO_EOL);
      break;
      
    default:
      assert(DS_FALSE);
      return DS_ERROR;
  }
  
  writer->CGSetIndentPos(0);
  if (GetSample())
  {
    result = GetSample()->Write(writer);
    if (result != DS_OK) return result;
  }
  else  // Default-Sample = 1
  {
    InsertString(*out, "1", 0, CG_NO_EOL);
  }
  if (GetValue())   // only with frequency or tally sensors
  {
    InsertString(*out, ", ", 0, CG_NO_EOL);
    result = GetValue()->Write(writer);
    if (result != DS_OK) return result;
  }
  writer->CGSetIndentPos(pos);
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

DSResult CGUpdate::WriteLabel(CGWriter *writer) const
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

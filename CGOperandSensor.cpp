/****************************************************************************
 *                 Modul: $RCSfile: CGOperandSensor.cpp,v $
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

#include <DS/DSSensor.h>
#include <DS/DSMachine.h>
#include <DS/DSBlock.h>

#include "CGFile.h"
#include "CGOperandSensor.h"

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

CGOperandSensor::CGOperandSensor(DSObject *father,
                                 DSSensorRef sen_ref) :
  DSOperandSensor(father, sen_ref)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGOperandSensor::~CGOperandSensor(void)
{
}

DSObject *CGOperandSensor::New(DSObject *father) const
{
  return new CGOperandSensor(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer OperandSensor
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGOperandSensor::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;

  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): schreibt einen als Sensor gespeicherten Operanden
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst DS_ERROR
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGOperandSensor::WriteImplementation(CGWriter *writer) const
{    
  DSSensorRef sensor;
  DSResult result;
  CGFile *out;
  CGPos   pos;
  DSObject *father;
  DSMachineRef machine_ref;
  DSBlock *block;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  sensor = GetSensorRef();
  assert(sensor);
  father = sensor->GetParent();
  assert(father);

  if (father->GetType() == DS_MACHINE) // process or atom is accessing
  {                                    // a machine sensor!
    InsertString(*out, "((", 0, CG_NO_EOL);
    machine_ref = (DSMachineRef)father;
    writer->CGSetIndentPos(0);
    machine_ref->Write(writer, CG_WRITE_IDENTIFIER);
    InsertString(*out, " *)", 0, CG_NO_EOL);
    block = (DSBlock *)machine_ref->GetParent();
    assert(block);
    block->Write(writer, CG_WRITE_IDENTIFIER);  // needed for atoms!
    InsertString(*out, "::", 0, CG_NO_EOL);
    InsertString(*out, PREFIX_MACHINE, 0, CG_NO_EOL);
    InsertString(*out, machine_ref->GetName(), 0, CG_NO_EOL);
    InsertString(*out, ")->", 0, CG_NO_EOL);
    writer->CGSetIndentPos(pos);
  }
  result = sensor->Write(writer, CG_WRITE_IDENTIFIER);

  return result;
}

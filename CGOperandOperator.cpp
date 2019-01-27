/****************************************************************************
 *                 Modul: $RCSfile: CGOperandOperator.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/01/14 10:59:58 $
 * $Revision: 1.5 $
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
#include <DS/DSExpression.h>
#include <DS/DSOperandSensor.h>
#include <DS/DSSensor.h>
#include <DS/DSSort.h>

#include "CGFile.h"
#include "CGOperandOperator.h"
#include "CGOperator.h"

#ifdef DEBUG
#include <dmalloc.h>
#endif

/****************************************************************************
 * Externe Variablen
 ****************************************************************************/

extern char *sensor_operator_names[];

/****************************************************************************
 * Globale Variablen
 ****************************************************************************/

/****************************************************************************
 * Konstruktoren
 ****************************************************************************/

CGOperandOperator::CGOperandOperator(DSObject          *father,
                                     DSOperatorRef      op,
                                     DSActualParamList *param_list) :
  DSOperandOperator(father, op, param_list)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGOperandOperator::~CGOperandOperator(void)
{
}

DSObject *CGOperandOperator::New(DSObject *father) const
{
  return new CGOperandOperator(father);
}

/****************************************************************************
 * Write(): Oberfunktion der Codegenerierung fuer OperandOperator
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK oder DS_ERROR
 *          Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGOperandOperator::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;

  (void)what;

  result = WriteImplementation((CGWriter *)writer);
  if (result != DS_OK) return result;

  return DS_OK;
}

/*****************************************************************************
 * WriteImplementation(): schreibt einen als Operator gespeicherten Operand
 *          -> writer
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst DS_ERROR
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGOperandOperator::WriteImplementation(CGWriter *writer) const
{
  DSOperatorRef optr;
  DSActualParam *actual_param;
  DSCardinal i;
  DSBoolean first;
  DSResult result;
  CGFile *out;
  CGPos pos;
  DSInteger optr_name_index = -1;
  DSObject *father;
  DSOperandSensor * operand_sensor;
  DSSensorRef sensor_ref;
  DSSortRef sort_ref;
  
  out = writer->CGGetFilePointer();
  pos = writer->CGGetIndentPos();

  optr = GetOperatorRef();
  assert(optr);

  if (*optr->GetName() == DS_SORT_OPERATOR_NAME_SAMPLE)
  {
    assert(GetFirstActualParam());

    result = GetFirstActualParam()->Write(writer);
    if (result != DS_OK) return result;
    InsertString(*out, "(", 0, CG_NO_EOL);
    InsertString(*out, ")", 0, CG_NO_EOL);
    return DS_OK;
  }

  if (*optr->GetName() == DS_SORT_OPERATOR_NAME_INIT)
  {
    assert(GetFirstActualParam());
  
    result = GetFirstActualParam()->Write(writer);
    if (result != DS_OK) return result;
    InsertString(*out, ".Init(", 0, CG_NO_EOL);
    i = 0;
    while ((actual_param = GetNextActualParam()) != NULL)
    {
      if (i > 0)
      {
        InsertString(*out, ", ", 0, CG_NO_EOL);
      }
      result = actual_param->Write(writer);
      if (result != DS_OK) return result;
      i++;
    }
    InsertString(*out, ")", 0, CG_NO_EOL);
    return DS_OK;
  }
  
  if (*optr->GetName() == DS_SORT_OPERATOR_NAME_LENGTH)
  {
    assert(GetFirstActualParam());
 
    result = GetFirstActualParam()->Write(writer);
    if (result != DS_OK) return result;
    InsertString(*out, ".Length()", 0, CG_NO_EOL);
    return DS_OK;
  }
      
  if (*optr->GetName() == DS_SORT_OPERATOR_NAME_FIRST)
  {
    assert(GetFirstActualParam());
 
    result = GetFirstActualParam()->Write(writer);
    if (result != DS_OK) return result;
    InsertString(*out, ".First()", 0, CG_NO_EOL);
    return DS_OK;
  }

  if (*optr->GetName() == DS_SORT_OPERATOR_NAME_LAST)
  {
    assert(GetFirstActualParam());
 
    result = GetFirstActualParam()->Write(writer);
    if (result != DS_OK) return result;
    InsertString(*out, ".Last()", 0, CG_NO_EOL);
    return DS_OK;
  }

  if (*optr->GetName() == DS_SORT_OPERATOR_NAME_SUBSTRING)
  {
    assert(GetFirstActualParam());
 
    result = GetFirstActualParam()->Write(writer);
    if (result != DS_OK) return result;
    InsertString(*out, ".SubString(", 0, CG_NO_EOL);

    first = DS_TRUE;
    for (actual_param = GetNextActualParam(); // not First here!
         actual_param;
         actual_param = GetNextActualParam())
    {
      if (first)
      {
        first = DS_FALSE;
      }
      else
      {
        InsertString(*out, ", ", 0, CG_NO_EOL);
      }
      result = actual_param->Write(writer);
      if (result != DS_OK) return result;
    }

    InsertString(*out, ")", 0, CG_NO_EOL);
    return DS_OK;
  }

  if (*optr->GetName() == DS_SORT_OPERATOR_NAME_MKSTRING)
  {
    actual_param = GetFirstActualParam();
    assert(actual_param);
    
    if (actual_param == NULL)
      return DS_ERROR;

    sort_ref = actual_param->GetSortRef();
    assert(sort_ref);
    
    if (sort_ref != GetSortRefByName(DS_SORT_NAME_CHARACTER))
    {
      InsertString(*out, "SCString<", pos, CG_NO_EOL);
      sort_ref->Write(writer, CG_WRITE_IDENTIFIER);
      InsertString(*out, ">::MkString(", pos, CG_NO_EOL);
      result = actual_param->Write(writer);
      if (result != DS_OK) return result;
      InsertString(*out, ")", 0, CG_NO_EOL);
      return DS_OK;
    }
    else
    {
      InsertString(*out, "SCCharstring::MkString(", pos, CG_NO_EOL);
      result = actual_param->Write(writer);
      if (result != DS_OK) return result;
      InsertString(*out, ")", 0, CG_NO_EOL);
      return DS_OK;
    }
  }
    
  if (*optr->GetName() == DS_SORT_OPERATOR_NAME_EXTRACT)
  {
    actual_param = GetFirstActualParam();
    assert(actual_param);
    
    if (actual_param == NULL)
      return DS_ERROR;
      
    result = actual_param->Write(writer);
    if (result != DS_OK) return result;

    InsertString(*out, "[", 0, CG_NO_EOL);

    actual_param = GetNextActualParam();
    if (actual_param == NULL)
      return DS_ERROR;
      
    result = actual_param->Write(writer);
    InsertString(*out, "]", 0, CG_NO_EOL);
    return DS_OK;
  }

  if (*optr->GetName() == DS_SORT_OPERATOR_NAME_MODIFY)
  {
    actual_param = GetFirstActualParam();
    assert(actual_param);
    
    if (actual_param == NULL)
      return DS_ERROR;
 
    result = actual_param->Write(writer);
    if (result != DS_OK) return result;
    InsertString(*out, ".Modify(", 0, CG_NO_EOL);
    actual_param = GetNextActualParam();
    assert(actual_param);
    result = actual_param->Write(writer);
    if (result != DS_OK) return result;
    InsertString(*out, ", ", 0, CG_NO_EOL);
    actual_param = GetNextActualParam();
    assert(actual_param);
    result = actual_param->Write(writer);
    if (result != DS_OK) return result;
    InsertString(*out, ")", 0, CG_NO_EOL);
    return DS_OK;
  }
      
  if (*optr->GetName() == DS_SORT_OPERATOR_NAME_FLOAT)
  {
    InsertString(*out, "(double)", pos, CG_NO_EOL);
    for (actual_param = GetFirstActualParam();
         actual_param;
         actual_param = GetNextActualParam())
    {
      result = actual_param->Write(writer);
      if (result != DS_OK) return result;
    }
    return DS_OK;
  }
    
  if (*optr->GetName() == DS_SORT_OPERATOR_NAME_FIX)
  {
    InsertString(*out, "(int)", pos, CG_NO_EOL);
    for (actual_param = GetFirstActualParam();
         actual_param;
         actual_param = GetNextActualParam())
    {
      result = actual_param->Write(writer);
      if (result != DS_OK) return result;
    }
    return DS_OK;
  } 
      
      
  if (*optr->GetName() == DS_SORT_OPERATOR_NAME_DURATION)
  {
    InsertString(*out, "(duration)", pos, CG_NO_EOL);
    for (actual_param = GetFirstActualParam();
         actual_param;
         actual_param = GetNextActualParam())
    {
      result = actual_param->Write(writer);
      if (result != DS_OK) return result;
    }
    return DS_OK;
  } 
    
  if (*optr->GetName() == DS_SORT_OPERATOR_NAME_SUM)
    optr_name_index = 0;
  else if (*optr->GetName() == DS_SORT_OPERATOR_NAME_SUMINT)
    optr_name_index = 1;
  else if (*optr->GetName() == DS_SORT_OPERATOR_NAME_SUMTIME)
    optr_name_index = 2;
  else if (*optr->GetName() == DS_SORT_OPERATOR_NAME_SUMTIMEINT)
    optr_name_index = 3;
  else if (*optr->GetName() == DS_SORT_OPERATOR_NAME_ABSFREQ)
    optr_name_index = 4;
  else if (*optr->GetName() == DS_SORT_OPERATOR_NAME_RELFREQ)
    optr_name_index = 5;
  else if (*optr->GetName() == DS_SORT_OPERATOR_NAME_LASTVALUE)
    optr_name_index = 6;
  else if (*optr->GetName() == DS_SORT_OPERATOR_NAME_CURRENTVALUE)
    optr_name_index = 7;
  else if (*optr->GetName() == DS_SORT_OPERATOR_NAME_LASTSAMPLE)
    optr_name_index = 8;
  else if (*optr->GetName() == DS_SORT_OPERATOR_NAME_NUM)
    optr_name_index = 9;
  else if (*optr->GetName() == DS_SORT_OPERATOR_NAME_NUMINT)
    optr_name_index = 10;
  else if (*optr->GetName() == DS_SORT_OPERATOR_NAME_MINIMUM)
    optr_name_index = 11;
  else if (*optr->GetName() == DS_SORT_OPERATOR_NAME_MAXIMUM)
    optr_name_index = 12;
  else if (*optr->GetName() == DS_SORT_OPERATOR_NAME_SUMSQUARE)
    optr_name_index = 13;
  else if (*optr->GetName() == DS_SORT_OPERATOR_NAME_AVERAGE)
    optr_name_index = 14;
  else if (*optr->GetName() == DS_SORT_OPERATOR_NAME_AVERAGEINT)
    optr_name_index = 15;
  else if (*optr->GetName() == DS_SORT_OPERATOR_NAME_VARIANCE)
    optr_name_index = 16;
  else if (*optr->GetName() == DS_SORT_OPERATOR_NAME_STDDEV)
    optr_name_index = 17;
  else if (*optr->GetName() == DS_SORT_OPERATOR_NAME_VARCOEFF)
    optr_name_index = 18;
  
  if (optr_name_index >= 0)  // Sensor-Operator entdeckt?
  {
    actual_param = GetFirstActualParam();
    assert(actual_param);
    
    // Der erste Parameter ist immer der Sensor selbst!
    result = actual_param->Write(writer); // Sensor schreiben
    if (result != DS_OK) return result;

    assert(actual_param->GetOperator() == DS_OPT_NONE);
    assert(actual_param->GetOperand1() != NULL);
    assert(actual_param->GetOperand2() == NULL);
    
    // Da die Sensoren auf System- und Block-Ebene
    // als Pointer deklariert sind muss hier leider
    // ein etwas unschoener Spezialfall abgefragt werden:
    
    if (actual_param->GetOperand1()->GetTag() == DS_OPN_SENSOR)
    {

      operand_sensor = (DSOperandSensor *)actual_param->GetOperand1();
      sensor_ref = operand_sensor->GetSensorRef();
      assert(sensor_ref);
 
      father = sensor_ref->GetParent();
 
      switch (father->GetType())
      {
        case DS_SYSTEM:
        case DS_BLOCK:
        case DS_BLOCKSUBSTRUCTURE:
          InsertString(*out, "->", 0, CG_NO_EOL);
          break;
 
        case DS_PROCESS:
        case DS_MACHINE:
          InsertString(*out, ".", 0, CG_NO_EOL);
          break;
 
        default:
          assert(DS_FALSE);
          return DS_ERROR;
      }
    }
    else // z.B. DS_OPN_PROCESS_ACCESS
    {
      InsertString(*out, ".", 0, CG_NO_EOL);
    }

    InsertString(*out, sensor_operator_names[optr_name_index], 0, CG_NO_EOL);
    InsertString(*out, "(", 0, CG_NO_EOL);

    actual_param = GetNextActualParam();
    
    if (actual_param)
    {
      result = actual_param->Write(writer);
      if (result != DS_OK) return result;
    }
    InsertString(*out, ")", 0, CG_NO_EOL);
    return DS_OK;
  }

  InsertString(*out, optr->GetName(), pos, CG_NO_EOL);
  InsertString(*out, "(", 0, CG_NO_EOL);
  first = DS_TRUE;
  for (actual_param = GetFirstActualParam();
       actual_param;
       actual_param = GetNextActualParam())
  {
    if (first)
    {
      first = DS_FALSE;
    }
    else
    {
      InsertString(*out, ", ", 0, CG_NO_EOL);
    }
    result = actual_param->Write(writer);
    if (result != DS_OK) return result;
  }
  InsertString(*out, ")", 0, CG_NO_EOL);
  return DS_OK;
}

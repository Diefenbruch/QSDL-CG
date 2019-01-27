/************************************************************************
 * Classname: CGSensorState
 * Purpose:   Definition eines Sensors
 ************************************************************************/

#include <iostream>

#include <DS/DSString.h>
#include <DS/DSState.h>
#include <DS/DSInput.h>

#include "CGFile.h"
#include "CGSystem.h"
#include "CGSensorState.h"

extern char *sensor_names[];

CGSensorState::CGSensorState (DSObject* father,
                              DSSensorType sType,
                              DSString* sName,
                              DSStateRef state) :
  DSSensorState (father, sType, sName, state)
{
  // do nothing.
}

CGSensorState::CGSensorState (DSObject* father,
                              DSSensorType sType,
                              DSString& sName,
                              DSStateRef state) :
  DSSensorState (father, sType, sName, state)
{
  // do nothing.
}

CGSensorState::CGSensorState (DSObject* father,
                              DSSensorType sType,
                              const char * sName,
                              DSStateRef state) :
  DSSensorState (father, sType, sName, state)
{
  // do nothing.
}

CGSensorState::~CGSensorState(void)
{
  // do nothing.
}


DSObject* CGSensorState::New (DSObject* father) const
{
  return new CGSensorState(father);
}

/****************************************************************************
 * Write(): schreibt eine SDL-Sensor
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                    DS_ERROR
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/
 
DSResult CGSensorState::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;
  
  switch (what)
  {
    case CG_WRITE_H:
      result = WriteSensor((CGWriter *)writer);
      break;

    case CG_WRITE_INIT:
      result = WriteInit((CGWriter *)writer);
      break;

    case CG_WRITE_SAVE:
      result = WriteSave((CGWriter *)writer);
      break;
    
    case CG_WRITE_RESTORE:
      result = WriteRestore((CGWriter *)writer);
      break;

    case CG_WRITE_SIZE:
      result = WriteSize((CGWriter *)writer);
      break;

    case CG_WRITE_UPDATE:
      result = WriteUpdate((CGWriter *)writer);
      break;

    case CG_WRITE_CREATE:
      result = WriteCreate((CGWriter *)writer);
      break;

    case CG_WRITE_DESTROY:
      result = WriteDestroy((CGWriter *)writer);
      break;

    case CG_WRITE_IDENTIFIER:
      result = WriteIdentifier((CGWriter *)writer);
      break;

    default:
      assert(DS_FALSE);
      return DS_ERROR;
  }
  return result;
}


/****************************************************************************
 * WriteSensor(): schreibt Variablen eines Prozesses oder einer Prozedur
 *                -> writer
 *                Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGSensorState::WriteSensor(CGWriter *writer) const
{
  DSResult  result;
  CGFile   *out;
  CGPos     pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, sensor_names[GetType()], pos, CG_NO_EOL);
  InsertString(*out, " ", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_SENSOR, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);

  InsertString(*out, ";", 0, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * WriteInit(): schreibt Variablen eines Prozesses oder einer Prozedur
 *              -> writer
 *              Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *              Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGSensorState::WriteInit(CGWriter *writer) const
{
  DSResult  result;
  CGFile   *out;
  CGPos     pos;
  DSString  str;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, ",", 0, CG_WITH_EOL);
  InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  switch(GetType())
  {
    case DS_SENSOR_PCS_WAITTIME_STATE:
    case DS_SENSOR_PCS_COUNT_STATE:
    case DS_SENSOR_PCS_COUNT_TRANSITION:
      InsertString(*out, "(\"", 0, CG_NO_EOL);
      InsertString(*out, GetName(), 0, CG_NO_EOL);
      InsertString(*out, "\", (SCDuration)1.0, (SCObject *)this)", 0, CG_NO_EOL);
      break;
    case DS_SENSOR_PCS_FREQ_TRANSITION:
      InsertString(*out, "(\"", 0, CG_NO_EOL);
      InsertString(*out, GetName(), 0, CG_NO_EOL);
      InsertString(*out, "\", kSCTransitionIDBase, ", 0, CG_NO_EOL);
      InsertCardinal(*out,
                     GetStateRef()->GetInputList()->NumberOfElements(),
                     0, CG_NO_EOL);
      InsertString(*out, ", (SCObject *)this)", 0, CG_NO_EOL);
      break;
    default:
      assert(DS_FALSE);
  }

  return DS_OK;
}


/****************************************************************************
 * WriteSave(): Speicher Sensor
 ****************************************************************************/

DSResult CGSensorState::WriteSave(CGWriter * writer) const
{
  DSResult result;
  CGFile *out;
  CGPos   pos;
  DSString str;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();
  
  str.Format("%s%s.Save(save_area);", PREFIX_SENSOR, GetName()->GetString());
  InsertString(*out, str, pos, CG_WITH_EOL);
  
  return DS_OK;
}

/****************************************************************************
 * WriteRestore(): Restauriert Sensor nach Backtracking
 ****************************************************************************/

DSResult CGSensorState::WriteRestore(CGWriter * writer) const
{
  DSResult result;
  CGFile *out;
  CGPos   pos;
  DSString str;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();
  
  str.Format("%s%s.Restore(save_area);", PREFIX_SENSOR, GetName()->GetString());
  InsertString(*out, str, pos, CG_WITH_EOL);
  
  return DS_OK;
}

/****************************************************************************
 * WriteSize(): Berechnet Speicherplatz fuer den Sensor
 ****************************************************************************/

DSResult CGSensorState::WriteSize(CGWriter * writer) const
{
  DSResult result;
  CGFile *out;
  CGPos   pos;
  DSString str;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();
  
  str.Format("%s%s.Size(cur_size);", PREFIX_SENSOR, GetName()->GetString());
  InsertString(*out, str, pos, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * WriteUpdate(): Schreibt Update-Aufruf in die SensorUpdate-Methode
 ****************************************************************************/

DSResult CGSensorState::WriteUpdate(CGWriter * writer) const
{
  DSResult result;
  CGFile  *out;
  CGPos    pos;
  DSCardinal event;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();
  
  event = writer->CGGetEvent();
  
  switch (GetType())
  {
    case DS_SENSOR_PCS_WAITTIME_STATE:
      if (event == cgTraceSpontTrans ||
          event == cgTraceContSignal ||
          event == cgTraceSignalConsume)
      {
        if (GetStateRef())
        {
          InsertString(*out, "if (info.theState == ", pos, CG_NO_EOL);
          InsertString(*out, PREFIX_STATE_TYPE_VARIABLE, 0, CG_NO_EOL);
          InsertString(*out, GetStateRef()->GetFirstName(), 0, CG_NO_EOL);
          InsertString(*out, ")", 0, CG_WITH_EOL);
          InsertString(*out, "{", pos, CG_WITH_EOL);
          pos += 2;
        }
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update();", "WaitTimeState (Process)", 0); 
        if (GetStateRef())
        {
          pos -= 2;
          InsertString(*out, "}", pos, CG_WITH_EOL);
        }
      }
      break;

    case DS_SENSOR_PCS_COUNT_STATE:
      if (event == cgTraceStateChange)
      {
        if (GetStateRef())
        {
          InsertString(*out, "if (info.theState == ", pos, CG_NO_EOL);
          InsertString(*out, PREFIX_STATE_TYPE_VARIABLE, 0, CG_NO_EOL);
          InsertString(*out, GetStateRef()->GetFirstName(), 0, CG_NO_EOL);
          InsertString(*out, ")", 0, CG_WITH_EOL);
          InsertString(*out, "{", pos, CG_WITH_EOL);
          pos += 2;
        }
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update();", "CountState (Process)", 0); 
        if (GetStateRef())
        {
          pos -= 2;
          InsertString(*out, "}", pos, CG_WITH_EOL);
        }
      }
      break;

    case DS_SENSOR_PCS_FREQ_TRANSITION:
      assert(GetStateRef());  // bei diesem Sensor ist der
                              // State-Parameter NICHT optional!

      if (event == cgTraceSpontTrans ||
          event == cgTraceContSignal ||
          event == cgTraceSignalConsume)
      {
        InsertString(*out, "if (info.theState == ", pos, CG_NO_EOL);
        InsertString(*out, PREFIX_STATE_TYPE_VARIABLE, 0, CG_NO_EOL);
        InsertString(*out, GetStateRef()->GetFirstName(), 0, CG_NO_EOL);
        InsertString(*out, ")", 0, CG_WITH_EOL);
        InsertString(*out, "{", pos, CG_WITH_EOL);
        InsertString(*out, PREFIX_SENSOR, pos + 2, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(+1, info.theTransition);", "FreqTransition (Process)", 0); 
        InsertString(*out, "}", pos, CG_WITH_EOL);
      }
      break;

    case DS_SENSOR_PCS_COUNT_TRANSITION:
      if (event == cgTraceSpontTrans ||
          event == cgTraceContSignal ||
          event == cgTraceSignalConsume)
      {
        if (GetStateRef())
        {
          InsertString(*out, "if (info.theState == ", pos, CG_NO_EOL);
          InsertString(*out, PREFIX_STATE_TYPE_VARIABLE, 0, CG_NO_EOL);
          InsertString(*out, GetStateRef()->GetFirstName(), 0, CG_NO_EOL);
          InsertString(*out, ")", 0, CG_WITH_EOL);
          InsertString(*out, "{", pos, CG_WITH_EOL);
          pos += 2;
        }
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update();", "CountTransition (Process)", 0); 
        if (GetStateRef())
        {
          pos -= 2;
          InsertString(*out, "}", pos, CG_WITH_EOL);
        }
      }
      break;

    default:
      std::cerr << "Got sensor type '" << sensor_names[GetType()] << "." << std::endl;
      assert(DS_FALSE);
  }

  return DS_OK;
}

/****************************************************************************
 * WriteIdentifier(): Schreibt den Namen des Sensors (mit notwendigen Prefixen)
 ****************************************************************************/

DSResult CGSensorState::WriteIdentifier(CGWriter * writer) const
{
  DSResult result;
  CGFile  *out;
  CGPos    pos;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  
  return DS_OK;
}


/****************************************************************************
 * WriteCreate(): schreibt fuer System- und Block-Sensoren den Auruf
 *                sensor.Write(scSensorCreate) explizit in die statische
 *                Initialisierungsmethode. Notwendig, da das Socket-Objekt
 *                zum Zeitpunkt der Initilisierung noch nicht existiert.
 *                -> writer
 *                Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGSensorState::WriteCreate(CGWriter *writer) const
{
  CGFile   *out;
//  CGPos     pos;

  out = writer->CGGetFilePointer();
  assert(out);
//  pos = writer->CGGetIndentPos();

  // Diese Methode wird nur fuer System- und
  // Block-Sensoren aufgerufen!
  
  return DS_OK;
}


/****************************************************************************
 * WriteDestroy(): schreibt fuer System- und Block-Sensoren den Auruf
 *                sensor.Write(scSensorDestroy) explizit in die statische
 *                Initialisierungsmethode. Notwendig, da das Socket-Objekt
 *                zum Zeitpunkt der Initilisierung noch nicht existiert.
 *                -> writer
 *                Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGSensorState::WriteDestroy(CGWriter *writer) const
{
  CGFile   *out;
//  CGPos     pos;

  out = writer->CGGetFilePointer();
  assert(out);
//  pos = writer->CGGetIndentPos();

  // Diese Methode wird nur fuer System- und
  // Block-Sensoren aufgerufen!
  
  return DS_OK;
}

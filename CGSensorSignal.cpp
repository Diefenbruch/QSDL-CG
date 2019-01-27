/************************************************************************
 * Classname: CGSensorSignal
 * Purpose:   Definition eines Sensors
 ************************************************************************/

#include <DS/DSString.h>
#include <DS/DSSignal.h>

#include "CGFile.h"
#include "CGSystem.h"
#include "CGSensorSignal.h"

extern char *sensor_names[];

CGSensorSignal::CGSensorSignal (DSObject* father,
                                DSSensorType sType,
                                DSString* sName,
                                DSSignalRefList * sig) :
  DSSensorSignal (father, sType, sName, sig)
{
  // do nothing.
}

CGSensorSignal::CGSensorSignal (DSObject* father,
                                DSSensorType sType,
                                DSString& sName,
                                DSSignalRefList * sig) :
  DSSensorSignal (father, sType, sName, sig)
{
  // do nothing.
}

CGSensorSignal::CGSensorSignal (DSObject* father,
                                DSSensorType sType,
                                const char * sName,
                                DSSignalRefList * sig) :
  DSSensorSignal (father, sType, sName, sig)
{
  // do nothing.
}

CGSensorSignal::~CGSensorSignal(void)
{
  // do nothing.
}


DSObject* CGSensorSignal::New (DSObject* father) const
{
  return new CGSensorSignal(father);
}

/****************************************************************************
 * Write(): schreibt eine SDL-Sensor
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                    DS_ERROR
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGSensorSignal::Write(DSWriter *writer, DSCardinal what) const
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

    case CG_WRITE_IDENTIFIER:
      result = WriteIdentifier((CGWriter *)writer);
      break;

    case CG_WRITE_CREATE:
      result = WriteCreate((CGWriter *)writer);
      break;

    case CG_WRITE_DESTROY:
      result = WriteDestroy((CGWriter *)writer);
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

DSResult CGSensorSignal::WriteSensor(CGWriter *writer) const
{
  DSResult  result;
  CGFile   *out;
  CGPos     pos;
  DSObject *father;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  father = GetParent();

  switch (father->GetType())
  {
    case DS_SYSTEM:
    case DS_BLOCK:
    case DS_BLOCKSUBSTRUCTURE:
      InsertString(*out, "static ", pos, CG_NO_EOL);
      InsertString(*out, sensor_names[GetType()], 0, CG_NO_EOL);
      InsertString(*out, " * ", 0, CG_NO_EOL);
      break;
    case DS_PROCESS:
    case DS_MACHINE:
      InsertString(*out, sensor_names[GetType()], pos, CG_NO_EOL);
      InsertString(*out, " ", 0, CG_NO_EOL);
      break;
    default:
      assert(DS_FALSE);
      break;
  }
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

DSResult CGSensorSignal::WriteInit(CGWriter *writer) const
{
  DSResult  result;
  CGFile   *out;
  CGPos     pos;
  DSString  str;
  DSObject *father;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  father = GetParent();

  switch (father->GetType())
  {
    //
    // System-Sensoren mit Signal-Parameter:
    //
    case DS_SYSTEM:
    case DS_BLOCK:
    case DS_BLOCKSUBSTRUCTURE:
      InsertString(*out, sensor_names[GetType()], pos, CG_NO_EOL);
      InsertString(*out, " * ", 0, CG_NO_EOL);
      father->Write(writer, CG_WRITE_IDENTIFIER);
      InsertString(*out, "::", 0, CG_NO_EOL);
      InsertString(*out, PREFIX_SENSOR, 0, CG_NO_EOL);
      InsertString(*out, GetName(), 0, CG_NO_EOL);
      InsertString(*out, " = NULL;", 0, CG_WITH_EOL);
      break;

    //
    // Prozess-Sensoren mit Signal-Parameter:
    //
    case DS_PROCESS:
      InsertString(*out, ",", 0, CG_WITH_EOL);
      InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
      InsertString(*out, GetName(), 0, CG_NO_EOL);
      switch(GetType())
      {
        case DS_SENSOR_PCS_WAITTIME_SIGNAL:
        case DS_SENSOR_PCS_TRANSMISSIONTIME_SIGNAL:
        case DS_SENSOR_PCS_COUNT_SIGNAL_RECEIVE:
        case DS_SENSOR_PCS_COUNT_SIGNAL_DROP:
        case DS_SENSOR_PCS_COUNT_SIGNAL_CONSUME:
        case DS_SENSOR_PCS_COUNT_SIGNAL_SAVE:
        case DS_SENSOR_PCS_COUNT_SIGNAL_LOSE:
        case DS_SENSOR_PCS_COUNT_SIGNAL_SEND:
          InsertString(*out, "(\"", 0, CG_NO_EOL);
          InsertString(*out, GetName(), 0, CG_NO_EOL);
          InsertString(*out, "\", (SCDuration)1.0, (SCObject *)this)", 0, CG_NO_EOL);
          break;
        default:
          std::cerr << "Got sensor type '" << sensor_names[GetType()] << "'." << std::endl;
          assert(DS_FALSE);
      }
      break;

    //
    // Maschinen-Sensoren mit Signal-Parameter:
    //
    case DS_MACHINE:
      std::cerr << "Got sensor type '" << sensor_names[GetType()] << "'." << std::endl;
      assert(DS_FALSE);
      break;

    default:
      assert(DS_FALSE);
      break;
  }

  return DS_OK;
}


/****************************************************************************
 * WriteSave(): Speicher Sensor
 ****************************************************************************/

DSResult CGSensorSignal::WriteSave(CGWriter * writer) const
{
  DSResult result;
  CGFile *out;
  CGPos   pos;
  DSString str;
  DSObject *father;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();
  
  father = GetParent();

  switch (father->GetType())
  {
    //
    // System-Sensoren ohne Parameter:
    //
    case DS_SYSTEM:
    case DS_BLOCK:
    case DS_BLOCKSUBSTRUCTURE:
      str.Format("%s%s->Save(save_area);", PREFIX_SENSOR, GetName()->GetString());
      break;
      
    case DS_PROCESS:
    case DS_MACHINE:
      str.Format("%s%s.Save(save_area);", PREFIX_SENSOR, GetName()->GetString());
      break;
      
    default:
      assert(DS_FALSE);
      return DS_ERROR;
  }
  InsertString(*out, str, pos, CG_WITH_EOL);
  
  return DS_OK;
}

/****************************************************************************
 * WriteRestore(): Restauriert Sensor nach Backtracking
 ****************************************************************************/

DSResult CGSensorSignal::WriteRestore(CGWriter * writer) const
{
  DSResult result;
  CGFile *out;
  CGPos   pos;
  DSString str;
  DSObject *father;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();
  
  father = GetParent();

  switch (father->GetType())
  {
    //
    // System-Sensoren ohne Parameter:
    //
    case DS_SYSTEM:
    case DS_BLOCK:
    case DS_BLOCKSUBSTRUCTURE:
      str.Format("%s%s->Restore(save_area);", PREFIX_SENSOR, GetName()->GetString());
      break;
      
    case DS_PROCESS:
    case DS_MACHINE:
      str.Format("%s%s.Restore(save_area);", PREFIX_SENSOR, GetName()->GetString());
      break;
      
    default:
      assert(DS_FALSE);
      return DS_ERROR;
  }
  InsertString(*out, str, pos, CG_WITH_EOL);
  
  return DS_OK;
}

/****************************************************************************
 * WriteSize(): Berechnet Speicherplatz fuer den Sensor
 ****************************************************************************/

DSResult CGSensorSignal::WriteSize(CGWriter * writer) const
{
  DSResult result;
  CGFile *out;
  CGPos   pos;
  DSString str;
  DSObject *father;
    
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();
  
  father = GetParent();

  switch (father->GetType())
  {
    case DS_SYSTEM:
    case DS_BLOCK:
      str.Format("%s%s->Size(cur_size);", PREFIX_SENSOR, GetName()->GetString());
      break;
      
    case DS_PROCESS:
    case DS_MACHINE:
      str.Format("%s%s.Size(cur_size);", PREFIX_SENSOR, GetName()->GetString());
      break;
      
    default:
      assert(DS_FALSE);
      return DS_ERROR;
  }
  InsertString(*out, str, pos, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * WriteUpdate(): Schreibt Update-Aufruf in die SensorUpdate-Methode
 ****************************************************************************/

DSResult CGSensorSignal::WriteUpdate(CGWriter * writer) const
{
  DSResult result;
  CGFile  *out;
  CGPos    pos;
  DSCardinal event;
  DSObject *father;
  DSSignalRef signal;
  DSBoolean first;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();
  
  father = GetParent();
  assert(father);

  event = writer->CGGetEvent();
  
  switch (GetType())
  {
    case DS_SENSOR_SYS_COUNT_SIGNAL:
      if (event == cgTraceSignalSend)
      {
        if (GetSignalRefList())
        {
          InsertString(*out, "if (", pos, CG_NO_EOL);
          first = DS_TRUE;
          for (signal = GetFirstSignalRef();
               signal;
               signal = GetNextSignalRef())
          {
            if (first)
            {
              first = DS_FALSE;
            }
            else
            {
              InsertString(*out, " ||", 0, CG_WITH_EOL);
              InsertString(*out, "", pos + 4, CG_NO_EOL);
            }
            InsertString(*out, "info.theSignal->GetSignalType() == ", 0, CG_NO_EOL);
            InsertString(*out, PREFIX_SIGNAL_TYPE_VARIABLE, 0, CG_NO_EOL);
            InsertString(*out, signal->GetName(), 0, CG_NO_EOL);
          }
          InsertString(*out, ")", 0, CG_WITH_EOL);
          InsertString(*out, "{", pos, CG_WITH_EOL);
          pos += 2;
          writer->CGSetIndentPos(pos);
        }
        father->Write(writer, CG_WRITE_IDENTIFIER);
        InsertString(*out, "::", 0, CG_NO_EOL);
        InsertString(*out, PREFIX_SENSOR, 0, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, "->Update();", "CountSignal (System)", 0); 
        if (GetSignalRefList())
        {
          pos -= 2;
          writer->CGSetIndentPos(pos);
          InsertString(*out, "}", pos, CG_WITH_EOL);
        }
      }
      break;

    case DS_SENSOR_PCS_WAITTIME_SIGNAL:
      if (event == cgTraceSignalConsume ||
          event == cgTraceSignalDrop)
      {
        if (GetSignalRefList())
        {
          InsertString(*out, "if (", pos, CG_NO_EOL);
          first = DS_TRUE;
          for (signal = GetFirstSignalRef();
               signal;
               signal = GetNextSignalRef())
          {
            if (first)
            {
              first = DS_FALSE;
            }
            else
            {
              InsertString(*out, " ||", 0, CG_WITH_EOL);
              InsertString(*out, "", pos + 4, CG_NO_EOL);
            }
            InsertString(*out, "info.theSignal->GetSignalType() == ", 0, CG_NO_EOL);
            InsertString(*out, PREFIX_SIGNAL_TYPE_VARIABLE, 0, CG_NO_EOL);
            InsertString(*out, signal->GetName(), 0, CG_NO_EOL);
          }
          InsertString(*out, ")", 0, CG_WITH_EOL);
          InsertString(*out, "{", pos, CG_WITH_EOL);
          pos += 2;
        }
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(info.theSignal);", "WaitTimeSignal (Process)", 0); 
        if (GetSignalRefList())
        {
          pos -= 2;
          InsertString(*out, "}", pos, CG_WITH_EOL);
        }
      }
      break;

    case DS_SENSOR_PCS_TRANSMISSIONTIME_SIGNAL:
      if (event == cgTraceSignalReceive)
      {
        if (GetSignalRefList())
        {
          InsertString(*out, "if (", pos, CG_NO_EOL);
          first = DS_TRUE;
          for (signal = GetFirstSignalRef();
               signal;
               signal = GetNextSignalRef())
          {
            if (first)
            {
              first = DS_FALSE;
            }
            else
            {
              InsertString(*out, " ||", 0, CG_WITH_EOL);
              InsertString(*out, "", pos + 4, CG_NO_EOL);
            }
            InsertString(*out, "info.theSignal->GetSignalType() == ", 0, CG_NO_EOL);
            InsertString(*out, PREFIX_SIGNAL_TYPE_VARIABLE, 0, CG_NO_EOL);
            InsertString(*out, signal->GetName(), 0, CG_NO_EOL);
          }
          InsertString(*out, ")", 0, CG_WITH_EOL);
          InsertString(*out, "{", pos, CG_WITH_EOL);
          pos += 2;
        }
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(info.theSignal);", "TransmissionTimeSignal (Process)", 0); 
        if (GetSignalRefList())
        {
          pos -= 2;
          InsertString(*out, "}", pos, CG_WITH_EOL);
        }
      }
      break;

    case DS_SENSOR_PCS_COUNT_SIGNAL_RECEIVE:
      if (event == cgTraceSignalReceive)
      {
        if (GetSignalRefList())
        {
          InsertString(*out, "if (", pos, CG_NO_EOL);
          first = DS_TRUE;
          for (signal = GetFirstSignalRef();
               signal;
               signal = GetNextSignalRef())
          {
            if (first)
            {
              first = DS_FALSE;
            }
            else
            {
              InsertString(*out, " ||", 0, CG_WITH_EOL);
              InsertString(*out, "", pos + 4, CG_NO_EOL);
            }
            InsertString(*out, "info.theSignal->GetSignalType() == ", 0, CG_NO_EOL);
            InsertString(*out, PREFIX_SIGNAL_TYPE_VARIABLE, 0, CG_NO_EOL);
            InsertString(*out, signal->GetName(), 0, CG_NO_EOL);
          }
          InsertString(*out, ")", 0, CG_WITH_EOL);
          InsertString(*out, "{", pos, CG_WITH_EOL);
          pos += 2;
        }
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update();", "CountSignalReceive (Process)", 0); 
        if (GetSignalRefList())
        {
          pos -= 2;
          InsertString(*out, "}", pos, CG_WITH_EOL);
        }
      }
      break;

    case DS_SENSOR_PCS_COUNT_SIGNAL_DROP:
      if (event == cgTraceSignalDrop)
      {
        if (GetSignalRefList())
        {
          InsertString(*out, "if (", pos, CG_NO_EOL);
          first = DS_TRUE;
          for (signal = GetFirstSignalRef();
               signal;
               signal = GetNextSignalRef())
          {
            if (first)
            {
              first = DS_FALSE;
            }
            else
            {
              InsertString(*out, " ||", 0, CG_WITH_EOL);
              InsertString(*out, "", pos + 4, CG_NO_EOL);
            }
            InsertString(*out, "info.theSignal->GetSignalType() == ", 0, CG_NO_EOL);
            InsertString(*out, PREFIX_SIGNAL_TYPE_VARIABLE, 0, CG_NO_EOL);
            InsertString(*out, signal->GetName(), 0, CG_NO_EOL);
          }
          InsertString(*out, ")", 0, CG_WITH_EOL);
          InsertString(*out, "{", pos, CG_WITH_EOL);
          pos += 2;
        }
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update();", "CountSignalDrop (Process)", 0); 
        if (GetSignalRefList())
        {
          pos -= 2;
          InsertString(*out, "}", pos, CG_WITH_EOL);
        }
      }
      break;

    case DS_SENSOR_PCS_COUNT_SIGNAL_CONSUME:
      if (event == cgTraceSignalConsume)
      {
        if (GetSignalRefList())
        {
          InsertString(*out, "if (", pos, CG_NO_EOL);
          first = DS_TRUE;
          for (signal = GetFirstSignalRef();
               signal;
               signal = GetNextSignalRef())
          {
            if (first)
            {
              first = DS_FALSE;
            }
            else
            {
              InsertString(*out, " ||", 0, CG_WITH_EOL);
              InsertString(*out, "", pos + 4, CG_NO_EOL);
            }
            InsertString(*out, "info.theSignal->GetSignalType() == ", 0, CG_NO_EOL);
            InsertString(*out, PREFIX_SIGNAL_TYPE_VARIABLE, 0, CG_NO_EOL);
            InsertString(*out, signal->GetName(), 0, CG_NO_EOL);
          }
          InsertString(*out, ")", 0, CG_WITH_EOL);
          InsertString(*out, "{", pos, CG_WITH_EOL);
          pos += 2;
        }
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update();", "CountSignalConsume (Process)", 0); 
        if (GetSignalRefList())
        {
          pos -= 2;
          InsertString(*out, "}", pos, CG_WITH_EOL);
        }
      }
      break;

    case DS_SENSOR_PCS_COUNT_SIGNAL_SAVE:
      if (event == cgTraceSignalSave)
      {
        if (GetSignalRefList())
        {
          InsertString(*out, "if (", pos, CG_NO_EOL);
          first = DS_TRUE;
          for (signal = GetFirstSignalRef();
               signal;
               signal = GetNextSignalRef())
          {
            if (first)
            {
              first = DS_FALSE;
            }
            else
            {
              InsertString(*out, " ||", 0, CG_WITH_EOL);
              InsertString(*out, "", pos + 4, CG_NO_EOL);
            }
            InsertString(*out, "info.theSignal->GetSignalType() == ", 0, CG_NO_EOL);
            InsertString(*out, PREFIX_SIGNAL_TYPE_VARIABLE, 0, CG_NO_EOL);
            InsertString(*out, signal->GetName(), 0, CG_NO_EOL);
          }
          InsertString(*out, ")", 0, CG_WITH_EOL);
          InsertString(*out, "{", pos, CG_WITH_EOL);
          pos += 2;
        }
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update();", "CountSignalSave (Process)", 0); 
        if (GetSignalRefList())
        {
          pos -= 2;
          InsertString(*out, "}", pos, CG_WITH_EOL);
        }
      }
      break;

    case DS_SENSOR_PCS_COUNT_SIGNAL_LOSE:
      if (event == cgTraceSignalLose)
      {
        if (GetSignalRefList())
        {
          InsertString(*out, "if (", pos, CG_NO_EOL);
          first = DS_TRUE;
          for (signal = GetFirstSignalRef();
               signal;
               signal = GetNextSignalRef())
          {
            if (first)
            {
              first = DS_FALSE;
            }
            else
            {
              InsertString(*out, " ||", 0, CG_WITH_EOL);
              InsertString(*out, "", pos + 4, CG_NO_EOL);
            }
            InsertString(*out, "info.theSignal->GetSignalType() == ", 0, CG_NO_EOL);
            InsertString(*out, PREFIX_SIGNAL_TYPE_VARIABLE, 0, CG_NO_EOL);
            InsertString(*out, signal->GetName(), 0, CG_NO_EOL);
          }
          InsertString(*out, ")", 0, CG_WITH_EOL);
          InsertString(*out, "{", pos, CG_WITH_EOL);
          pos += 2;
        }
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update();", "CountSignalLose (Process)", 0); 
        if (GetSignalRefList())
        {
          pos -= 2;
          InsertString(*out, "}", pos, CG_WITH_EOL);
        }
      }
      break;

    case DS_SENSOR_PCS_COUNT_SIGNAL_SEND:
      if (event == cgTraceSignalSend)
      {
        if (GetSignalRefList())
        {
          InsertString(*out, "if (", pos, CG_NO_EOL);
          first = DS_TRUE;
          for (signal = GetFirstSignalRef();
               signal;
               signal = GetNextSignalRef())
          {
            if (first)
            {
              first = DS_FALSE;
            }
            else
            {
              InsertString(*out, " ||", 0, CG_WITH_EOL);
              InsertString(*out, "", pos + 4, CG_NO_EOL);
            }
            InsertString(*out, "info.theSignal->GetSignalType() == ", 0, CG_NO_EOL);
            InsertString(*out, PREFIX_SIGNAL_TYPE_VARIABLE, 0, CG_NO_EOL);
            InsertString(*out, signal->GetName(), 0, CG_NO_EOL);
          }
          InsertString(*out, ")", 0, CG_WITH_EOL);
          InsertString(*out, "{", pos, CG_WITH_EOL);
          pos += 2;
        }
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update();", "CountSignalSend (Process)", 0); 
        if (GetSignalRefList())
        {
          pos -= 2;
          InsertString(*out, "}", pos, CG_WITH_EOL);
        }
      }
      break;


    default:
      std::cerr << "Got sensor type '" << sensor_names[GetType()] << "'." << std::endl;
      assert(DS_FALSE);
  }

  return DS_OK;
}

/****************************************************************************
 * WriteIdentifier(): Schreibt den Namen des Sensors (mit notwendigen Prefixen)
 ****************************************************************************/

DSResult CGSensorSignal::WriteIdentifier(CGWriter * writer) const
{
  DSResult result;
  CGFile  *out;
  CGPos    pos;
  DSObject *father;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  father = GetParent();
  assert(father);
  
  switch (father->GetType())
  {
    case DS_SYSTEM:
    case DS_BLOCK:
    case DS_BLOCKSUBSTRUCTURE:
      father->Write(writer, CG_WRITE_IDENTIFIER);
      InsertString(*out, "::", 0, CG_NO_EOL);
      InsertString(*out, PREFIX_SENSOR, 0, CG_NO_EOL);
      InsertString(*out, GetName(), 0, CG_NO_EOL);
      break;

    case DS_PROCESS:
    case DS_MACHINE:
      InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
      InsertString(*out, GetName(), 0, CG_NO_EOL);
      break;
      
    default:
      assert(DS_FALSE);
  }
  
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

DSResult CGSensorSignal::WriteCreate(CGWriter *writer) const
{
  DSResult  result;
  CGFile   *out;
  CGPos     pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " = new ", 0, CG_NO_EOL);
  InsertString(*out, sensor_names[GetType()], 0, CG_NO_EOL);

  switch(GetType())
  {
    case DS_SENSOR_SYS_COUNT_SIGNAL:
      InsertString(*out, "(\"", 0, CG_NO_EOL);
      InsertString(*out, GetName(), 0, CG_NO_EOL);
      InsertString(*out, "\", (SCDuration)1.0, (SCObject *)NULL);", 0, CG_WITH_EOL);
      break;
    default:
      std::cerr << "Got sensor type '" << sensor_names[GetType()] << "'." << std::endl;
      assert(DS_FALSE);
      return DS_ERROR;
      break;
  }

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

DSResult CGSensorSignal::WriteDestroy(CGWriter *writer) const
{
  DSResult  result;
  CGFile   *out;
  CGPos     pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "delete ", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_SENSOR, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, ";", 0, CG_WITH_EOL);

  return DS_OK;
}

/************************************************************************
 * Classname: CGSensor
 * Purpose:   Definition eines Sensors
 ************************************************************************/

#include <DS/DSString.h>
#include <DS/DSBlock.h>
#include <DS/DSState.h> // MS VC++ needs this!

#include "CGFile.h"
#include "CGSystem.h"
#include "CGProcess.h"
#include "CGSensor.h"
#include "CGSensorNames.h"

extern char *event_names[]; // included in CGProcess.cc

CGSensor::CGSensor (DSObject* father,
                    DSSensorType sType,
                    DSString* sName) :
  DSSensor (father, sType, sName)
{
  // do nothing.
}

CGSensor::CGSensor (DSObject* father,
                    DSSensorType sType,
                    DSString& sName) :
  DSSensor (father, sType, sName)
{
  // do nothing.
}

CGSensor::CGSensor (DSObject* father,
                    DSSensorType sType,
                    const char * sName) :
  DSSensor (father, sType, sName)
{
  // do nothing.
}

CGSensor::~CGSensor(void)
{
  // do nothing.
}


DSObject* CGSensor::New (DSObject* father) const
{
  return new CGSensor(father);
}

/****************************************************************************
 * Write(): schreibt eine SDL-Sensor
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                    DS_ERROR
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/
 
DSResult CGSensor::Write(DSWriter *writer, DSCardinal what) const
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
 * WriteSensor(): schreibt Sensoren des Systems/Blocks oder eines Prozesses
 *                -> writer
 *                Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGSensor::WriteSensor(CGWriter *writer) const
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
 * WriteInit(): schreibt die Initialisierung eines Sensor ohne Parameter.
 *              Wird nur fuer Systemsensoren benoetigt da diese static
 *              deklariert werden.
 *              -> writer
 *              Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *              Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSResult CGSensor::WriteInit(CGWriter *writer) const
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
    //
    // System-Sensoren ohne Parameter:
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
    // Prozess-Sensoren ohne Parameter:
    //
    case DS_PROCESS:
      InsertString(*out, ",", 0, CG_WITH_EOL);
      InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
      InsertString(*out, GetName(), 0, CG_NO_EOL);
      switch(GetType())
      {
        case DS_SENSOR_FREQUENCY:
          InsertString(*out, "(\"", 0, CG_NO_EOL);
          InsertString(*out, GetName(), 0, CG_NO_EOL);
          InsertString(*out, "\", (SCObject *)this, true)", 0, CG_NO_EOL);
          break;
        case DS_SENSOR_PCS_FREQ_SIGNAL_RECEIVE:
        case DS_SENSOR_PCS_FREQ_SIGNAL_DROP:
        case DS_SENSOR_PCS_FREQ_SIGNAL_CONSUME:
        case DS_SENSOR_PCS_FREQ_SIGNAL_SAVE:
        case DS_SENSOR_PCS_FREQ_SIGNAL_LOSE:
        case DS_SENSOR_PCS_FREQ_SIGNAL_SEND:
        case DS_SENSOR_PCS_FREQ_REQUEST:
        case DS_SENSOR_PCS_QUEUE_CONTENT:
          InsertString(*out, "(\"", 0, CG_NO_EOL);
          InsertString(*out, GetName(), 0, CG_NO_EOL);
          InsertString(*out, "\", (SCObject *)this)", 0, CG_NO_EOL);
          break;
        case DS_SENSOR_PCS_FREQ_STATE:
          InsertString(*out, "(\"", 0, CG_NO_EOL);
          InsertString(*out, GetName(), 0, CG_NO_EOL);
          InsertString(*out, "\", state_type_id_start, state_type_id_start + ", 0, CG_NO_EOL);
          InsertCardinal(*out,
                         ((DSProcess *)father)->GetStateList()->NumberOfElements(),
                         0, CG_NO_EOL);
          InsertString(*out, ", (SCObject *)this)", 0, CG_NO_EOL);
          break;
        case DS_SENSOR_COUNTER:
        case DS_SENSOR_TALLY:
          InsertString(*out, "(\"", 0, CG_NO_EOL);
          InsertString(*out, GetName(), 0, CG_NO_EOL);
          InsertString(*out, "\", (SCDuration)1.0, this, true)", 0, CG_NO_EOL);
          break;
        case DS_SENSOR_PCS_QUEUE_LENGTH:
          InsertString(*out, "(\"", 0, CG_NO_EOL);
          InsertString(*out, GetName(), 0, CG_NO_EOL);
          InsertString(*out, "\", this, (SCDuration)1.0)", 0, CG_NO_EOL);
          break;
        default:
          assert(DS_FALSE);
          return DS_ERROR;
      }
      break;

    //
    // Maschinen-Sensoren ohne Parameter:
    //
    case DS_MACHINE:
      InsertString(*out, ",", 0, CG_WITH_EOL);
      InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
      InsertString(*out, GetName(), 0, CG_NO_EOL);
      switch(GetType())
      {
        case DS_SENSOR_MACH_FREQ_REQUEST:
        case DS_SENSOR_MACH_QUEUE_CONTENT:
          InsertString(*out, "(\"", 0, CG_NO_EOL);
          InsertString(*out, GetName(), 0, CG_NO_EOL);
          InsertString(*out, "\", (SCObject *)this)", 0, CG_NO_EOL);
          break;
        case DS_SENSOR_MACH_QUEUE_LENGTH:
        case DS_SENSOR_MACH_UTILIZATION:
          InsertString(*out, "(\"", 0, CG_NO_EOL);
          InsertString(*out, GetName(), 0, CG_NO_EOL);
          InsertString(*out, "\", this, (SCDuration)1.0)", 0, CG_NO_EOL);
          break;
        default:
          assert(DS_FALSE);
      }
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

DSResult CGSensor::WriteSave(CGWriter * writer) const
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

DSResult CGSensor::WriteRestore(CGWriter * writer) const
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

DSResult CGSensor::WriteSize(CGWriter * writer) const
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
    case DS_BLOCKSUBSTRUCTURE:
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

DSResult CGSensor::WriteUpdate(CGWriter * writer) const
{
  DSResult result;
  CGFile  *out;
  CGPos    pos;
  DSCardinal event;
  DSObject *father;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  father = GetParent();
  assert(father);

  event = writer->CGGetEvent();
  
  switch (GetType())
  {
    case DS_SENSOR_COUNTER:    // diese Sensoren muss der Spezifizierer
    case DS_SENSOR_FREQUENCY:  // selbst updaten!
    case DS_SENSOR_TALLY:
      break;
      
    case DS_SENSOR_SYS_FREQ_SIGNAL:
      if (event == cgTraceSignalSend)
      {
        father->Write(writer, CG_WRITE_IDENTIFIER);
        InsertString(*out, "::", 0, CG_NO_EOL);
        InsertString(*out, PREFIX_SENSOR, 0, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, "->Update(+1, info.theSignal);", "FreqSignal (System)", 0);
      }
      break;
    case DS_SENSOR_SYS_FREQ_REQUEST:
      if (event == cgTraceServiceRequest)
      {
        father->Write(writer, CG_WRITE_IDENTIFIER);
        InsertString(*out, "::", 0, CG_NO_EOL);
        InsertString(*out, PREFIX_SENSOR, 0, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, "->Update(+1, info.theRequest);", "FreqRequest (System)", 0); 
      }
      break;
    case DS_SENSOR_SYS_FREQ_PROCESS_CREATE:
      if (event == cgTraceProcessCreate)
      {
        father->Write(writer, CG_WRITE_IDENTIFIER);
        writer->CGSetIndentPos(pos);
        InsertString(*out, "::", 0, CG_NO_EOL);
        InsertString(*out, PREFIX_SENSOR, 0, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, "->Update(+1, ((SCProcess *)info.thePassiveAutomaton)->GetType());",
                            "FreqProcess (System)", 0); 
      }
      break;
    case DS_SENSOR_SYS_FREQ_PROCESS_STOP:
      if (event == cgTraceProcessStop)
      {
        father->Write(writer, CG_WRITE_IDENTIFIER);
        writer->CGSetIndentPos(pos);
        InsertString(*out, "::", 0, CG_NO_EOL);
        InsertString(*out, PREFIX_SENSOR, 0, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, "->Update(+1, ((SCProcess *)info.theActiveAutomaton)->GetType());",
                            "FreqProcess (System)", 0); 
      }
      break;
    case DS_SENSOR_PCS_FREQ_SIGNAL_RECEIVE:
      if (event == cgTraceSignalReceive)
      {
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(+1, info.theSignal);", "FreqSignalReceive (Process)", 0); 
      }
      break;
    case DS_SENSOR_PCS_FREQ_SIGNAL_DROP:
      if (event == cgTraceSignalDrop)
      {
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(+1, info.theSignal);", "FreqSignalDrop (Process)", 0); 
      }
      break;
    case DS_SENSOR_PCS_FREQ_SIGNAL_CONSUME:
      if (event == cgTraceSignalConsume)
      {
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(+1, info.theSignal);", "FreqSignalConsume (Process)", 0); 
      }
      if (event == cgTraceSpontTrans ||
          event == cgTraceContSignal)
      {
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".ClearLastValue();", "FreqSignalConsume (Process)", 0); 
      }
      break;
    case DS_SENSOR_PCS_FREQ_SIGNAL_SAVE:
      if (event == cgTraceSignalSave)
      {
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(+1, info.theSignal);", "FreqSignalSave (Process)", 0); 
      }
      break;
    case DS_SENSOR_PCS_FREQ_SIGNAL_LOSE:
      if (event == cgTraceSignalLose)
      {
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(+1, info.theSignal);", "FreqSignalLose (Process)", 0); 
      }
      break;
    case DS_SENSOR_PCS_FREQ_SIGNAL_SEND:
      if (event == cgTraceSignalSend)
      {
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(+1, info.theSignal);", "FreqSignalSend (Process)", 0); 
      }
      break;
    case DS_SENSOR_PCS_FREQ_REQUEST:
      if (event == cgTraceServiceRequest)
      {
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(+1, info.theRequest);", "FreqRequest (Process)", 0); 
      }
      break;
      
    case DS_SENSOR_PCS_QUEUE_CONTENT:
      if (event == cgTraceSignalReceive)
      {
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(+1, info.theSignal);", "QueueContent (Process)", 0); 
      }
      if (event == cgTraceSignalConsume ||
          event == cgTraceSignalDrop ||
          event == cgTraceTimerRemove)
      {
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(-1, info.theSignal);", "QueueContent (Process)", 0); 
      }
      break;

    case DS_SENSOR_PCS_QUEUE_LENGTH:
      if (event == cgTraceSignalReceive)
      {
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(+1);", "QueueLength (Process)", 0); 
      }
      if (event == cgTraceSignalConsume ||
          event == cgTraceSignalDrop ||
          event == cgTraceTimerRemove)
      {
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(-1);", "QueueLength (Process)", 0); 
      }
      break;

    case DS_SENSOR_PCS_FREQ_STATE:
      if (event == cgTraceStateChange)
      {
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(+1, info.theState);", "FreqState (Process)", 0); 
      }
      break;
    case DS_SENSOR_MACH_QUEUE_CONTENT:
      if (event == cgTraceServiceRequest ||
          event == cgTraceServiceInterrupt)
      {
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(+1, info.theRequest);", "QueueContent (Machine)", 0); 
      }
      if (event == cgTraceServiceStart)
      {
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(-1, info.theRequest);", "QueueContent (Machine)", 0); 
      }
      break;
    case DS_SENSOR_MACH_QUEUE_LENGTH:
      if (event == cgTraceServiceRequest ||
          event == cgTraceServiceInterrupt)
      {
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(+1);", "QueueLength (Machine)", 0); 
      }
      if (event == cgTraceServiceStart)
      {
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(-1);", "QueueLength (Machine)", 0); 
      }
      break;
    case DS_SENSOR_MACH_FREQ_REQUEST:
      if (event == cgTraceServiceRequest)
      {
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(+1, info.theRequest);", "FreqRequest (Machine)", 0); 
      }
      break;
    case DS_SENSOR_MACH_UTILIZATION:
      if (event == cgTraceServiceStart)
      {
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(-1);", "Utilization (Machine)", 0); 
      }
      if (event == cgTraceServiceInterrupt ||
          event == cgTraceServiceFinish)
      {
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(+1);", "Utilization (Machine)", 0); 
      }
      break;
    default:
      assert(DS_FALSE);
  }
  return DS_OK;
}

/****************************************************************************
 * WriteIdentifier(): Schreibt den Namen des Sensors (mit notwendigen Prefixen)
 ****************************************************************************/

DSResult CGSensor::WriteIdentifier(CGWriter * writer) const
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

DSResult CGSensor::WriteCreate(CGWriter *writer) const
{
  DSResult  result;
  CGFile   *out;
  CGPos     pos;
  DSObject *father;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  father = GetParent();

  InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, " = new ", 0, CG_NO_EOL);
  InsertString(*out, sensor_names[GetType()], 0, CG_NO_EOL);

  switch(GetType())
  {
    case DS_SENSOR_FREQUENCY:
    case DS_SENSOR_SYS_FREQ_SIGNAL:  // param: father
    case DS_SENSOR_SYS_FREQ_REQUEST:
      InsertString(*out, "(\"", 0, CG_NO_EOL);
      InsertString(*out, GetName(), 0, CG_NO_EOL);
      InsertString(*out, "\", (SCObject *)NULL);", 0, CG_WITH_EOL);
      break;
    case DS_SENSOR_SYS_FREQ_PROCESS_CREATE:
    case DS_SENSOR_SYS_FREQ_PROCESS_STOP:
      InsertString(*out, "(\"", 0, CG_NO_EOL);
      InsertString(*out, GetName(), 0, CG_NO_EOL);
      InsertString(*out, "\", kSCProcessTypeIDBase, kSCProcessTypeIDBase + ", 0, CG_NO_EOL);
      if (father->GetType() == DS_SYSTEM)
      {
        InsertCardinal(*out, ((DSSystem *)father)->GetNumOfProcesses(), 0, CG_NO_EOL);
      }
      else // Vater ist ein Block!
      {
        InsertCardinal(*out, ((DSBlock *)father)->GetNumOfProcesses(), 0, CG_NO_EOL);
      }
      InsertString(*out, " - 1, (SCObject *)NULL);", 0, CG_WITH_EOL);
      break;
    case DS_SENSOR_COUNTER:
    case DS_SENSOR_TALLY:
      InsertString(*out, "(\"", 0, CG_NO_EOL);
      InsertString(*out, GetName(), 0, CG_NO_EOL);
      InsertString(*out, "\", (SCDuration)1.0, (SCObject *)NULL);", 0, CG_WITH_EOL);
      break;
    default:
      assert(DS_FALSE);
      return DS_ERROR;
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

DSResult CGSensor::WriteDestroy(CGWriter *writer) const
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

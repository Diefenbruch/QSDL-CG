/************************************************************************
 * Classname: CGSensorMachineService
 * Purpose:   Definition eines Sensors
 ************************************************************************/

#include <DS/DSString.h>
#include <DS/DSMachineService.h>

#include "CGFile.h"
#include "CGSensorMachineService.h"
#include "CGSystem.h"

extern char *sensor_names[];

CGSensorMachineService::CGSensorMachineService (DSObject* father,
                                                DSSensorType sType,
                                                DSString* sName,
                                                DSMachineServiceRefList * ms) :
  DSSensorMachineService (father, sType, sName, ms)
{
  // do nothing.
}

CGSensorMachineService::CGSensorMachineService (DSObject* father,
                                  DSSensorType sType,
                                  DSString& sName,
                                  DSMachineServiceRefList * ms) :
  DSSensorMachineService (father, sType, sName, ms)
{
  // do nothing.
}

CGSensorMachineService::CGSensorMachineService (DSObject* father,
                                  DSSensorType sType,
                                  const char * sName,
                                  DSMachineServiceRefList * ms) :
  DSSensorMachineService (father, sType, sName, ms)
{
  // do nothing.
}

CGSensorMachineService::~CGSensorMachineService(void)
{
  // do nothing.
}


DSObject* CGSensorMachineService::New (DSObject* father) const
{
  return new CGSensorMachineService(father);
}

/****************************************************************************
 * Write(): schreibt eine SDL-Sensor
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                    DS_ERROR
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/

DSResult CGSensorMachineService::Write(DSWriter *writer, DSCardinal what) const
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

DSResult CGSensorMachineService::WriteSensor(CGWriter *writer) const
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

DSResult CGSensorMachineService::WriteInit(CGWriter *writer) const
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
    // System-Sensoren mit MachineService-Parameter:
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
    // Prozess-Sensoren mit MachineService-Parameter:
    //
    case DS_PROCESS:
      InsertString(*out, ",", 0, CG_WITH_EOL);
      InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
      InsertString(*out, GetName(), 0, CG_NO_EOL);
      switch(GetType())
      {
        case DS_SENSOR_PCS_RESPONSETIME_REQUEST:
        case DS_SENSOR_PCS_COUNT_REQUEST:
          InsertString(*out, "(\"", 0, CG_NO_EOL);
          InsertString(*out, GetName(), 0, CG_NO_EOL);
          InsertString(*out, "\", ", 0, CG_NO_EOL);
          InsertString(*out, "(SCDuration)1.0, (SCObject *)this)", 0, CG_NO_EOL);
          break;
        default:
          std::cerr << "Got sensor type '" << sensor_names[GetType()] << "'." << std::endl;
          assert(DS_FALSE);
      }
      break;

    //
    // Maschinen-Sensoren mit MachineService-Parameter:
    //
    case DS_MACHINE:
      InsertString(*out, ",", 0, CG_WITH_EOL);
      InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
      InsertString(*out, GetName(), 0, CG_NO_EOL);
      switch(GetType())
      {
        case DS_SENSOR_MACH_THROUGHPUT:
        case DS_SENSOR_MACH_WAITTIME_REQUEST:
        case DS_SENSOR_MACH_SERVICETIME_REQUEST:
        case DS_SENSOR_MACH_COUNT_REQUEST:
          InsertString(*out, "(\"", 0, CG_NO_EOL);
          InsertString(*out, GetName(), 0, CG_NO_EOL);
          InsertString(*out, "\", ", 0, CG_NO_EOL);
          InsertString(*out, "(SCDuration)1.0, (SCObject *)this)", 0, CG_NO_EOL);
          break;
        default:
          std::cerr << "Got sensor type '" << sensor_names[GetType()] << "'." << std::endl;
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

DSResult CGSensorMachineService::WriteSave(CGWriter * writer) const
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

DSResult CGSensorMachineService::WriteRestore(CGWriter * writer) const
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

DSResult CGSensorMachineService::WriteSize(CGWriter * writer) const
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

DSResult CGSensorMachineService::WriteUpdate(CGWriter * writer) const
{
  DSResult result;
  CGFile  *out;
  CGPos    pos;
  DSCardinal event;
  DSObject *father;
  DSMachineServiceRef machine_service;
  DSBoolean first;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();
  
  father = GetParent();
  assert(father);

  event = writer->CGGetEvent();
  
  switch (GetType())
  {
    case DS_SENSOR_SYS_COUNT_REQUEST:
      if (event == cgTraceServiceRequest)
      {
        if (GetMachineServiceRefList())
        {
          InsertString(*out, "if (", pos, CG_NO_EOL);
          first = DS_TRUE;
          for (machine_service = GetFirstMachineServiceRef();
               machine_service;
               machine_service = GetNextMachineServiceRef())
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
            InsertString(*out, "info.theRequest->GetRequestType() == ", 0, CG_NO_EOL);
            InsertString(*out, PREFIX_MACHINE_SERVICE_TYPE_VARIABLE, 0, CG_NO_EOL);
            InsertString(*out, machine_service->GetName(), 0, CG_NO_EOL);
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
        InsertStringComment(*out, "->Update();", "CountMachineService (System)", 0); 
        if (GetMachineServiceRefList())
        {
          pos -= 2;
          writer->CGSetIndentPos(pos);
          InsertString(*out, "}", pos, CG_WITH_EOL);
        }
      }
      break;

    case DS_SENSOR_PCS_COUNT_REQUEST:
      if (event == cgTraceServiceRequest)
      {
        if (GetMachineServiceRefList())
        {
          InsertString(*out, "if (", pos, CG_NO_EOL);
          first = DS_TRUE;
          for (machine_service = GetFirstMachineServiceRef();
               machine_service;
               machine_service = GetNextMachineServiceRef())
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
            InsertString(*out, "info.theRequest->GetRequestType() == ", 0, CG_NO_EOL);
            InsertString(*out, PREFIX_MACHINE_SERVICE_TYPE_VARIABLE, 0, CG_NO_EOL);
            InsertString(*out, machine_service->GetName(), 0, CG_NO_EOL);
          }
          InsertString(*out, ")", 0, CG_WITH_EOL);
          InsertString(*out, "{", pos, CG_WITH_EOL);
          pos += 2;
        }
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update();", "CountMachineService (Process)", 0); 
        if (GetMachineServiceRefList())
        {
          pos -= 2;
          InsertString(*out, "}", pos, CG_WITH_EOL);
        }
      }
      break;

    case DS_SENSOR_PCS_RESPONSETIME_REQUEST:
      if (event == cgTraceServiceFinish)
      {
        if (GetMachineServiceRefList())
        {
          InsertString(*out, "if (", pos, CG_NO_EOL);
          first = DS_TRUE;
          for (machine_service = GetFirstMachineServiceRef();
               machine_service;
               machine_service = GetNextMachineServiceRef())
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
            InsertString(*out, "info.theRequest->GetRequestType() == ", 0, CG_NO_EOL);
            InsertString(*out, PREFIX_MACHINE_SERVICE_TYPE_VARIABLE, 0, CG_NO_EOL);
            InsertString(*out, machine_service->GetName(), 0, CG_NO_EOL);
          }
          InsertString(*out, ")", 0, CG_WITH_EOL);
          InsertString(*out, "{", pos, CG_WITH_EOL);
          pos += 2;
        }
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(info.theRequest);", "ResponseTimeMachineService (Process)", 0); 
        if (GetMachineServiceRefList())
        {
          pos -= 2;
          InsertString(*out, "}", pos, CG_WITH_EOL);
        }
      }
      break;

    case DS_SENSOR_MACH_COUNT_REQUEST:
      if (event == cgTraceServiceRequest)
      {
        if (GetMachineServiceRefList())
        {
          InsertString(*out, "if (", pos, CG_NO_EOL);
          first = DS_TRUE;
          for (machine_service = GetFirstMachineServiceRef();
               machine_service;
               machine_service = GetNextMachineServiceRef())
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
            InsertString(*out, "info.theRequest->GetRequestType() == ", 0, CG_NO_EOL);
            InsertString(*out, PREFIX_MACHINE_SERVICE_TYPE_VARIABLE, 0, CG_NO_EOL);
            InsertString(*out, machine_service->GetName(), 0, CG_NO_EOL);
          }
          InsertString(*out, ")", 0, CG_WITH_EOL);
          InsertString(*out, "{", pos, CG_WITH_EOL);
          pos += 2;
        }
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update();", "CountMachineService (Machine)", 0); 
        if (GetMachineServiceRefList())
        {
          pos -= 2;
          InsertString(*out, "}", pos, CG_WITH_EOL);
        }
      }
      break;

    case DS_SENSOR_MACH_THROUGHPUT:
      if (event == cgTraceServiceFinish)
      {
        if (GetMachineServiceRefList())
        {
          InsertString(*out, "if (", pos, CG_NO_EOL);
          first = DS_TRUE;
          for (machine_service = GetFirstMachineServiceRef();
               machine_service;
               machine_service = GetNextMachineServiceRef())
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
            InsertString(*out, "info.theRequest->GetRequestType() == ", 0, CG_NO_EOL);
            InsertString(*out, PREFIX_MACHINE_SERVICE_TYPE_VARIABLE, 0, CG_NO_EOL);
            InsertString(*out, machine_service->GetName(), 0, CG_NO_EOL);
          }
          InsertString(*out, ")", 0, CG_WITH_EOL);
          InsertString(*out, "{", pos, CG_WITH_EOL);
          pos += 2;
        }
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(info.theRequest);", "Throughput (Machine)",0); 
        if (GetMachineServiceRefList())
        {
          pos -= 2;
          InsertString(*out, "}", pos, CG_WITH_EOL);
        }
      }
      break;

    case DS_SENSOR_MACH_WAITTIME_REQUEST:
      if (event == cgTraceServiceStart)
      {
        if (GetMachineServiceRefList())
        {
          InsertString(*out, "if (", pos, CG_NO_EOL);
          first = DS_TRUE;
          for (machine_service = GetFirstMachineServiceRef();
               machine_service;
               machine_service = GetNextMachineServiceRef())
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
            InsertString(*out, "info.theRequest->GetRequestType() == ", 0, CG_NO_EOL);
            InsertString(*out, PREFIX_MACHINE_SERVICE_TYPE_VARIABLE, 0, CG_NO_EOL);
            InsertString(*out, machine_service->GetName(), 0, CG_NO_EOL);
          }
          InsertString(*out, ")", 0, CG_WITH_EOL);
          InsertString(*out, "{", pos, CG_WITH_EOL);
          pos += 2;
        }
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(info.theRequest);", "WaitTimeMachineService (Machine)",0); 
        if (GetMachineServiceRefList())
        {
          pos -= 2;
          InsertString(*out, "}", pos, CG_WITH_EOL);
        }
      }
      break;

    case DS_SENSOR_MACH_SERVICETIME_REQUEST:
      if (event == cgTraceServiceInterrupt)
      {
        if (GetMachineServiceRefList())
        {
          InsertString(*out, "if (", pos, CG_NO_EOL);
          first = DS_TRUE;
          for (machine_service = GetFirstMachineServiceRef();
               machine_service;
               machine_service = GetNextMachineServiceRef())
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
            InsertString(*out, "info.theRequest->GetRequestType() == ", 0, CG_NO_EOL);
            InsertString(*out, PREFIX_MACHINE_SERVICE_TYPE_VARIABLE, 0, CG_NO_EOL);
            InsertString(*out, machine_service->GetName(), 0, CG_NO_EOL);
          }
          InsertString(*out, ")", 0, CG_WITH_EOL);
          InsertString(*out, "{", pos, CG_WITH_EOL);
          pos += 2;
        }
        InsertString(*out, PREFIX_SENSOR, pos, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, ".Update(info.theRequest);", "ServiceTimeMachineService (Machine)",0); 
        if (GetMachineServiceRefList())
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

DSResult CGSensorMachineService::WriteIdentifier(CGWriter * writer) const
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

DSResult CGSensorMachineService::WriteCreate(CGWriter *writer) const
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
    case DS_SENSOR_SYS_COUNT_REQUEST:
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

DSResult CGSensorMachineService::WriteDestroy(CGWriter *writer) const
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

/************************************************************************
 * Classname: CGSensorProcess
 * Purpose:   Definition eines Sensors
 ************************************************************************/

#include <DS/DSString.h>
#include <DS/DSProcess.h>

#include "CGFile.h"
#include "CGSystem.h"
#include "CGSensorProcess.h"

extern char *sensor_names[];

CGSensorProcess::CGSensorProcess (DSObject* father,
                                  DSSensorType sType,
                                  DSString* sName,
                                  DSProcessRefList * pcs) :
  DSSensorProcess (father, sType, sName, pcs)
{
  // do nothing.
}

CGSensorProcess::CGSensorProcess (DSObject* father,
                                  DSSensorType sType,
                                  DSString& sName,
                                  DSProcessRefList * pcs) :
  DSSensorProcess (father, sType, sName, pcs)
{
  // do nothing.
}

CGSensorProcess::CGSensorProcess (DSObject* father,
                                  DSSensorType sType,
                                  const char * sName,
                                  DSProcessRefList * pcs) :
  DSSensorProcess (father, sType, sName, pcs)
{
  // do nothing.
}

CGSensorProcess::~CGSensorProcess(void)
{
  // do nothing.
}


DSObject* CGSensorProcess::New (DSObject* father) const
{
  return new CGSensorProcess(father);
}

/****************************************************************************
 * Write(): schreibt eine SDL-Sensor
 *          -> writer
 *          -> what
 *          Ergebnis: DS_OK,falls Aktion erfolgreich war, sonst 
 *                    DS_ERROR
 *          Seiteneffekte: Zieldatei wird gefuellt
 ****************************************************************************/
 
DSResult CGSensorProcess::Write(DSWriter *writer, DSCardinal what) const
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

DSResult CGSensorProcess::WriteSensor(CGWriter *writer) const
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

DSResult CGSensorProcess::WriteInit(CGWriter *writer) const
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

    case DS_PROCESS:  // z.Z. keine Sensoren auf Process- oder
    case DS_MACHINE:  // Maschinenebene mit Prozess-Parameter
    default:
      assert(DS_FALSE);
      break;
  }

  return DS_OK;
}


/****************************************************************************
 * WriteSave(): Speicher Sensor
 ****************************************************************************/

DSResult CGSensorProcess::WriteSave(CGWriter * writer) const
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

DSResult CGSensorProcess::WriteRestore(CGWriter * writer) const
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

DSResult CGSensorProcess::WriteSize(CGWriter * writer) const
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

DSResult CGSensorProcess::WriteUpdate(CGWriter * writer) const
{
  DSResult result;
  CGFile  *out;
  CGPos    pos;
  DSCardinal event;
  DSObject *father;
  DSProcessRef process;
  DSBoolean first;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();
  
  father = GetParent();
  assert(father);

  event = writer->CGGetEvent();
  
  switch (GetType())
  {
    case DS_SENSOR_SYS_LIFETIME_PROCESS:
      if (event == cgTraceProcessStop)
      {
        if (GetProcessRefList())
        {
          InsertString(*out, "if (", pos, CG_NO_EOL);
          first = DS_TRUE;
          for (process = GetFirstProcessRef();
               process;
               process = GetNextProcessRef())
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
            InsertString(*out, "((SCProcess *)info.theActiveAutomaton)->GetType() == ", 0, CG_NO_EOL);
            InsertString(*out, PREFIX_PROCESS_TYPE_VARIABLE, 0, CG_NO_EOL);
            InsertString(*out, process->GetName(), 0, CG_NO_EOL);
          }
          InsertString(*out, ")", 0, CG_WITH_EOL);
          InsertString(*out, "{", pos, CG_WITH_EOL);
          pos += 2;
        }
        writer->CGSetIndentPos(pos);
        father->Write(writer, CG_WRITE_IDENTIFIER);
        InsertString(*out, "::", 0, CG_NO_EOL);
        InsertString(*out, PREFIX_SENSOR, 0, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, "->Update((SCProcess *)info.theActiveAutomaton);", "LifeTimeProcess (System)", 0); 
        if (GetProcessRefList())
        {
          pos -= 2;
          InsertString(*out, "}", pos, CG_WITH_EOL);
        }
        writer->CGSetIndentPos(pos);
      }
      break;

    case DS_SENSOR_SYS_COUNT_PROCESS_CREATE:
      if (event == cgTraceProcessCreate)
      {
        if (GetProcessRefList())
        {
          InsertString(*out, "if (", pos, CG_NO_EOL);
          first = DS_TRUE;
          for (process = GetFirstProcessRef();
               process;
               process = GetNextProcessRef())
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
            InsertString(*out, "((SCProcess *)info.thePassiveAutomaton)->GetType() == ", 0, CG_NO_EOL);
            InsertString(*out, PREFIX_PROCESS_TYPE_VARIABLE, 0, CG_NO_EOL);
            InsertString(*out, process->GetName(), 0, CG_NO_EOL);
          }
          InsertString(*out, ")", 0, CG_WITH_EOL);
          InsertString(*out, "{", pos, CG_WITH_EOL);
          pos += 2;
        }
        writer->CGSetIndentPos(pos);
        father->Write(writer, CG_WRITE_IDENTIFIER);
        InsertString(*out, "::", 0, CG_NO_EOL);
        InsertString(*out, PREFIX_SENSOR, 0, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, "->Update();", "CountProcessCreate (System)", 0); 
        if (GetProcessRefList())
        {
          pos -= 2;
          InsertString(*out, "}", pos, CG_WITH_EOL);
        }
        writer->CGSetIndentPos(pos);
      }
      break;

    case DS_SENSOR_SYS_COUNT_PROCESS_STOP:
      if (event == cgTraceProcessStop)
      {
        if (GetProcessRefList())
        {
          InsertString(*out, "if (", pos, CG_NO_EOL);
          first = DS_TRUE;
          for (process = GetFirstProcessRef();
               process;
               process = GetNextProcessRef())
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
            InsertString(*out, "((SCProcess *)info.theActiveAutomaton)->GetType() == ", 0, CG_NO_EOL);
            InsertString(*out, PREFIX_PROCESS_TYPE_VARIABLE, 0, CG_NO_EOL);
            InsertString(*out, process->GetName(), 0, CG_NO_EOL);
          }
          InsertString(*out, ")", 0, CG_WITH_EOL);
          InsertString(*out, "{", pos, CG_WITH_EOL);
          pos += 2;
        }
        writer->CGSetIndentPos(pos);
        father->Write(writer, CG_WRITE_IDENTIFIER);
        InsertString(*out, "::", 0, CG_NO_EOL);
        InsertString(*out, PREFIX_SENSOR, 0, CG_NO_EOL);
        InsertString(*out, GetName(), 0, CG_NO_EOL);
        InsertStringComment(*out, "->Update();", "CountProcessStop (System)", 0); 
        if (GetProcessRefList())
        {
          pos -= 2;
          InsertString(*out, "}", pos, CG_WITH_EOL);
        }
        writer->CGSetIndentPos(pos);
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

DSResult CGSensorProcess::WriteIdentifier(CGWriter * writer) const
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

DSResult CGSensorProcess::WriteCreate(CGWriter *writer) const
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
    case DS_SENSOR_SYS_COUNT_PROCESS_CREATE:
    case DS_SENSOR_SYS_COUNT_PROCESS_STOP:
    case DS_SENSOR_SYS_LIFETIME_PROCESS:
      InsertString(*out, "(\"", 0, CG_NO_EOL);
      InsertString(*out, GetName(), 0, CG_NO_EOL);
      InsertString(*out, "\", (SCDuration)1.0, (SCObject *)NULL);", 0, CG_WITH_EOL);
      break;
    default:
      std::cerr << "Got sensor type '" << sensor_names[GetType()] << "'." << std::endl;
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

DSResult CGSensorProcess::WriteDestroy(CGWriter *writer) const
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

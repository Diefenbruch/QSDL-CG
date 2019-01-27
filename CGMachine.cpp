/****************************************************************************
 *                 Modul: $RCSfile: CGMachine.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/02/09 11:09:56 $
 * $Revision: 1.6 $_
 *
 * Aufgabe: Dieses Modul enthaelt die Funktionen, mit denen der C++-Code aus
 *          der Datenstruktur generiert wird
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
#include <DS/DSSystem.h>
#include <DS/DSExpression.h>

#include "CGFile.h"
#include "CGBlock.h"
#include "CGMachine.h"

#ifdef DEBUG
#include <dmalloc.h>
#endif

/****************************************************************************
 * Externe Variablen
 ****************************************************************************/

extern char *event_names[];
extern CGEventRoles event_roles[];
extern char *event_role_names[];

/****************************************************************************
 * Konstruktoren
 ****************************************************************************/

CGMachine::CGMachine(DSObject *father, DSString *mach_name, DSExpression *exp1,
                     DSExpression *exp2) : DSMachine(father, mach_name, exp1, exp2)
{
}

CGMachine::CGMachine(DSObject *father, DSString &mach_name, DSExpression *exp1,
                     DSExpression *exp2) : DSMachine(father, mach_name, exp1, exp2)
{
}

CGMachine::CGMachine(DSObject *father, const char *mach_name, DSExpression *exp1,
                     DSExpression *exp2) : DSMachine(father, mach_name, exp1, exp2)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGMachine::~CGMachine(void)
{
}

/****************************************************************************
 * Virtueller Konstruktor
 ****************************************************************************/

DSObject *CGMachine::New(DSObject *father) const
{
  return new CGMachine(father);
}

/****************************************************************************
 * Write: Oberfunktion der Codegenerierung fuer Maschinen
 *        -> writer
 *        -> what
 *        Ergebnis: DS_OK, falls alles geklappt hat,sonst DS_ERROR
 *        Seiteneffekte: Fuellen der Files
 ****************************************************************************/

DSResult CGMachine::Write(DSWriter *writer, DSCardinal what) const
{
  DSResult result;
  
  switch (what)
  {
    case CG_WRITE_H:
      result = WriteHeader((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;

    case CG_WRITE_CC:
      result = WriteImplementation((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
            
    case CG_WRITE_INIT:
      result = WriteInit((CGWriter *)writer);
      return (result == DS_OK) ? DS_OK : DS_ERROR;
      break;
      
    case CG_WRITE_ENUM:
      result = WriteEnum((CGWriter *)writer);
      return (result == DS_OK) ? DS_OK : DS_ERROR;
      break;
      
    case CG_WRITE_SPEED_H:
      result = WriteSpeedHeader((CGWriter *)writer);
      return (result == DS_OK) ? DS_OK : DS_ERROR;
      break;
      
    case CG_WRITE_SPEED_CC:
      result = WriteSpeedImplementation((CGWriter *)writer);
      return (result == DS_OK) ? DS_OK : DS_ERROR;
      break;
      
    case CG_WRITE_CREATE:
      result = WriteCreate((CGWriter *)writer);
      return (result == DS_OK) ? DS_OK : DS_ERROR;
      break;
      
    case CG_WRITE_DEFINITION:
      result = WriteDefinition((CGWriter *)writer);
      return (result == DS_OK) ? DS_OK : DS_ERROR;
      break;
      
    case CG_WRITE_SCHEDULE:
      result = WriteSchedule((CGWriter *)writer);
      return (result == DS_OK) ? DS_OK : DS_ERROR;
      break;
      
    case CG_WRITE_IDENTIFIER:
      result = WriteIdentifier((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
      
    case CG_WRITE_INCLUDE:
      result = WriteInclude((CGWriter *)writer);
      if (result != DS_OK) return DS_ERROR;
      break;
      
    default:
      assert(DS_FALSE);
      return DS_ERROR;
      break;
  }
  return DS_OK;
}        
 
/****************************************************************************
 * WriteHeader(): Oberfunktion der Generierung des h-Files fuer Maschinen.
 *                Wird nur aufgerufen wenn Sensor-Tracing aktiviert ist.
 *                -> writer
 *                Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                Seiteneffekte: Anlegen und Fuellen der .h-Files
 ****************************************************************************/

DSResult CGMachine::WriteHeader(CGWriter *writer) const
{
  DSResult result;
  DSString str;
//  DSSystemRef system;
  CGBlock *block;
  DSString filename;

  block = (CGBlock *)GetThisBlock();
  assert(block);

//  system = GetThisSystem();

  filename = GetIdentifier();
  if (writer->CGIncrementalCodeGeneration())
  {
    filename += SUFFIX_H_INC;
  }
  else
  {
    filename += SUFFIX_H;
  }

  CGFile h_file(writer->CGGetOutputDir(),
                &filename, writer->CGPrettyPrinting());

  if (!h_file.CGFileOpen())
  {
    std::cerr << "Can't open " << h_file.CGFilePath()
         << h_file.CGFileName() << std::endl;
    return DS_ERROR;
  }
  writer->CGSetFilePointer(&h_file);

  InsertCopyright(h_file);

  filename.ToUpper();

  InsertString(h_file, "#ifndef _", 0, CG_NO_EOL);
  InsertString(h_file, filename, 0, CG_NO_EOL);
  InsertString(h_file,"_" , 0, CG_WITH_EOL);
  InsertString(h_file, "#define _", 0, CG_NO_EOL);
  InsertString(h_file, filename, 0, CG_NO_EOL);
  InsertString(h_file,"_" , 0, CG_WITH_EOL);
  InsertLine(h_file);

  InsertHead(h_file, "Includes", 0);
  block->Write(writer, CG_WRITE_INCLUDE);
  InsertLine(h_file);

  InsertIncludeSCL(h_file, "SCMachine.h", 0, CG_WITH_EOL);
  if (GetSensorList())
  {
    InsertIncludeSCL(h_file, "SCTraceSensor.h", 0, CG_WITH_EOL);
    InsertIncludeSCL(h_file, "SCSensorMachine.h", 0, CG_WITH_EOL);
  }
  InsertLine(h_file);

  str.Format("Maschinenklasse der QSDL-Maschine '%s'",
             GetName()->GetString());
  InsertHead(h_file, str, 0);
  InsertString(h_file, "class ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(h_file, " : public SCMachine", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = GetDiscipline()->Write(writer);
  if (result != DS_OK) return result;
  InsertString(h_file, ", public ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  block->WriteIdentifier(writer);
  InsertLine(h_file);
  InsertString(h_file, "{", 0, CG_WITH_EOL);

  InsertString(h_file, "public:", 2, CG_WITH_EOL);
  InsertLine(h_file);

  /********* Konstruktoren der Maschinenklasse *******************************/

  InsertHead(h_file, "Konstruktoren", 4);
  writer->CGSetIndentPos(4);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(h_file, "(SCRunnableID, SCNatural, SCNatural", 0, CG_NO_EOL); 
  InsertString(h_file, ", const SCDuration *const, const char *", 0, CG_NO_EOL);
  InsertString(h_file, ", SCObject * = NULL);", 0, CG_WITH_EOL); 
  InsertLine(h_file);

  /********* Destruktor der Prozessklasse **********************************/

  InsertHead(h_file, "Destruktor", 4);
  InsertString(h_file, "~", 4, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(h_file, "(void);", 0, CG_WITH_EOL); 
  InsertLine(h_file);

  /********* Save-/Restore-Funktion **********************************/

  InsertHead(h_file, "Virtuelle Save- & Restore-Methoden", 4);
  InsertString(h_file, "SCBoolean Save(SCMem &) const;", 4, CG_WITH_EOL); 
  InsertString(h_file, "SCBoolean Restore(SCMem &);", 4, CG_WITH_EOL); 
  InsertLine(h_file);
  InsertHead(h_file, "Virtuelle Size-Methode", 4);
  InsertString(h_file, "void Size(SCSize *) const;", 4, CG_WITH_EOL); 
  InsertLine(h_file);
  InsertHead(h_file, "Display-Methoden", 4);
  InsertString(h_file, "friend SCStream& operator<<(SCStream &, const ", 4, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(h_file, " &);", 0, CG_WITH_EOL);
  InsertLine(h_file);

  if (GetSensorList())
  {
    InsertHead(h_file, "SensorUpdate-Methode", 4);
    InsertString(h_file,
                 "void SensorUpdate(const SCInteger, const SCTraceSensorRole, const struct SCTraceSensorUpdate &);",
                 4, CG_WITH_EOL); 
    InsertLine(h_file);
  }

  /******** Sensoren *************************************************/

  if (GetSensorList())
  {
    InsertHead(h_file, "Sensoren der Maschine", 4);
    writer->CGSetIndentPos(4);
    result = Run(writer, DS_SENSOR, CG_WRITE_H);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(h_file);
  }

  InsertString(h_file, "};", 0, CG_WITH_EOL);
  InsertLine(h_file);
  InsertString(h_file, "#endif", 0, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * WriteImplementation(): Oberfunktion der der Generierung des cc-Files 
 *                        fuer Maschinen
 *                       -> writer
 *                       Ergebnis: DS_OK, falls alles geklappt hat, sonst 
 *                       DS_ERROR
 *                       Seiteneffekte: Anlegen und Fuellen der .cc-Files
 ****************************************************************************/

DSResult CGMachine::WriteImplementation(CGWriter *writer) const
{
  DSResult result;
  CGBlock *block;
  DSString str;
  DSString filename;

  block = (CGBlock *)GetThisBlock();
  assert(block);

  filename = GetIdentifier();
  if (writer->CGIncrementalCodeGeneration())
  {
    filename += SUFFIX_C_INC;
  }
  else
  {
    filename += SUFFIX_C;
  }

  CGFile c_file(writer->CGGetOutputDir(), &filename,
                writer->CGPrettyPrinting());

  if (!c_file.CGFileOpen())                
  {
    std::cerr << "Can't open " << c_file.CGFilePath() 
         << c_file.CGFileName() << std::endl;
    return DS_ERROR;
  }
  writer->CGSetFilePointer(&c_file);

  InsertCopyright(c_file);
  InsertHead(c_file, "Includes", 0);

  block->WriteInclude(writer);  
  WriteInclude(writer);
  InsertLine(c_file);

  if (writer->CGDebug())
  {
    str.Format("#include <%s>", CG_DEBUG_INCLUDE);
    InsertString(c_file, str, 0, CG_WITH_EOL);
    InsertLine(c_file);
  }

  /********* Konstruktoren der Maschinenklasse *******************************/
  
  InsertHead(c_file, "Konstruktoren", 0);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(c_file, "::", 0, CG_NO_EOL); 
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(c_file, "(SCRunnableID thisMachine, SCNatural numOfServers,", 0, CG_NO_EOL); 
  InsertString(c_file, " SCNatural numOfServices, const SCDuration *const speedVector, const char *name", 0, CG_NO_EOL); 
  InsertString(c_file, ", SCObject *father) : ", 0, CG_WITH_EOL);
  InsertString(c_file, "SCMachine", 2, CG_NO_EOL); 
  writer->CGSetIndentPos(0);
  result = GetDiscipline()->Write(writer);
  if (result != DS_OK) return result;
  InsertString(c_file, "(thisMachine, numOfServers, numOfServices, speedVector, name", 0, CG_NO_EOL); 
  InsertString(c_file, ", father)", 0, CG_NO_EOL);

  // Sensoren initialisieren:

  if (GetSensorList())
  {
    writer->CGSetIndentPos(2);
    result = Run(writer, DS_SENSOR, CG_WRITE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
  }
  InsertLine(c_file);

  InsertString(c_file, "{", 0, CG_WITH_EOL);

  InsertString(c_file, "#if _SC_VALIDATION_OPTIMIZE", 0, CG_WITH_EOL);
  InsertHead(c_file, "Ausgangsgroesse der Maschine initialisieren:", 2);
  InsertString(c_file, "SCSize my_size;", 2, CG_WITH_EOL);
  InsertLine(c_file);
  InsertString(c_file, "Size(&my_size);", 2, CG_WITH_EOL);
  InsertString(c_file, "SetCurrentSize(my_size.size);", 2, CG_WITH_EOL); 
  InsertString(c_file, "SetCurrentHistorySize(my_size.historySize);", 2, CG_WITH_EOL); 
  InsertString(c_file, "#endif", 0, CG_WITH_EOL);
  InsertLine(c_file);

  InsertString(c_file, "}", 0, CG_WITH_EOL);
  InsertLine(c_file);


  /********* Destruktor der Maschinenklasse **********************************/
  
  InsertHead(c_file, "Destruktor", 0);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(c_file, "::~", 0, CG_NO_EOL); 
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(c_file, "(void)", 0, CG_WITH_EOL); 
  InsertString(c_file, "{", 0, CG_WITH_EOL); 
  InsertString(c_file, "}", 0, CG_WITH_EOL); 
  InsertLine(c_file);

  writer->CGSetIndentPos(0);
  InsertLine(c_file);  
  InsertHead(c_file, "Save-Methode zur Speicherung des Maschinenzustands", 0);
  result = WriteSave(writer);
  if (result != DS_OK) return result;

  InsertLine(c_file);  
  InsertHead(c_file, "Restore-Methode zur Restaurierung des Maschinenzustands", 0);
  result = WriteRestore(writer);
  if (result != DS_OK) return result;
    
  InsertLine(c_file);  
  InsertHead(c_file, "Size-Methode liefert benoetigten Speicherplatz", 0);
  result = WriteSize(writer);
  if (result != DS_OK) return result;
    
  InsertLine(c_file);  
  InsertHead(c_file, "Display-Methode", 0);
  result = WriteStreamOperator(writer);
  if (result != DS_OK) return result;

  if (GetSensorList())
  {
    InsertLine(c_file);  
    InsertHead(c_file, "SensorUpdate-Methode", 0);
    result = WriteSensorUpdate(writer);
    if (result != DS_OK) return result;
  }

  return DS_OK;
}

/****************************************************************************
 * WriteInit(): Initialisiert die Maschinen, um sie fuer die SCL nutzbar
 *              zu machen
 *              -> writer
 *              Ergebnis: DS_OK, falls alles geklappt hat,sonst DS_ERROR
 *              Seiteneffekte: Fuellen des .h-Files
 ****************************************************************************/

DSResult CGMachine::WriteInit(CGWriter *writer) const
{
  DSResult result;
  DSString str;
  CGFile *out;
  CGPos pos;
  DSCardinal i;
  DSObject *father;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  father = GetParent();
  assert(father);

  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  InsertString(*out, " * ", 0, CG_NO_EOL);
  father->Write(writer, CG_WRITE_IDENTIFIER);
  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_MACHINE, 0, CG_NO_EOL); // hier NICHT
  InsertString(*out, GetName(), 0, CG_NO_EOL);      // WriteIdentifier()
                                                    // verwenden!
  InsertString(*out, " = NULL;", 0, CG_WITH_EOL);

  // Geschwindigkeiten initialisieren:
  InsertString(*out, "SCReal ", pos, CG_NO_EOL);
  father->Write(writer, CG_WRITE_IDENTIFIER);
  InsertString(*out, "::", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_MACHINE_SPEED, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "[", 0, CG_NO_EOL);
  str.Format("%d", writer->CGGetLastServiceID());
  InsertString(*out, str, 0, CG_NO_EOL);
  InsertString(*out, "] = {", 0, CG_NO_EOL);
  for (i = 0; i < writer->CGGetLastServiceID(); i++)
  {
    if (i != 0) InsertString(*out, ", ", 0, CG_NO_EOL);
    InsertString(*out, "-1", 0, CG_NO_EOL);
  }
  InsertString(*out, "};", 0, CG_WITH_EOL);
  writer->CGSetIndentPos(pos);

  return DS_OK;
}

/****************************************************************************
 * WriteCreate(): Erzeugt die SCL-Maschinenobjekte.
 *                -> writer
 *                Ergebnis: DS_OK, falls alles geklappt hat,sonst DS_ERROR
 *                Seiteneffekte: Fuellen des .cc-Files
 ****************************************************************************/

DSResult CGMachine::WriteCreate(CGWriter *writer) const
{
  DSResult result;
  DSString str;
  CGFile *out;
  CGPos pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, PREFIX_MACHINE, pos, CG_NO_EOL); // hier NICHT
  InsertString(*out, GetName(), 0, CG_NO_EOL);        // WriteIdentifier()
                                                      // verwenden!
  InsertString(*out, " = new ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  InsertString(*out, "(", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_MACHINE_ID, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, ", ", 0, CG_NO_EOL);
  result = GetNumberOfServers()->Write(writer);
  if (result != DS_OK) return result;
  str.Format(", %d, ", writer->CGGetLastServiceID());
  InsertString(*out, str, 0, CG_NO_EOL); 
  InsertString(*out, PREFIX_MACHINE_SPEED, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, ", \"", 0 ,CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "\"", 0 ,CG_NO_EOL);
  InsertString(*out, ");", 0 ,CG_WITH_EOL);
  InsertString(*out, "assert(", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_MACHINE, 0, CG_NO_EOL);   // hier NICHT
  InsertString(*out, GetName(), 0, CG_NO_EOL);        // WriteIdentifier()
                                                      // verwenden!
  InsertString(*out, ");", 0, CG_WITH_EOL);
  writer->CGSetIndentPos(pos);

  return DS_OK;
}

/****************************************************************************
 * WriteSpeedHeader(): Schreiben der Maschinengeschwindigkeiten
 *                     -> writer
 *                     Ergebnis: DS_OK, falls alles geklappt hat, 
 *                     sonst DS_ERROR
 *                     Seiteneffekte: Anlegen und Fuellen der .h-Files
 ****************************************************************************/

DSResult CGMachine::WriteSpeedHeader(CGWriter *writer) const
{
  DSResult result;
  DSString str;
  CGFile *out;
  CGPos pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "static SCReal ", pos, CG_NO_EOL);
  InsertString(*out, PREFIX_MACHINE_SPEED, 0, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);
  InsertString(*out, "[", 0, CG_NO_EOL);
  str.Format("%d", writer->CGGetLastServiceID());
  InsertString(*out, str, 0, CG_NO_EOL);
  InsertString(*out, "];", 0, CG_WITH_EOL);

  return DS_OK;         
} 

/****************************************************************************
 * WriteSpeedImplementation(): Schreiben der Maschinengeschwindigkeiten
 *                             -> writer
 *                             Ergebnis: DS_OK, falls alles geklappt hat, 
 *                                       sonst DS_ERROR
 *                             Seiteneffekte: Anlegen und Fuellen der .cc-Files
 ****************************************************************************/

DSResult CGMachine::WriteSpeedImplementation(CGWriter *writer) const
{
  DSResult result;

  /****** nur die Felder des Maschinen-Arrays werden belegt, die ueber
          den Enum-Wert des Services angesprochen werden duerfen,
          die anderen Dienste erhalten -1 als Geschwindigkeit **********/

  result = Run(writer, DS_MACHINESERVICESPEED);

  return result;         
} 

/****************************************************************************
 * WriteEnum(): Schreiben der MaschinenIDs in Aufzaehlungstyp
 *              -> writer
 *              Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *              Seiteneffekte: Anlegen und Fuellen der .h-Files
 ****************************************************************************/
 
DSResult CGMachine::WriteEnum(CGWriter *writer) const
{
  DSResult  result;
  DSBlock  *block;
  DSString *tmp_str;
  CGFile   *out;
  CGPos     pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  block = (CGBlock *)GetThisBlock();
  assert(block);

  if (block->GetMachineList()->AtStartOfList())
  {
    InsertString(*out, "enum", pos, CG_WITH_EOL);
    InsertString(*out, "{", pos, CG_WITH_EOL);
  }

  InsertString(*out, PREFIX_MACHINE_ID, pos + 2, CG_NO_EOL);
  InsertString(*out, GetName(), 0, CG_NO_EOL);

  InsertString(*out, " = kSCMachineIDBase - ",0, CG_NO_EOL);
  tmp_str = new DSString(writer->CGGetLastMachineID());
  InsertString(*out, tmp_str, 0, CG_NO_EOL);
  delete tmp_str;
  writer->CGSetLastMachineID(writer->CGGetLastMachineID() + 1);

  if (!block->GetMachineList()->AtEndOfList())
  {
    InsertString(*out, ",", 0, CG_WITH_EOL);
  }
  else
  {
    InsertString(*out, "", 0, CG_WITH_EOL);
    InsertString(*out, "};", pos, CG_WITH_EOL);
  }

  return DS_OK;
}

/****************************************************************************
 * WriteDefinition(): Schreiben der Maschinentypdefinitionen
 *                    -> writer
 *                    Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                    Seiteneffekte: Anlegen und Fuellen der .h-Files
 ****************************************************************************/
 
DSResult CGMachine::WriteDefinition(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();
     
  InsertString(*out, "static class ", pos, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  WriteIdentifier(writer);
  writer->CGSetIndentPos(pos);
  if (result != DS_OK) return result;
  InsertString(*out, " *", 0, CG_NO_EOL);
  InsertString(*out, PREFIX_MACHINE, 0, CG_NO_EOL); // hier NICHT
  InsertString(*out, GetName(), 0, CG_NO_EOL);      // WriteIdentifier()
                                                    // verwenden!
  InsertString(*out, ";", 0, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * WriteSchedule(): Einhaengen der Maschinen in den SCScheduler
 *                  -> writer
 *                  Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                  Seiteneffekte: Anlegen und Fuellen der cc-Files
 ****************************************************************************/
 
DSResult CGMachine::WriteSchedule(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();
     
  InsertString(*out, CG_NAME_SCL_SCHEDULER, pos, CG_NO_EOL);
  InsertString(*out, "::Schedule(", 0, CG_NO_EOL); 
  InsertString(*out, PREFIX_MACHINE, 0, CG_NO_EOL); // hier NICHT
  InsertString(*out, GetName(), 0, CG_NO_EOL);      // WriteIdentifier()
                                                    // verwenden!
  InsertString(*out, ");", 0 , CG_WITH_EOL); 

  return DS_OK;
}


/****************************************************************************
 * GetIdentifier(): Liefert den vollen Namen des Prozesses (mit Prefix)
 *                      Ergebnis: Referenz auf DSString-Objekt
 *                      Seiteneffekte: keine
 ****************************************************************************/
 
DSString & CGMachine::GetIdentifier(void) const
{
  DSString * full_name;
  DSObject * father;
  
  full_name = new DSString;
  assert(full_name);

  father = GetParent();
  
  while (father)
  {
    switch(father->GetType())
    {
      case DS_SYSTEM:
        break;
      case DS_BLOCK:
        *full_name = *((DSBlock *)father)->GetName() + "_" + *full_name;
        break;
      case DS_BLOCKSUBSTRUCTURE:
        *full_name = *((DSBlockSubstructure *)father)->GetName() + "_" + *full_name;
        break;
    default:
      std::cerr << "Unexpected type of parent object for CGMachine !" << std::endl;
      assert(DS_FALSE);
    }
    father = father->GetParent();
  }
  
  *full_name = PREFIX_MACHINE + *full_name;

  assert(GetName());    
  *full_name += *GetName();
  
  return *full_name;    
}


/****************************************************************************
 * WriteIdentifier(): Schreiben des Maschinen-Namens (mit notwendigen Prefixen)
 *                    -> writer
 *                    Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                    Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGMachine::WriteIdentifier(CGWriter *writer) const
{  
  DSString full_name;
  DSResult result;
  CGFile *out;
  CGPos pos;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();

  full_name = GetIdentifier();
  InsertString(*out, full_name, pos, CG_NO_EOL);

  return DS_OK;
}


/****************************************************************************
 * WriteSave(): Speicherung aller relevanten Maschinendaten
 *             -> writer
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *             Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGMachine::WriteSave(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;
  DSString str;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean ", pos, CG_NO_EOL);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "::Save(SCMem &save_area) const", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertHead(*out, "Speicherung der Daten der Basisklasse", pos + 2);
  InsertString(*out, "SCMachine", pos + 2, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = GetDiscipline()->Write(writer);
  writer->CGSetIndentPos(pos);
  if (result != DS_OK) return result;
  InsertString(*out, "::Save(save_area);", 0, CG_WITH_EOL);
  InsertLine(*out);
  writer->CGSetIndentPos(pos + 2);
  if (GetSensorList())
  {
    InsertHead(*out, "Sicherung der Sensoren:", pos + 2);
    result = Run(writer, DS_SENSOR, CG_WRITE_SAVE);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }
  InsertString(*out, "return true;", pos + 2, CG_WITH_EOL); 
  InsertString(*out, "}", pos, CG_WITH_EOL);
  writer->CGSetIndentPos(pos);

  return DS_OK;
}

/****************************************************************************
 * WriteRestore(): Restaurierung aller relevanten Prozessdaten
 *                 -> writer
 *                 Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                 Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGMachine::WriteRestore(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;
  DSString str;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCBoolean ", pos, CG_NO_EOL);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "::Restore(SCMem &save_area)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertHead(*out, "Restaurierung der Daten der Basisklasse", pos + 2);
  InsertString(*out, "SCMachine", pos + 2, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = GetDiscipline()->Write(writer);
  writer->CGSetIndentPos(pos);
  if (result != DS_OK) return result;
  InsertString(*out, "::Restore(save_area);", 0, CG_WITH_EOL);
  InsertLine(*out);
  writer->CGSetIndentPos(pos + 2);
  if (GetSensorList())
  {
    InsertHead(*out, "Restaurierung der Sensoren:", pos + 2);
    result = Run(writer, DS_SENSOR, CG_WRITE_RESTORE);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }
  InsertString(*out, "return true;", pos + 2, CG_WITH_EOL); 
  InsertString(*out, "}", pos, CG_WITH_EOL);
  writer->CGSetIndentPos(pos);

  return DS_OK;
}

/****************************************************************************
 * WriteSize(): Berechnet benoetigen Speicherplatz fuer Save und Restore
 *              -> writer
 *              Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *              Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGMachine::WriteSize(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos pos;
  DSString str;

  out = writer->CGGetFilePointer();
  assert (out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "void ", pos, CG_NO_EOL);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, "::Size(SCSize *cur_size) const", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
/*  InsertString(*out, "SCNatural size = cur_size->size, historySize = cur_size->historySize;", pos + 2, CG_WITH_EOL);
  InsertLine(*out);
  InsertString(*out, "#if _SC_VALIDATION_OPTIMIZE", 0, CG_WITH_EOL);
  InsertString(*out, "if (!IsModified())", pos + 2, CG_WITH_EOL); 
  InsertString(*out, "{", pos + 2, CG_WITH_EOL);
  InsertString(*out, "cur_size->size += GetCurrentSize();", pos + 4, CG_WITH_EOL); 
  InsertString(*out, "cur_size->historySize += GetCurrentHistorySize();", pos + 4, CG_WITH_EOL); 
  InsertString(*out, "return;", pos + 4, CG_WITH_EOL); 
  InsertString(*out, "}", pos + 2, CG_WITH_EOL);
  InsertString(*out, "#endif", 0, CG_WITH_EOL);
  InsertLine(*out); */
  InsertHead(*out, "Berechnung der Groesse der Basisklasse", pos + 2);
  InsertString(*out, "SCMachine", pos + 2, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = GetDiscipline()->Write(writer);
  writer->CGSetIndentPos(pos);
  if (result != DS_OK) return result;
  InsertString(*out, "::Size(cur_size);", 0, CG_WITH_EOL);

  InsertLine(*out);
  writer->CGSetIndentPos(pos + 2);
  if (GetSensorList())
  {
    InsertHead(*out, "Berechnung der Sensorgroessen:", pos + 2);
    result = Run(writer, DS_SENSOR, CG_WRITE_SIZE);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(*out);
  }
  InsertString(*out, "}", pos, CG_WITH_EOL);
  writer->CGSetIndentPos(pos);

  return DS_OK;
}

DSResult CGMachine::WriteDisplay(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCStream& ", pos, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  writer->CGSetIndentPos(pos);
  InsertString(*out, "::Display(SCStream &out) const", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);

  InsertString(*out, "return (out << *this);", pos + 2, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGMachine::WriteStreamOperator(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSString str;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertString(*out, "SCStream& ", pos, CG_NO_EOL);
  InsertString(*out, "operator<<(SCStream &out, const ", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  writer->CGSetIndentPos(pos);
  InsertString(*out, " &me)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "return me.Display(out);", pos + 2, CG_WITH_EOL);

  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

DSResult CGMachine::WriteSensorUpdate(CGWriter *writer) const
{ 
  DSResult     result;
  CGFile      *out;
  CGPos        pos;
  DSCardinal   i;
  DSSystem    *system;
  DSString     str;
  
  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  system = (DSSystem *)GetThisSystem();
  assert(system);

  InsertString(*out, "void ", pos, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  writer->CGSetIndentPos(pos);
  InsertString(*out, "::SensorUpdate(const SCInteger action, ", 0, CG_NO_EOL);
  InsertString(*out, "const SCTraceSensorRole role, ", 0, CG_NO_EOL);
  InsertString(*out, "const struct SCTraceSensorUpdate &info)", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "switch (action)", pos + 2, CG_WITH_EOL);
  InsertString(*out, "{", pos + 2, CG_WITH_EOL);

  // fuer jeden Event werden alle Sensor-Listen durchlaufen:
  for (i = cgTraceSchedulerInit;
       i < cgTraceMax;
       i++)
  {
    InsertString(*out, "case ", pos + 4, CG_NO_EOL);
    InsertString(*out, event_names[i], 0, CG_NO_EOL);
    InsertString(*out, ":", 0, CG_WITH_EOL); 
    if (event_roles[i] != cgTSRoleNone) // ist die Rolle wichtig ?
    {
      // Zusaetzliche if-Abfrage generieren um die richtige
      // Rolle des Prozesses zu gewaehrleisten:
      str.Format("if (role == %s)", event_role_names[event_roles[i]]);
      InsertString(*out, str, pos + 6, CG_WITH_EOL);
      InsertString(*out, "{", pos + 6, CG_WITH_EOL);
      pos += 2;
    }
    writer->CGSetIndentPos(pos + 6);
    writer->CGSetEvent(i);
    system->Run(writer, DS_SENSOR, CG_WRITE_UPDATE);  // System-Sensoren updaten
    Run(writer, DS_SENSOR, CG_WRITE_UPDATE);          // Prozess-Sensoren updaten
    if (event_roles[i] != cgTSRoleNone) // ist die Rolle wichtig ?
    {
      pos -= 2;
      InsertString(*out, "}", pos + 6, CG_WITH_EOL);
    }
    writer->CGSetIndentPos(pos);
    InsertString(*out, "break;", pos + 6, CG_WITH_EOL);
  } 
  InsertString(*out, "default:", pos + 4, CG_WITH_EOL);
  InsertString(*out, "break;", pos + 6, CG_WITH_EOL);
  InsertString(*out, "}", pos + 2, CG_WITH_EOL);
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * WriteInclude(): Schreiben eines Prozessincludes
 *             -> writer
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *             Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/
 
DSResult CGMachine::WriteInclude(CGWriter *writer) const
{  
  DSResult result;
  CGFile *out;

  out = writer->CGGetFilePointer();
  assert(out);

  InsertString(*out, "#include \"", 0, CG_NO_EOL);
  writer->CGSetIndentPos(0);
  result = Write(writer, CG_WRITE_IDENTIFIER);
  if (result != DS_OK) return result;
  InsertString(*out, SUFFIX_H, 0, CG_NO_EOL);
  InsertString(*out, "\"", 0, CG_WITH_EOL);
  return DS_OK;
}

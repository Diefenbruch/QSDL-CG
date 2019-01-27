/****************************************************************************
 *                 Modul: $RCSfile: CGWriter.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/02/13 06:51:41 $
 * $Revision: 1.9 $
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

#define NUM_OF_COLOURS 16

/****************************************************************************
 * Include-Anweisungen
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#ifndef _MSC_VER

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>

#else

#include <direct.h>

#endif // _MSC_VER

#include <assert.h>
#include <errno.h>

#include <DS/DSString.h>
#include <DS/DSMachine.h>
#include <DS/DSExpression.h>
#include <DS/DSSyntype.h>
#include <DS/DSGenerator.h>
#include <DS/DSOperandConstant.h>
#include <DS/DSMachineService.h>

#ifdef _MSC_VER

#include <DS/DSState.h>
#include <DS/DSMachineServiceSpeed.h>

#endif // _MSC_VER

#include "CGWriter.h"
#include "CGFile.h"
#include "CGSystem.h"
#include "CGBlock.h"
#include "CGProcess.h"
#include "CGProcedure.h"
#include "CGMake.h"
#include "CGMain.h"
#include "CGSort.h"
#ifdef DEBUG
#include <dmalloc.h>
#endif

/****************************************************************************
 * Globale Variablen
 ****************************************************************************/

static const char *display_colours[NUM_OF_COLOURS] =
{
  "blue",
  "red",
  "green",
  "brown",
  "DarkKhaki",
  "MediumSeaGreen",
  "CornflowerBlue",
  "indianred",
  "DarkSlateBlue",
  "Purple",
  "MidnightBlue",
  "MediumTurquoise",
  "DarkOliveGreen",
  "DarkOrange",
  "RosyBrown",
  "MediumVioletRed"
};

/****************************************************************************
 * Konstruktoren
 ****************************************************************************/

CGWriter::CGWriter(DSSystem *sys,
                   DSStream &myCout,
                   DSBoolean makefile,
                   DSBoolean templatefile,
                   DSBoolean experimentfile,
                   DSBoolean verb,
                   DSBoolean pretty,
                   DSBoolean informal_text,
                   DSBoolean increment_code,
                   DSBoolean sup_pev,
                   DSBoolean debug) :
  system(sys),
  cgCout(myCout),
  generate_makefile(makefile),
  generate_templatefile(templatefile),
  generate_experimentfile(experimentfile),
  verbose(verb),
  pretty_printing(pretty),
  generate_informal_text(informal_text),
  incremental_code_generation(increment_code),
  support_pev(sup_pev),
  generate_debug(debug)
{
  assert(system);
  assert(system->GetName());

  output_dir = NULL;

  last_signal_id = 0;
  last_process_id = 0;
  last_procedure_id = 0;
  last_machine_id = 0;
  last_service_id = 0;
  last_state_id = 0;
  last_transition_id = 0;
  last_request_id = 0;
  last_call_id = 0;
  last_atom_id = 0;
  machine_service_list = new DSMachineServiceRefList;
  assert(machine_service_list);
  label_list = NULL;
  real_state = NULL;
  trace_event = 0;
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGWriter::~CGWriter(void)
{
  if (output_dir) delete output_dir;
  if (label_list) delete label_list;
  if (machine_service_list) delete machine_service_list;
}

/****************************************************************************
 * CGWrite(): Oberfunktion der Codegenerierung
 *               Ergebnis: DS_OK, falls alles geklappt hat, sonst enthaelt
 *               DS_RESULT den aufgetretenen Fehler
 *               Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/

DSResult CGWriter::CGWrite(void)
{
  DSResult result;

  if (verbose)
    cgCout << "Evaluating Environment..." << std::endl;
  result = CGEvaluateEnvironment();
  if (result != DS_OK)
  {
    return DS_ERROR;
  }

  if (verbose)
    cgCout << "Generating System Files..." << std::endl;
  result = CGWriteSystemFiles();
  if (result != DS_OK)
  {
    return DS_ERROR;
  }

  if (verbose)
    cgCout << "Generating Main File..." << std::endl;
  result = CGWriteMainFile();
  if (result != DS_OK)
  {
    return DS_ERROR;
  }       

  if (generate_templatefile)
  {
    if (verbose)
      cgCout << "Generating Template File..." << std::endl;
    result = CGWriteTemplateFile();
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
  }

  if (generate_makefile)
  {
    if (verbose)
      cgCout << "Generating Makefile..." << std::endl;
    result = CGWriteMakeFile();
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
  }

  if (generate_experimentfile && support_pev)
  {
    if (verbose)
      cgCout << "Generating Experiment File..." << std::endl;
    result = CGWriteExperimentFile();
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
  }
  return DS_OK;
}
  
/****************************************************************************
 * CGWriteSystemFiles(): erzeugt und fuellt <system>.h, <system.cc>
 *                       Ergebnis: CG_OK, falls alles geklappt hat,sonst enthaelt
 *                                 CG_RESULT den aufgetretenen Fehler
 *                       Seiteneffekte: Anlegen und Fuellen der Systemfiles
 ****************************************************************************/

DSResult CGWriter::CGWriteSystemFiles(void)
{
  DSResult result;
      
  assert(system);

  result = system->Write(this, CG_WRITE_H);
  if (result != DS_OK)
  {
    return result;
  }
  result = system->Write(this, CG_WRITE_CC);
  if (result != DS_OK)
  {
    return result;
  }

  result = CGWriteBlockFiles(system->GetBlockList());
  
  return result;
}

/****************************************************************************
 * CGWriteBlockFiles(): erzeugt und fuellt <block>.h, <block.cc>
 *                      Ergebnis: CG_OK, falls alles geklappt hat,sonst enthaelt
 *                      CG_RESULT den aufgetretenen Fehler
 *                      Seiteneffekte: Anlegen und Fuellen der Blockfiles
 ****************************************************************************/

DSResult CGWriter::CGWriteBlockFiles(DSBlockKeyList *block_list)
{
  DSResult result;
  DSResult status;
  DSBlock *block;
  DSBlockSubstructure *substructure;
  DSBlock *save_pointer_blk;

  if (block_list == NULL)
    return DS_OK;
    
  /******** Erzeugen der Blockdateien ****************************************/
  
  for (status = block_list->MoveFirst();
       status == DS_OK;
       status = block_list->MoveNext())
  {
    block = block_list->GetCurrentData(); 
    assert(block);
    
    save_pointer_blk = block;

    if (verbose)
      cgCout << "  Block " << block->GetName() << "...";
    result = block->Write(this, CG_WRITE_H);
    if (result != DS_OK) return result;
    result = block->Write(this, CG_WRITE_CC);
    if (result != DS_OK) return result;
    if (verbose)
      cgCout << " done." << std::endl;

    substructure = block->GetBlockSubstructure();
    
    if (substructure)
    {
      if (verbose)
        cgCout << "  Block Substructure " << substructure->GetName() << "...";
      result = substructure->Write(this, CG_WRITE_H);
      if (result != DS_OK) return result;
      result = substructure->Write(this, CG_WRITE_CC);
      if (result != DS_OK) return result;
      if (verbose)
        cgCout << " done." << std::endl;
    }
    
    result = CGWriteProcessFiles(block->GetProcessList());
    if (result != DS_OK) return result;
    
    result = CGWriteProcedureFiles(block->GetProcedureList());
    if (result != DS_OK) return result;

    result = CGWriteMachineFiles(block->GetMachineList());
    if (result != DS_OK) return result;

    // Rekursiv weitersuchen...      
    if (block->GetBlockSubstructure())
    {
      CGWriteBlockFiles(block->GetBlockSubstructure()->GetBlockList());
    }

    assert(block_list->GotoElement(save_pointer_blk) == DS_OK);
                                              /* Blockzeiger wieder auf    */
                                              /* alten Wert setzen         */

  } 

  return DS_OK;
}


/****************************************************************************
 * CGWriteProcessFiles(): erzeugt und fuellt <process>.h, <process.cc>
 *                        Ergebnis: CG_OK, falls alles geklappt hat,sonst enthaelt
 *                        CG_RESULT den aufgetretenen Fehler
 *                        Seiteneffekte: Anlegen und Fuellen der Prozessfiles
 ****************************************************************************/

DSResult CGWriter::CGWriteProcessFiles(DSProcessKeyList *process_list)
{
  DSResult result;
  DSResult status;
  DSProcess *process;
  DSProcess *save_pointer_pcs;
      
  if (process_list == NULL)
    return DS_OK;
    
  /******** Erzeugen der Prozessdateien  *************************************/
  
  for (status = process_list->MoveFirst();
       status == DS_OK;
       status = process_list->MoveNext())
  {
    process = process_list->GetCurrentData(); 
    assert(process);

    save_pointer_pcs = process;

    if (verbose)
      cgCout << "  Process " << process->GetName() << "..." << std::endl;
    result = process->Write(this, CG_WRITE_H);
    if (result != DS_OK) return result;
    result = process->Write(this, CG_WRITE_CC);
    if (result != DS_OK) return result;

    result = CGWriteProcedureFiles(process->GetProcedureList());
    if (result != DS_OK) return result;

    assert(process_list->GotoElement(save_pointer_pcs) == DS_OK);
                                              /* Prozesszeiger wieder auf  */
                                              /* alten Wert setzen         */

    if (verbose)
      cgCout << "  done." << std::endl;
  }

  return DS_OK;  
}


/****************************************************************************
 * CGWriteProcedureFiles(): erzeugt und fuellt <procedure>.h, <process.cc>
 *                        Ergebnis: CG_OK, falls alles geklappt hat,sonst enthaelt
 *                        CG_RESULT den aufgetretenen Fehler
 *                        Seiteneffekte: Anlegen und Fuellen der Prozessfiles
 ****************************************************************************/

DSResult CGWriter::CGWriteProcedureFiles(DSProcedureKeyList *procedure_list)
{
  DSResult result;
  DSResult status;
  DSProcedure *procedure;
  DSProcedure *save_pointer_pcd;
      
  if (procedure_list == NULL)
    return DS_OK;
    
  /******** Erzeugen der Prozessdateien  *************************************/
  
  for (status = procedure_list->MoveFirst();
       status == DS_OK;
       status = procedure_list->MoveNext())
  {
    procedure = procedure_list->GetCurrentData(); 
    assert(procedure);

    if (!procedure->GetIsComplex())
      continue;

    save_pointer_pcd = procedure;

    if (verbose)
      cgCout << "  Procedure " << procedure->GetName() << "..." << std::endl;
    result = procedure->Write(this, CG_WRITE_H);
    if (result != DS_OK) return result;
    result = procedure->Write(this, CG_WRITE_CC);
    if (result != DS_OK) return result;

    result = CGWriteProcedureFiles(procedure->GetProcedureList());
    if (result != DS_OK) return result;

    assert(procedure_list->GotoElement(save_pointer_pcd) == DS_OK);
                                              /* Prozedurzeiger wieder auf  */
                                              /* alten Wert setzen         */

    if (verbose)
      cgCout << "  done." << std::endl;
  }

  return DS_OK;  
}

/****************************************************************************
 * CGWriteMachineFiles(): erzeugt und fuellt <machine>.h, <machine.cc>
 *                        Ergebnis: CG_OK, falls alles geklappt hat,sonst enthaelt
 *                        CG_RESULT den aufgetretenen Fehler
 *                        Seiteneffekte: Anlegen und Fuellen der Maschinenfiles
 ****************************************************************************/

DSResult CGWriter::CGWriteMachineFiles(DSMachineKeyList *machine_list)
{
  DSResult result;
  DSResult status;
  DSMachine *machine;
  DSMachine *save_pointer_mach;
      
  if (machine_list == NULL)
    return DS_OK;
    
  /******** Erzeugen der Prozessdateien  *************************************/
  
  for (status = machine_list->MoveFirst();
       status == DS_OK;
       status = machine_list->MoveNext())
  {
    machine = machine_list->GetCurrentData(); 
    assert(machine);

    save_pointer_mach = machine;

    if (verbose)
      cgCout << "  Machine " << machine->GetName() << "..." << std::endl;
    result = machine->Write(this, CG_WRITE_H);
    if (result != DS_OK) return result;
    result = machine->Write(this, CG_WRITE_CC);
    if (result != DS_OK) return result;

    assert(machine_list->GotoElement(save_pointer_mach) == DS_OK);
                                              /* Maschinenzeiger wieder auf  */
                                              /* alten Wert setzen         */

    if (verbose)
      cgCout << "  done." << std::endl;
  }

  return DS_OK;  
}

DSResult CGWriter::CGWriteTemplates(DSSortList *sort_list)
{
  DSResult result;
  DSResult status;
  CGSort *sort, *father_sort, *elem_sort;
  DSGenerator *generator;
  DSSyntype *syntype;
  DSString *sort_name;
  DSNamedObject *father;
  DSString name;
  DSStringKeyList *generator_keylist;
  DSStringKeyList *syntype_keylist;
  DSString *elem, *key;
  CGFile *out;

  if (!sort_list) return DS_OK;

  out = CGGetFilePointer();
  assert(out);

  generator_keylist = new DSStringKeyList;
  MemAssert(generator_keylist);
  syntype_keylist = new DSStringKeyList;
  MemAssert(syntype_keylist);

  for (status = sort_list->MoveFirst();
       status == DS_OK;
       status = sort_list->MoveNext())
  {
    sort = (CGSort *)sort_list->GetCurrentData();
    assert(sort);
    
    if (sort->GetIsPredefined())  // e.g. Charstring
    {
      continue;
    }
    
    father = (DSNamedObject *)sort->GetParent();
    assert(father);

    name = father->GetIdentifier();
    
    switch (sort->GetTag())
    {
      case DS_SORT_GENERATOR:
        generator = (DSGenerator *)sort;

        // Bei Writeren werden im Moment nur Arrays und Strings unterstuetzt:
        if (*(generator->GetGeneratorName()) != DS_GENERATOR_NAME_ARRAY && 
            *(generator->GetGeneratorName()) != DS_GENERATOR_NAME_STRING)
        {
          break;
        }
        elem_sort = (CGSort *)generator->GetFirstSortRef();
        if (*(generator->GetGeneratorName()) == DS_GENERATOR_NAME_ARRAY)
        {
          elem_sort = (CGSort *)generator->GetNextSortRef();
        }
        sort_name = elem_sort->GetName();
        assert (sort_name);
        // Feststellen, ob dieses Template nicht bereits
        // instantiiert wurde:
        if (!generator_keylist->IsKeyInList(sort_name))
        {
          if (*(generator->GetGeneratorName()) == DS_GENERATOR_NAME_ARRAY)
          { 
            InsertString(*out, "template class SCArray<", 0, CG_NO_EOL); 
          }
          else
          { 
            InsertString(*out, "template class SCString<", 0, CG_NO_EOL); 
          }
          if (!elem_sort->GetIsPredefined())
          {
            InsertString(*out, name, 0, CG_NO_EOL); 
            InsertString(*out, "::", 0, CG_NO_EOL);
          }
          CGSetIndentPos(0);
          elem_sort->Write(this, CG_WRITE_IDENTIFIER);
          InsertString(*out, ">;", 0, CG_WITH_EOL);
          
          // fuer Strings zusaetzlich SCList<T> und SCListCons<T> instantiieren,
          // das SCList Basisklasse von SCString ist:
          if (*(generator->GetGeneratorName()) == DS_GENERATOR_NAME_STRING)
          { 
            InsertString(*out, "template class SCList<", 0, CG_NO_EOL); 
            if (!elem_sort->GetIsPredefined())
            {
              InsertString(*out, name, 0, CG_NO_EOL);
              InsertString(*out, "::", 0, CG_NO_EOL);
            }
            CGSetIndentPos(0);
            elem_sort->Write(this, CG_WRITE_IDENTIFIER);
            InsertString(*out, ">;", 0, CG_WITH_EOL);
            InsertString(*out, "template class SCListCons<", 0, CG_NO_EOL); 
            if (!elem_sort->GetIsPredefined())
            {
              InsertString(*out, name, 0, CG_NO_EOL);
              InsertString(*out, "::", 0, CG_NO_EOL);
            }
            CGSetIndentPos(0);
            elem_sort->Write(this, CG_WRITE_IDENTIFIER);
            InsertString(*out, ">;", 0, CG_WITH_EOL);
          }
                    
          elem = new DSString(*sort_name);
          assert(elem);
          key = new DSString(*sort_name);
          assert(key);
          assert(generator_keylist->Append(elem, key) == DS_OK);
        }
        break;
        
      case DS_SORT_SYNTYPE:
        syntype = (DSSyntype *)sort;
        father_sort = (CGSort *)syntype->GetParentSortRef();
        assert(father_sort);
        sort_name = father_sort->GetName();
        assert (sort_name);
        // Feststellen, ob dieses Template nicht bereits
        // instantiiert wurde:
        if (!syntype_keylist->IsKeyInList(sort_name))
        {
          InsertString(*out, "template class SCSyntype<", 0, CG_NO_EOL); 
          if (!father_sort->GetIsPredefined())
          {
            InsertString(*out, name, 0, CG_NO_EOL); 
            InsertString(*out, "::", 0, CG_NO_EOL);
          }
          CGSetIndentPos(0);
          father_sort->Write(this, CG_WRITE_IDENTIFIER);
          InsertString(*out, ">;", 0, CG_WITH_EOL);
          elem = new DSString(*sort_name);
          assert(elem);
          key = new DSString(*sort_name);
          assert(key);
          assert(syntype_keylist->Append(elem, key) == DS_OK);
        }
        break;
        
      default:
        break;
    }
  }
  delete generator_keylist;
  delete syntype_keylist;

  return DS_OK;
}

/****************************************************************************
 * CGWriteTemplatefile(): Instanziieren der Templates in einer separaten Datei
 *                        Ergebnis: DS_OK, falls alles geklappt hat, sonst enthaelt
 *                        DSResult den aufgetretenen Fehler
 *                        Seiteneffekte: Anlegen und Fuellen der .h und .cc-Files
 ****************************************************************************/

DSResult CGWriter::CGWriteTemplateFile(void)
{
  DSResult result;
  DSBlock *block;
  DSProcess *process;
  DSString *filename;

  assert (system);
  
  filename = new DSString(FILE_TEMPLATES);
  if (CGIncrementalCodeGeneration())
  {
    *filename += SUFFIX_C_INC;
  }
  else
  {
    *filename += SUFFIX_C;
  }

  CGFile out(output_dir, filename, pretty_printing);

  delete filename;

  if (!out.CGFileOpen())                
  {
    cgCout << "\nCan't open " << out.CGFilePath() 
         << out.CGFileName() << "\n";
    return DS_WRITE_ERROR;
  }
  CGSetFilePointer(&out);

  InsertCopyright(out);
  InsertLine(out);
  InsertIncludeSCL(out, "SCEnvironment.h", 0, CG_WITH_EOL);
  InsertLine(out);
  InsertIncludeSCL(out, "SCArray.tmpl.h", 0, CG_WITH_EOL); 
  InsertIncludeSCL(out, "SCList.tmpl.h", 0, CG_WITH_EOL); 
  InsertIncludeSCL(out, "SCString.tmpl.h", 0, CG_WITH_EOL); 
  InsertIncludeSCL(out, "SCSyntype.tmpl.h", 0, CG_WITH_EOL); 

  for (block = system->GetFirstBlock();
       block;
       block = system->GetNextBlock())
  {
    block->Run(this, DS_PROCESS, CG_WRITE_INCLUDE);
  }

  InsertLine(out); 
  
  /******** Suche und Rausschreiben von Templates auf Systemebene   **********/

  result = CGWriteTemplates(system->GetSortList());
  if (result != DS_OK)
  {
    return DS_ERROR;
  }

  /******** Suche und Rausschreiben von Templates auf Blockebene   **********/

  for (block = system->GetFirstBlock();
       block; 
       block = system->GetNextBlock())
  {
    result = CGWriteTemplates(block->GetSortList());
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    for (process = block->GetFirstProcess();
         process;
         process = block->GetNextProcess())
    {
      result = CGWriteTemplates(process->GetSortList());
      if (result != DS_OK)
      {
        return DS_ERROR;
      }
    }
  }
  InsertLine(out);

  return DS_OK;
}  
  
/****************************************************************************
 * CGWriteMakeFile(): Schreiben des Makefiles
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst enthaelt
 *             result den aufgetretenen Fehler
 *             Seiteneffekte: Anlegen und Fuellen des Makefiles
 ****************************************************************************/

DSResult CGWriter::CGWriteMakeFile(void)
{
  CGMake *makefile_generator;
  DSResult result;

  makefile_generator = new CGMake();
  MemAssert(makefile_generator);

  if ((result = makefile_generator->CGMakefile(this)) != DS_OK) 
  {
    cgCout << "\nError while writing of Makefile occured\n";
    return result;
  }
  delete makefile_generator;

  return DS_OK;
}

/****************************************************************************
 * CGWriteMainfile(): Schreiben des Mainfiles
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst enthaelt
 *             result den aufgetretenen Fehler
 *             Seiteneffekte: Anlegen und Fuellen des Mainfiles
 ****************************************************************************/

DSResult CGWriter::CGWriteMainFile(void)
{
  CGMain *main_generator;
  DSResult result;

  main_generator = new CGMain();
  MemAssert(main_generator);

  result = main_generator->CGWriteMain(this);
  if (result != DS_OK)
  {
    delete main_generator;
    return result;
  }
  delete main_generator;

  return DS_OK;
}

/****************************************************************************
 * CGWriteProcessSensors(): Schreiben der Infos fuer das Sensorenfenster
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst enthaelt
 *             result den aufgetretenen Fehler
 *             Seiteneffekte: Anlegen und Fuellen des Mainfiles
 ****************************************************************************/

DSResult CGWriter::CGWriteProcessSensors(DSProcess *process)
{
  DSResult result;
  DSString *process_name, *block_name;
  DSString str;
  CGFile *out;
  
  out = CGGetFilePointer();
  assert(out);
  
  process_name = process->GetName();
  assert(process_name);
  block_name = process->GetThisBlock()->GetName();
  assert(block_name);

  InsertString(*out, "// Sensors for Process '", 2, CG_NO_EOL);
  InsertString(*out, process_name, 0, CG_NO_EOL);
  InsertString(*out, "':", 0, CG_WITH_EOL);
  InsertLine(*out);

  if (process->HasSaves(DS_FALSE) ||    // vergeht Zeit im Prozess?
      process->HasAction(DS_REQUEST, DS_FALSE))
  {
    InsertString(*out, "// 1. Queue Sensors:", 2, CG_WITH_EOL);

    InsertString(*out, CG_NAME_PEV_SENSOR_PROC_QUEUE_LEN, 2, CG_NO_EOL);
    InsertString(*out, " PQL_", 0, CG_NO_EOL);
    InsertString(*out, block_name, 0, CG_NO_EOL);
    InsertString(*out, "_", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    InsertString(*out, " : \"", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    InsertString(*out, "\";", 0, CG_WITH_EOL);

    InsertString(*out, CG_NAME_PEV_SENSOR_PROC_QUEUE_LEN_FREQ, 2, CG_NO_EOL);
    InsertString(*out, " PQLF_", 0, CG_NO_EOL);
    InsertString(*out, block_name, 0, CG_NO_EOL);
    InsertString(*out, "_", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    InsertString(*out, " : \"", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    InsertString(*out, "\";", 0, CG_WITH_EOL);
    InsertLine(*out);
  }

  if (process->GetMaxInstances() == NULL ||
      process->GetMaxInstances()->GetOperator() != DS_OPT_NONE ||
      process->GetMaxInstances()->GetOperand1()->GetTag() != DS_OPN_CONSTANT ||
      *((DSOperandConstant*)process->GetMaxInstances()->GetOperand1())->GetConstant() != "1")
  {
    InsertString(*out, "// 2. Number of Instances Sensors:", 2, CG_WITH_EOL);

    InsertString(*out, CG_NAME_PEV_SENSOR_PROC_NUMBER, 2, CG_NO_EOL);
    InsertString(*out, " PN_", 0, CG_NO_EOL);
    InsertString(*out, block_name, 0, CG_NO_EOL);
    InsertString(*out, "_", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    InsertString(*out, " : \"", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    InsertString(*out, "\";", 0, CG_WITH_EOL);
    InsertLine(*out);
	}

  if (process->NumberOfRealStates() > 1)
  {
    InsertString(*out, "// 3. State Sensors:", 2, CG_WITH_EOL);

    InsertString(*out, CG_NAME_PEV_SENSOR_PROC_STATE_FREQ, 2, CG_NO_EOL);
    InsertString(*out, " PSTF_", 0, CG_NO_EOL);
    InsertString(*out, block_name, 0, CG_NO_EOL);
    InsertString(*out, "_", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    InsertString(*out, " : \"", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    InsertString(*out, "\";", 0, CG_WITH_EOL);
    InsertLine(*out);
  }

  InsertString(*out, "// 4. Signal Sensors:", 2, CG_WITH_EOL);

  if (process->GetValidSignalSet())
  {
    InsertString(*out, CG_NAME_PEV_SENSOR_PROC_SIGNAL_IN_FREQ, 2, CG_NO_EOL);
    InsertString(*out, " PSF_IN_", 0, CG_NO_EOL);
    InsertString(*out, block_name, 0, CG_NO_EOL);
    InsertString(*out, "_", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    InsertString(*out, " : \"", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    InsertString(*out, "\";", 0, CG_WITH_EOL);
  }

  if (process->HasAction(DS_OUTPUT, DS_FALSE))
  {
    InsertString(*out, CG_NAME_PEV_SENSOR_PROC_SIGNAL_OUT_FREQ, 2, CG_NO_EOL);
    InsertString(*out, " PSF_OUT_", 0, CG_NO_EOL);
    InsertString(*out, block_name, 0, CG_NO_EOL);
    InsertString(*out, "_", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    InsertString(*out, " : \"", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    InsertString(*out, "\";", 0, CG_WITH_EOL);
  }

  if (process->HasSaves(DS_FALSE) ||    // vergeht Zeit im Prozess?
      process->HasAction(DS_REQUEST, DS_FALSE))
  {
    InsertString(*out, CG_NAME_PEV_SENSOR_PROC_SIGNAL_WAIT_TIME, 2, CG_NO_EOL);
    InsertString(*out, " PSWT_", 0, CG_NO_EOL);
    InsertString(*out, block_name, 0, CG_NO_EOL);
    InsertString(*out, "_", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    InsertString(*out, " : \"\", \"", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    InsertString(*out, "\";", 0, CG_WITH_EOL);
    InsertLine(*out);
  }

  if (process->HasAction(DS_REQUEST, DS_FALSE))
  {
    InsertString(*out, "// 5. Request Sensors:", 2, CG_WITH_EOL);

    InsertString(*out, CG_NAME_PEV_SENSOR_PROC_REQUEST_FREQ, 2, CG_NO_EOL);
    InsertString(*out, " PRF_", 0, CG_NO_EOL);
    InsertString(*out, block_name, 0, CG_NO_EOL);
    InsertString(*out, "_", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    InsertString(*out, " : \"", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    InsertString(*out, "\";", 0, CG_WITH_EOL);
  }

  InsertLine(*out);

  return DS_OK;
}

/****************************************************************************
 * CGWriteMachineSensors(): Schreiben der Infos fuer das Sensorenfenster
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst enthaelt
 *             result den aufgetretenen Fehler
 *             Seiteneffekte: Anlegen und Fuellen des Mainfiles
 ****************************************************************************/

DSResult CGWriter::CGWriteMachineSensors(DSMachine *machine)
{
  DSResult result;
  DSString *machine_name, *block_name;
  DSString str;
  CGFile *out;
  CGPos pos;

  out = CGGetFilePointer();
  assert(out);
  pos = CGGetIndentPos();

  machine_name = machine->GetName();
  assert(machine_name);
  block_name = machine->GetThisBlock()->GetName();
  assert(block_name);

  InsertString(*out, "// Sensors for Machine '", 2, CG_NO_EOL);
  InsertString(*out, machine_name, 0, CG_NO_EOL);
  InsertString(*out, "':", 0, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "// 1. Queue Sensors:", pos, CG_WITH_EOL);

  InsertString(*out, CG_NAME_PEV_SENSOR_MACH_QUEUE_LEN, pos, CG_NO_EOL);
  InsertString(*out, " MQL_", 0, CG_NO_EOL);
  InsertString(*out, block_name, 0, CG_NO_EOL);
  InsertString(*out, "_", 0, CG_NO_EOL);
  InsertString(*out, machine_name, 0, CG_NO_EOL);
  InsertString(*out, " : \"", 0, CG_NO_EOL);
  InsertString(*out, machine_name, 0, CG_NO_EOL);
  InsertString(*out, "\";", 0, CG_WITH_EOL);

  InsertString(*out, CG_NAME_PEV_SENSOR_MACH_QUEUE_LEN_FREQ, pos, CG_NO_EOL);
  InsertString(*out, " MQLF_", 0, CG_NO_EOL);
  InsertString(*out, block_name, 0, CG_NO_EOL);
  InsertString(*out, "_", 0, CG_NO_EOL);
  InsertString(*out, machine_name, 0, CG_NO_EOL);
  InsertString(*out, " : \"", 0, CG_NO_EOL);
  InsertString(*out, machine_name, 0, CG_NO_EOL);
  InsertString(*out, "\";", 0, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "// 2. Request Sensors:", pos, CG_WITH_EOL);

  if (machine->GetMachineServiceSpeedList() &&
      machine->GetMachineServiceSpeedList()->NumberOfElements() > 1)
  {
    InsertString(*out, CG_NAME_PEV_SENSOR_MACH_REQUEST_FREQ, pos, CG_NO_EOL);
    InsertString(*out, " MRF_", 0, CG_NO_EOL);
    InsertString(*out, block_name, 0, CG_NO_EOL);
    InsertString(*out, "_", 0, CG_NO_EOL);
    InsertString(*out, machine_name, 0, CG_NO_EOL);
    InsertString(*out, " : \"", 0, CG_NO_EOL);
    InsertString(*out, machine_name, 0, CG_NO_EOL);
    InsertString(*out, "\";", 0, CG_WITH_EOL);
  }

  InsertString(*out, CG_NAME_PEV_SENSOR_MACH_REQUEST_WAIT_TIME, pos, CG_NO_EOL);
  InsertString(*out, " MRWT_", 0, CG_NO_EOL);
  InsertString(*out, block_name, 0, CG_NO_EOL);
  InsertString(*out, "_", 0, CG_NO_EOL);
  InsertString(*out, machine_name, 0, CG_NO_EOL);
  InsertString(*out, " : \"\", \"", 0, CG_NO_EOL);
  InsertString(*out, machine_name, 0, CG_NO_EOL);
  InsertString(*out, "\";", 0, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "// 3. Machine Sensors:", pos, CG_WITH_EOL);

  InsertString(*out, CG_NAME_PEV_SENSOR_MACH_UTILIZATION, pos, CG_NO_EOL);
  InsertString(*out, " MU_", 0, CG_NO_EOL);
  InsertString(*out, block_name, 0, CG_NO_EOL);
  InsertString(*out, "_", 0, CG_NO_EOL);
  InsertString(*out, machine_name, 0, CG_NO_EOL);
  InsertString(*out, " : \"", 0, CG_NO_EOL);
  InsertString(*out, machine_name, 0, CG_NO_EOL);
  InsertString(*out, "\";", 0, CG_WITH_EOL);

  InsertLine(*out);

  return DS_OK;
}

/****************************************************************************
 * CGWriteProcessDisplays(): Schreiben der Infos fuer die PEV
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst enthaelt
 *             result den aufgetretenen Fehler
 *             Seiteneffekte: Anlegen und Fuellen des Mainfiles
 ****************************************************************************/

DSResult CGWriter::CGWriteProcessDisplays(DSProcess *process,
                                          DSCardinal &colour_index)
{
  DSResult result;
  DSString *process_name, *block_name;
  DSString str;
  CGFile *out;
  CGPos pos;

  out = CGGetFilePointer();
  assert(out);
  pos = CGGetIndentPos();

  process_name = process->GetName();
  assert(process_name);
  block_name = process->GetThisBlock()->GetName();
  assert(block_name);

  InsertString(*out, "// Displays for Process '", 2, CG_NO_EOL);
  InsertString(*out, process_name, 0, CG_NO_EOL);
  InsertString(*out, "':", 0, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "// 1. Process Instances Displays:", pos, CG_WITH_EOL);

  if (process->GetMaxInstances() == NULL ||
      process->GetMaxInstances()->GetOperator() != DS_OPT_NONE ||
      process->GetMaxInstances()->GetOperand1()->GetTag() != DS_OPN_CONSTANT ||
      *((DSOperandConstant*)process->GetMaxInstances()->GetOperand1())->GetConstant() != "1")
  {
    // Anzahl von Instanzen als Kurve
    // (Mittelwert + Mittelwert ueber Intervall):
    InsertString(*out, CG_NAME_PEV_DISPLAY_CURVES, pos, CG_NO_EOL);
    str.Format(" \"Average Instances of Process '%s'\": ",
               process_name->GetString());
    InsertString(*out, str, 0, CG_WITH_EOL);
    InsertString(*out, " PN_", 4, CG_NO_EOL);
    InsertString(*out, block_name, 0, CG_NO_EOL);
    InsertString(*out, "_", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    str.Format("(avg, \"%s\"),",
               display_colours[colour_index % NUM_OF_COLOURS]);
    InsertString(*out, str, 0, CG_WITH_EOL);
    colour_index++;
    InsertString(*out, " PN_", 4, CG_NO_EOL);
    InsertString(*out, block_name, 0, CG_NO_EOL);
    InsertString(*out, "_", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    str.Format("(avi, \"%s\");",
               display_colours[colour_index % NUM_OF_COLOURS]);
    InsertString(*out, str, 0, CG_WITH_EOL);
    colour_index++;
    InsertLine(*out);
  }

  if (process->NumberOfRealStates() > 1)
  {
    InsertString(*out, "// 2. State Displays:", pos, CG_WITH_EOL);

    // State-Distribution als Balkendiagramm:
    InsertString(*out, CG_NAME_PEV_DISPLAY_FREQ, pos, CG_NO_EOL);
    str.Format(" \"State Distribution of Process '%s'\":",
               process_name->GetString());
    InsertString(*out, str, 0, CG_WITH_EOL);
    InsertString(*out, " PSTF_", 4, CG_NO_EOL);
    InsertString(*out, block_name, 0, CG_NO_EOL);
    InsertString(*out, "_", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    str.Format("(\"%s\");",
               display_colours[colour_index % NUM_OF_COLOURS]);
    InsertString(*out, str, 0, CG_WITH_EOL);
    colour_index++;
    InsertLine(*out);

    // State-Distribution als Gantt-Diagramm:
    InsertString(*out, CG_NAME_PEV_DISPLAY_GANTT, pos, CG_NO_EOL);
    str.Format(" \"Gantt diagramm of Process '%s'\":",
               process_name->GetString());
    InsertString(*out, str, 0, CG_WITH_EOL);
    InsertString(*out, " PSTF_", 4, CG_NO_EOL);
    InsertString(*out, block_name, 0, CG_NO_EOL);
    InsertString(*out, "_", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    str.Format("(\"%s\");",
               display_colours[colour_index % NUM_OF_COLOURS]);
    InsertString(*out, str, 0, CG_WITH_EOL);
    colour_index++;
    InsertLine(*out);
  }

  InsertString(*out, "// 3. Signal Displays:", pos, CG_WITH_EOL);

  if (process->GetValidSignalSet())
  {
    // Signal-Distribution (Input) als Balkendiagramm:
    InsertString(*out, CG_NAME_PEV_DISPLAY_FREQ, pos, CG_NO_EOL);
    str.Format(" \"Signal Distribution of Process '%s' (in)\": ",
               process_name->GetString());
    InsertString(*out, str, 0, CG_WITH_EOL);
    InsertString(*out, " PSF_IN_", 4, CG_NO_EOL);
    InsertString(*out, block_name, 0, CG_NO_EOL);
    InsertString(*out, "_", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    str.Format("(\"%s\");",
               display_colours[colour_index % NUM_OF_COLOURS]);
    InsertString(*out, str, 0, CG_WITH_EOL);
    colour_index++;
  }

  if (process->HasAction(DS_OUTPUT, DS_FALSE))
  {
    // Signal-Distribution (Output) als Balkendiagramm:
    InsertString(*out, CG_NAME_PEV_DISPLAY_FREQ, pos, CG_NO_EOL);
    str.Format(" \"Signal Distribution of Process '%s' (out)\": ",
               process_name->GetString());
    InsertString(*out, str, 0, CG_WITH_EOL);
    InsertString(*out, " PSF_OUT_", 4, CG_NO_EOL);
    InsertString(*out, block_name, 0, CG_NO_EOL);
    InsertString(*out, "_", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    str.Format("(\"%s\");",
               display_colours[colour_index % NUM_OF_COLOURS]);
    InsertString(*out, str, 0, CG_WITH_EOL);
    colour_index++;
  }

  // Signal-Wartezeit als Kurve
  // (Mittelwert + Mittelwert ueber Intervall):
  if (process->HasSaves(DS_FALSE) ||    // vergeht Zeit im Prozess?
      process->HasAction(DS_REQUEST, DS_FALSE))
  {
    InsertString(*out, CG_NAME_PEV_DISPLAY_CURVES, pos, CG_NO_EOL);
    str.Format(" \"Average Signal Wait Time of Process '%s'\": ",
               process_name->GetString());
    InsertString(*out, str, 0, CG_WITH_EOL);
    InsertString(*out, " PSWT_", 4, CG_NO_EOL);
    InsertString(*out, block_name, 0, CG_NO_EOL);
    InsertString(*out, "_", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    str.Format("(avg, \"%s\"),",
               display_colours[colour_index % NUM_OF_COLOURS]);
    InsertString(*out, str, 0, CG_WITH_EOL);
    colour_index++;
    InsertString(*out, " PSWT_", 4, CG_NO_EOL);
    InsertString(*out, block_name, 0, CG_NO_EOL);
    InsertString(*out, "_", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    str.Format("(avi, \"%s\");",
               display_colours[colour_index % NUM_OF_COLOURS]);
    InsertString(*out, str, 0, CG_WITH_EOL);
    colour_index++;
    InsertLine(*out);
  }

  if (process->HasAction(DS_REQUEST, DS_FALSE))
  {
    InsertString(*out, "// 4. Request Displays:", pos, CG_WITH_EOL);

    // Request-Distribution als Balkendiagramm:
    InsertString(*out, CG_NAME_PEV_DISPLAY_FREQ, pos, CG_NO_EOL);
    str.Format(" \"Request Distribution of Process '%s'\": ",
               process_name->GetString());
    InsertString(*out, str, 0, CG_WITH_EOL);
    InsertString(*out, " PRF_", 4, CG_NO_EOL);
    InsertString(*out, block_name, 0, CG_NO_EOL);
    InsertString(*out, "_", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    str.Format("(\"%s\");",
               display_colours[colour_index % NUM_OF_COLOURS]);
    InsertString(*out, str, 0, CG_WITH_EOL);
    colour_index++;
    InsertLine(*out);
  }

  if (process->HasSaves(DS_FALSE) ||    // vergeht Zeit im Prozess?
      process->HasAction(DS_REQUEST, DS_FALSE))
  {
    InsertString(*out, "// 5. Queue Displays:", pos, CG_WITH_EOL);

    // Queue-Laengenverteilung als Balkendiagramm:
    InsertString(*out, CG_NAME_PEV_DISPLAY_FREQ, pos, CG_NO_EOL);
    str.Format(" \"Queue Length Distribution of Process '%s'\": ",
               process_name->GetString());
    InsertString(*out, str, 0, CG_WITH_EOL);
    InsertString(*out, " PQLF_", 4, CG_NO_EOL);
    InsertString(*out, block_name, 0, CG_NO_EOL);
    InsertString(*out, "_", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    str.Format("(\"%s\");",
               display_colours[colour_index % NUM_OF_COLOURS]);
    InsertString(*out, str, 0, CG_WITH_EOL);
    colour_index++;

    // durchschnittliche Queuelaenge als Kurve
    // (Mittelwert + Mittelwert im Intervall):
    InsertString(*out, CG_NAME_PEV_DISPLAY_CURVES, pos, CG_NO_EOL);
    str.Format(" \"Average Queue Length of Process '%s'\": ",
               process_name->GetString());
    InsertString(*out, str, 0, CG_WITH_EOL);
    InsertString(*out, " PQL_", 4, CG_NO_EOL);
    InsertString(*out, block_name, 0, CG_NO_EOL);
    InsertString(*out, "_", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    str.Format("(avg, \"%s\"),",
               display_colours[colour_index % NUM_OF_COLOURS]);
    InsertString(*out, str, 0, CG_WITH_EOL);
    colour_index++;
    InsertString(*out, " PQL_", 4, CG_NO_EOL);
    InsertString(*out, block_name, 0, CG_NO_EOL);
    InsertString(*out, "_", 0, CG_NO_EOL);
    InsertString(*out, process_name, 0, CG_NO_EOL);
    str.Format("(avi, \"%s\");",
               display_colours[colour_index % NUM_OF_COLOURS]);
    InsertString(*out, str, 0, CG_WITH_EOL);
    colour_index++;
    InsertLine(*out);
  }

  return DS_OK;
}

/****************************************************************************
 * CGWriteMachineDisplays(): Schreiben der Infos fuer die PEV
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst enthaelt
 *             result den aufgetretenen Fehler
 *             Seiteneffekte: Anlegen und Fuellen des Mainfiles
 ****************************************************************************/

DSResult CGWriter::CGWriteMachineDisplays(DSMachine *machine,
                                          DSCardinal &colour_index)
{
  DSResult result;
  DSString *machine_name, *block_name;
  DSString str;
  CGFile *out;
  CGPos pos;

  out = CGGetFilePointer();
  assert(out);
  pos = CGGetIndentPos();

  machine_name = machine->GetName();
  assert(machine_name);
  block_name = machine->GetThisBlock()->GetName();
  assert(block_name);

  InsertString(*out, "// Displays for Machine '", 2, CG_NO_EOL);
  InsertString(*out, machine_name, 0, CG_NO_EOL);
  InsertString(*out, "':", 0, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "// 1. Request Displays:", pos, CG_WITH_EOL);

  if (machine->GetMachineServiceSpeedList() &&
      machine->GetMachineServiceSpeedList()->NumberOfElements() > 1)
  {
    // Request-Distribution als Balkendiagramm:
    InsertString(*out, CG_NAME_PEV_DISPLAY_FREQ, pos, CG_NO_EOL);
    str.Format(" \"Request Distribution of Machine '%s'\": ",
               machine_name->GetString());
    InsertString(*out, str, 0, CG_WITH_EOL);
    InsertString(*out, " MRF_", 4, CG_NO_EOL);
    InsertString(*out, block_name, 0, CG_NO_EOL);
    InsertString(*out, "_", 0, CG_NO_EOL);
    InsertString(*out, machine_name, 0, CG_NO_EOL);
    str.Format("(\"%s\");",
               display_colours[colour_index % NUM_OF_COLOURS]);
    InsertString(*out, str, 0, CG_WITH_EOL);
    colour_index++;
  }

  // Request-Wartezeit als Kurve
  // (Mittelwert + Mittelwert im Intervall):
  InsertString(*out, CG_NAME_PEV_DISPLAY_CURVES, pos, CG_NO_EOL);
  str.Format(" \"Average Request Wait Time of Machine '%s'\": ",
             machine_name->GetString());
  InsertString(*out, str, 0, CG_WITH_EOL);
  InsertString(*out, " MRWT_", 4, CG_NO_EOL);
  InsertString(*out, block_name, 0, CG_NO_EOL);
  InsertString(*out, "_", 0, CG_NO_EOL);
  InsertString(*out, machine_name, 0, CG_NO_EOL);
  str.Format("(avg, \"%s\"),",
             display_colours[colour_index % NUM_OF_COLOURS]);
  InsertString(*out, str, 0, CG_WITH_EOL);
  colour_index++;
  InsertString(*out, " MRWT_", 4, CG_NO_EOL);
  InsertString(*out, block_name, 0, CG_NO_EOL);
  InsertString(*out, "_", 0, CG_NO_EOL);
  InsertString(*out, machine_name, 0, CG_NO_EOL);
  str.Format("(avi, \"%s\");",
             display_colours[colour_index % NUM_OF_COLOURS]);
  InsertString(*out, str, 0, CG_WITH_EOL);
  colour_index++;
  InsertLine(*out);

  InsertString(*out, "// 2. Queue Displays:", pos, CG_WITH_EOL);

  // Queuelaengen-Distribution als Balkendiagramm:
  InsertString(*out, CG_NAME_PEV_DISPLAY_FREQ, pos, CG_NO_EOL);
  str.Format(" \"Queue Length Distribution of Machine '%s'\": ",
             machine_name->GetString());
  InsertString(*out, str, 0, CG_WITH_EOL);
  InsertString(*out, " MQLF_", 4, CG_NO_EOL);
  InsertString(*out, block_name, 0, CG_NO_EOL);
  InsertString(*out, "_", 0, CG_NO_EOL);
  InsertString(*out, machine_name, 0, CG_NO_EOL);
  str.Format("(\"%s\");",
             display_colours[colour_index % NUM_OF_COLOURS]);
  InsertString(*out, str, 0, CG_WITH_EOL);
  colour_index++;

  // Queuelaengen-Distribution als Kurve
  // (Mittelwert + Mittelwert im Intervall):
  InsertString(*out, CG_NAME_PEV_DISPLAY_CURVES, pos, CG_NO_EOL);
  str.Format(" \"Average Queue Length of Machine '%s'\": ",
             machine_name->GetString());
  InsertString(*out, str, 0, CG_WITH_EOL);
  InsertString(*out, " MQL_", 4, CG_NO_EOL);
  InsertString(*out, block_name, 0, CG_NO_EOL);
  InsertString(*out, "_", 0, CG_NO_EOL);
  InsertString(*out, machine_name, 0, CG_NO_EOL);
  str.Format("(avg, \"%s\"),",
             display_colours[colour_index % NUM_OF_COLOURS]);
  InsertString(*out, str, 0, CG_WITH_EOL);
  colour_index++;
  InsertString(*out, " MQL_", 4, CG_NO_EOL);
  InsertString(*out, block_name, 0, CG_NO_EOL);
  InsertString(*out, "_", 0, CG_NO_EOL);
  InsertString(*out, machine_name, 0, CG_NO_EOL);
  str.Format("(avi, \"%s\");",
             display_colours[colour_index % NUM_OF_COLOURS]);
  InsertString(*out, str, 0, CG_WITH_EOL);
  colour_index++;

  // Maschinenauslastung als Kurve
  // (Mittelwert + Mittelwert im Intervall):
  InsertString(*out, CG_NAME_PEV_DISPLAY_CURVES, pos, CG_NO_EOL);
  str.Format(" \"Average Machine Utilization '%s'\": ",
             machine_name->GetString());
  InsertString(*out, str, 0, CG_WITH_EOL);
  InsertString(*out, " MU_", 4, CG_NO_EOL);
  InsertString(*out, block_name, 0, CG_NO_EOL);
  InsertString(*out, "_", 0, CG_NO_EOL);
  InsertString(*out, machine_name, 0, CG_NO_EOL);
  str.Format("(avg, \"%s\"),",
             display_colours[colour_index % NUM_OF_COLOURS]);
  InsertString(*out, str, 0, CG_WITH_EOL);
  colour_index++;
  InsertString(*out, " MU_", 4, CG_NO_EOL);
  InsertString(*out, block_name, 0, CG_NO_EOL);
  InsertString(*out, "_", 0, CG_NO_EOL);
  InsertString(*out, machine_name, 0, CG_NO_EOL);
  str.Format("(avi, \"%s\");",
             display_colours[colour_index % NUM_OF_COLOURS]);
  InsertString(*out, str, 0, CG_WITH_EOL);
  colour_index++;

  InsertLine(*out);

  return DS_OK;
}

/****************************************************************************
 * CGWriteExperimentfile(): Schreiben einer Standard-PEV-Experimentbeschrei-
 *                          bung
 *             Ergebnis: DS_OK, falls alles geklappt hat, sonst enthaelt
 *             result den aufgetretenen Fehler
 *             Seiteneffekte: Anlegen und Fuellen des Mainfiles
 ****************************************************************************/

DSResult CGWriter::CGWriteExperimentFile(void)
{
  DSResult result;
  DSBlock *block;
  DSProcess *process;
  DSString *filename;
  DSString *system_name, *block_name;
  DSMachine *machine;
  DSCardinal colour_index = 0;
  DSString str;

  assert (system);
  system_name = system->GetName();

  filename = new DSString(*system_name);
  *filename += SUFFIX_PEV;

  CGFile out(output_dir, filename);

  delete filename;

  if (!out.CGFileOpen())                
  {
    cgCout << "\nCan't open " << out.CGFilePath() 
           << out.CGFileName() << "\n";
    return DS_WRITE_ERROR;
  }
  CGSetFilePointer(&out);
  
  InsertString(out, "// Experiment Description for Specification '",
               0, CG_NO_EOL);
  InsertString(out, system_name, 0, CG_NO_EOL);
  InsertString(out, "'", 0, CG_WITH_EOL);
  InsertString(out, "// --------------------------------------------------",
               0, CG_WITH_EOL);
  InsertLine(out);
  InsertString(out, CG_NAME_PEV_SYSTEM_EXPERIMENT, 0, CG_NO_EOL);
  InsertString(out, " : \"Experimenter for '", 0, CG_NO_EOL);
  InsertString(out, system_name, 0, CG_NO_EOL);
  InsertString(out, "'\";", 0, CG_WITH_EOL);
  InsertString(out, CG_NAME_PEV_SYSTEM_SPECIFICATION, 0, CG_NO_EOL);
  InsertString(out, " : \"", 0, CG_NO_EOL);
  InsertString(out, system_name, 0, CG_NO_EOL);
  InsertString(out, "\";", 0, CG_WITH_EOL);
  InsertString(out, CG_NAME_PEV_SYSTEM_REPORT_FILE, 0, CG_NO_EOL);
  InsertString(out, " : \"", 0, CG_NO_EOL);
  InsertString(out, system_name, 0, CG_NO_EOL);
  InsertString(out, SUFFIX_PEV_REPORT, 0, CG_NO_EOL);
  InsertString(out, "\";", 0, CG_WITH_EOL);
  InsertString(out, CG_NAME_PEV_SYSTEM_CURVE_POINTS, 0, CG_NO_EOL);
  InsertString(out, " : 32;", 0, CG_WITH_EOL);
  InsertString(out, CG_NAME_PEV_SYSTEM_SCALE_ADAPTION, 0, CG_NO_EOL);
  InsertString(out, " : 16;", 0, CG_WITH_EOL);
  InsertString(out, CG_NAME_PEV_SYSTEM_DEFAULT_INTERVAL, 0, CG_NO_EOL);
  InsertString(out, " : 2;", 0, CG_WITH_EOL);
  InsertLine(out);

  InsertString(out, CG_NAME_PEV_SENSORS, 0, CG_WITH_EOL);
  InsertString(out, "{", 0, CG_WITH_EOL);
  InsertString(out, "//", 2, CG_WITH_EOL);
  InsertString(out, "// Global Sensors:", 2, CG_WITH_EOL);
  InsertString(out, "//", 2, CG_WITH_EOL);
  InsertLine(out);
  if (system->HasAction(DS_OUTPUT, DS_TRUE))
  {
    InsertString(out, CG_NAME_PEV_SENSOR_GLOBAL_SIGNAL_FREQ, 2, CG_NO_EOL);
    InsertString(out, " GSF;", 0, CG_WITH_EOL);
  }
  if (system->HasAction(DS_REQUEST, DS_TRUE))
  {
    InsertString(out, CG_NAME_PEV_SENSOR_GLOBAL_REQUEST_FREQ, 2, CG_NO_EOL);
    InsertString(out, " GRF;", 0, CG_WITH_EOL);
  }
  InsertLine(out);
  for (block = system->GetFirstBlock();
       block;
       block = system->GetNextBlock())
  {
    block_name = block->GetName();
    assert(block_name);
    InsertString(out, "//", 2, CG_WITH_EOL);
    InsertString(out, "// Sensors for block '", 2, CG_NO_EOL);
    InsertString(out, block_name, 0, CG_NO_EOL);
    InsertString(out, "':", 0, CG_WITH_EOL);
    InsertString(out, "//", 2, CG_WITH_EOL);
    InsertLine(out);

    for (process = block->GetFirstProcess();
         process;
         process = block->GetNextProcess())
    {
      CGSetIndentPos(2);
      result = CGWriteProcessSensors(process);
      if (result != DS_OK) return result;
    }
    for (machine = block->GetFirstMachine();
         machine;
         machine = block->GetNextMachine())
    {
      CGSetIndentPos(2);
      result = CGWriteMachineSensors(machine);
      if (result != DS_OK) return result;
    }
  }
  InsertString(out, "}", 0, CG_WITH_EOL);
  InsertLine(out);

  //
  // Display-Section:
  //

  InsertString(out, CG_NAME_PEV_DISPLAYS, 0, CG_WITH_EOL);
  InsertString(out, "{", 0, CG_WITH_EOL);

  if (system->HasAction(DS_OUTPUT, DS_TRUE))
  {
    // Globale Signalverteilung:
    InsertString(out, "//", 2, CG_WITH_EOL);
    InsertString(out, "// Global Displays:", 2, CG_WITH_EOL);
    InsertString(out, "//", 2, CG_WITH_EOL);
    InsertLine(out);
    InsertString(out, CG_NAME_PEV_DISPLAY_FREQ, 2, CG_NO_EOL);
    InsertString(out, " \"Global Signal Distribution\": ",
                 0, CG_WITH_EOL);
    InsertString(out, " GSF", 4, CG_NO_EOL);
    str.Format("(\"%s\");",
               display_colours[colour_index % NUM_OF_COLOURS]);
    InsertString(out, str, 0, CG_WITH_EOL);
    colour_index++;
  }
  
  if (system->HasAction(DS_REQUEST, DS_TRUE))
  {
    // Globale Requestverteilung:
    InsertString(out, CG_NAME_PEV_DISPLAY_FREQ, 2, CG_NO_EOL);
    InsertString(out, " \"Global Request Distribution\": ",
                 0, CG_WITH_EOL);
    InsertString(out, " GRF", 4, CG_NO_EOL);
    str.Format("(\"%s\");",
               display_colours[colour_index % NUM_OF_COLOURS]);
    InsertString(out, str, 0, CG_WITH_EOL);
    colour_index++;
    InsertLine(out);
  }

  for (block = system->GetFirstBlock();
       block;
       block = system->GetNextBlock())
  {
    block_name = block->GetName();
    assert(block_name);
    InsertString(out, "//", 2, CG_WITH_EOL);
    InsertString(out, "// Displays for Block '", 2, CG_NO_EOL);
    InsertString(out, block_name, 0, CG_NO_EOL);
    InsertString(out, "':", 0, CG_WITH_EOL);
    InsertString(out, "//", 2, CG_WITH_EOL);
    InsertLine(out);
    for (process = block->GetFirstProcess();
         process;
         process = block->GetNextProcess())
    {
      CGSetIndentPos(2);
      result = CGWriteProcessDisplays(process, colour_index);
      if (result != DS_OK) return result;
    }
    for (machine = block->GetFirstMachine();
         machine;
         machine = block->GetNextMachine())
    {
      CGSetIndentPos(2);
      result = CGWriteMachineDisplays(machine, colour_index);
      if (result != DS_OK) return result;
    }
  }
  InsertString(out, "}", 0, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * CGMakeDir: Funktion legt ein Directory an.
 *            -> path: Pfad des Directories
 ****************************************************************************/

DSResult CGWriter::CGMakeDir(DSString *path)
{
  assert(path);
  assert(path->GetString());


  errno = 0;  

#ifndef _MSC_VER
  if (mkdir(path->GetString(), S_IRWXU | S_IRWXG) != 0)
#else
  if (_mkdir(path->GetString()) != 0)
#endif
  {
    switch (errno)
    {
      case EEXIST:
        break;
      
      case ENOENT:
      case ENOTDIR:
      case ENOSPC:
        cgCout << "Can't create directory " << path << " !\n";
        return DS_ERROR;
        break;
      default:
        break;
    }
  }
  

  return DS_OK;
}

/****************************************************************************
 * CGEvaluateEnvironment: Funktion liest die Environment-Variablen und
 *                        speichert die enthaltenen Strings
 ****************************************************************************/

DSResult CGWriter::CGEvaluateEnvironment(void)
{
#ifndef _MSC_VER
  uid_t uid;
  struct passwd *user;
  char *user_login;
  struct stat stat_buffer;
#endif
  DSString bin_dir;
  
  if (getenv("QUEST_OUTPUT_DIR") != NULL)
  {
    output_dir = new DSString((char *)getenv("QUEST_OUTPUT_DIR"));
    assert(output_dir);
    if (CGMakeDir(output_dir) != DS_OK) return DS_ERROR;

    *output_dir += CG_PATH_SEPARATOR_STRING;

#ifndef _MSC_VER
    // Fileinformationen des Directories holen:
    if (stat(output_dir->GetString(), &stat_buffer) != 0)
    {
      cgCout << "Can't access directory '" << *output_dir << "'" << std::endl;
      return DS_ERROR;
    }

    // User-ID holen:
    uid = getuid();

    // Wenn das Output-Directory nicht dem User gehoert,
    // dann legen wir nun ein Unterverzeichnis mit dem
    // Login-Namen des Users an:
    if (stat_buffer.st_uid != uid)
    {
      user = getpwuid(uid);
      assert(user);
      user_login = user->pw_name;
      assert(user_login);
      *output_dir += user_login;
      if (CGMakeDir(output_dir) != DS_OK) return DS_ERROR;
      *output_dir += CG_PATH_SEPARATOR_STRING;
    }
#endif
    *output_dir += *system->GetName();
    if (CGMakeDir(output_dir) != DS_OK) return DS_ERROR;
    bin_dir = *output_dir;
    bin_dir += CG_PATH_SEPARATOR_STRING;
    bin_dir += "obj";
    if (CGMakeDir(&bin_dir) != DS_OK) return DS_ERROR;
    if (verbose)
      cgCout << "  Output directory: " << output_dir << std::endl;
  }
  else
  {
#ifdef macintosh
    output_dir = new DSString (CG_PATH_SEPARATOR_STRING);
    *output_dir += *system->GetName();
#else
    output_dir = new DSString(*system->GetName());
#endif
    assert(output_dir);
    if (CGMakeDir(output_dir) != DS_OK) return DS_ERROR;
    if (verbose)
      cgCout << "  Output directory: " << output_dir << std::endl;
  }

  return DS_OK; 
}

/****************************************************************************
 * CGGet...(): Ruckgabe der Datentypen des Writerobjektes
 *             Ergebnis: DSString-Zeiger
 *             Seiteneffekte: keine
 ****************************************************************************/

DSString *CGWriter::CGGetOutputDir(void) const
{
  return output_dir;
}

DSSystem *CGWriter::CGGetSystem(void) const
{
  return system;
}

DSBoolean CGWriter::CGCreateMakefile(void) const
{
  return generate_makefile;
}

DSBoolean CGWriter::CGCreateTemplatefile(void) const
{
  return generate_templatefile;
}

DSBoolean CGWriter::CGVerbose(void) const
{
  return verbose;
}

DSBoolean CGWriter::CGPrettyPrinting(void) const
{
  return pretty_printing;
}

DSBoolean CGWriter::CGSupportPEV(void) const
{
  return support_pev;
}

CGFile *CGWriter::CGGetFilePointer(void) const
{
  return current_file;
}

CGPos CGWriter::CGGetIndentPos(void) const
{
  return current_pos;
}

/****************************************************************************
 * CGGetLastSignalID(): liefert den Wert der letzten Signal-ID
 *                Ergebnis: DSCardinal
 *                CG_RESULT den aufgetretenen Fehler
 *                Seiteneffekte: Anlegen und Fuellen des .h-Files
 ****************************************************************************/

DSCardinal CGWriter::CGGetLastSignalID() const
{
  return last_signal_id;
}

DSCardinal CGWriter::CGGetLastProcessID(void) const
{
  return last_process_id;
}

DSCardinal CGWriter::CGGetLastProcedureID(void) const
{
  return last_procedure_id;
}

DSCardinal CGWriter::CGGetLastStateID(void) const
{
  return last_state_id;
}

DSCardinal CGWriter::CGGetLastTransitionID(void) const
{
  return last_transition_id;
}

DSCardinal CGWriter::CGGetLastRequestID(void) const
{
  return last_request_id;
}

DSCardinal CGWriter::CGGetLastCallID(void) const
{
  return last_call_id;
}

DSCardinal CGWriter::CGGetLastAtomID(void) const
{
  return last_atom_id;
}

DSStringKeyList *CGWriter::CGGetLabelList(void) const
{
  return label_list;
}

DSResult CGWriter::CGSetLabelList(DSStringKeyList *new_list)
{
  if (new_list == label_list) return DS_OK;

  if (label_list) delete label_list;

  label_list = new_list;

  return DS_OK;
}

DSCardinal CGWriter::CGGetLastMachineID(void) const
{
  return last_machine_id;
}

DSCardinal CGWriter::CGGetLastServiceID(void) const
{
  return last_service_id;
}

DSMachineServiceRefList *CGWriter::CGGetServiceList(void) const
{
  return machine_service_list;
}

DSStateRef CGWriter::CGGetState(void) const
{
  return real_state;
}

DSCardinal CGWriter::CGGetEvent(void) const
{
  return trace_event;
}

DSBoolean CGWriter::CGGetWriteInformalText(void) const
{
  return generate_informal_text;
}

DSBoolean CGWriter::CGIncrementalCodeGeneration(void) const
{
  return incremental_code_generation;
}

DSBoolean CGWriter::CGDebug(void) const
{
  return generate_debug;
}

DSResult CGWriter::CGSetLastSignalID(DSCardinal new_value)
{
  last_signal_id = new_value;
  return DS_OK;
}

DSResult CGWriter::CGSetLastProcessID(DSCardinal new_value)
{
  last_process_id = new_value;
  return DS_OK;
}

DSResult CGWriter::CGSetLastProcedureID(DSCardinal new_value)
{
  last_procedure_id = new_value;
  return DS_OK;
}

DSResult CGWriter::CGSetLastMachineID(DSCardinal new_value)
{
  last_machine_id = new_value;
  return DS_OK;
}

DSResult CGWriter::CGSetLastServiceID(DSCardinal new_value)
{
  last_service_id = new_value;
  return DS_OK;
}

DSResult CGWriter::CGSetLastStateID(DSCardinal new_value)
{
  last_state_id = new_value;
  return DS_OK;
}

DSResult CGWriter::CGSetLastTransitionID(DSCardinal new_value)
{
  last_transition_id = new_value;
  return DS_OK;
}

DSResult CGWriter::CGSetLastRequestID(DSCardinal new_value)
{
  last_request_id = new_value;
  return DS_OK;
}

DSResult CGWriter::CGSetLastCallID(DSCardinal new_value)
{
  last_call_id = new_value;
  return DS_OK;
}

DSResult CGWriter::CGSetLastAtomID(DSCardinal new_value)
{
  last_atom_id = new_value;
  return DS_OK;
}

DSResult CGWriter::CGSetFilePointer(CGFile *new_file)
{
  current_file = new_file;
  return DS_OK;
}

DSResult CGWriter::CGSetIndentPos(CGPos new_pos)
{
  current_pos = new_pos;

  return DS_OK;
}

DSResult CGWriter::CGSetState(DSStateRef new_state)
{
  real_state = new_state;

  return DS_OK;
}

DSResult CGWriter::CGSetEvent(DSCardinal event)
{
  trace_event = event;

  return DS_OK;
}

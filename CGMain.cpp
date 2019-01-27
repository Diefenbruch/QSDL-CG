/****************************************************************************
 *                 Modul: $RCSfile: CGMain.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/02/13 06:51:41 $
 * $Revision: 1.14 $
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
#include <DS/DSAtom.h>

#include "CGFile.h"
#include "CGMain.h"

#ifdef DEBUG
#include <dmalloc.h>
#endif

/****************************************************************************
 * Konstruktoren
 ****************************************************************************/

CGMain::CGMain(void)
{
}

/****************************************************************************
 * Destruktor
 ****************************************************************************/

CGMain::~CGMain(void)
{
}

/****************************************************************************
 * CGWriteMain(): Oberfunktion der Codegenerierung fuer das Main-File
 *                -> writer
 *                Ergebnis: DS_OK, falls alles geklappt hat, sonst DS_ERROR
 *                Seiteneffekte: Anlegen und Fuellen des .cc-Files
 ****************************************************************************/

DSResult CGMain::CGWriteMain(CGWriter *writer) const
{
  DSResult  result;
  DSString  filename(FILE_MAIN);
  DSString *system_name;
  DSString  str;
  DSSystem *system;
  
  system = writer->CGGetSystem();
  assert(system);
  system_name = writer->CGGetSystem()->GetName();
  assert(system_name);

  if (writer->CGIncrementalCodeGeneration())
  {
    filename += SUFFIX_C_INC;
  }
  else
  {
    filename += SUFFIX_C;
  }

  CGFile out(writer->CGGetOutputDir(),
             &filename,
             writer->CGPrettyPrinting());

  if (!(result = out.CGFileOpen()))
  {
    std::cerr << "Can't open " << out.CGFilePath()<< out.CGFileName() << std::endl;

    return DS_WRITE_ERROR;
  }
  writer->CGSetFilePointer(&out);

  /**** Includes der Systemdatei und der Prozessdateien **********************/

  InsertCopyright(out);
  InsertHead(out, "Includes", 0);
#ifdef macintosh
  InsertString(out, "#ifdef macintosh", 0, CG_WITH_EOL);
  InsertString(out, "#include <console.h>", 2, CG_WITH_EOL);
  InsertString(out, "#endif", 0, CG_WITH_EOL);
  InsertLine(out);
#endif
  InsertString(out, "#include \"", 0, CG_NO_EOL)
  InsertString(out, PREFIX_SYSTEM, 0, CG_NO_EOL)
  InsertString(out, system_name, 0, CG_NO_EOL);
  str.Format("%s\"", SUFFIX_H);
  InsertString(out, str, 0, CG_WITH_EOL);
  InsertLine(out);
  InsertIncludeSCL(out, "SCIndetSim.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(out, "SCBitState.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(out, "SCFullState.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(out, "SCIndetVal.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(out, "SCThreadTypes.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(out, "SCMainThreadTypes.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(out, "SCScheduler.h", 0, CG_WITH_EOL);
  InsertIncludeSCL(out, "SCSocket.h", 0, CG_WITH_EOL);
  if (system->GetAtomList())
  {
    InsertIncludeTL(out, "TL.h", 0, CG_WITH_EOL);
    InsertIncludeTL(out, "TLIndetVerify.h", 0, CG_WITH_EOL);
  }
  InsertLine(out);
#ifndef _PRODUCTION_RELEASE
  InsertHead(out, "Variablen zur Speicherung der Profiling-Informationen", 0);
  InsertString(out, "#if _SC_PROFILING", 0, CG_WITH_EOL);
  InsertString(out, "int trans_ineff = 0;", 0, CG_WITH_EOL);
  InsertString(out, "int timer_ineff = 0;", 0, CG_WITH_EOL);
  InsertString(out, "int set_timer_ineff = 0;", 0, CG_WITH_EOL);
  InsertString(out, "int locate_timer_ineff = 0;", 0, CG_WITH_EOL);
  InsertString(out, "int remove_timer_ineff = 0;", 0, CG_WITH_EOL);
  InsertString(out, "int kill_timer_ineff = 0;", 0, CG_WITH_EOL);
  InsertString(out, "int table_ineff = 0;", 0, CG_WITH_EOL);
  InsertString(out, "int list_ineff = 0;", 0, CG_WITH_EOL);
  InsertString(out, "int save_ineff = 0;", 0, CG_WITH_EOL);
  InsertString(out, "int restore_ineff = 0;", 0, CG_WITH_EOL);
  InsertString(out, "int size_ineff = 0;", 0, CG_WITH_EOL);
  InsertString(out, "#endif", 0, CG_WITH_EOL);
  InsertLine(out);
#endif
  InsertHead(out, "Variablen zur Speicherung der Laufzeitoptionen", 0);
  InsertString(out, "SCTime run_time = 0.0;", 0, CG_WITH_EOL);
  InsertString(out, "SCIndet *indet = NULL;", 0, CG_WITH_EOL);
  InsertString(out, "SCSocket *sensor_socket = NULL;", 0, CG_WITH_EOL);
  InsertString(out, "char indet_name[80] = \"\";", 0, CG_WITH_EOL);
  InsertString(out, "char search_name[80] = \"\";", 0, CG_WITH_EOL);
  InsertString(out, "char formula[256] = \"\";", 0, CG_WITH_EOL);
  InsertString(out, "SCIndetType indet_type = scIndetSimulation;", 0, CG_WITH_EOL);
  InsertString(out, "SCStateSpaceType search_type = scBitStateSpace;", 0, CG_WITH_EOL);
  InsertString(out, "SCBoolean verbose = false;", 0, CG_WITH_EOL);
  InsertString(out, "SCBoolean fair = false;", 0, CG_WITH_EOL);
  InsertString(out, "SCBoolean find_errors = false;", 0, CG_WITH_EOL);
  InsertString(out, "SCNatural num_errors = 0;", 0, CG_WITH_EOL);
  InsertString(out, "SCBoolean find_dead_code = false;", 0, CG_WITH_EOL);
  InsertString(out, "SCBoolean find_loop = false;", 0, CG_WITH_EOL);
  InsertString(out, "SCNatural hash_table_power = kSCHashTablePower;", 0, CG_WITH_EOL);
  InsertString(out, "SCNatural hash_function_index = kSCHashFunctionIndex;", 0, CG_WITH_EOL);
  InsertString(out, "SCNatural max_depth = kSCMaxDepth;", 0, CG_WITH_EOL);
  InsertString(out, "SCBoolean msc_tracing = false;", 0, CG_WITH_EOL);
  InsertString(out, "SCBoolean socket_tracing = false;", 0, CG_WITH_EOL);
  if (writer->CGSupportPEV())
  {
    InsertString(out, "SCBoolean pev_tracing = false;", 0, CG_WITH_EOL);
  }
  InsertString(out,
               "char trace_file_name[MAX_PATH] = \"",
               0, CG_NO_EOL);
  str.Format("%s%s\";",
             FILE_MSC_TRACE, SUFFIX_MSC_TRACE);
  InsertString(out, str, 0, CG_WITH_EOL);
  if (writer->CGSupportPEV())
  {
    str.Format("char config_file_name[MAX_PATH] = \"%s%s\";",
               system_name->GetString(), SUFFIX_PEV);
    InsertString(out, str, 0, CG_WITH_EOL);
  }
  InsertString(out, "SCReal display_rate = 1.0;", 0, CG_WITH_EOL);
  InsertString(out, "char host_name[64] = \"\";", 0, CG_WITH_EOL);
  InsertString(out, "SCNatural port_number = 20000;", 0, CG_WITH_EOL);
  InsertLine(out);

  if (system->GetAtomList())
  {
    InsertHead(out, "Array der Atome (statisch)", 0);
    InsertString(out, "SCName2PropFuncPtr ", 0, CG_NO_EOL);
    InsertString(out, PREFIX_VARIABLE, 0, CG_NO_EOL);
    InsertString(out, "name2FuncTable[", 0, CG_NO_EOL);
    InsertCardinal(out, system->GetAtomList()->NumberOfElements(), 0, CG_NO_EOL);
    InsertString(out, "] =", 0, CG_WITH_EOL);
    InsertString(out, "{", 0, CG_WITH_EOL);
    writer->CGSetIndentPos(2);
    result = system->Run(writer, DS_ATOM, CG_WRITE_INIT);
    if (result != DS_OK)
    {
      return DS_ERROR;
    }
    InsertLine(out);
    InsertString(out, "};", 0, CG_WITH_EOL);
    InsertLine(out);

    InsertString(out, "char quit_atom_name[64] = \"\";", 0, CG_WITH_EOL);
    InsertString(out, "SCName2PropFuncPtr * quit_atom = NULL;", 0, CG_WITH_EOL);
    InsertLine(out);
  }

  writer->CGSetIndentPos(0);
  if ((result = CGWriteHelpFunction(writer)) != DS_OK) return result;
  InsertLine(out);
  writer->CGSetIndentPos(0);
  if ((result = CGWriteMessageFunction(writer)) != DS_OK) return result;
  InsertLine(out);
  writer->CGSetIndentPos(0);
  if ((result = CGWriteParseOptions(writer)) != DS_OK) return result;

  /******** Beginn des Hauptprogrammes ***************************************/

  InsertLine(out);
  InsertHead(out, "Hauptfunktion des Evaluators", 0);
  InsertString(out, "int main(int argc, char *argv[])", 0, CG_WITH_EOL);
  InsertString(out, "{", 0, CG_WITH_EOL);

  InsertString(out, "SCStateSpace * state_space = NULL;", 2, CG_WITH_EOL);
  InsertString(out, "SCStateSpace * cycle_state_space = NULL;", 2, CG_WITH_EOL);
  InsertLine(out);

  InsertString(out, "PrintMessage();", 2, CG_WITH_EOL);
  InsertLine(out);
  InsertHead(out, "Laufzeitoptionen parsen:", 2);

#ifdef macintosh
  InsertString(out, "#ifdef macintosh", 2, CG_WITH_EOL);
  InsertString(out, "argc = ccommand (&argv);", 4, CG_WITH_EOL);
  InsertString(out, "#endif", 2, CG_WITH_EOL);
#endif

  InsertString(out, "if (ParseOptions(&argc, argv) != 1)", 2, CG_WITH_EOL);
  InsertString(out, "{", 2, CG_WITH_EOL);
  InsertString(out, "PrintHelp();", 4, CG_WITH_EOL);
  InsertString(out, "exit(-1);", 4, CG_WITH_EOL); 
  InsertString(out, "}", 2, CG_WITH_EOL); 
  InsertLine(out);

  InsertString(out, "switch (indet_type)", 2, CG_WITH_EOL); 
  InsertString(out, "{", 2, CG_WITH_EOL);
  InsertString(out, "case scIndetValidation:", 4, CG_WITH_EOL);
  InsertString(out, "switch (search_type)", 6, CG_WITH_EOL);
  InsertString(out, "{", 6, CG_WITH_EOL);
  InsertString(out, "case scBitStateSpace:", 8, CG_WITH_EOL);
  InsertString(out, "default:", 8, CG_WITH_EOL);
  InsertString(out, "state_space = new SCBitState(hash_table_power, hash_function_index);", 10, CG_WITH_EOL);
  InsertString(out, "break;", 10, CG_WITH_EOL);
  InsertString(out, "case scFullStateSpace:", 8, CG_WITH_EOL);
  InsertString(out, "state_space = new SCFullState(hash_table_power, hash_function_index);", 10, CG_WITH_EOL);
  InsertString(out, "break;", 10, CG_WITH_EOL);
  InsertString(out, "}", 6, CG_WITH_EOL);
  InsertString(out, "assert(state_space);", 6, CG_WITH_EOL);
  InsertString(out, "if (find_loop)", 6, CG_WITH_EOL);
  InsertString(out, "{", 6, CG_WITH_EOL);
  InsertString(out, "switch (search_type)", 8, CG_WITH_EOL);
  InsertString(out, "{", 8, CG_WITH_EOL);
  InsertString(out, "case scBitStateSpace:", 10, CG_WITH_EOL);
  InsertString(out, "default:", 10, CG_WITH_EOL);
  InsertString(out, "cycle_state_space = new SCBitState(hash_table_power, hash_function_index);", 12, CG_WITH_EOL);
  InsertString(out, "break;", 12, CG_WITH_EOL);
  InsertString(out, "case scFullStateSpace:", 10, CG_WITH_EOL);
  InsertString(out, "cycle_state_space = new SCFullState(hash_table_power, hash_function_index);", 12, CG_WITH_EOL);
  InsertString(out, "break;", 12, CG_WITH_EOL);
  InsertString(out, "}", 8, CG_WITH_EOL);
  InsertString(out, "assert(cycle_state_space);", 8, CG_WITH_EOL);
  InsertString(out, "}", 6, CG_WITH_EOL);
  InsertString(out, "indet = new SCIndetVal(run_time, find_errors, num_errors, ",
               6, CG_NO_EOL);
  InsertString(out, "find_dead_code, find_loop, ", 0, CG_NO_EOL);
  InsertString(out, "state_space, cycle_state_space, max_depth, kSCMaxCreatedInstances",
               0, CG_NO_EOL);
  if (system->GetAtomList())
  {
    InsertString(out, ", quit_atom);", 0, CG_WITH_EOL);
  }
  else
  {
    InsertString(out, ");", 0, CG_WITH_EOL);
  }
  InsertString(out, "assert(indet);", 6, CG_WITH_EOL);
  InsertString(out, "break;", 6, CG_WITH_EOL);

  if (system->GetAtomList())
  {
    InsertString(out, "case scIndetVerification:", 4, CG_WITH_EOL);
    InsertString(out, "if (formula[0] == '\\0')", 6, CG_WITH_EOL);
    InsertString(out, "{", 6, CG_WITH_EOL);
    InsertString(out, "std::cout << \"No formula to be verified. Use -r option!\" << std::endl << std::endl;", 8, CG_WITH_EOL); 
    InsertString(out, "exit(-1);", 8, CG_WITH_EOL);
    InsertString(out, "}", 6, CG_WITH_EOL);
    InsertString(out, "switch (search_type)", 6, CG_WITH_EOL);
    InsertString(out, "{", 6, CG_WITH_EOL);
    InsertString(out, "case scBitStateSpace:", 8, CG_WITH_EOL);
    InsertString(out, "default:", 8, CG_WITH_EOL);
    InsertString(out, "state_space = new SCBitState(hash_table_power, hash_function_index);", 10, CG_WITH_EOL);
    InsertString(out, "cycle_state_space = new SCBitState(hash_table_power, hash_function_index);", 10, CG_WITH_EOL);
    InsertString(out, "break;", 10, CG_WITH_EOL);
    InsertString(out, "case scFullStateSpace:", 8, CG_WITH_EOL);
    InsertString(out, "state_space = new SCFullState(hash_table_power, hash_function_index);", 10, CG_WITH_EOL);
    InsertString(out, "cycle_state_space = new SCFullState(hash_table_power, hash_function_index);", 10, CG_WITH_EOL);
    InsertString(out, "break;", 10, CG_WITH_EOL);
    InsertString(out, "}", 6, CG_WITH_EOL);
    InsertString(out, "assert(state_space);", 6, CG_WITH_EOL);
    InsertString(out, "assert(cycle_state_space);", 6, CG_WITH_EOL);
    InsertString(out, "indet = new TLIndetVerify(formula, ", 6, CG_NO_EOL);
    InsertString(out, PREFIX_VARIABLE, 0, CG_NO_EOL);
    InsertString(out, "name2FuncTable, ", 0, CG_NO_EOL);
    InsertCardinal(out, writer->CGGetLastAtomID(), 0, CG_NO_EOL);
    InsertString(out,
                 ", run_time, true, num_errors, find_dead_code, state_space",
                 0, CG_NO_EOL);
    InsertString(out, ", cycle_state_space, max_depth, quit_atom);", 0, CG_WITH_EOL);
    InsertString(out, "assert(indet);", 6, CG_WITH_EOL);
    InsertString(out, "if (!((TLIndetVerify *)indet)->IsFulFillable())", 6, CG_WITH_EOL);
    InsertString(out, "{", 6, CG_WITH_EOL);
    InsertString(out, "delete indet;", 8, CG_WITH_EOL);
    InsertString(out, "std::cout << formula << \" is not fulfillable!\" << std::endl;", 8, CG_WITH_EOL); 
    InsertString(out, "exit(-1);", 8, CG_WITH_EOL);
    InsertString(out, "}", 6, CG_WITH_EOL);
    InsertString(out, "break;", 6, CG_WITH_EOL);
  }
  InsertString(out, "case scIndetSimulation:", 4, CG_WITH_EOL);
  InsertString(out, "indet = new SCIndetSim(run_time, find_errors, ", 6, CG_NO_EOL);
  InsertString(out, "num_errors, find_dead_code, max_depth", 0, CG_NO_EOL);
  if (system->GetAtomList())
  {
    InsertString(out, ", quit_atom);", 0, CG_WITH_EOL);
  }
  else
  {
    InsertString(out, ");", 0, CG_WITH_EOL);
  }
  
  InsertString(out, "assert(indet);", 6, CG_WITH_EOL);
  InsertString(out, "break;", 6, CG_WITH_EOL);
  InsertString(out, "default:", 4, CG_WITH_EOL);
  InsertString(out, "std::cerr << std::endl << \"Illegal indet type!\" << std::endl;", 6, CG_WITH_EOL);
  InsertString(out, "exit(-1);", 6, CG_WITH_EOL);
  InsertString(out, "}", 2, CG_WITH_EOL);
  InsertLine(out);

  InsertHead(out, "Socket initialisieren:", 2);
  InsertString(out, "if (host_name[0] != '\\0')", 2, CG_WITH_EOL);
  InsertString(out, "{", 2, CG_WITH_EOL);
  InsertString(out, "sensor_socket = new SCSocket(host_name, port_number);", 4, CG_WITH_EOL);
  InsertString(out, "assert(sensor_socket);", 4, CG_WITH_EOL);
  InsertString(out, "}", 2, CG_WITH_EOL);
  InsertLine(out);
  
  InsertHead(out, "Betriebssystem-Threads initialisieren:", 2);
  InsertString(out,
               "if (verbose) std::cout << \"Initializing system threads...\" << std::endl;",
               2, CG_WITH_EOL);
  InsertString(out, "SCSystemThread::Initialize();", 2, CG_WITH_EOL);
  InsertLine(out);

  InsertHead(out, "Main-Thread-Struktur initialisieren:", 2);
  InsertString(out,
               "if (verbose) std::cout << \"Initializing main thread...\" << std::endl;",
               2, CG_WITH_EOL);
  InsertString(out, "SCMainThread::Initialize();", 2, CG_WITH_EOL);
  InsertLine(out);

  /******** System initialisieren *****************************************/

  InsertHead(out, "Laufzeitsystem initialisieren:", 2);
  InsertString(out, "if (", 2, CG_NO_EOL);
  InsertString(out, PREFIX_SYSTEM, 0, CG_NO_EOL);
  InsertString(out, system_name, 0, CG_NO_EOL);
  InsertString(out, "::Initialize(indet, sensor_socket, fair, verbose", 0, CG_NO_EOL);
  InsertString(out, ", msc_tracing", 0, CG_NO_EOL);
  InsertString(out, ", socket_tracing", 0, CG_NO_EOL);
  if (writer->CGSupportPEV())
  {
    InsertString(out, ", pev_tracing", 0, CG_NO_EOL);
  }
  InsertString(out, ", trace_file_name",
               0, CG_NO_EOL);
  InsertString(out, ", host_name, port_number",
               0, CG_NO_EOL);
  if (writer->CGSupportPEV())
  {
    InsertString(out, ", config_file_name",
                 0, CG_NO_EOL);
  }
  InsertString(out, ", display_rate))", 0, CG_WITH_EOL);
  InsertString(out, "{", 2, CG_WITH_EOL);
  InsertHead(out, "Evaluator starten:", 4);
  InsertString(out, "indet->StartMessage();", 4, CG_WITH_EOL);
  InsertString(out, "SCScheduler::Run();", 4, CG_WITH_EOL);
  InsertString(out, "indet->EndMessage();", 4, CG_WITH_EOL);
  InsertString(out, "}", 2, CG_WITH_EOL);
  InsertLine(out);

  InsertHead(out, "Laufzeitsystem beenden:", 2);
  InsertString(out,
               "if (verbose) std::cout << \"Finishing...\" << std::endl;",
               2, CG_WITH_EOL);
  InsertString(out, PREFIX_SYSTEM, 2, CG_NO_EOL);
  InsertString(out, system_name, 0, CG_NO_EOL);
  InsertString(out, "::Finish();", 0, CG_WITH_EOL);
  InsertString(out, "SCScheduler::Finish();", 2, CG_WITH_EOL);
  InsertString(out, "SCMainThread::Finish();", 2, CG_WITH_EOL);
  InsertLine(out);

#ifndef _PRODUCTION_RELEASE
  InsertString(out, "#if _SC_PROFILING", 0, CG_WITH_EOL);
  InsertString(out, "std::cout << \"trans_ineff = \" << trans_ineff << std::endl;", 2, CG_WITH_EOL);
  InsertString(out, "std::cout << \"timer_ineff = \" << timer_ineff << std::endl;", 2, CG_WITH_EOL);
  InsertString(out, "std::cout << \"set_timer_ineff = \" << set_timer_ineff << std::endl;", 2, CG_WITH_EOL);
  InsertString(out, "std::cout << \"locate_timer_ineff = \" << locate_timer_ineff << std::endl;", 2, CG_WITH_EOL);
  InsertString(out, "std::cout << \"remove_timer_ineff = \" << remove_timer_ineff << std::endl;", 2, CG_WITH_EOL);
  InsertString(out, "std::cout << \"kill_timer_ineff = \" << kill_timer_ineff << std::endl;", 2, CG_WITH_EOL);
  InsertString(out, "std::cout << \"table_ineff = \" << table_ineff << std::endl;", 2, CG_WITH_EOL);
  InsertString(out, "std::cout << \"list_ineff = \" << list_ineff << std::endl;", 2, CG_WITH_EOL);
  InsertString(out, "std::cout << \"save_ineff = \" << save_ineff << std::endl;", 2, CG_WITH_EOL);
  InsertString(out, "std::cout << \"restore_ineff = \" << restore_ineff << std::endl;", 2, CG_WITH_EOL);
  InsertString(out, "std::cout << \"size_ineff = \" << size_ineff << std::endl;", 2, CG_WITH_EOL);
  InsertString(out, "#endif", 0, CG_WITH_EOL);
  InsertLine(out);
#endif
  InsertString(out, "return 0;", 2, CG_WITH_EOL);
  InsertString(out, "}", 0, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * CGWriteParseOptions: Funktion wertet die Optionen die dem Validator
 *                      uebergeben werden aus.
 *                      -> writer
 *                      Ergebnis: DS_OK oder DS_ERROR
 ****************************************************************************/

DSResult CGMain::CGWriteParseOptions(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos   pos;
  DSSystem *system;
  
  system = writer->CGGetSystem();

  out = writer->CGGetFilePointer();
  assert(out);
  pos = 0;

  InsertHead(*out, "Funktion parst die Laufzeitoptionen des Evaluators", 0);
  InsertString(*out, "static int ParseOptions (int *argc, char **argv)", pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "unsigned int ci, errs = 0;", pos + 2, CG_WITH_EOL);
  InsertString(*out, "int ai;", pos + 2, CG_WITH_EOL);
  InsertString(*out, "int new_argc = 1;", pos +2, CG_WITH_EOL);
  InsertString(*out, "char trace_type_name[80];", pos + 2, CG_WITH_EOL);
  if (system->GetAtomList())
  {
    InsertString(*out, "int i;", pos + 2, CG_WITH_EOL);
  }
  InsertLine(*out);

  InsertString(*out, "for (ai = 1;", pos + 2, CG_WITH_EOL);                                  /* alle Argumente durch-    */
  InsertString(*out, "ai < *argc;", pos + 7, CG_WITH_EOL);
  InsertString(*out, "ai++)", pos + 7, CG_WITH_EOL);
  InsertString(*out, "{", pos + 2, CG_WITH_EOL);
  InsertString(*out, "if (argv[ai][0] == '-')", pos + 4, CG_WITH_EOL);
  InsertString(*out, "{", pos + 4, CG_WITH_EOL);
  InsertString(*out, "for (ci = 1;", pos + 6, CG_WITH_EOL);                              
  InsertString(*out, "ci < strlen(argv[ai]);", pos + 11, CG_WITH_EOL);              
  InsertString(*out, "ci++)", pos + 11, CG_WITH_EOL);                               
  InsertString(*out, "{", pos + 6, CG_WITH_EOL);
  InsertString(*out, "switch(argv[ai][ci])", pos + 8, CG_WITH_EOL);                   
  InsertString(*out, "{", pos + 8, CG_WITH_EOL);
  InsertString(*out, " ", pos + 2, CG_WITH_EOL);

  InsertString(*out, "case 'h':", pos + 10, CG_WITH_EOL);
  InsertString(*out, "PrintHelp();", pos + 12, CG_WITH_EOL);
  InsertString(*out, "exit(0);", pos + 12, CG_WITH_EOL);
  InsertString(*out, "break;", pos + 12, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "case 'v':", pos + 10, CG_WITH_EOL);
  InsertString(*out, "verbose = true;", pos + 12, CG_WITH_EOL);
  InsertString(*out, "break;", pos + 12, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "case 'y': ", pos + 10, CG_WITH_EOL);                            
  InsertString(*out, "if (ci < strlen(argv[ai]) - 1)", pos + 12, 
               CG_WITH_EOL);      
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);                                   
  InsertString(*out, "strcpy(indet_name, &(argv[ai][ci + 1]));", 
               pos + 14, CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "else", pos + 12, CG_WITH_EOL);
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);
  InsertString(*out, "if (ai + 1 == *argc)", pos + 14, CG_WITH_EOL);             
  InsertString(*out, "{", pos + 14, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"Analysis type is missing after -y\" << std::endl << std::endl;", 
               pos + 16, CG_WITH_EOL);
  InsertString(*out, "return -1;", pos + 16, CG_WITH_EOL);
  InsertString(*out, " }", pos + 14, CG_WITH_EOL);
  InsertString(*out, "strcpy(indet_name, argv[++ai]);", pos + 14, 
               CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "if (!strcasecmp(indet_name, \"s\"))", pos + 12, CG_WITH_EOL);
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);
  InsertString(*out, "indet_type = scIndetSimulation;", pos + 14, CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "else if (!strcasecmp(indet_name, \"r\"))", pos + 12, CG_WITH_EOL);
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);
  InsertString(*out, "indet_type = scIndetValidation;", pos + 14, CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  if (system->GetAtomList())
  {
    InsertString(*out, "else if (!strcasecmp(indet_name, \"v\"))", pos + 12, CG_WITH_EOL);
    InsertString(*out, "{", pos + 12, CG_WITH_EOL);
    InsertString(*out, "indet_type = scIndetVerification;", pos + 14, CG_WITH_EOL);
    InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  }
  InsertString(*out, "else", pos + 12, CG_WITH_EOL);
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);
  InsertString(*out, "  std::cerr << \"Unknown analysis type.\" << std::endl << std::endl;", pos + 12, CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "ci = strlen(argv[ai]);", pos + 12, CG_WITH_EOL);
  InsertString(*out, "break;", pos + 12, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "case 'f':", pos + 10, CG_WITH_EOL);
  InsertString(*out, "fair = true;", pos + 12, CG_WITH_EOL);
  InsertString(*out, "break;", pos + 12, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "case 'e': ", pos + 10, CG_WITH_EOL);                            
  InsertString(*out, "find_errors = true;", pos + 12, CG_WITH_EOL);
  InsertString(*out, "if (ci < strlen(argv[ai]) - 1)", pos + 12, 
               CG_WITH_EOL);      
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);                                   
  InsertString(*out, "num_errors = atoi(&(argv[ai][ci + 1]));", 
               pos + 14, CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "else", pos + 12, CG_WITH_EOL);
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);
  InsertString(*out, "if (ai + 1 == *argc)", pos + 14, CG_WITH_EOL);             
  InsertString(*out, "{", pos + 14, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"Num of errors is missing after -e \" << std::endl << std::endl;", 
               pos + 16, CG_WITH_EOL);
  InsertString(*out, "return -1;", pos + 16, CG_WITH_EOL);
  InsertString(*out, " }", pos + 14, CG_WITH_EOL);
  InsertString(*out, "num_errors = atoi(argv[++ai]);", pos + 14, 
               CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "ci = strlen(argv[ai]);", pos + 12, CG_WITH_EOL);           
  InsertString(*out, "break;", pos + 12, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "case 's': ", pos + 10, CG_WITH_EOL);                            
  InsertString(*out, "if (ci < strlen(argv[ai]) - 1)", pos + 12, 
               CG_WITH_EOL);      
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);                                   
  InsertString(*out, "strcpy(search_name, &(argv[ai][ci + 1]));", 
               pos + 14, CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "else", pos + 12, CG_WITH_EOL);
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);
  InsertString(*out, "if (ai + 1 == *argc)", pos + 14, CG_WITH_EOL);             
  InsertString(*out, "{", pos + 14, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"Search type is missing after -s\" << std::endl << std::endl;", 
               pos + 16, CG_WITH_EOL);
  InsertString(*out, "return -1;", pos + 16, CG_WITH_EOL);
  InsertString(*out, " }", pos + 14, CG_WITH_EOL);
  InsertString(*out, "strcpy(search_name, argv[++ai]);", pos + 14, 
               CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "if (!strcasecmp(search_name, \"bs\"))", pos + 12, CG_WITH_EOL);
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);
  InsertString(*out, "search_type = scBitStateSpace;", pos + 14, CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "else if (!strcasecmp(search_name, \"fs\"))", pos + 12, CG_WITH_EOL);
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);
  InsertString(*out, "search_type = scFullStateSpace;", pos + 14, CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "else", pos + 12, CG_WITH_EOL);
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);
  InsertString(*out, "  std::cerr << \"Unknown search type.\" << std::endl << std::endl;", pos + 12, CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "ci = strlen(argv[ai]);", pos + 12, CG_WITH_EOL);
  InsertString(*out, "break;", pos + 12, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "case 'u':", pos + 10, CG_WITH_EOL);
  InsertString(*out, "find_dead_code = true;", pos + 12, CG_WITH_EOL);
  InsertString(*out, "break;", pos + 12, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "case 'l':", pos + 10, CG_WITH_EOL);
  InsertString(*out, "find_loop = true;", pos + 12, CG_WITH_EOL);
  InsertString(*out, "break;", pos + 12, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "case 'p': ", pos + 10, CG_WITH_EOL);                            
  InsertString(*out, "if (ci < strlen(argv[ai]) - 1)", pos + 12, 
               CG_WITH_EOL);      
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);                                   
  InsertString(*out, "hash_table_power = atoi(&(argv[ai][ci + 1]));", 
               pos + 14, CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "else", pos + 12, CG_WITH_EOL);
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);
  InsertString(*out, "if (ai + 1 == *argc)", pos + 14, CG_WITH_EOL);
  InsertString(*out, "{", pos + 14, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"Hash table power is missing after -p\" << std::endl << std::endl;",
               pos + 16, CG_WITH_EOL);
  InsertString(*out, "return -1;", pos + 16, CG_WITH_EOL);
  InsertString(*out, " }", pos + 14, CG_WITH_EOL);
  InsertString(*out, "hash_table_power = atoi(argv[++ai]);", pos + 14,
               CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "ci = strlen(argv[ai]);", pos + 12, CG_WITH_EOL);
  InsertString(*out, "break;", pos + 12, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "case 'i': ", pos + 10, CG_WITH_EOL);                            
  InsertString(*out, "if (ci < strlen(argv[ai]) - 1)", pos + 12,
               CG_WITH_EOL);
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);                                   
  InsertString(*out, "hash_function_index = atoi(&(argv[ai][ci + 1]));", 
               pos + 14, CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "else", pos + 12, CG_WITH_EOL);
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);
  InsertString(*out, "if (ai + 1 == *argc)", pos + 14, CG_WITH_EOL);             
  InsertString(*out, "{", pos + 14, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"Hash function index is missing after -i\" << std::endl << std::endl;", 
               pos + 16, CG_WITH_EOL);
  InsertString(*out, "return -1;", pos + 16, CG_WITH_EOL);
  InsertString(*out, " }", pos + 14, CG_WITH_EOL);
  InsertString(*out, "hash_function_index = atoi(argv[++ai]);", pos + 14, 
               CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "ci = strlen(argv[ai]);", pos + 12, CG_WITH_EOL);           
  InsertString(*out, "break;", pos + 12, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "case 'r': ", pos + 10, CG_WITH_EOL);                            
  InsertString(*out, "if (ci < strlen(argv[ai]) - 1)", pos + 12, 
               CG_WITH_EOL);      
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);                                   
  InsertString(*out, "strcpy(formula, &(argv[ai][ci + 1]));", 
               pos + 14, CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "else", pos + 12, CG_WITH_EOL);
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);
  InsertString(*out, "if (ai + 1 == *argc)", pos + 14, CG_WITH_EOL);             
  InsertString(*out, "{", pos + 14, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"Temporal formula is missing after -r \" << std::endl << std::endl;",
               pos + 16, CG_WITH_EOL);
  InsertString(*out, "return -1;", pos + 16, CG_WITH_EOL);
  InsertString(*out, " }", pos + 14, CG_WITH_EOL);
  InsertString(*out, "strcpy(formula, argv[++ai]);", pos + 14, 
               CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "ci = strlen(argv[ai]);", pos + 12, CG_WITH_EOL);           
  InsertString(*out, "break;", pos + 12, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "case 'd': ", pos + 10, CG_WITH_EOL);                            
  InsertString(*out, "if (ci < strlen(argv[ai]) - 1)", pos + 12, 
               CG_WITH_EOL);      
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);                                   
  InsertString(*out, "max_depth = atoi(&(argv[ai][ci + 1]));", 
               pos + 14, CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "else", pos + 12, CG_WITH_EOL);
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);
  InsertString(*out, "if (ai + 1 == *argc)", pos + 14, CG_WITH_EOL);             
  InsertString(*out, "{", pos + 14, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"Validation depth is missing after -d\" << std::endl << std::endl;",
               pos + 16, CG_WITH_EOL);
  InsertString(*out, "return -1;", pos + 16, CG_WITH_EOL);
  InsertString(*out, " }", pos + 14, CG_WITH_EOL);
  InsertString(*out, "max_depth = atoi(argv[++ai]);", pos + 14,
               CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "ci = strlen(argv[ai]);", pos + 12, CG_WITH_EOL);           
  InsertString(*out, "break;", pos + 12, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "case 't':", pos + 10, CG_WITH_EOL);
  InsertString(*out, "if (ci < strlen(argv[ai]) - 1)", pos + 12, CG_WITH_EOL);
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);
  InsertString(*out, "strcpy(trace_type_name, &(argv[ai][ci + 1]));", pos + 14, CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "else", pos + 12, CG_WITH_EOL);
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);
  InsertString(*out, "if (ai + 1 == *argc)", pos + 14, CG_WITH_EOL);
  InsertString(*out, "{", pos + 14, CG_WITH_EOL);
  InsertString(*out, "std::cerr << \"Trace type is missing after -t.\" << std::endl << std::endl;", pos + 16, CG_WITH_EOL);
  InsertString(*out, "return -1;", pos + 16, CG_WITH_EOL);
  InsertString(*out, "}", pos + 14, CG_WITH_EOL);
  InsertString(*out, "strcpy(trace_type_name, argv[++ai]);", pos + 14, CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "if (!strcasecmp(trace_type_name, \"", pos + 12, CG_NO_EOL);
  InsertString(*out, CG_TRACE_TYPE_NAME_MSC, 0, CG_NO_EOL);
  InsertString(*out, "\"))", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);
  InsertString(*out, "msc_tracing = true;", pos + 14, CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "else if (!strcasecmp(trace_type_name, \"", pos + 12, CG_NO_EOL);
  InsertString(*out, CG_TRACE_TYPE_NAME_SOCKET, 0, CG_NO_EOL);
  InsertString(*out, "\"))", 0, CG_WITH_EOL);
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);
  InsertString(*out, "socket_tracing = true;", pos + 14, CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  if (writer->CGSupportPEV())
  {
    InsertString(*out, "else if (!strcasecmp(trace_type_name, \"", pos + 12, CG_NO_EOL);
    InsertString(*out, CG_TRACE_TYPE_NAME_PEV, 0, CG_NO_EOL);
    InsertString(*out, "\"))", 0, CG_WITH_EOL);
    InsertString(*out, "{", pos + 12, CG_WITH_EOL);
    InsertString(*out, "pev_tracing = true;", pos + 14, CG_WITH_EOL);
    InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  }
  InsertString(*out, "else", pos + 12, CG_WITH_EOL);
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);
  InsertString(*out, "std::cerr << \"Unknown trace type.\" << std::endl << std::endl;", pos + 14, CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "ci = strlen(argv[ai]);", pos + 12, CG_WITH_EOL);
  InsertString(*out, "break;", pos + 12, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "case 'o':", pos + 10, CG_WITH_EOL);
  InsertString(*out, "if (ci < strlen(argv[ai]) - 1)", pos + 12, 
               CG_WITH_EOL);
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);
  InsertString(*out, "strcpy(trace_file_name, &(argv[ai][ci + 1]));", 
               pos + 14, CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "else ", pos + 12, CG_WITH_EOL);                               
  InsertString(*out, "{ ", pos + 12, CG_WITH_EOL);
  InsertString(*out, "if (ai + 1 == *argc)", pos + 14, CG_WITH_EOL);                               
  InsertString(*out, "{", pos + 14, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"Filename is missing after -o\" << std::endl << std::endl;",
               pos + 16, CG_WITH_EOL);                               
  InsertString(*out, "return -1;", pos + 16, CG_WITH_EOL);                               
  InsertString(*out, "}", pos + 14, CG_WITH_EOL);                               
  InsertString(*out, "strcpy(trace_file_name, argv[++ai]);", pos + 14,
               CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "ci = strlen(argv[ai]);", pos + 12, CG_WITH_EOL);                               
  InsertString(*out, "break;", pos + 12, CG_WITH_EOL);
  InsertLine(*out);
                                               
  if (system->GetAtomList())
  {
    InsertString(*out, "case 'q':", pos + 10, CG_WITH_EOL);
    InsertString(*out, "if (ci < strlen(argv[ai]) - 1)", pos + 12, CG_WITH_EOL);
    InsertString(*out, "{", pos + 12, CG_WITH_EOL);
    InsertString(*out, "strcpy(quit_atom_name, &(argv[ai][ci + 1]));", pos + 14, CG_WITH_EOL);
    InsertString(*out, "}", pos + 12, CG_WITH_EOL);
    InsertString(*out, "else", pos + 12, CG_WITH_EOL);
    InsertString(*out, "{", pos + 12, CG_WITH_EOL);
    InsertString(*out, "if (ai + 1 == *argc)", pos + 14, CG_WITH_EOL);
    InsertString(*out, "{", pos + 14, CG_WITH_EOL);
    InsertString(*out, "std::cerr << \"Quit atom name is missing after -q.\" << std::endl << std::endl;", pos + 16, CG_WITH_EOL);
    InsertString(*out, "return -1;", pos + 16, CG_WITH_EOL);
    InsertString(*out, "}", pos + 14, CG_WITH_EOL);
    InsertString(*out, "strcpy(quit_atom_name, argv[++ai]);", pos + 14, CG_WITH_EOL);
    InsertString(*out, "}", pos + 12, CG_WITH_EOL);
    InsertString(*out, "ci = strlen(argv[ai]);", pos + 12, CG_WITH_EOL);                               

    InsertString(*out, "for (i = 0; i < ", pos + 12, CG_NO_EOL);
    InsertCardinal(*out, system->GetAtomList()->NumberOfElements(), 0, CG_NO_EOL);
    InsertString(*out, "; i++)", 0, CG_WITH_EOL);
    InsertString(*out, "{", pos + 12, CG_WITH_EOL);
    InsertString(*out, "if (!strcasecmp(", pos + 14, CG_NO_EOL);
    InsertString(*out, PREFIX_VARIABLE, 0, CG_NO_EOL);
    InsertString(*out, "name2FuncTable[i].name, quit_atom_name))", 0, CG_WITH_EOL);
    InsertString(*out, "{", pos + 14, CG_WITH_EOL);
    InsertString(*out, "quit_atom = &", pos + 16, CG_NO_EOL);
    InsertString(*out, PREFIX_VARIABLE, 0, CG_NO_EOL);
    InsertString(*out, "name2FuncTable[i];", 0, CG_WITH_EOL);
    InsertString(*out, "break;", pos + 16, CG_WITH_EOL);
    InsertString(*out, "}", pos + 14, CG_WITH_EOL);
    InsertString(*out, "}", pos + 12, CG_WITH_EOL);
    InsertString(*out, "if (quit_atom == NULL)", pos + 12, CG_NO_EOL);
    InsertString(*out, "{", pos + 12, CG_WITH_EOL);
    InsertString(*out, "std::cerr << \"Quit atom name is not valid.\" << std::endl << std::endl;", pos + 14, CG_WITH_EOL);
    InsertString(*out, "return -1;", pos + 16, CG_WITH_EOL);
    InsertString(*out, "}", pos + 12, CG_WITH_EOL);
    InsertString(*out, "break;", pos + 12, CG_WITH_EOL);
    InsertLine(*out);
  }

  if (writer->CGSupportPEV())
  {
    InsertString(*out, "case 'c': ", pos + 10, CG_WITH_EOL);                            
    InsertString(*out, "if (ci < strlen(argv[ai]) - 1)", pos + 12, 
                 CG_WITH_EOL);      
    InsertString(*out, "{", pos + 12, CG_WITH_EOL);                                   
    InsertString(*out, "strcpy(config_file_name, &(argv[ai][ci + 1]));", 
                 pos + 14, CG_WITH_EOL);
    InsertString(*out, "}", pos + 12, CG_WITH_EOL);
    InsertString(*out, "else", pos + 12, CG_WITH_EOL);
    InsertString(*out, "{", pos + 12, CG_WITH_EOL);
    InsertString(*out, "if (ai + 1 == *argc)", pos + 14, CG_WITH_EOL);             
    InsertString(*out, "{", pos + 14, CG_WITH_EOL);
    InsertString(*out, "std::cout << \"Config file name is missing after -c\" << std::endl << std::endl;", 
                 pos + 16, CG_WITH_EOL);
    InsertString(*out, "return -1;", pos + 16, CG_WITH_EOL);
    InsertString(*out, " }", pos + 14, CG_WITH_EOL);
    InsertString(*out, "strcpy(config_file_name, argv[++ai]);", pos + 14, 
                 CG_WITH_EOL);
    InsertString(*out, "}", pos + 12, CG_WITH_EOL);
    InsertString(*out, "ci = strlen(argv[ai]);", pos + 12, CG_WITH_EOL);           
    InsertString(*out, "break;", pos + 12, CG_WITH_EOL);
    InsertLine(*out);

  }

  InsertString(*out, "case 'x': ", pos + 10, CG_WITH_EOL);                            
  InsertString(*out, "if (ci < strlen(argv[ai]) - 1)", pos + 12, 
               CG_WITH_EOL);      
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);                                   
  InsertString(*out, "display_rate = atof(&(argv[ai][ci + 1]));", 
               pos + 14, CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "else", pos + 12, CG_WITH_EOL);
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);
  InsertString(*out, "if (ai + 1 == *argc)", pos + 14, CG_WITH_EOL);             
  InsertString(*out, "{", pos + 14, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"Display rate is missing after -x\" << std::endl << std::endl;", 
               pos + 16, CG_WITH_EOL);
  InsertString(*out, "return -1;", pos + 16, CG_WITH_EOL);
  InsertString(*out, " }", pos + 14, CG_WITH_EOL);
  InsertString(*out, "display_rate = atof(argv[++ai]);", pos + 14, 
               CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "ci = strlen(argv[ai]);", pos + 12, CG_WITH_EOL);           
  InsertString(*out, "break;", pos + 12, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "case 'n': ", pos + 10, CG_WITH_EOL);                            
  InsertString(*out, "if (ci < strlen(argv[ai]) - 1)", pos + 12, 
               CG_WITH_EOL);      
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);                                   
  InsertString(*out, "port_number = atoi(&(argv[ai][ci + 1]));", 
               pos + 14, CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "else", pos + 12, CG_WITH_EOL);
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);
  InsertString(*out, "if (ai + 1 == *argc)", pos + 14, CG_WITH_EOL);             
  InsertString(*out, "{", pos + 14, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"Port number is missing\" << std::endl << std::endl;", 
               pos + 16, CG_WITH_EOL);
  InsertString(*out, "return -1;", pos + 16, CG_WITH_EOL);
  InsertString(*out, " }", pos + 14, CG_WITH_EOL);
  InsertString(*out, "port_number = atoi(argv[++ai]);", pos + 14, 
               CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "ci = strlen(argv[ai]);", pos + 12, CG_WITH_EOL);           
  InsertString(*out, "break;", pos + 12, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "case 'm':", pos + 10, CG_WITH_EOL);
  InsertString(*out, "if (ci < strlen(argv[ai]) - 1)", pos + 12, 
               CG_WITH_EOL);      
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);
  InsertString(*out, "strcpy(host_name, &(argv[ai][ci + 1]));", 
               pos + 14, CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "else ", pos + 12, CG_WITH_EOL);                               
  InsertString(*out, "{ ", pos + 12, CG_WITH_EOL); 
  InsertString(*out, "if (ai + 1 == *argc)", pos + 14, CG_WITH_EOL);                               
  InsertString(*out, "{", pos + 14, CG_WITH_EOL);                               
  InsertString(*out, "std::cout << \"Hostname is missing after -m\" << std::endl << std::endl;", 
               pos + 16, CG_WITH_EOL);                               
  InsertString(*out, "return -1;", pos + 16, CG_WITH_EOL);                               
  InsertString(*out, "}", pos + 14, CG_WITH_EOL);                               
  InsertString(*out, "strcpy(host_name, argv[++ai]);", pos + 14,
               CG_WITH_EOL);                               
  
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "ci = strlen(argv[ai]);", pos + 12, CG_WITH_EOL);                               
  InsertString(*out, "break;", pos + 12, CG_WITH_EOL);
  InsertLine(*out);
  
  InsertString(*out, "case 'z': ", pos + 10, CG_WITH_EOL);                            
  InsertString(*out, "if (ci < strlen(argv[ai]) - 1)", pos + 12, 
               CG_WITH_EOL);      
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);                                   
  InsertString(*out, "run_time = atof(&(argv[ai][ci + 1]));", 
               pos + 14, CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "else", pos + 12, CG_WITH_EOL);
  InsertString(*out, "{", pos + 12, CG_WITH_EOL);
  InsertString(*out, "if (ai + 1 == *argc)", pos + 14, CG_WITH_EOL);             
  InsertString(*out, "{", pos + 14, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"Max. modell time is missing\" << std::endl << std::endl;", 
               pos + 16, CG_WITH_EOL);
  InsertString(*out, "return -1;", pos + 16, CG_WITH_EOL);
  InsertString(*out, " }", pos + 14, CG_WITH_EOL);
  InsertString(*out, "run_time = atof(argv[++ai]);", pos + 14, 
               CG_WITH_EOL);
  InsertString(*out, "}", pos + 12, CG_WITH_EOL);
  InsertString(*out, "ci = strlen(argv[ai]);", pos + 12, CG_WITH_EOL);           
  InsertString(*out, "break;", pos + 12, CG_WITH_EOL);
  InsertLine(*out);

  InsertString(*out, "default:", pos + 10, CG_WITH_EOL)
  InsertString(*out, "std::cerr << \"Unknown option '\" << argv[ai][ci] << \"' !\" << std::endl;",
               pos + 12, CG_WITH_EOL);
  InsertString(*out, "errs++;", pos + 12, CG_WITH_EOL);
  InsertString(*out, "break;", pos + 12, CG_WITH_EOL);
  
  InsertString(*out, "}", pos + 8, CG_WITH_EOL);
  InsertString(*out, "}", pos + 6, CG_WITH_EOL);
  InsertString(*out, "}", pos + 4, CG_WITH_EOL);
  InsertString(*out, "else", pos + 4, CG_WITH_EOL);                                        
  InsertString(*out, "{", pos + 4, CG_WITH_EOL);
  InsertString(*out, "argv[new_argc] = argv[ai];", pos + 6, CG_WITH_EOL);
  InsertString(*out, "new_argc++;", pos + 6, CG_WITH_EOL);
  InsertString(*out, "}", pos + 4, CG_WITH_EOL);
  InsertString(*out, "}", pos + 2, CG_WITH_EOL);
  InsertString(*out, "*argc = new_argc;", pos + 2, CG_WITH_EOL);
  InsertString(*out, "if (errs > 0)", pos + 2, CG_WITH_EOL);
  InsertString(*out, "{", pos + 2, CG_WITH_EOL);
  InsertString(*out, "return -1;", pos + 4, CG_WITH_EOL);
  InsertString(*out, "}", pos + 2, CG_WITH_EOL);
  InsertString(*out, "else", pos + 2, CG_WITH_EOL);
  InsertString(*out, "{", pos + 2, CG_WITH_EOL);
  InsertString(*out, "return 1;", pos + 4, CG_WITH_EOL);
  InsertString(*out, "}", pos + 2, CG_WITH_EOL);
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * CGWriteHelpFunction: Funktion schreibt die Funktion, welche den Hilfe-
 *                      text ausgibt.
 *                      -> writer
 *                      Ergebnis: DS_OK oder DS_ERROR
 ****************************************************************************/

DSResult CGMain::CGWriteHelpFunction(CGWriter *writer) const
{
  DSResult result;
  DSString str;
  CGFile *out;
  CGPos   pos;
  DSSystem *system;
  
  system = writer->CGGetSystem();
  assert(system);

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertHead(*out, "Funktion gibt eine Hilfemeldung auf dem Bildschirm aus",
             pos);
  InsertString(*out, "static void PrintHelp(void)", pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"Available Options:\" << std::endl;",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"  -h                  : Display this help.\" << std::endl;",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"  -v                  : Verbose message output.\" << std::endl;",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"  -f                  : Fair progress of all instances.\" << std::endl;",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"  -e <num>            : Report functional errors (num = num of errors).\" << std::endl;",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"  -u                  : Report dead code.\" << std::endl;",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"  -l                  : Loop detection.\" << std::endl;",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"  -d <num>            : Max. depth of analysis (default: unlimited).\" << std::endl;",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"  -y <char>           : Analysis type (s = simulation\" << std::endl;",
               pos + 2, CG_WITH_EOL);
  if (system->GetAtomList())
  {
    InsertString(*out, "std::cout << \"                                       r = reachability analysis\" << std::endl;",
                 pos + 2, CG_WITH_EOL);
    InsertString(*out, "std::cout << \"                                       v = verification).\" << std::endl;",
                 pos + 2, CG_WITH_EOL);
    InsertString(*out, "std::cout << \"  -r <text>           : TL formula to be verified.\" << std::endl;",
                 pos + 2, CG_WITH_EOL);
  }
  else
  {
    InsertString(*out, "std::cout << \"                                       r = reachability analysis).\" << std::endl;",
                 pos + 2, CG_WITH_EOL);
  }
  InsertString(*out, "std::cout << \"  -s <text>           : Search type ('bs' = bit state space\" << std::endl;",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"                                     'fs' = full state space).\" << std::endl;",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"  -p <num>            : Size of hash table (2^<num>, default: 16) (not with -y s).\" << std::endl;",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"  -i <num>            : Index of hash function (1-5, default: 1) (not with -y s).\" << std::endl;",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"  -m <text>           : Hostname for tracing or sensor display.\" << std::endl;",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"  -n                  : Portnumber for tracing or sensor display.\" << std::endl;",
               pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"  -z <time>           : Max. modell time.\" << std::endl;",
               pos + 2, CG_WITH_EOL);
  if (system->GetAtomList())
  {
    InsertString(*out, "std::cout << \"  -q <atom>           : Quit condition.\" << std::endl;",
                 pos + 2, CG_WITH_EOL);
  }
  InsertString(*out, "std::cout << \"  -t <msc|socket", pos + 2, CG_NO_EOL);
  if (writer->CGSupportPEV())
  {
    InsertString(*out, "|pev>", 0, CG_NO_EOL);
  }
  else
  {
    InsertString(*out, ">    ", 0, CG_NO_EOL);
  }
  InsertString(*out, " : Enable tracing.\" << std::endl;", 0, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"  -o <file>           : Name of MSC trace file.\" << std::endl;",
               pos + 2, CG_WITH_EOL);
  if (writer->CGSupportPEV())
  {
    InsertString(*out, "std::cout << \"  -x <rate>           : Display rate for PEV.\" << std::endl;",
                 pos + 2, CG_WITH_EOL);
    InsertString(*out, "std::cout << \"  -c <file>           : Name of experiment file for PEV.\" << std::endl;",
                 pos + 2, CG_WITH_EOL);
  }
  InsertString(*out, "std::cout << std::endl;", pos + 2, CG_WITH_EOL);
  if (writer->CGSupportPEV())
  {
    InsertString(*out, "std::cout << \"Example:\" << std::endl;", pos + 2, CG_WITH_EOL);
    str.Format("std::cout << \"  %s -vy s -t pev -x 14 -c Test.pev\" << std::endl;",
               writer->CGGetSystem()->GetName()->GetString());
    InsertString(*out, str, pos + 2, CG_WITH_EOL);
  }
  else
  {
    InsertString(*out, "std::cout << \"Example:\" << std::endl;", pos + 2, CG_WITH_EOL);
    str.Format("std::cout << \"  %s -vy s -e 1 -t msc -o Test.mpr 100\" << std::endl;",
               writer->CGGetSystem()->GetName()->GetString());
    InsertString(*out, str, pos + 2, CG_WITH_EOL);
  }
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

/****************************************************************************
 * CGWriteMessageFunction: Funktion wertet die Optionen die dem Validator
 *                         uebergeben werden aus.
 *                         -> writer
 *                         Ergebnis: DS_OK oder DS_ERROR
 ****************************************************************************/

DSResult CGMain::CGWriteMessageFunction(CGWriter *writer) const
{
  DSResult result;
  CGFile *out;
  CGPos   pos;

  out = writer->CGGetFilePointer();
  assert(out);
  pos = writer->CGGetIndentPos();

  InsertHead(*out, "Funktion gibt die Copyrightmeldung zur Laufzeit aus", 0);
  InsertString(*out, "static void PrintMessage(void)", pos, CG_WITH_EOL);
  InsertString(*out, "{", pos, CG_WITH_EOL);
  InsertString(*out, "std::cout << std::endl << \"*********************************************\" << std::endl;", pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"* QSDL-Evaluator                            *\" << std::endl;", pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"*                                           *\" << std::endl;", pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"* Version 1.5.0                             *\" << std::endl;", pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"*                                           *\" << std::endl;", pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"* Copyright (C) 1995-98 Marc Diefenbruch    *\" << std::endl;", pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"*                   and Wolfgang Textor     *\" << std::endl;", pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"*                   and Axel Hirche         *\" << std::endl;", pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"*                                           *\" << std::endl;", pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"* University of Essen, Germany              *\" << std::endl;", pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"*                                           *\" << std::endl;", pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"* All rights reserved                       *\" << std::endl;", pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"*                                           *\" << std::endl;", pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"* Bug reports: qsdl@informatik.uni-essen.de *\" << std::endl;", pos + 2, CG_WITH_EOL);
  InsertString(*out, "std::cout << \"*********************************************\" << std::endl << std::endl;", pos + 2, CG_WITH_EOL);
  InsertString(*out, "}", pos, CG_WITH_EOL);

  return DS_OK;
}

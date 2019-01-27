#include <iostream.h> 

#include "CGWriter.h"
#include "CGFile.h"

int main(void)
{
//  CGFile   *test_file;
  DSResult  result;

  CGFile test_file(new DSString("."),
                   new DSString("CGTest.out"),
                   DS_TRUE);
//  assert(test_file);

  InsertString(test_file,
               "pcs = new pcs_philosopher_philosopher(SCScheduler::NewProcessID(),",
               4, CG_NO_EOL);
  InsertString(test_file,
               "kSCNoProcessID, pcs_type_philosopher, tracing);",
               0, CG_WITH_EOL);

  return 0;  
}

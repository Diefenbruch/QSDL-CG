#ifndef _CGFILE_H_
#define _CGFILE_H_

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <DS/DSBasicTypes.h>

#include "CGBasicTypes.h"

#define POS_STACK_SIZE 2000

class CGPosStack
{
  public:
    CGPosStack(void)
    {
      stack = new CGPos[POS_STACK_SIZE];
      assert(stack);
      stack_pointer = -1;
		}
   ~CGPosStack(void)
    {
      delete[] stack;
    }

    DSResult Push(CGPos elem)
    {
      if (stack_pointer == POS_STACK_SIZE - 2)
      {
        assert(DS_FALSE);
        return DS_ERROR;
      }
      stack_pointer++;
      stack[stack_pointer] = elem;

      return DS_OK;
    }

    CGPos Pop(void)          /* Hole & Entferne oberstes Element */
    {
      assert(stack_pointer >= 0);
      stack_pointer--;
      return stack[stack_pointer + 1];
    }
    CGPos Top(void)         /* Hole oberstes Element            */
    {
      assert(stack_pointer >= 0);
      return stack[stack_pointer];
    }

    DSBoolean IsEmpty(void)
    {
      if (stack_pointer == -1)
        return DS_TRUE;
      else
        return DS_FALSE;
    }

    DSResult Empty(void)
    {
      while (!IsEmpty())
      {
        Pop();
      }
      return DS_OK;
    }

  private:
    CGPos *stack;
    int    stack_pointer;
};

/******************************************************************************
 * Klasse CGFile
 * Zweck: Pretty-Printer zur formatierten Ausgabe von uebergebenen Strings
 *        sowie Sonderfunktionen wie Einfuegen von Kommentar usw.
 ******************************************************************************/

class CGFile
{
  
  public:
    CGFile(class DSString * = NULL,
           class DSString * = NULL,
           DSBoolean = DS_FALSE);
    CGFile(const char *, const char *, DSBoolean = DS_FALSE);
    ~CGFile(void);

    DSResult  CGFileOpen(void);
    DSResult  CGFileClose(void);
    DSResult  CGInsertString(const char *, CGPos = 0, CGNewLine = CG_NO_EOL);
    DSResult  CGInsertString(class DSString *, CGPos = 0, CGNewLine = CG_NO_EOL);
    DSResult  CGInsertString(class DSString &, CGPos = 0, CGNewLine = CG_NO_EOL);
    DSResult  CGInsertCardinal(DSCardinal, CGPos = 0, CGNewLine = CG_NO_EOL);
    CGPos     CGPosition(void);
    class DSString *CGFileName(void);
    class DSString *CGFilePath(void);
    DSResult  CGInsertStringComment(const char *, const char *, CGPos = 0);
    DSResult  CGInsertStringComment(class DSString *, class DSString *, CGPos = 0);
    DSResult  CGInsertStringComment(class DSString &, class DSString &, CGPos = 0);
    DSResult  CGInsertHeadline(CGPos = 0);
    DSResult  CGInsertBaseline(CGPos = 0);
    DSResult  CGInsertCopyright(void);
    DSResult  CGInsertHead(const char *, CGPos = 0);
    DSResult  CGInsertHead(class DSString *, CGPos = 0);
    DSResult  CGInsertHead(class DSString &, CGPos = 0);
    
  private:
    class DSString  *CGDivide(class DSString *, CGPos, CGNewLine);
    DSResult   CGUpdateStack(class DSString *);
    DSResult   CGEmptyStack(CGPosStack &);

    FILE      *file_pointer;
    class DSString  *file_name;
    class DSString  *file_path;
    DSBoolean  file_open;
    CGPos      actual_line_position;
    CGPosStack stack;
    CGPosStack outstream_stack;
    DSBoolean  is_in_text;
    DSBoolean  no_line_break;
    DSBoolean  pretty_printing;
    DSBoolean  is_in_comment;
    DSBoolean  is_in_comment_line;
};

/**************************************************************************
 * Makro zum Vereinfachen der Codegenerierung. Erwartet dass
 * an der Verwendungsstelle die Variable 'result' vom Typ CGBoolean
 * definiert ist.
 * 1. Parameter: Codegenerierungsfile vom Type 'CGFile'
 * 2. Parameter: String der generiert werden soll
 * 3. Parameter: Einrueckungstiefe
 * 4. Parameter: Anhaengen von EOL (ja/nein)
 **************************************************************************/

#define InsertString(m_file, m_string, m_depth, m_eol)\
  if ((result = (m_file).CGInsertString((m_string),\
                                        (m_depth), (m_eol))) != DS_OK)\
  {\
     std::cerr << "Error while writing on file "\
          << (m_file).CGFilePath()\
          << (m_file).CGFileName()\
          << "." << std::endl;\
     return result;\
  }

#define InsertIncludeSCL(m_file, m_string, m_depth, m_eol)\
  InsertString(m_file, "#include <" CG_PATH_SCL_DIR m_string ">", (m_depth), (m_eol));

#define InsertIncludePEV(m_file, m_string, m_depth, m_eol)\
  InsertString(m_file, "#include <" CG_PATH_PEV_DIR m_string ">", (m_depth), (m_eol));

#define InsertIncludeTL(m_file, m_string, m_depth, m_eol)\
  InsertString(m_file, "#include <" CG_PATH_TL_DIR m_string ">", (m_depth), (m_eol));

#define InsertCardinal(m_file, m_card, m_depth, m_eol)\
  if ((result = (m_file).CGInsertCardinal((m_card),\
                                          (m_depth), (m_eol))) != DS_OK)\
  {\
     std::cerr << "Error while writing on file "\
          << (m_file).CGFilePath()\
          << (m_file).CGFileName()\
          << "." << std::endl;\
     return result;\
  }

#define InsertStringComment(m_file, m_string, m_comment, m_depth)\
  if ((result = (m_file).CGInsertStringComment((m_string),\
                                               (m_comment),\
                                               (m_depth))) != DS_OK)\
  {\
     std::cerr << "Error while writing on file "\
          << (m_file).CGFilePath()\
          << (m_file).CGFileName()\
          << "." << std::endl;\
     return result;\
  }

#define InsertHead(m_file, m_string, m_depth)\
  if ((result = (m_file).CGInsertHead((m_string),\
                                      (m_depth))) != DS_OK)\
  {\
     std::cerr << "Error while writing on file "\
          << (m_file).CGFilePath()\
          << (m_file).CGFileName()\
          << "." << std::endl;\
     return result;\
  }

#define InsertLine(m_file)\
  if ((result = (m_file).CGInsertString("", 0, CG_WITH_EOL)) != DS_OK)\
  {\
     std::cerr << "Error while writing on file "\
          << (m_file).CGFilePath()\
          << (m_file).CGFileName()\
          << "." << std::endl;\
     return result;\
  }

#define InsertCopyright(m_file)\
  if ((result = (m_file).CGInsertCopyright()) != DS_OK)\
  {\
     std::cerr << "Error while writing on file "\
          << (m_file).CGFilePath()\
          << (m_file).CGFileName()\
          << "." << std::endl;\
     return result;\
  }

/*************************************************************/
/* Hilfsmakro MemAssert: Ueberprueft ob allokierter Speicher */
/*                       erhalten wurde. Falls nicht, wird   */
/*                       die Fehlermeldung CG_OUT_OF_MEMORY  */
/*                       zurueckgegeben.                     */
/*************************************************************/

#define MemAssert(pointer)\
{\
  if (!pointer)\
  {\
    std::cerr << "Memory allocation fault in file " << __FILE__ << " at line "\
         << __LINE__ << "! Bailing out.";\
    return DS_OUT_OF_MEMORY;\
  }\
}

#endif

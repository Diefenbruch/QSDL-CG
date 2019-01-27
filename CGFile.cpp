/****************************************************************************
 * $RCSfile: CGFile.cpp,v $
 *
 * $Author: md $
 * $Date: 1998/02/13 06:51:41 $
 * $Revision: 1.5 $
 *
 * Aufgabe: Dieses Modul stellt eine Klasse mit Dateioperationen
 *          zur Verfuegung:
 *          o Oeffnen einer Datei
 *          o Schliessen einer Datei
 *          o Einfuegen eines Strings an das Ende einer Datei
 *          o Abfragen nach aktueller Position, Name und Pfad der Datei
 * Funktionen: CGFileOpen(): Oeffnet eine Datei
 *             CGFileClose(): Schliesst eine Datei
 *             CGInsertString(): Fuegt einen String an das Dateiende an
 *             CGPosition(): Liefert die aktuelle Position in der Datei
 *             CGFileName(): Liefert den Dateinamen zurueck
 *             CGFilePath(): Liefert den Dateipfad zurueck
 *             CGFile(): Konstruktor
 *             ~CGFile(): Destruktor
 *             CGInsertBaseline(): fuegt eine Kommentarzeile in eine Datei ein
 *             CGDivide(): Teilt Strings, sobald die zulaessige Spaltenposition
 *                         ueberschritten wird
 *             CGUpdateStack(): merkt sich die geoeffneten Klammern eines 
 *                              Strings auf einem Stack
 ***************************************************************************/
  
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <iostream>

#ifndef _MSC_VER

#include <sys/utsname.h>               // benoetigt fuer InsertCopyright
#include <unistd.h>                    // benoetigt fuer Makro
#include <pwd.h>                       // benoetigt fuer Makro
#include <sys/types.h>                 // benoetigt fuer Makro
#include <time.h>

#endif // _MSC_VER

#include <DS/DSString.h>
#include "CGFile.h"

#ifdef DEBUG
#include <dmalloc.h>
#endif

/***************************************************************************
 * CGFile(): Konstruktor
 *           -> filename: Name der Datei
 *           -> path    : Name des Pfades
 *           -> pretty  : Prettyprinter an/aus ?
 *           Seiteneffekte: keine
 ***************************************************************************/

CGFile::CGFile(DSString *path, DSString *filename, DSBoolean pretty)
{
  if (!filename)                                /* Ausgabe nach stdout !!    */
  {
    file_name = NULL;
    return;
  }

  file_name = new DSString(*filename);
                                                 /* Dateiname speichern      */
  assert(file_name);

  if (path)
  {
    file_path = new DSString(*path);            /* sonst nur kopieren       */
    assert(file_path);
    if ((*path)[path->Length() - 1] != CG_PATH_SEPARATOR_CHAR)
    {
      *file_path += CG_PATH_SEPARATOR_STRING;
    }
  }
  else // kein Pfad angegeben!
  {
    file_path = new DSString(".");
    assert(file_path);
    *file_path += CG_PATH_SEPARATOR_STRING;
  }

  file_pointer = NULL;
  file_open = DS_FALSE;
  actual_line_position = 0;
  is_in_text = DS_FALSE;
  is_in_comment_line = is_in_comment = DS_FALSE;
  pretty_printing = pretty;
}

CGFile::CGFile(const char *path, const char *filename, DSBoolean pretty)
{
  if (!filename)                                /* Ausgabe nach stdout !!    */
  {
    file_name = NULL;
    return;
  }

  file_name = new DSString(filename);
                                                 /* Dateiname speichern      */
  assert(file_name);

  if (path)
  {
    file_path = new DSString(path);             /* sonst nur kopieren       */
    assert(file_path);
    if (path[strlen(path) - 1] != CG_PATH_SEPARATOR_CHAR)
    {
      *file_path += CG_PATH_SEPARATOR_STRING;
    }
  }
  else // kein Pfad angegeben!
  {
    file_path = new DSString(".");
    assert(file_path);
    *file_path += CG_PATH_SEPARATOR_STRING;
  }

  file_pointer = NULL;
  file_open = DS_FALSE;
  actual_line_position = 0;
  is_in_text = DS_FALSE;
  is_in_comment_line = is_in_comment = DS_FALSE;
  pretty_printing = pretty;
}

/****************************************************************************
 * ~CGFile(): Destruktor
 *            Seiteneffekte: keine
 ****************************************************************************/

CGFile::~CGFile(void)
{
  if (file_name) delete file_name;
  if (file_path) delete file_path;
  if (file_open)                                /* ggf. Datei schliessen    */
  {
    CGFileClose();
  }
}

/****************************************************************************
 * CGFileOpen: Erzeugt und oeffnet eine neue Datei
 *                     Ergebnis: DS_OK, falls die Datei erfolgreich geoeffnet
 *                               werden konnte, ansonsten DS_FALSE
 *                     Seiteneffekte: keine
 ***************************************************************************/

DSResult CGFile::CGFileOpen(void)
{
  DSString *filename;

  if (!file_name)                               /* keine Ausgabedatei ange-  */
  {                                             /* ben, dann nach stdout um- */
    file_pointer = stdout;                      /* leiten                    */
    file_open = DS_TRUE;
    return DS_OK;
  }

  filename = new DSString(*file_path);           /* zuerst den Pfad         */
  MemAssert(filename);
  *filename += *file_name;                       /* dann den eigentlichen   */
                                                 /* Filenamen               */

  file_pointer = fopen(filename->GetString(),    /* Datei oeffnen           */
                       "wt");
  if (file_pointer == NULL)                      /* war Oeffnen erfolgreich */
  {
    return DS_ERROR;                             /* nein !!                 */
  }
  file_open = DS_TRUE;
  delete filename;                               /* temp. Speicher freigeben*/
  return DS_OK;
}

/****************************************************************************
 * CGFileClose: Schliesst eine Datei
 *              Ergebnis: DS_OK, falls die Datei erfolgreich geschlossen
 *                        werden konnte, ansonsten DS_FALSE
 *              Seiteneffekte: keine
 ***************************************************************************/

DSResult CGFile::CGFileClose(void)
{
  return (fclose(file_pointer) == 0 ? DS_OK : DS_ERROR); 
}

/****************************************************************************
 * CGInsertString: Fuegt einen String an das Ende einer Datei an
 *                 -> str: Der einzufuegende String
 *                 -> pos: Anzahl der Leerzeichen vor dem String, 
 *                         Standardwert ist 0
 *                 -> with_EOL: Zeilenumbruch erwuenscht ?
 *                 Ergebnis: DS_TRUE, genug Platz vorhanden war, 
                             ansonsten DS_FALSE
 *                 Seiteneffekte: keine
 ***************************************************************************/

DSResult CGFile::CGInsertString(const char *str,
                                CGPos pos,
                                CGNewLine with_EOL)
{
  DSResult result;
  DSString *tmp = new DSString(str);
  MemAssert(tmp);
  result = CGInsertString(tmp, pos, with_EOL);
  delete tmp;
  return result;
}

/****************************************************************************
 * CGInsertString: Fuegt einen String an das Ende einer Datei an
 *                 -> str: Der einzufuegende String
 *                 -> pos: Anzahl der Leerzeichen vor dem String, 
 *                         Standardwert ist 0
 *                 -> with_EOL: Zeilenumbruch erwuenscht ?
 *                 Ergebnis: DS_TRUE, genug Platz vorhanden war, ansonsten 
 *                           DS_FALSE
 *                 Seiteneffekte: keine
 ***************************************************************************/

DSResult CGFile::CGInsertString(DSString &str,
                                CGPos pos,
                                CGNewLine with_EOL)
{
  DSResult result;
  result = CGInsertString(&str, pos, with_EOL);
  return result;
}

/****************************************************************************
 * CGInsertString: Fuegt einen String an das Ende einer Datei an
 *                 -> str: Der einzufuegende String
 *                 -> pos: Anzahl der Leerzeichen vor dem String, 
 *                         Standardwert ist 0
 *                 -> with_EOL: Zeilenumbruch erwuenscht ?
 *                 Ergebnis: DS_TRUE, genug Platz vorhanden war, 
 *                           ansonsten DS_FALSE
 *                 Seiteneffekte: keine
 ***************************************************************************/

DSResult CGFile::CGInsertString(DSString *str,
                                CGPos pos,
                                CGNewLine with_EOL)
{
  DSCardinal i = 0;
  DSString *substring = NULL;
  DSString *string;
  DSString *nstring;
  DSInteger blanks;
  DSResult  result;

  assert(str);

  if (!file_open)                               /* Sicherstellen, dass die    */
  {                                             /* Datei geoeffnet ist        */
    if (CGFileOpen() != DS_OK)
    {
      return DS_ERROR;
    }
  }

  string = new DSString(*str);                  /* Kopie des Originals, das   */
  MemAssert(string);                            /* nicht veraendert werden    */
                                                /* soll                       */
  if (pretty_printing)                          /* eingeschaltet ?            */
  {
    substring = CGDivide(string, pos, with_EOL);
    blanks = (DSInteger)pos;
  }
  else
  {
    blanks = 1;
  }

  assert(file_pointer);

  if (pos == 0 || !pretty_printing)             /* String an Zeilenanfang   */
  {                                             /* einfuegen                */
    if (with_EOL)
    {
      if (fprintf(file_pointer, "%s%c", string->GetString(), CG_EOL_CHAR) < 0)
      {
        delete string;
        return DS_ERROR;
      }
    }
    else
    {
      if (fprintf(file_pointer, "%s", string->GetString()) < 0)
      {
        delete string;
        return DS_ERROR;
      }
    }
  }
  else                                          /* Leerzeichen abzueglich   */
  {                                             /* vorhandenen einfuegen    */
    while (i < string->Length() &&
           (*string)[i] == CG_BLANK_CHAR) 
    {                                           /* Zaehlen der bereits vor- */
      blanks--;                                 /* handenen fuehrenden      */
      i++;                                      /* Leerzeichen              */
    }
    if (blanks <= 0)                            /* genug Leerzeichen in     */
    {                                           /* String enthalten ?       */
      if (with_EOL)
      {
        if (fprintf(file_pointer, "%s%c", string->GetString(), CG_EOL_CHAR) < 0)
        {
          return DS_ERROR;
        }
      }
      else
      {
        if (fprintf(file_pointer,"%s", string->GetString()) < 0)
        {
          return DS_ERROR;
        }
      }
    }
    else                                        /* nein, also nstring       */
    {                                           /* erzeugen                 */
      nstring = new DSString(blanks, CG_BLANK_CHAR);
      MemAssert(nstring);

      *nstring += *string;                      /* Text an Leerstring anh.  */

      if (with_EOL)
      {
        if (fprintf(file_pointer, "%s%c", nstring->GetString(), CG_EOL_CHAR) < 0)
        {
          delete nstring;
          return DS_ERROR;
        }
      }
      else
      {
        if (fprintf(file_pointer, "%s", nstring->GetString()) < 0)
        {
          delete nstring;
          return DS_ERROR;
        }
      }
      delete nstring;
    }
  }

  if ((with_EOL == CG_WITH_EOL ||
       (string->Length() > 1 && (*string)[string->Length() - 1] == CG_EOL_CHAR))
      && !substring)
  {                                             /* ist letztes Zeichen EOL?  */
    actual_line_position = 0;                   /* akt. Spalte ist wieder 0  */
  }
  else                                          /* sonst neue Position merken*/
  {
    actual_line_position += string->Length();
  }

  // Ist noch was von dem String uebriggeblieben?
  if (substring != NULL) 
  {
    actual_line_position = 0;                  /* Spalte ist wieder 0         */
    if (!outstream_stack.IsEmpty())            /* cout, cerr... umgebrochen   */
    {
      CGInsertString(substring, outstream_stack.Top(), with_EOL);
    }
    else                                       /* normaler Umbruch            */
    {
      if (!stack.IsEmpty())                    /* hole letzte offene Klammer  */
      {
        CGInsertString(substring, stack.Top(), with_EOL);
      }
      else
      {
        CGInsertString(substring, pos, with_EOL);
      }
    }
    delete substring;
  }
  if ((result = outstream_stack.Empty()) != DS_OK ) return result;
  delete string;

  return DS_OK;  
}

/****************************************************************************
 * CGInsertStringComment: Fuegt eine Befehlszeile und einen Kommentar
 *                         in eine Datei ein
 *                       -> str    : die einzufuegende Befehlszeile
 *                       -> comment: der einzufuegende Kommentar
 *                       -> pos    : Anfang der Befehlszeile
 *                       Ergebnis: DS_OK, wenn Einfuegen erfolgreich war,
 *                                 ansonsten DS_ERROR
 *                                 Seiteneffekte: keine
 ****************************************************************************/
 
DSResult CGFile::CGInsertStringComment(const char *str, 
                                       const char *comment, 
                                       CGPos pos)
{
  DSResult result;
  DSString *tmp1 = new DSString(str);
  MemAssert(tmp1);
  DSString *tmp2 = new DSString(comment);
  MemAssert(tmp2);
  result = CGInsertStringComment(tmp1, tmp2, pos);
  delete tmp1;
  delete tmp2;
  return result;
}

/****************************************************************************
 * CGInsertStringComment: Fuegt eine Befehlszeile und einen Kommentar
 *                         in eine Datei ein
 *                       -> str: die einzufuegende Befehlszeile
 *                       -> comment: der einzufuegende Kommentar
 *                       -> pos: Anfang der Befehlszeile
 *                       Ergebnis: DS_OK, wenn Einfuegen erfolgreich war,
 *                                 ansonsten DS_ERROR
 *                                 Seiteneffekte: keine
 ****************************************************************************/
 
DSResult CGFile::CGInsertStringComment(DSString &str, 
                                       DSString &comment, 
                                       CGPos pos)
{
  DSResult result;
  result = CGInsertStringComment(&str, &comment, pos);
  return result;
}

/****************************************************************************
 * CGInsertStringComment: Fuegt eine Befehlszeile und einen Kommentar
 *                         in eine Datei ein
 *                       -> string: die einzufuegende Befehlszeile
 *                       -> comment: der einzufuegende Kommentar
 *                       -> pos: Anfang der Befehlszeile
 *                       Ergebnis: DS_OK, wenn Einfuegen erfolgreich war,
 *                                 ansonsten DS_ERROR
 *                                 Seiteneffekte: keine
 ****************************************************************************/
 
DSResult CGFile::CGInsertStringComment(DSString *str, 
                                       DSString *comment, 
                                       CGPos pos)
{
  DSString *string;

  string = new DSString(*str);                  /* Originalstring soll nicht*/
  MemAssert(string);                            /* veraendert werden        */

  if ((*string)[string->Length() - 1] == CG_EOL_CHAR) /* ggf. EOL entfernen und   */
  { 
    (*string)[string->Length() - 1] = CG_EOS_CHAR; /* durch Terminator ersetzen*/
  }

  if (CGInsertString(string,                    /* Einfuegen                */
                     pos,
                     pretty_printing ? CG_NO_EOL : CG_WITH_EOL) != DS_OK)
  {
    delete string;
    return DS_ERROR;
  }

  if (!pretty_printing) return DS_OK;
                                                /* sonst Kommentar einfuegen*/
  delete string;
  if (CGInsertString(CG_LINE_COM,             /* Einfuegen                */
                     0, CG_NO_EOL) != DS_OK)  /* in Datei fehlgeschlagen !*/
  {
    return DS_ERROR;
  }
  return (CGInsertString(comment, 0, CG_WITH_EOL));
} 

/****************************************************************************
 * CGInsertCardinal: Fuegt eine Cardinalzahl an das Ende einer Datei an
 *                 -> card: Die einzufuegende Zahl
 *                 -> pos: Anzahl der Leerzeichen vor dem String, 
 *                         Standardwert ist 0
 *                 -> with_EOL: Zeilenumbruch erwuenscht ?
 *                 Ergebnis: DS_TRUE, genug Platz vorhanden war, 
                             ansonsten DS_FALSE
 *                 Seiteneffekte: keine
 ***************************************************************************/

DSResult CGFile::CGInsertCardinal(DSCardinal card,
                                  CGPos pos,
                                  CGNewLine with_EOL)
{
  DSResult result;
  DSString *tmp = new DSString(card);
  MemAssert(tmp);
  result = CGInsertString(tmp, pos, with_EOL);
  delete tmp;
  return result;
}

/****************************************************************************
 * CGInsertHeadline: Fuegt eine standardisierte Kopfzeile ein
 *                   Ergebnis: DS_OK, wenn Einfuegen erfolgreich war,
 *                             ansonsten DS_ERROR
 *                   Seiteneffekte: keine
 ***************************************************************************/
 
DSResult CGFile::CGInsertHeadline(CGPos pos)
{
  DSString line(CG_LINE_LEN + 1, ' ');         /* Hilfsstring              */
  int      i;                                  /* Zaehlvariable fuer die   */
                                               /* for-Schleife             */

  if (!pretty_printing) return DS_OK;

  assert(CG_LINE_LEN >= CG_END_COM_POS + 2);
  assert(pos < CG_LINE_LEN - 1);

  for (i = 0;                                   /* Kommentar fuellen z.B.   */
       i < pos;                                 /* mit ' '                  */
       i++)
  {
    line[i] = CG_BLANK_CHAR;
  }
  line[pos] = CG_EOS_CHAR;

  line += CG_BEG_COM_STRING;                    /* Das Zeichen, mit dem ein */
                                                /* Kommentar eingeleitet    */
                                                /* wird, kopieren           */
  for (i = pos + strlen(CG_BEG_COM_STRING);     /* Kommentar fuellen z.B.   */
       i <= CG_END_COM_POS;                     /* mit '*'                  */
       i++)
  {
    line[i] = CG_HEAD_CHAR;
  }
  line[i] = CG_EOL_CHAR;
  line[i + 1] = CG_EOS_CHAR;

  return (CGInsertString(line));
}

/****************************************************************************
 * CGInsertBaseline: Fuegt eine standardisierte Fusszeile ein
 *                   -> pos: Einrueckungstiefe
 *                   Ergebnis: DS_OK, wenn Einfuegen erfolgreich war,
 *                             ansonsten DS_ERROR
 *                   Seiteneffekte: keine
 ***************************************************************************/
 
DSResult CGFile::CGInsertBaseline(CGPos pos)
{
  DSString   line(CG_LINE_LEN + 1, ' ');        /* Hilfsstring              */
  DSCardinal i;                                 /* Zaehlvariable fuer die   */
                                                /* for-Schleife             */

  if (!pretty_printing) return DS_OK;

  assert(CG_LINE_LEN >= CG_END_COM_POS + 2);

  for (i = 0;                                   /* Kommentar fuellen z.B.   */
       i <= pos;                                /* mit ' '                  */
       i++)
  {
    line[i] = CG_BLANK_CHAR;
  }
  for (i = pos + 1;                             /* Kommentar fuellen z.B.   */
       i <= CG_END_COM_POS - strlen(CG_END_COM_STRING); /* mit '*'                  */
       i++)
  {
    line[i] = CG_HEAD_CHAR;
  }
  line[i] = CG_EOS_CHAR;

  line += CG_END_COM_STRING;                    /* Das Zeichen, mit dem ein */
                                                /* Kommentar beendet        */
                                                /* wird, kopieren           */
  line += CG_EOL_STRING;
  return (CGInsertString(line, 0, CG_WITH_EOL));
}
   
/****************************************************************************
 * CGInsertHead: Fuegt eine standardisierte Kommentarzeile ein
 *               -> text: enthaelt die Kommentarzeile
 *               -> pos : Einrueckungstiefe
 *               Ergebnis: DS_OK, wenn Einfuegen erfolgreich war,
 *                         ansonsten DS_ERROR
 *               Seiteneffekte: keine
 ***************************************************************************/
 
DSResult CGFile::CGInsertHead(const char *text, CGPos pos)
{
  DSResult result;
  DSString *tmp;

  if (!pretty_printing) return DS_OK;

  tmp = new DSString(text);
  MemAssert(tmp);

  result = CGInsertHead(tmp, pos);
  delete tmp;
  return result;
}
   
/****************************************************************************
 * CGInsertHead: Fuegt eine standardisierte Kommentarzeile ein
 *               -> text: enthaelt die Kommentarzeile
 *               -> pos : Einrueckungstiefe
 *               Ergebnis: DS_OK, wenn Einfuegen erfolgreich war,
 *                         ansonsten DS_ERROR
 *               Seiteneffekte: keine
 ***************************************************************************/
 
DSResult CGFile::CGInsertHead(DSString &text, CGPos pos)
{
  DSResult result;

  if (!pretty_printing) return DS_OK;

  result = CGInsertHead(&text, pos);

  return result;
}
   
/****************************************************************************
 * CGInsertHead: Fuegt eine standardisierte Kommentarzeile ein
 *               -> text: enthaelt die Kommentarzeile
 *               -> pos : Einrueckungstiefe
 *               Ergebnis: DS_OK, wenn Einfuegen erfolgreich war,
 *                         ansonsten DS_ERROR
 *               Seiteneffekte: keine
 ***************************************************************************/
 
DSResult CGFile::CGInsertHead(DSString *text, CGPos pos)
{
  DSString   line(CG_LINE_LEN + 1, ' ');        /* Hilfsstring              */
  DSResult   result;
  DSCardinal i;                                 /* Zaehlvariable fuer die   */
                                                /* for-Schleife             */
  if (!pretty_printing) return DS_OK;

//  assert(pos + text->Length() + 1 <= CG_LINE_LEN);

  if (pos + text->Length() + 1 > CG_LINE_LEN) return DS_OK;

  if ((result = CGInsertHeadline(pos)) != DS_OK) return result;

  for (i = 0;                                   /* Kommentar fuellen z.B.   */
       i <= pos;                                /* mit ' '                  */
       i++)
  {
    line[i] = CG_BLANK_CHAR;
  }
  line[pos + 1] = CG_HEAD_CHAR;
  line[pos + 2] = CG_BLANK_CHAR;
  line[pos + 3] = CG_EOS_CHAR;

  line += *text;
  line += CG_EOL_STRING;
  line += CG_EOS_STRING;
  if ((result = CGInsertString(line)) != DS_OK) return result;
  if ((result = CGInsertBaseline(pos)) != DS_OK) return result;

  return DS_OK;
}
         
/****************************************************************************
 * CGPosition: Liefert den Offset des aktuellen Byte der Datei
 *             Ergebnis: Offset
 *             Seiteneffekte: keine
 ***************************************************************************/

CGPos CGFile::CGPosition(void)
{
  return (CGPos)ftell(file_pointer);
}

/****************************************************************************
 * CGFileName: Liefert den Namen der Datei
 *             Ergebnis: Namensstring
 *             Seiteneffekte: keine
 ***************************************************************************/

DSString* CGFile::CGFileName(void)
{
  return(file_name);
}

/****************************************************************************
 * CGFilePath: Liefert die Pfadangabe der Datei
 *             Ergebnis: Pfadstring
 *             Seiteneffekte: keine
 ***************************************************************************/

DSString* CGFile::CGFilePath(void)
{
  return(file_path);
} 

/****************************************************************************
 * CGDivide(): teilt den uebergebenen String in zwei Strings auf, falls
 *             die zulaessige Spaltenlaenge ueberschritten wird
 *             -> string: der ggf. aufzuteilende String
 *             -> pos: Spaltenposition, an der String in die Datei eingefuegt
 *                     werden soll
 *             -> with_EOL: ist Zeilenumbruch erwuenscht ?
 *             <- string: eventuelle verkuerzter String
 *             Ergebnis: Reststring oder NULL (falls keine Teilung erforderlich)
 *             Seiteneffekte: keine
 ***************************************************************************/

DSString *CGFile::CGDivide(DSString *string, CGPos pos, CGNewLine with_EOL)
{  
  DSCardinal i = 0;
  char c1, c2;
  DSResult result;
  DSBoolean found = DS_FALSE;
  DSString *substring = NULL;               /* wird ggf. neuer Substring  */

  actual_line_position += pos;              /* Spaltenposition aktualisieren */

  // kein Umbruch bis zur .. Spalte !!!
  if ((int)(string->Length() + actual_line_position) < CG_LINE_BREAK_POS) 
  {                                            
    // Merken der moeglichen Einrueckungspositionen auf einem Stack:
    if ((result = CGUpdateStack(string) ) != DS_OK) return NULL;

    if (with_EOL) is_in_text = is_in_comment_line = DS_FALSE;
    return NULL;
  }
  
  /** Der String s wird nach dem ersten Komma oder '<<' beendet
       und der Rest in den neuen String substring kopiert:  */
  i = 0;
  found = DS_FALSE;
  while (found != DS_TRUE &&                       /* Suche nach Umbruchpos  */
         i < string->Length()) 
  {
    c1 = (*string)[i++];
    c2 = (*string)[i];     /* wenn hier i = string->Length() gilt ist */
                           /* c2 == '\0' !                            */

    if (c1 == ',' &&
        !is_in_text &&
        !is_in_comment_line &&
        !is_in_comment &&
        actual_line_position + i >= CG_LINE_BREAK_POS)  /* Komma gefunden!    */
    {
      found = DS_TRUE;
      break;
    }

    if (c1 == '<' &&
        c2 == '<' &&
        !is_in_text &&
        !is_in_comment_line &&
        !is_in_comment &&
        actual_line_position + i >= CG_LINE_BREAK_POS)
    {
      i++;
      found = DS_TRUE;
      break;
    }
    
    if (c1 != '\\' &&
        c2 == '\"' &&
        !is_in_comment &&
        !is_in_comment_line)                  /* Kommentar: kein Umbruch !  */
    { 
      if (is_in_text)
      {
        is_in_text = DS_FALSE;
      }
      else
      {
        is_in_text = DS_TRUE;
      }
    }
    
    // Kommentarende fuer einzeilige Kommentare:
    if (c1 == '\n' &&
        !is_in_text &&
        !is_in_comment)
    {
      is_in_comment_line = DS_FALSE;
    }

    // C-Kommentarbeginn:
    if (c1 == '/' &&
        c2 == '*' &&
        !is_in_text &&
        !is_in_comment_line)                     /* Kommentar: kein Umbruch !  */
    {
      is_in_comment = DS_TRUE;
    }
    
    // C-Kommentarende:
    if (c1 == '*' &&
        c2 == '/' &&
        !is_in_text && 
        !is_in_comment_line)                    /* Kommentar Ende !!          */
    {
      is_in_comment = DS_FALSE;
    }
    
    // C-Kommentarbeginn:
    if (c1 == '/' &&
        c2 == '/' &&
        !is_in_comment &&
        !is_in_text)                            /* Kommentar: kein Umbruch !  */
    {
      is_in_comment_line = DS_TRUE;
    }
    
    // Makefile-Kommentarbeginn:
    if (c1 == '#' &&
        !is_in_text &&
        !is_in_comment)
    {
      is_in_comment_line = DS_TRUE;            /* Makefile: kein Umbruch !   */
    }
    
    if (((c1 == '&' &&                         /* auch hiernach umbrechen    */
          c2 == '&') ||
         (c1 == '|' &&
          c2 == '|')) &&
         !is_in_text &&
         !is_in_comment &&
         !is_in_comment_line)
    {
      i++;
      found = DS_TRUE;
      break;
    }
  }
    
  if (!found)                                   /* keine Umbruchposition      */
  {                                             /* gefunden, also Ende        */
    // Merken der moeglichen Einrueckungspositionen auf einem Stack:
    result = CGUpdateStack(string);
    if (result != DS_OK) return NULL;

    if (with_EOL) is_in_text = is_in_comment_line = DS_FALSE;
    return NULL;                             
  }
  
  if (i == string->Length())                    /* Umbruchposition an letztem */
  {                                             /* Zeichen und kein Zeilen-   */
    if (with_EOL == CG_NO_EOL)                  /* umbruch geplant ?          */
    {
      substring = new DSString("");             /* zwecks Zeilenumbruch !    */
      assert(substring);
    }
  }
  else
  {
    substring = string->SubString(i);           /* String 2 anlegen           */
    assert(substring);                          /* beginnend an Position i    */

    (*string)[i] = CG_EOS_CHAR;                 /* String 1 abschneiden       */
  }
  
  if (!with_EOL)                                /* wenn true bricht die       */
  {                                             /* InsertString um, sonst     */
    *string += CG_EOL_STRING;                   /* Zeilenumbruch explizit     */
                                                /* hinzufuegen                */
  }                                             

  // Merken der moeglichen Einrueckungspositionen auf einem Stack:
  result = CGUpdateStack(string);
  if (result != DS_OK) return NULL;
  
  return substring;
}
 
/****************************************************************************
 * CGUpdateStack: Legt die Klammerpositionen auf dem Stack ab
 *             -> s: der abzuspeichernde String
 *             Ergebnis: DS_TRUE
 *             Seiteneffekte: keine
 ***************************************************************************/

DSResult CGFile::CGUpdateStack(DSString *str)
{
  DSCardinal j = 0;

  assert(str);
  while (j < str->Length())                    /* String wird durchlaufen    */
  {
    switch ((*str)[j++])
    {
      case '(':                                /* oeffnende Klammer: Push    */
        stack.Push(j + actual_line_position);
        break;
      case ')':                                /* schliessende Klammer: Pop  */
        if (!stack.IsEmpty())
        {
          stack.Pop();
        }
        break;
      case '<':                                /* Umleitungen auf Extrastack */
        if ((*str)[j] == '<' && outstream_stack.IsEmpty())
        {
          outstream_stack.Push(j + actual_line_position + 2);
        }
        break;
      default:
        break;
    }
  }
  return DS_OK;
}

/****************************************************************************
 * CGInsertCopyright: Fuegt standardisierte Copyrights in Zieldatei ein
 *                    Ergebnis: DS_TRUE
 *                    Seiteneffekte: keine
 ***************************************************************************/

DSResult CGFile::CGInsertCopyright(void)
{
#ifndef _MSC_VER
  struct utsname name;
  time_t timer;
  uid_t uid;
  struct passwd *user;
  char *user_login;
  char *user_name;
  char time_str[CG_MAX_STRING];
#endif
  DSBoolean old_pretty = pretty_printing;
  DSString str;

  if (!pretty_printing)
  {
    pretty_printing = DS_TRUE;
  }

  if (CGInsertHeadline() != DS_OK) return DS_WRITE_ERROR;

  str.Format("%c                            %s", CG_HEAD_CHAR,
             CGFileName()->GetString());
  if (CGInsertString(str, 1, CG_WITH_EOL) != DS_OK) return DS_ERROR;
  str.Format("%c", CG_HEAD_CHAR);
  if (CGInsertString(str, 1, CG_WITH_EOL) != DS_OK) return DS_ERROR;

#ifndef _MSC_VER
  time(&timer);
  strftime(time_str, CG_LINE_LEN,
           "* Generated by QCG in %B, %d, %Y at %T", localtime(&timer));
  if (CGInsertString(time_str, 1, CG_WITH_EOL) != DS_OK) return DS_ERROR;

  assert(uname(&name) >= 0);
  assert(name.nodename);
  assert(name.sysname);
  assert(name.release);
  assert(name.machine);
  str.Format("*           on system: %s (%s %s %s)",
             name.nodename,
             name.sysname,
             name.release,
             name.machine);
  if (CGInsertString(str, 1, CG_WITH_EOL) != DS_OK) return DS_ERROR;
  str.Format("%c", CG_HEAD_CHAR);
  if (CGInsertString(str, 1, CG_WITH_EOL) != DS_OK) return DS_ERROR;
  uid = getuid();
  user = getpwuid(uid);
  assert(user);
  user_login = user->pw_name;
  assert(user_login);
  user_name = user->pw_gecos;
  str.Format("%c User: %s", CG_HEAD_CHAR,
          user_name != NULL ? user_name : user_login);
  if (CGInsertString(str, 1, CG_WITH_EOL) != DS_OK) return DS_ERROR;
  str.Format("%c", CG_HEAD_CHAR);
#endif
  if (CGInsertString(str, 1, CG_WITH_EOL) != DS_OK) return DS_ERROR;
  //
  // Wer die folgenden Namen entfernt, wird mit ewiger Verachtung bestraft!
  //
  str.Format("%c Copyright (C) 1995-98 Marc Diefenbruch (md@informatik.uni-essen.de)",
             CG_HEAD_CHAR);
  if (CGInsertString(str, 1, CG_WITH_EOL) != DS_OK) return DS_ERROR;
  str.Format("%c                and Joerg Ruehl (jruehl@informatik.uni-essen.de)",
             CG_HEAD_CHAR);
  if (CGInsertString(str, 1, CG_WITH_EOL) != DS_OK) return DS_ERROR;
  str.Format("%c", CG_HEAD_CHAR);
  if (CGInsertString(str, 1, CG_WITH_EOL) != DS_OK) return DS_ERROR;
  str.Format("%c All rights reserved.", CG_HEAD_CHAR);
  if (CGInsertString(str, 1, CG_WITH_EOL) != DS_OK) return DS_ERROR;
  str.Format("%c", CG_HEAD_CHAR);
  if (CGInsertString(str, 1, CG_WITH_EOL) != DS_OK) return DS_ERROR;

  if (CGInsertBaseline() != DS_OK) return DS_WRITE_ERROR;

  pretty_printing = old_pretty;

  return DS_OK;
}

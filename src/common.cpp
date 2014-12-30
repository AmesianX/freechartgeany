/*
 * common.cpp
 * 
 * Copyright 2013 Lucas Tsatiris <chartgeany@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <QtGui/QApplication>
#include <QTime>
#include <QIcon>
#include <QMessageBox>
#include <QTemporaryFile>
#include <QTextStream>
#include "common.h"

// delay nsecs
void 
delay(int secs)
{
  QTime dieTime= QTime::currentTime().addSecs(secs);
  while( QTime::currentTime() < dieTime )
  QCoreApplication::processEvents(QEventLoop::AllEvents, 100);    
}

// show message box
void
showMessage (QString message)
{
  QMessageBox msgBox;
  QFont fnt;

  QApplication::restoreOverrideCursor();

  fnt = msgBox.font ();
  fnt.setPixelSize (14);
  fnt.setFamily (DEFAULT_FONT_FAMILY);
  fnt.setWeight (QFont::DemiBold);

  msgBox.setWindowTitle ("Message");
  msgBox.setWindowIcon (QIcon (QString::fromLocal8Bit (":/png/images/icons/PNG/cglogo.png")));
  msgBox.setFont (fnt);
  msgBox.setIcon (QMessageBox::Information);
  msgBox.setText(message + "           ");
  msgBox.setStandardButtons(QMessageBox::Close);
  msgBox.setDefaultButton(QMessageBox::Close);
  correctWidgetFonts (&msgBox);
  msgBox.exec ();
}

// show download message box
bool
showDownloadMessage ()
{
  QMessageBox msgBox;
  QPushButton *downloadBtn, *closeBtn;
  QFont fnt;

  fnt = msgBox.font ();
  fnt.setPixelSize (14);
  fnt.setFamily (DEFAULT_FONT_FAMILY);
  fnt.setWeight (QFont::DemiBold);
  
  msgBox.setWindowTitle ("Download new version");
  msgBox.setWindowIcon (QIcon (QString::fromLocal8Bit (":/png/images/icons/PNG/cglogo.png")));
  msgBox.setFont (fnt);
  msgBox.setIcon (QMessageBox::Warning);
  msgBox.setText("There is a new version available for download");
  downloadBtn = msgBox.addButton ("Download", QMessageBox::AcceptRole);
  closeBtn = msgBox.addButton ("Close", QMessageBox::RejectRole);
  correctWidgetFonts (&msgBox);
  msgBox.exec ();
  if (msgBox.clickedButton() == (QAbstractButton *) downloadBtn)
    return true;
  return false;
}


// show Ok/Cancel message box
bool
showOkCancel (QString message)
{
  QMessageBox msgBox;
  QFont fnt;

  QApplication::restoreOverrideCursor();
  fnt = msgBox.font ();
  fnt.setPixelSize (14);
  fnt.setFamily (DEFAULT_FONT_FAMILY);
  fnt.setWeight (QFont::DemiBold);

  msgBox.setWindowTitle ("Question");
  msgBox.setWindowIcon (QIcon (QString::fromLocal8Bit (":/png/images/icons/PNG/cglogo.png")));
  msgBox.setFont (fnt);
  msgBox.setIcon (QMessageBox::Question);
  msgBox.setText(message);
  msgBox.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Cancel);
  correctWidgetFonts (&msgBox);
  msgBox.exec ();
  if (msgBox.clickedButton() == msgBox.button (QMessageBox::Ok))
    return true;

  return false;
}

// error messages
QString
errorMessage (CG_ERR_RESULT err)
{
  QStringList ErrorMessage;
  ErrorMessage  << "No error" <<
                "Cannot open file" <<
                "Cannot create temporary table" <<
                "Cannot create table" <<
                "Cannot insert data" <<
                "Cannot delete data" <<
                "Cannot access database" <<
                "Invalid reply or network error" <<
                "Cannot create temporary file" <<
                "Cannot write to file" <<
                "Transaction error" <<
                "Not enough memory" <<
                "Symbol does not exist" <<
                "Cannot access data" <<
                "Network timeout" <<
                "Invalid data";
  return ErrorMessage[err];
}

// set global error
void
setGlobalError(CG_ERR_RESULT err, const char *_file_, int _line_)
{
  if (GlobalError == CG_ERR_OK)
    GlobalError = err;
  
  return;
}

// sqlite drop table
static void
sql_drop_table (QString tablename)
{
  QString tempfilename, SQLCommand;
  char sqlcommand[512];

  SQLCommand = "drop table if exists " + tablename + ";";
  strcpy (sqlcommand, qPrintable(SQLCommand));
  sqlite3_exec(Application_Settings->db, sqlcommand, NULL, NULL, NULL);
}

// formats' callback
int
sqlcb_formats(void *dummy, int argc, char **argv, char **column)
{
  QString colname;

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    colname = QString::fromUtf8(column[counter]);
    if (colname == "FORMAT")
      ComboItems->formatList << QString::fromUtf8 (argv[counter]);
  }
  return 0;
}

// timeframes' callback
int
sqlcb_timeframes(void *dummy, int argc, char **argv, char **column)
{
  QString colname;

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    colname = QString::fromUtf8(column[counter]);
    if (colname == "TIMEFRAME")
      ComboItems->timeframeList << QString::fromUtf8 (argv[counter]);
  }
  return 0;
}

// currencies' callback
int
sqlcb_currencies(void *dummy, int argc, char **argv, char **column)
{
  QString colname;

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    colname = QString::fromUtf8(column[counter]);
    if (colname == "SYMBOL")
      ComboItems->currencyList << QString::fromUtf8 (argv[counter]);
  }
  return 0;
}

// currencies' callback
int
sqlcb_markets(void *dummy, int argc, char **argv, char **column)
{
  QString colname;

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    colname = QString::fromUtf8(column[counter]);
    if (colname == "MARKET")
      ComboItems->marketList << QString::fromUtf8 (argv[counter]);
  }
  return 0;
}

// datafeeds' callback
int
sqlcb_datafeeds (void *dummy, int argc, char **argv, char **column)
{
  QString colname;

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    colname = QString::fromUtf8(column[counter]);
    if (colname == "FEEDNAME")
      ComboItems->datafeedsList << QString::fromUtf8 (argv[counter]);
  }
  return 0;
}

// symbol's data frames callback
int
sqlcb_dataframes (void *dummy, int argc, char **argv, char **column)
{
  QTAChartFrame Frame;
  QString colname;
  
  for (qint32 counter = 0; counter < argc; counter ++)
  {
    colname = QString::fromUtf8(column[counter]);
    if (colname == "OPEN")
      Frame.Open = QString::fromUtf8(argv[counter]).toFloat ();

    if (colname == "HIGH")
      Frame.High = QString::fromUtf8(argv[counter]).toFloat ();

    if (colname == "LOW")
      Frame.Low = QString::fromUtf8(argv[counter]).toFloat ();

    if (colname == "CLOSE")
      Frame.Close = QString::fromUtf8(argv[counter]).toFloat ();
    
    if (colname == "ADJCLOSE")
      Frame.AdjClose = QString::fromUtf8(argv[counter]).toFloat ();  

    if (colname == "VOLUME")
      // Frame.Volume = QString::fromUtf8(argv[counter]).toLongLong ();
      Frame.Volume = QString::fromUtf8(argv[counter]).toFloat ();

    if (colname == "DATE")
      strcpy (Frame.Date, argv[counter]);

    if (colname == "TIME")
      strcpy (Frame.Time, argv[counter]);
  }

  Frames << Frame;
  return 0;
}

// sqlite3_exec callback for retrieving application options
int
sqlcb_options (void *classptr, int argc, char **argv, char **column)
{
  AppOptions *options = (AppOptions *) classptr;
  
  for (int counter = 0; counter < argc; counter ++)
  {
	QString colname = QString::fromUtf8(column[counter]);
	
	if (colname == "SHOWSPLASHSCREEN")
	{
	  if (atoi (argv[counter]) == 1)
	    options->showsplashscreen = true;
	  else
	    options->showsplashscreen = false;
	}
	
	if (colname == "CHECKNEWVERSION")
	{
	  if (atoi (argv[counter]))
	    options->checknewversion = true;
	  else
	    options->checknewversion = false;
	}
	
	if (colname == "ENABLEPROXY")
	{
	  if (atoi (argv[counter]))
	    options->enableproxy = true;
	  else
	    options->enableproxy = false;
	}
	
	if (colname == "PROXYHOST")
	  options->proxyhost = QString::fromUtf8 (argv[counter]); 
	
	if (colname == "PROXYUSER")
	  options->proxyuser = QString::fromUtf8 (argv[counter]);   
	
	if (colname == "PROXYPASS")
	  options->proxypass = QString::fromUtf8 (argv[counter]);   
	
	if (colname == "PROXYPORT")
	  options->proxyport = QString::fromUtf8(argv[counter]).toShort ();  
	
	if (colname == "NETTIMEOUT")
	  options->nettimeout = QString::fromUtf8(argv[counter]).toShort ();    
  }
  return 0;	
}

// nsymbols callback
int
sqlcb_nsymbols(void *nsymptr, int argc, char **argv, char **column)
{
  QString colname;
  int nsymbols;
  
  nsymbols = QString::fromUtf8(argv[0]).toInt ();
  *(int *) nsymptr = nsymbols; 
  
  return 0;
}

// load application options
CG_ERR_RESULT
loadAppOptions (AppOptions *options)
{
  QString query = "";
  int rc;
  
  query += "SELECT * FROM options WHERE recid = 1;";  
  rc = sqlite3_exec(Application_Settings->db, query.toUtf8(), 
                    sqlcb_options, options, NULL);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    return CG_ERR_DBACCESS;
  } 
  
  FULL = true;
  return CG_ERR_OK; 	
}	

// save application options
CG_ERR_RESULT
saveAppOptions (AppOptions *options)
{
  QString query = "";
  int rc;
  
  query += "BEGIN; UPDATE options SET ";
  query.append ("\n");
  query += "pak = \"\"";
  if (options->showsplashscreen)
    query += ", showsplashscreen = 1 ";
  else  
    query += ", showsplashscreen = 0 ";
  if (options->checknewversion)
    query += ", checknewversion = 1 ";
  else  
    query += ", checknewversion = 0 ";
  if (options->enableproxy)
    query += ", enableproxy = 1, ";
  else  
    query += ", enableproxy = 0, ";  
  query += "proxyhost = \"" + options->proxyhost + "\", ";
  query += "proxyuser = \"" + options->proxyuser + "\", ";
  query += "proxypass = \"" + options->proxypass + "\", ";
  query.append ("\n");
  query += "proxyport = " + QString::number (options->proxyport) + ", ";
  query += "nettimeout = " + QString::number (options->nettimeout) + " ";
  query += "WHERE recid = 1; COMMIT;";

  rc = sqlite3_exec(Application_Settings->db, query.toUtf8(), 
                    NULL, NULL, NULL);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    return CG_ERR_DBACCESS;
  } 
  
  FULL = true;
  return CG_ERR_OK; 	
}

static unsigned short
checksum16 (const char *data, int len)
{
  unsigned int sum = 0;
  if ((len & 1) == 0)
    len = len >> 1;
  else
    len = (len >> 1) + 1;
  while (len > 0)
    {
      sum += *((unsigned short *) data);
      data += sizeof (unsigned short);
      len--;
    }
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  return (~sum);
}

// set font size
void
correctFontSize (QWidget *widget)
{
  QFont fnt;
  
  fnt = widget->font ();
  if (fnt.pointSize () == -1)
    return;
  	
  fnt.setPixelSize (fnt.pointSize () + FONT_SIZE_PAD);
  fnt.setFamily (DEFAULT_FONT_FAMILY);
  widget->setFont (fnt);
}

// correct font size for widget and children
void
correctWidgetFonts (QWidget *widget)
{
   QList<QWidget *> allWidgets = widget->findChildren<QWidget *>();	
   QWidget *wid;
   
   foreach (wid, allWidgets)
     correctFontSize (wid);
   
   correctFontSize (widget);  
}

void
correctWidgetFonts (QDialog *widget)
{
   QList<QWidget *> allWidgets = widget->findChildren<QWidget *>();	
   QWidget *wid;
   
   foreach (wid, allWidgets)
     correctFontSize (wid);
   
   correctFontSize (widget);  
}

void
correctWidgetFonts (QMessageBox *widget)
{
   QList<QWidget *> allWidgets = widget->findChildren<QWidget *>();	
   QWidget *wid;
   
   foreach (wid, allWidgets)
     correctFontSize (wid);
   
   correctFontSize (widget);  
}

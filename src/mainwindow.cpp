/*
 * mainwindow.cpp
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

#include <QDesktopServices>
#include <QTime>
#include <QDir>
#include <QFontDatabase>
#include <QFileInfo>
#include <QFileDialog>
#include <QProgressBar>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDateTime>
#include <QColor>
#include <QPixmap>
#include <QResizeEvent>
#include <QFocusEvent>
#include "sqlite3.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "splashdialog.h"
#include "qtachart.h"

AppSettings *Application_Settings;
SQLists *ComboItems;
FrameVector Frames;
LoadCSVDialog *loadcsvdialog;
DownloadDataDialog *downloaddatadialog;
ProgressDialog *progressdialog;
QProgressBar *GlobalProgressBar;
QString Year, Month, Day;
bool FULL = false;

int GlobalError = CG_ERR_OK;

// load application fonts
void 
MainWindow::loadFonts ()
{
  QFile file;
  QByteArray barray;
  
  file.setFileName (":/fonts/fonts/DejaVuSans.ttf");
  if (file.open(QIODevice::ReadOnly))
  {
    barray = file.readAll ();
    QFontDatabase::addApplicationFontFromData (barray);
    file.close ();
  }
  
  file.setFileName (":/fonts/fonts/Monaco.ttf");
  if (file.open(QIODevice::ReadOnly))
  {
	barray.clear ();    
    barray = file.readAll ();
    QFontDatabase::addApplicationFontFromData (barray);
    file.close ();
  }
  
  file.setFileName (":/fonts/fonts/DroidSerif-Regular.ttf");
  if (file.open(QIODevice::ReadOnly))
  {
	barray.clear ();  
    barray = file.readAll ();
    QFontDatabase::addApplicationFontFromData (barray);
    file.close ();
  }
  
  file.setFileName (":/fonts/fonts/tahoma.ttf");
  if (file.open(QIODevice::ReadOnly))
  {
	barray.clear ();  
    barray = file.readAll ();
    QFontDatabase::addApplicationFontFromData (barray);
    file.close ();
  }
  
  barray.clear ();
}  

// check if there is new version available
void
MainWindow::checkNewVersion ()	
{
  QNetworkAccessManager *UpdateManager;
 
  newversion = false; 
  UpdateManager = new QNetworkAccessManager (this);
  connect (UpdateManager, SIGNAL (finished (QNetworkReply *)), this,
           SLOT (updateReplyFinished (QNetworkReply *)));
  
  UpdateManager->get (QNetworkRequest
#ifdef Q_OS_LINUX  
       (QUrl ("http://freechartgeany.sourceforge.net/version/linux/current.txt")));         
#else
#ifdef Q_OS_WIN
       (QUrl ("http://freechartgeany.sourceforge.net/version/windows/current.txt"))); 
#else       
#ifdef Q_OS_MAC
       (QUrl ("http://freechartgeany.sourceforge.net/version/mac/current.txt"))); 
#endif
#endif
#endif
}

static int
sqlcb_dbversion (void *versionptr, int argc, char **argv, char **column)
{
  QString colname;
  int version = -1;

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    colname = QString::fromUtf8(column[counter]);
    if (colname == "VERSION")
      version = QString::fromUtf8 (argv[counter]).toInt ();
  }
  *(int *) versionptr = version;
  return 0;
}

// constructor
MainWindow::MainWindow (QWidget * parent):
  QMainWindow (parent), ui (new Ui::MainWindow)
{
  SplashDialog *splash = NULL;	
  QDateTime datetime;
  QFileInfo dbfile;
  QFile initcopy;
  QString stylesheet = "background: transparent; background-color: white; color:black",
          SQLCommand = "";
  int rc, dbversion;
  bool showlicense = false;
  
  SQLCommand = "PRAGMA locking_mode = EXCLUSIVE;BEGIN EXCLUSIVE;COMMIT;";
  SQLCommand += "PRAGMA max_page_count = 4294967291;";
  SQLCommand += "PRAGMA mmap_size = 268435456;";
  ui->setupUi (this);
  expandedChartFlag = false;
  newversion = false;
  setWindowTitle (QApplication::applicationName ());
  this->setStatusBar ( 0 );
  this->setStyleSheet ("background-color:white;");
  tabWidget = ui->tabWidget;
  ui->tabWidget->clear ();
  ui->tabWidget->setUsesScrollButtons(true);
  ui->tabWidget->setStyleSheet("QTabBar::tab { height: 25px;}");
  ui->managerButton->setStyleSheet (stylesheet);
  ui->screenshotButton->setStyleSheet (stylesheet);
  ui->infoButton->setStyleSheet (stylesheet);
  ui->optionsButton->setStyleSheet (stylesheet);
  ui->exitButton->setStyleSheet (stylesheet);

  // current year, month - 1, day
  datetime = QDateTime::currentDateTime ();
  Year = QString::number(datetime.date ().year ());
  Month = QString::number(datetime.date ().month () - 1);
  Day = QString::number(datetime.date ().day ());
  
  // connect to signals
  connect (ui->tabWidget, SIGNAL(tabCloseRequested(int)), this,
           SLOT(closeTab_clicked (int)));
  connect (ui->tabWidget, SIGNAL(currentChanged(int)), this,
           SLOT(currentTab_changed (int)));
  connect (ui->managerButton, SIGNAL (clicked ()), this,
           SLOT (managerButton_clicked ()));
  connect (ui->screenshotButton, SIGNAL (clicked ()), this,
           SLOT (screenshotButton_clicked ()));
  connect (ui->optionsButton, SIGNAL (clicked ()), this,
           SLOT (optionsButton_clicked ())); 
  connect (ui->infoButton, SIGNAL (clicked ()), this,
           SLOT (infoButton_clicked ()));                          
  connect (ui->exitButton, SIGNAL (clicked ()), this,
           SLOT (exitButton_clicked ()));

  // export application settings
  Application_Settings = &appsettings;
  
  // set the sqlite db path
  appsettings.sqlitefile = QDir::homePath () + "/" + ".config" + "/" + APPDIR + "/" + DBNAME;

  // check db existence and create it if needed
  dbfile.setFile (appsettings.sqlitefile);
  if (dbfile.exists () == false)
  {
	showlicense = true;  
	if (!QDir (QDir::homePath () + "/" + ".config" + "/" + APPDIR).exists ())
	  QDir ().mkpath (QDir::homePath () + "/" + ".config" + "/" + APPDIR);

	if (dbman (1, appsettings) != CG_ERR_OK)
	{
	  showMessage (QString::fromUtf8 ("Cannot create or open database. Application quits."));  
	  qApp->exit (1);
      exit (1);
    }  
/*  Keep this here for development and debug. DO NOT DELETE
    initcopy.setFileName ("geanymasterbase.dat");
    initcopy.copy (appsettings.sqlitefile);
*/
  }
  
  // open sqlite db
  rc = sqlite3_open(qPrintable (appsettings.sqlitefile), &appsettings.db);
  if (rc != SQLITE_OK) // if open failed, quit application
  {
    showMessage (QString::fromUtf8 ("Cannot create or open database. Application quits."));
    sqlite3_close (appsettings.db);
    qApp->exit (1);
    exit (1);
  }

  // export classes and variables
  ComboItems = &comboitems;

  // apply pragmas
  rc = sqlite3_exec(Application_Settings->db, SQLCommand.toUtf8(), NULL, this, NULL);
  if (rc != SQLITE_OK) // if open failed, quit application
  {
    showMessage (QString::fromUtf8 ("Cannot create or open database. Application quits."));
    sqlite3_close (appsettings.db);
    qApp->exit (1);
    exit (1);
  }
  
  // check version
  SQLCommand = "SELECT * FROM VERSION;";
  rc = sqlite3_exec(Application_Settings->db, SQLCommand.toUtf8(), sqlcb_dbversion, (void *) &dbversion, NULL);
  if (rc != SQLITE_OK) // if open failed, quit application
  {
    showMessage (QString::fromUtf8 ("Cannot create or open database. Application quits."));
    sqlite3_close (appsettings.db);
    qApp->exit (1);
    exit (1);
  }
  
  if (dbversion == -1) // invalid dbversion
  {
	showMessage (QString::fromUtf8 ("Invalid data file. Application quits."));
    sqlite3_close (appsettings.db);
    qApp->exit (1);
    exit (1);
  }    
  
  if (dbversion < DBVERSION) // upgrade db file
  {
	dbversion ++;
	if (dbman (dbversion, appsettings) != CG_ERR_OK)
	{
	  showMessage (QString::fromUtf8 ("Cannot create or open database. Application quits."));  
	  sqlite3_close (appsettings.db);
	  qApp->exit (1);
      exit (1);
    }  
  }
  
  // show splash
  loadAppOptions (&Application_Settings->options);
  if (Application_Settings->options.showsplashscreen == true)
  {
    splash = new SplashDialog;
    splash->show ();
  }  
  
  // check for new vesrion
  checkNewVersion ();
    
  // load application fonts
  loadFonts ();
  
  // initialize SQL statements
  strcpy (comboitems.formats_query, "select FORMAT from FORMATS");
  strcpy (comboitems.timeframes_query, "select TIMEFRAME from TIMEFRAMES_ORDERED");
  strcpy (comboitems.currencies_query, "select SYMBOL from CURRENCIES");
  strcpy (comboitems.markets_query, "select MARKET from MARKETS");
  strcpy (comboitems.datafeeds_query, "select FEEDNAME from DATAFEEDS");

  // create widgets
  loadcsvdlg = new LoadCSVDialog (this);
  downloaddatadlg = new DownloadDataDialog (this);
  datamanagerdlg = new DataManagerDialog (this);
  progressdlg = new ProgressDialog (this);
  // optionsdlg = new OptionsDialog (this);
  infodlg = new InfoDialog (this);
  
  // export classes and variables
  loadcsvdialog = loadcsvdlg;
  downloaddatadialog = downloaddatadlg;
  progressdialog = progressdlg;

  // check if some initialization failed
  if (GlobalError != CG_ERR_OK)
  {
    ui->managerButton->setEnabled (false);
    ui->screenshotButton->setEnabled (false);
    ui->infoButton->setEnabled (false);
    ui->optionsButton->setEnabled (false);
    ui->tabWidget->setEnabled (false);
  }

  // initially remove all tabs
  while (ui->tabWidget->count () > 1)
    ui->tabWidget->removeTab (0);

  ui->tabWidget->resize (width () - 2, height () - 60);
  
  if (Application_Settings->options.showsplashscreen == true)
  {
    delay (3);
    splash->hide ();
    delete splash;
  }  
   
  if (showlicense)
  {
    infodlg->licensedlg->show ();
    delay (2);
  }  
    
  if (newversion)
    if (Application_Settings->options.checknewversion == true)
      if (showDownloadMessage ())
        QDesktopServices::openUrl (QUrl (APPWEBPAGE));
}

// destructor
MainWindow::~MainWindow ()
{
  // delete optionsdlg; // save settings before sqlite3_close	
  delete infodlg;
  delete ui;
  
  // vacuum database if possible
  sqlite3_exec(Application_Settings->db, "VACUUM", NULL, NULL, NULL);

  // close database
  sqlite3_close (Application_Settings->db);
}

// add a new chart
CG_ERR_RESULT
MainWindow::addChart (TableDataVector & datavector)
{
  QList<QAbstractButton*> allPButtons;	
  QTAChart *tachart;
  TableDataClass tdc;
  QString SQLCommand, title, subtitle, tablename, 
          symbol, timeframe, name;
  int rc;
  CG_ERR_RESULT result = CG_ERR_OK;
  
  tachart = new QTAChart (ui->tabWidget);
  foreach (tdc, datavector)
  {
	tablename = tdc.tablename;
	symbol = tdc.symbol;
	timeframe = tdc.timeframe;
	name = tdc.name;
	  
    Frames.clear ();
    SQLCommand = "select * from " + tablename + ";";
    rc = sqlite3_exec(Application_Settings->db, SQLCommand.toUtf8(),
                      sqlcb_dataframes, NULL, NULL);
    if (rc != SQLITE_OK)
    {
	  delete tachart;  
      result = CG_ERR_ACCESS_DATA;
      setGlobalError(result, __FILE__, __LINE__);
      showMessage ("Symbol " +symbol +": " + errorMessage (result));
      return result;
    }
  
    if (timeframe == "DAY")
      tachart->loadFrames (Frames, "D", timeframe);
    else
    if (timeframe == "WEEK")
      tachart->loadFrames (Frames, "W", timeframe);
    else
    if (timeframe == "MONTH")
      tachart->loadFrames (Frames, "M", timeframe);  
    else
    if (timeframe == "YEAR")
      tachart->loadFrames (Frames, "Y", timeframe);   
  }
  
  tachart->setSymbol (symbol);  
  title = symbol;
  subtitle = name;
  
  tachart->setAlwaysRedraw (true);
  tachart->setTitle (title, subtitle);
  ui->tabWidget->addTab (tachart, symbol + " " + (tdc.adjusted == "NO"?"RAW":"ADJ"));
  ui->tabWidget->setCurrentIndex (ui->tabWidget->count () - 1);
  
  // remove tooltip from close tab buttons
  allPButtons = ui->tabWidget->findChildren<QAbstractButton*>();	
  for (int ind = 0; ind < allPButtons.size(); ind++) 
    if (allPButtons.at(ind)->inherits("CloseButton"))
      allPButtons.at(ind)->setToolTip("");
  
  return result;
}

// expanded chart
bool
MainWindow::expandedChart ()
{
  return expandedChartFlag;
}

// expand/shrink chart
void
MainWindow::setExpandChart (bool expandflag)
{
  if (expandflag == expandedChartFlag)
    return;

  expandedChartFlag = expandflag;
  if (!expandflag)
  {
    int max = ui->tabWidget->count ();

    if (max == 0)
      return;
    ui->managerButton->show ();    
    ui->screenshotButton->show ();  
    ui->optionsButton->show (); 
    ui->infoButton->show ();  
    ui->exitButton->show ();
    ui->tabWidget->move (0, 55);
    ui->tabWidget->resize (width () - 2, height () - 60);

    for (qint32 counter = 0; counter < max; counter ++)
      ui->tabWidget->widget(counter)->resize (ui->tabWidget->width () - 2,
                                              ui->tabWidget->height () - 20);
  }
  else
  {
	ui->managerButton->hide ();  
	ui->screenshotButton->hide ();   
	ui->optionsButton->hide ();  
	ui->infoButton->hide ();     
    ui->exitButton->hide ();
    ui->tabWidget->move (1, 1);
    ui->tabWidget->resize (width () - 2, height ());
    int max = ui->tabWidget->count ();
    if (max == 0)
      return;

    for (qint32 counter = 0; counter < max; counter ++)
      ui->tabWidget->widget(counter)->resize (ui->tabWidget->width () - 2,
                                              ui->tabWidget->height () - 20);
  }
}

/// Signals
///

// update reply
void 
MainWindow::updateReplyFinished (QNetworkReply * updatereply)
{
  QByteArray replybytes = updatereply->readAll ();
  QString replystring (replybytes);
    
  QStringList versiondigits =
    replystring.split (".", QString::SkipEmptyParts);
  
  newversion = false;
  if (versiondigits.length () == 3)	
  {
	 if (versiondigits[0].toInt () > VERSION_MAJOR)
	 {
	   newversion = true;
	   return;
	 }  
	 
	 if (versiondigits[0].toInt () < VERSION_MAJOR)
	   return;
	 	 
	 if (versiondigits[1].toInt () > VERSION_MINOR)
	 {
	   newversion = true;
	   return;
	 }
	 
	 if (versiondigits[1].toInt () < VERSION_MINOR)
	   return;
	 
	 if (versiondigits[2].toInt () > VERSION_PATCH)
	 {
	   newversion = true;
	   return;
	 }
  }
}

// managerButton_clicked ()
void
MainWindow::managerButton_clicked ()
{
  datamanagerdlg->show ();
}

// managerButton_clicked ()
void
MainWindow::screenshotButton_clicked ()
{
  QFileDialog *fileDialog;
  QTAChart *chart;
  QString fileName = "";
  QPixmap screenshot;
  
  if (ui->tabWidget->count () == 0)
  {
	showMessage ("Open a chart first please.");  
    return;
  }  
  
  fileDialog = new QFileDialog;  
  chart = (QTAChart *) ui->tabWidget->widget(ui->tabWidget->currentIndex ());
  chart->setCustomBottomText (APPWEBPAGE);
  screenshot = QPixmap::grabWidget (chart);
  fileName = fileDialog->getSaveFileName(this, "Save chart", "", "Image (*.png)"); 
  
  if (fileName == "")
    goto screenshotButton_clicked_end;  
  
  if (fileName.mid (fileName.size () - 4).toLower () != ".png")
    fileName += ".png";
  
  screenshot.save(fileName, "PNG");  
  showMessage ("Screenshot saved.");

screenshotButton_clicked_end:  
  chart->restoreBottomText ();
  delete fileDialog;
  
}

// exit_clicked ()
void
MainWindow::exitButton_clicked ()
{
  bool answer;

  answer = showOkCancel ("Quit " + QApplication::applicationName () + "?");
  if (!answer)
    return;

  qApp->exit (0);
}

// options_clicked ()
void
MainWindow::optionsButton_clicked ()
{
  optionsdlg = new OptionsDialog (this);	
  optionsdlg->exec ();
  delete optionsdlg;
}

// info_clicked ()
void
MainWindow::infoButton_clicked ()
{
  infodlg->show ();
}

// closeTab_clicked ()
void
MainWindow::closeTab_clicked (int index)
{
  QWidget *chart;

  if (ui->tabWidget->count () == 1)
    setExpandChart (false);

  ui->tabWidget->widget (index)->hide ();
  chart = ui->tabWidget->widget (index);
  ui->tabWidget->removeTab (index);
  delete chart;
}

// currentTab_changed ()
void
MainWindow::currentTab_changed (int index)
{
  int max;

  max = ui->tabWidget->count ();
  if (max < 1)
    return;

  for (qint32 counter = 0; counter < max; counter ++)
    ui->tabWidget->findChild<QTabBar *>(QLatin1String("qt_tabwidget_tabbar"))->
    setTabTextColor (counter, QColor(Qt::black));

  ui->tabWidget->findChild<QTabBar *>(QLatin1String("qt_tabwidget_tabbar"))->
  setTabTextColor (index, QColor(Qt::blue));

  ui->tabWidget->widget (index)->setFocus (Qt::OtherFocusReason);
}

/// Events
///
// resize
void
MainWindow::resizeEvent (QResizeEvent * event)
{
  QSize newsize;	
  int max;
  
  newsize = event->size ();
  ui->exitButton->move (newsize.width () - 50, ui->exitButton->y ());
  if (!expandedChartFlag )
    ui->tabWidget->resize (newsize.width () - 2, newsize.height () - 60);
  else
    ui->tabWidget->resize (newsize.width () - 2, newsize.height ());

  max = ui->tabWidget->count ();
  if (max == 0)
    return;

  for (qint32 counter = 0; counter < max; counter ++)
    ui->tabWidget->widget(counter)->resize (ui->tabWidget->width () - 2,
                                            ui->tabWidget->height () - 20);
}

// focus
void
MainWindow::focusInEvent (QFocusEvent * event)
{
  ui->tabWidget->setFocus (event->reason ());
}

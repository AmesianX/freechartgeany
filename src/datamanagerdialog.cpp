/*
 * datamanagerdialog.cpp
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
#include <QDialogButtonBox>
#include <QScrollBar>
#include <QResizeEvent>
#include "ui_datamanagerdialog.h"
#include "mainwindow.h"
#include "common.h"
#include "feedyahoo.h"
#include "feedgoogle.h"

static const int NCOLUMNS = 11;

// constructor
DataManagerDialog::DataManagerDialog (QWidget * parent):
  QDialog (parent), ui (new Ui::DataManagerDialog)
{
  QString stylesheet = "background: transparent; background-color: white;",
          buttonstylesheet = "background: transparent; background-color: white; color:black";
  int colwidth = 0;

  cheadersList << "  Symbol  "
               << "  Name  "
               << "  Market  "
               << "  Feed  "
               << "  T.F.  "
               << "  Starts  "
               << "  Ends  "
               << "  Curr.  "
               << "  Key  "
               << "  Adj.  "
               << "  Base  ";


  ui->setupUi (this);
  
  this->setStyleSheet (stylesheet);
  ui->tableWidget->setColumnCount (NCOLUMNS);
  ui->tableWidget->setHorizontalHeaderLabels (cheadersList);
  ui->tableWidget->sortByColumn (0, Qt::AscendingOrder);
  ui->tableWidget->setStyleSheet (stylesheet);
  ui->tableWidget->verticalScrollBar ()->setStyleSheet ("background: transparent; background-color:lightgray;");
  ui->tableWidget->horizontalScrollBar ()->setStyleSheet ("background: transparent; background-color:lightgray;");
  ui->importButton->setStyleSheet (buttonstylesheet);
  ui->downloadButton->setStyleSheet (buttonstylesheet);
  ui->trashButton->setStyleSheet (buttonstylesheet);
  ui->refreshButton->setStyleSheet (buttonstylesheet);
  ui->updateButton->setStyleSheet (buttonstylesheet);
  ui->exitButton->setStyleSheet (buttonstylesheet);
  ui->chartButton->setStyleSheet (buttonstylesheet);
  ui->tableWidget->setColumnHidden (4, true);
  ui->tableWidget->setColumnHidden (8, true);
  ui->tableWidget->setColumnHidden (10, true);
  reloadSymbols ();

  for (qint32 counter = 0; counter < NCOLUMNS - 1; counter ++)
    colwidth += ui->tableWidget->columnWidth (counter);

  this->resize (colwidth + 40, height ());

  // connect to signals
  connect (ui->chartButton, SIGNAL (clicked ()), this,
           SLOT (chartButton_clicked ()));
  connect (ui->importButton, SIGNAL (clicked ()), this,
           SLOT (importButton_clicked ()));
  connect (ui->downloadButton, SIGNAL (clicked ()), this,
           SLOT (downloadButton_clicked ()));
  connect (ui->refreshButton, SIGNAL (clicked ()), this,
           SLOT (refreshButton_clicked ()));
  connect (ui->exitButton, SIGNAL (clicked ()), this,
           SLOT (exitButton_clicked ()));
  connect (ui->trashButton, SIGNAL (clicked ()), this,
           SLOT (trashButton_clicked ()));
  connect (ui->updateButton, SIGNAL (clicked ()), this,
           SLOT (updateButton_clicked ()));
  connect(ui->tableWidget, SIGNAL(doubleClicked(const QModelIndex &)), this,
          SLOT(symbol_double_clicked ()));
  
  correctWidgetFonts (this);        
}

// destructor
DataManagerDialog::~DataManagerDialog ()
{
  cleartable ();
  delete ui;
}

// clear table
void
DataManagerDialog::cleartable ()
{
  int row, nrows, col, ncols = NCOLUMNS;
  
  nrows = ui->tableWidget->rowCount ();
  for (row = 0; row < nrows; row ++)
    for (col = 0; col < ncols; col ++)
       delete ui->tableWidget->takeItem(row,col);
}

// fill table column
void
DataManagerDialog::fillcolumn (QStringList list, int col)
{
  for (qint32 counter = 0; counter < list.size (); counter++)
  {
	QTableWidgetItem *item;
	  
    item = new QTableWidgetItem;
    item->setText (list[counter]);
    ui->tableWidget->setItem(counter,col,item);
  }
}

// reload all symbols
void
DataManagerDialog::reloadSymbols ()
{
  int rc;
  const char query[] = "select SYMBOL, DESCRIPTION, MARKET, SOURCE, TIMEFRAME, \
                        DATEFROM, DATETO, CURRENCY, KEY, ADJUSTED, BASE from SYMBOLS_ORDERED";

  symbolList.clear ();
  descList.clear ();
  marketList.clear ();
  sourceList.clear ();
  timeframeList.clear ();
  datefromList.clear ();
  datetoList.clear ();
  currencyList.clear ();
  keyList.clear ();
  adjustedList.clear ();
  baseList.clear ();

  rc = sqlite3_exec(Application_Settings->db, query, sqlcb_symbol_table, this, NULL);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    showMessage (errorMessage (CG_ERR_DBACCESS));
    this->hide ();
    return;
  }

  cleartable ();
  ui->tableWidget->setSortingEnabled (false);
  ui->tableWidget->setRowCount (symbolList.size ());
  fillcolumn (symbolList, 0);
  fillcolumn (descList, 1);
  fillcolumn (marketList, 2);
  fillcolumn (sourceList, 3);
  fillcolumn (timeframeList, 4);
  fillcolumn (datefromList, 5);
  fillcolumn (datetoList, 6);
  fillcolumn (currencyList, 7);
  fillcolumn (keyList, 8);
  fillcolumn (adjustedList, 9);
  fillcolumn (baseList, 10);
  
  ui->tableWidget->resizeColumnToContents (0);
  ui->tableWidget->resizeColumnToContents (1);
  ui->tableWidget->resizeColumnToContents (2);
  ui->tableWidget->resizeColumnToContents (3);
  ui->tableWidget->resizeColumnToContents (4);
  ui->tableWidget->resizeColumnToContents (5);
  ui->tableWidget->resizeColumnToContents (6);
  ui->tableWidget->resizeColumnToContents (7);
  ui->tableWidget->resizeColumnToContents (9);
  ui->tableWidget->viewport()->update();
  
  ui->tableWidget->setSortingEnabled (true);
}

// symbol list poppulate
static void
populate_table (DataManagerDialog *dialog, char *str, int col)
{
  switch (col)
  {
  case 0:
    dialog->symbolList << QString::fromUtf8 (str);
    break;

  case 1:
    dialog->descList << QString::fromUtf8 (str);
    break;

  case 2:
    dialog->marketList << QString::fromUtf8 (str);
    break;

  case 3:
    dialog->sourceList << QString::fromUtf8 (str);
    break;

  case 4:
    dialog->timeframeList << QString::fromUtf8 (str);
    break;

  case 5:
    dialog->datefromList << QString::fromUtf8 (str);
    break;

  case 6:
    dialog->datetoList << QString::fromUtf8 (str);
    break;

  case 7:
    dialog->currencyList << QString::fromUtf8 (str);
    break;
    
  case 8:
    dialog->keyList << QString::fromUtf8 (str);
    break;
  
  case 9:
    dialog->adjustedList << QString::fromUtf8 (str);
    break; 
    
  case 10:
    dialog->baseList << QString::fromUtf8 (str);
    break;   
  }
}

// sqlite3_exec callback for retrieving symbol table
int
sqlcb_symbol_table(void *classptr, int argc, char **argv, char **column)
{
  DataManagerDialog *dialog = (DataManagerDialog *) classptr;
  
  for (int counter = 0; counter < argc; counter ++)
    populate_table (dialog, argv[counter], counter);

  return 0;
}

/// Signals
///
// importButton_clicked ()
void
DataManagerDialog::importButton_clicked ()
{
  ui->tableWidget->clearSelection ();
  loadcsvdialog->show ();
}

// downloadButton_clicked ()
void
DataManagerDialog::downloadButton_clicked ()
{
  QString SQLCommand = "SELECT * FROM NSYMBOLS;";
  int rc, nsymbols;
  
  rc = sqlite3_exec(Application_Settings->db, SQLCommand.toUtf8(), sqlcb_nsymbols, 
                    (void *) &nsymbols, NULL);
  if (rc != SQLITE_OK) 
  {
	setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);  
    showMessage (errorMessage (CG_ERR_DBACCESS));
    return;
  }
  	
  ui->tableWidget->clearSelection ();
  downloaddatadialog->show ();
}

// refreshButton_clicked ()
void
DataManagerDialog::refreshButton_clicked ()
{
  ui->tableWidget->clearSelection ();
  reloadSymbols ();
}

// exitButton_clicked ()
void
DataManagerDialog::exitButton_clicked ()
{
  ui->tableWidget->clearSelection ();
  this->hide ();
}

// sqlite3_exec callback for retieving sqlite_master table
int
sqlcb_sqlite_master (void *classptr, int argc, char **argv, char **column)
{
  DataManagerDialog *dialog = (DataManagerDialog *) classptr;
  
  if (argc >= 2)
  {
    dialog->sqlite_master_type << QString::fromUtf8 (argv[0]);
    dialog->sqlite_master_name << QString::fromUtf8 (argv[1]);
  }
    
  return 0;
}

// form a DROP SQL sentence for the given table or view
QString
DataManagerDialog::formSQLDropSentence (QString table, qint32 *nentries)
{
  QString query, SQLCommand = "";
  int rc;
  
  *nentries = 0;  
  query = "select type, name from sqlite_master where name like '" +
                table + "%';"; 	
  rc = sqlite3_exec(Application_Settings->db, query.toUtf8(), sqlcb_sqlite_master, this, NULL);
  
  if (rc == SQLITE_OK)
  for (int counter = 0, maxcounter = sqlite_master_type.size (); 
       counter < maxcounter; counter ++)
  {
	if (sqlite_master_type[counter] == "table" || 
	    sqlite_master_type[counter] == "view") 
	{    
	
	  (*nentries) ++;  
	  SQLCommand += "DROP " +  sqlite_master_type[counter] +
	               " IF EXISTS " + sqlite_master_name[counter] + ";";
	  SQLCommand += "DELETE FROM SYMBOLS WHERE KEY = '" + 
	              sqlite_master_name[counter] + "'; ";             
	  SQLCommand.append ('\n');      
	}  
  }	
  
  return SQLCommand;
}

// trashButton_clicked ()
void
DataManagerDialog::trashButton_clicked ()
{
  QString SQLCommand, table;
  QStringList selected_tables;
  qint32 entries = 0, totalentries = 0;
  int row, maxrow, rc;

  selected_tables.clear ();
  sqlite_master_name.clear ();
  sqlite_master_type.clear ();

  maxrow = ui->tableWidget->rowCount ();
  for (row = 0; row < maxrow; row ++)
    if (ui->tableWidget->item (row, 0)->isSelected ())
      selected_tables << ui->tableWidget->item (row, 8)->text ();

  ui->tableWidget->clearSelection ();

  if (selected_tables.size () == 0)
  {
	showMessage ("Select symbols first please.");    
    return;
  }  

  if (showOkCancel ("Delete selected entries ? ") == false)
    return;

  SQLCommand = "begin; ";
  maxrow = selected_tables.size ();
  for (row = 0; row < maxrow; row ++)
  {
    table = selected_tables[row];

    if (maxrow > 100)
      GlobalProgressBar->setValue (GlobalProgressBar->value () + (100/maxrow));
    
    SQLCommand += formSQLDropSentence (table, &entries);
    totalentries += entries;
  }
  SQLCommand += "commit;";
  
  if (totalentries > 1)
  {    
    if (showOkCancel ("Entries depended on other entries (eg: entries for adjusted prices) will be deleted too. Are you sure ? ") == false)
      return;	  
  }
  
  if (maxrow > 100)
  {
    GlobalProgressBar = progressdialog->getProgressBar ();
    GlobalProgressBar->setValue (0);
    progressdialog->setMessage ("Deleting selected entries");
    progressdialog->show ();
  }
  
  rc = sqlite3_exec(Application_Settings->db, SQLCommand.toUtf8(),
                    NULL, NULL, NULL);
  if (maxrow > 100)
    progressdialog->hide ();

  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DELETE_DATA, __FILE__, __LINE__);
    showMessage (errorMessage (CG_ERR_DELETE_DATA));
  }

  refreshButton_clicked ();
}

// updateButton_clicked ()
void
DataManagerDialog::updateButton_clicked ()
{
  YahooFeed YF;
  GoogleFeed GF;
  QStringList symbol, timeframe, currency, feed, dateto, adjusted, base, key;
  QString prevbase = "";
  CG_ERR_RESULT result;
  int row, maxrow;
  bool adjbool = true;

  maxrow = ui->tableWidget->rowCount ();
  for (int counter = 0; counter < maxrow; counter ++)
  {
	if (ui->tableWidget->item (counter, 0)->isSelected ())  
      for (row = 0; row < maxrow; row ++)
      {
	    if (ui->tableWidget->item (counter, 10)->text () ==
	        ui->tableWidget->item (row, 8)->text ())
	      if (!base.contains (ui->tableWidget->item (counter, 10)->text (), Qt::CaseSensitive))
	      {
			symbol << ui->tableWidget->item (row, 0)->text ();
            timeframe << ui->tableWidget->item (row, 4)->text ();
            currency << ui->tableWidget->item (row, 7)->text ();
            feed << ui->tableWidget->item (row, 3)->text ();
            dateto << ui->tableWidget->item (row, 6)->text ();
            key << ui->tableWidget->item (row, 8)->text ();
            adjusted << ui->tableWidget->item (row, 9)->text ();
            base << ui->tableWidget->item (row, 10)->text ();  
	      }	     
      }	 	  
  }

  ui->tableWidget->clearSelection ();
  if (feed.size () == 0)
  {
	showMessage ("Select symbols first please.");  
    return;
  }  

  if (showOkCancel ("Update selected entries ? ") == false)
    return;

  GlobalProgressBar = progressdialog->getProgressBar ();
  progressdialog->show ();
  maxrow = feed.size ();
  for (row = 0; row < maxrow; row ++)
  {
    if (feed[row] == "YAHOO")
    {
	  
      progressdialog->setMessage ("Updating entries for symbol: " + symbol[row]);
      result = YF.downloadData (symbol[row], timeframe[row], currency[row], 
                                "UPDATE", adjbool);
      if (result != CG_ERR_OK)
      {
        progressdialog->hide ();
        showMessage (errorMessage (result));
        return;
      }
    }
    
    if (feed[row] == "GOOGLE")
    {
	  
      progressdialog->setMessage ("Updating entries for symbol: " + symbol[row]);
      result = GF.downloadData (symbol[row], timeframe[row], currency[row], 
                                "UPDATE", adjbool);
      if (result != CG_ERR_OK)
      {
        progressdialog->hide ();
        showMessage (errorMessage (result));
        return;
      }
    }
  }

  progressdialog->hide ();
  if (result == CG_ERR_OK)
    showMessage ("Update completed");
  refreshButton_clicked ();
}

// callback for sqlite3_exec
static int
sqlcb_table_data (void *classptr, int argc, char **argv, char **column)
{ 
  DataManagerDialog *dmd = (DataManagerDialog *) classptr;	
  TableDataClass tdc;
  
  tdc.tablename = QString::fromUtf8 (argv[0]);	
  tdc.symbol = QString::fromUtf8 (argv[1]);
  tdc.timeframe = QString::fromUtf8 (argv[2]);
  tdc.name = QString::fromUtf8 (argv[3]);
  tdc.adjusted = QString::fromUtf8 (argv[4]);
  dmd->TDVector += tdc;
  
  return 0;	
}

// fill the TableDataVector
CG_ERR_RESULT
DataManagerDialog::fillTableDataVector (QString base, QString adjusted) 
{
  QString query = "";
  int rc;
  
  query += "SELECT key, symbol,  timeframe, description, adjusted ";
  query += "FROM symbols WHERE base = '" + base + "' AND ";  
  query += "ADJUSTED = '" + adjusted + "' ORDER BY tfresolution ASC;";
  
  TDVector.clear ();  
  rc = sqlite3_exec(Application_Settings->db, query.toUtf8(), 
                    sqlcb_table_data, this, NULL);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    showMessage (errorMessage (CG_ERR_DBACCESS));
    return CG_ERR_DBACCESS;
  } 
  
  return CG_ERR_OK; 	
}

// chartButton_clicked ()
void
DataManagerDialog::chartButton_clicked ()
{
  QStringList tablename, symbol, timeframe, name, adjusted, base;
  int row, maxrow;

  maxrow = ui->tableWidget->rowCount ();
  for (row = 0; row < maxrow; row ++)
    if (ui->tableWidget->item (row, 0)->isSelected ())
    {
      tablename << ui->tableWidget->item (row, 8)->text ();
      symbol << ui->tableWidget->item (row, 0)->text ();
      name << ui->tableWidget->item (row, 1)->text ();
      timeframe << ui->tableWidget->item (row, 4)->text ();
      adjusted << ui->tableWidget->item (row, 9)->text ();
      base << ui->tableWidget->item (row, 10)->text ();
    }

  maxrow = tablename.size ();
  if (maxrow == 0)
  {
	showMessage ("Select symbols first please.");  
    return;
  }  
  
  this->hide ();
  for (row = 0; row < maxrow; row ++)
  {
	if (fillTableDataVector (base[row], adjusted[row]) != CG_ERR_OK)
	  return;  
	  
    ((MainWindow*) parent ())->addChart (TDVector);
    qApp->processEvents ();
  }
}

/// Events
///
// resize
void
DataManagerDialog::resizeEvent (QResizeEvent * event)
{
  QSize newsize;
  newsize = event->size ();	
  ui->exitButton->move (newsize.width () - 50, ui->exitButton->y ());
  ui->tableWidget->resize (newsize.width () - 10, newsize.height () - 60);
}

// show
void
DataManagerDialog::showEvent (QShowEvent * event)
{
  ui->tableWidget->clearSelection ();
}

// change
void
DataManagerDialog::changeEvent (QEvent * event)
{
  refreshButton_clicked ();
}

// symbol double clicked
void
DataManagerDialog::symbol_double_clicked ()
{
  chartButton_clicked ();
}

// delete key
void 
DataManagerDialog::keyPressEvent (QKeyEvent * event)
{
  if (event->key () == Qt::Key_Delete)	
    trashButton_clicked ();
}

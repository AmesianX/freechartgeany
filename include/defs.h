/*
 * defs.h
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

#ifndef DEFS_H
#define DEFS_H

#include <QStringList>
#include <QProgressBar>
#include "appdata.h"
#include "sqlite3.h"

#define QSTR_SIZE			128

// Error codes
typedef enum
{
  CG_ERR_OK = 0,				// "No error"
  CG_ERR_OPEN_FILE,	        	// "Cannot open file"
  CG_ERR_CREATE_TEMPTABLE,		// "Cannot create temporary table"
  CG_ERR_CREATE_TABLE,			// "Cannot create table"
  CG_ERR_INSERT_DATA,			// "Cannot insert data"
  CG_ERR_DELETE_DATA,			// "Cannot delete data"
  CG_ERR_DBACCESS,				// "Cannot access database"
  CG_ERR_NETWORK,				// "Network error"
  CG_ERR_CREATE_TEMPFILE,		// "Cannot create temporary file"
  CG_ERR_WRITE_FILE,    		// "Cannot write to file"
  CG_ERR_TRANSACTION,			// "Transaction error"
  CG_ERR_NOMEM,  				// "Not enough memory"
  CG_ERR_NOSYMBOL,				// "Symbol does not exist"
  CG_ERR_ACCESS_DATA,			// "Cannot access data"
  CG_ERR_NETWORK_TIMEOUT,		// "Network timeout"
  CG_ERR_INVALID_DATA			// "Invalid data"
} CG_ERRORS;

typedef int CG_ERR_RESULT;

// Application options structure
class AppOptions
{
public:	
  QString proxyhost;			// proxy host name or IP
  QString proxyuser;			// proxy user name
  QString proxypass;			// proxy password
  qint16  nettimeout;			// network timeout in seconds
  qint16  proxyport;			// proxy port
  bool showsplashscreen;		// show splash screen
  bool checknewversion;			// check new version
  bool enableproxy;				// enable proxy
};

// Lists populated by SQL statements
class SQLists
{
public:
  QStringList formatList;		// list of supported formats
  QStringList timeframeList;	// list of supported timeframes
  QStringList currencyList;		// list of supported currencies
  QStringList marketList;		// list of supported markets
  QStringList datafeedsList;	// list of supported datafeeds
  char formats_query[QSTR_SIZE];  	 // "select FORMAT from FORMATS"
  char timeframes_query[QSTR_SIZE];  // "select TIMEFRAME from TIMEFRAMES_ORDERED"
  char currencies_query[QSTR_SIZE];  // "select SYMBOL from CURRENCIES"
  char markets_query[QSTR_SIZE];     // "select MARKET from MARKETS"
  char datafeeds_query[QSTR_SIZE];     // "select FEEDNAME from DATAFEEDS"
};


// application settings
class AppSettings
{
public:
  sqlite3 *db;			// database handler
  QString sqlitefile;   // full path of database file
  AppOptions options;   // application options
};


// symbol entry input data
class SymbolEntry
{
public:
  QString csvfile;		// path of csv file
  QString tablename;	// table name (eg AAPL_OTHER_DAY_CSV)
  QString tmptablename; // temporary table name (eg TMP_GSPC_OTHER_DAY_CSV)
  QString symbol;		// symbol (eg AAPL)
  QString name;			// company/index name (eg Apple Inc.)
  QString market;		// market (eg NYSE)
  QString timeframe;	// timeframe (DAY, WEEK, MONTH)
  QString currency;		// currency (USD, EUR etc)
  QString format;		// csv format (eg YAHOO)
  QString source;		// source (CSV, YAHOO etc)
  QString dnlstring;	// download string
  bool    adjust;       // true: adjust data, false: do not adjust
};

class TableDataClass
{
public:
  QString tablename;
  QString symbol;
  QString timeframe;
  QString name; 
  QString adjusted;	
};

typedef QVector < TableDataClass > TableDataVector;

extern AppSettings *Application_Settings;		// application settings
extern int GlobalError;							// global error code
extern QProgressBar *GlobalProgressBar;			// progress bar proxy
extern QString Year, Month, Day;
extern bool FULL;								// true if full version, false otherwise

// set global error
extern void
setGlobalError(CG_ERR_RESULT err, const char *_file_, int _line_);

// load a csv file to sqlite
// operation may be "CREATE" or "UPDATE"
extern CG_ERR_RESULT
csv2sqlite (SymbolEntry *data, QString operaion);

// sqlite3_exec callback for retrieving application options
extern int
sqlcb_options (void *classptr, int argc, char **argv, char **column);

// load application options
extern CG_ERR_RESULT loadAppOptions (AppOptions *options);

// save application options
extern CG_ERR_RESULT saveAppOptions (AppOptions *options);

// database manager
extern CG_ERR_RESULT 
dbman (int dbversion, AppSettings appsettings);

// windows vista check
#ifdef Q_WS_WIN
extern bool IsWinVistaOrLater();
#endif

// define tooltip style 
#ifndef TOOLTIP
#ifdef Q_WS_WIN		// windows
// #define TOOLTIP  (IsWinVistaOrLater()?QString ("<span style=\"background-color:white; color: black;\">"):QString ("<span style=\"background-color:black; color: white;\">"))
#define TOOLTIP  (IsWinVistaOrLater()?QString ("<span style=\"\">"):QString ("<span style=\"background-color:black; color: white;\">"))
#else  				// unix
#define TOOLTIP  QString ("<span style=\"background-color:black; color: white; font: 11px;\">")
#endif   
#endif

// default font and size pad
#define DEFAULT_FONT_FAMILY "Tahoma"
#define FONT_SIZE_PAD		3

#endif // DEFS_H

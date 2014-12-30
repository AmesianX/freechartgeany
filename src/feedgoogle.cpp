/*
 * feedgoogle.cpp
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

#include <QApplication>
#include <QByteArray>
#include <QNetworkRequest>
#include <QTimer>
#include "feedgoogle.h"

// constructor
GoogleFeed::GoogleFeed ()
{
  checkManager = NULL;
  downloadManager = NULL;
}

// destructor
GoogleFeed::~GoogleFeed ()
{
  return;
}

// validate symbol
bool
GoogleFeed::validSymbol (QString symbol)
{

  return true;
}

// return symbol check URL
// eg: http://www.google.com/finance/info?infotype=infoquoteall&q=C
QString
GoogleFeed::symbolURL (QString symbol)
{
  QString urlstr = "";

  if (symbol.size () == 0)
    return urlstr;

  urlstr = "http://www.google.com/finance/info?infotype=infoquoteall&q=";
  urlstr += symbol;
  return urlstr;
}

// return historical download URL
// eg:
// http://www.google.com/finance/historical?q=c&startdate=Jan+1%2C+2000&output=csv
QString
GoogleFeed::downloadURL (QString symbol, QString timeframe)
{
  QString downstr = "";

  if (symbol.size () == 0)
    return downstr;
  
  downstr = "http://www.google.com/finance/historical?q=";
  downstr += symbol;
  downstr += "&startdate=Jan+1,+2000&output=csv";
  
  return downstr;
}

// return update URL
QString
GoogleFeed::updateURL (QString symbol, QString timeframe, QString datefrom)
{
  return downloadURL (symbol, timeframe);
}

// callback of  checkManager
void
GoogleFeed::symbolCheckFinished (QNetworkReply *checkreply)
{
  QNetworkRequest request;	
  QVariant replyStatus;
  QString redirectUrl, line;
  QStringList token;
          
  replyStatus = checkreply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  if (replyStatus.isNull () || (!replyStatus.isValid ()))
  {
    setGlobalError(CG_ERR_NETWORK, __FILE__, __LINE__);
    exists = 0;
    return;
  }

  if (replyStatus == 200) // Ok
  {
    replyBytes = checkreply->readAll ();
    line = QString::fromLocal8Bit (replyBytes.constData ());
    line.replace ("\\x26", "&");
    line.remove (10);
    line.remove (13);
    line.remove ('/');
    line.remove ('[');
    line.remove ('{');
    line.remove ('}');
    line.remove (']');
    line = line.simplified ();
    line.remove (':');
    line.remove (',');
    token.clear ();
    token = line.split('"', QString::SkipEmptyParts);
   
    symbolName = token[token.indexOf ("name", 0) + 2];
    symbolName = symbolName.simplified ();
    Market = token[token.indexOf ("e", 0) + 2];
    exists = 1;
  }
  else if (replyStatus == 301) // redirect
  {
    redirectUrl =
      checkreply->attribute (QNetworkRequest::RedirectionTargetAttribute).toString();
    request.setUrl(QUrl(redirectUrl));
    request.setRawHeader("User-Agent", 
    "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"); 
    checkManager->get (request);
  }
  return;
}

// check if symbol exists
bool
GoogleFeed::symbolExistence (QString & symbol, QString & name, QString & market)
{
  QNetworkRequest request;	
  QTimer timer;	
  QString urlstr;	
  qint64 rbytes;	
  QNetworkReply	*checkreply;
  
  symbol = symbol.trimmed ();
  if (!validSymbol (symbol))
    return false;

  urlstr = symbolURL (symbol);
  if (urlstr.size () == 0)
    return false;

  Symbol = symbol;
  symbolName = name;
  
  exists = 2;
  request.setUrl(QUrl(urlstr));
  request.setRawHeader("User-Agent", 
                       "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"); 
  
  if (checkManager != NULL)
  {
    delete checkManager;
    checkManager = NULL;                    
  }  
  checkManager = new QNetworkAccessManager (this);
  connect (checkManager, SIGNAL (finished (QNetworkReply *)), this,
           SLOT (symbolCheckFinished (QNetworkReply *)));
  
  checkreply = checkManager->get (request);
  checkreply->setParent (this);
  
  connect(checkreply, SIGNAL(downloadProgress(qint64, qint64)), this, 
          SLOT(updateNetworkTransferCounters (qint64, qint64)));
  
  rbytes = readBytes = totalBytes = 0;
  timer.setSingleShot (true);
  timer.start (Application_Settings->options.nettimeout * 1000);
    
  while (exists == 2)
  {
    qApp->processEvents();
    if (!timer.isActive ())
    {
	  if (rbytes == readBytes)
	  {
	    setGlobalError(CG_ERR_NETWORK_TIMEOUT, __FILE__, __LINE__);
        exists = 0;
        timer.stop ();
        return false;	  
      }
      rbytes = readBytes;
      timer.start (Application_Settings->options.nettimeout * 1000);	
    }
  }  
  
  timer.stop ();
  if (exists == 0)
    return false;

  market = Market;
  name = symbolName;
  return true;
}

// callback of downloadManager
void
GoogleFeed::downloadDataFinished (QNetworkReply *downloadreply)
{
  QNetworkRequest request;		
  QVariant replyStatus;
  QString redirectUrl;

  replyStatus = downloadreply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  if (replyStatus.isNull () || (!replyStatus.isValid ()))
  {
    setGlobalError(CG_ERR_NETWORK, __FILE__, __LINE__);
    finished = 1;
    return;
  }

  if (replyStatus == 200) // Ok
    replyBytes = downloadreply->readAll ();
  else if (replyStatus == 301) // redirect
  {
    redirectUrl =
      downloadreply->attribute (QNetworkRequest::RedirectionTargetAttribute).toString();
    request.setUrl(QUrl(redirectUrl));
    request.setRawHeader("User-Agent", 
    "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"); 
    downloadManager->get (request);
  }
  finished = 1;
}

// download historical data
CG_ERR_RESULT
GoogleFeed::downloadData (QString symbol, QString timeframe, QString currency, 
                         QString task, bool adjust)
{
  QNetworkRequest request;	
  QTimer timer;
  QString url;	
  qint64 rbytes;
  QNetworkReply *downloadreply;
  CG_ERR_RESULT result = CG_ERR_OK, err;
    
  if (GlobalProgressBar != NULL)
    GlobalProgressBar->setValue (0);
  GlobalError = CG_ERR_OK;
  
  // check symbol existence
  if (symbol != Symbol)
    if (!symbolExistence (symbol, entry.name, entry.market))
    {
      if (GlobalError == CG_ERR_OK)
      {
        result = CG_ERR_NOSYMBOL;
        setGlobalError(result, __FILE__, __LINE__);
      }
      else
      {
		result = GlobalError;
        setGlobalError(result, __FILE__, __LINE__);  
      }
      return result;
    }

  // open temporary file
  if (!tempFile.open ())
  {
    result = CG_ERR_CREATE_TEMPFILE;
    setGlobalError(result, __FILE__, __LINE__);
    return result;
  }
  tempFile.resize (0);
 
  // fill symbol entry
  entry.symbol = Symbol;
  entry.timeframe = timeframe;
  entry.csvfile = tempFile.fileName ();
  entry.source = "GOOGLE";
  entry.format = "GOOGLE CSV";
  entry.currency = currency;
  entry.name = symbolName;
  entry.market = Market;
  entry.adjust = adjust;
  entry.tablename = entry.symbol + "_" +
                    entry.market + "_" +
                    entry.source + "_";
  
  entry.tablename += entry.timeframe;
  entry.tmptablename = "TMP_" + entry.tablename;
  url = downloadURL (symbol, timeframe);
  
  finished = 0;
  rbytes = readBytes = totalBytes = 0;
  timer.setSingleShot (true);
  timer.start (Application_Settings->options.nettimeout * 1000);
  request.setUrl(QUrl(url));
  request.setRawHeader("User-Agent", 
    "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"); 
  if (downloadManager != NULL)
  {
	delete downloadManager;
	downloadManager = NULL;  
  }
  downloadManager = new QNetworkAccessManager (this);
  connect (downloadManager, SIGNAL (finished (QNetworkReply *)), this,
           SLOT (downloadDataFinished (QNetworkReply *)));
     
  downloadreply = downloadManager->get (request);
  downloadreply->setParent (this);
  connect(downloadreply, SIGNAL(downloadProgress(qint64, qint64)), this, 
          SLOT(updateNetworkTransferCounters (qint64, qint64)));
  
  while (finished == 0)
  {
    qApp->processEvents();
    if (!timer.isActive ())
    {
	  if (rbytes == readBytes)
	  {
	    setGlobalError(CG_ERR_NETWORK_TIMEOUT, __FILE__, __LINE__);
        finished = 0;
        timer.stop ();
        return false;
      }
      rbytes = readBytes;
      timer.start (Application_Settings->options.nettimeout * 1000);	
    }
  }  
  timer.stop ();
  
  if (GlobalProgressBar != NULL)
    GlobalProgressBar->setValue (33);
  
  if (GlobalError != CG_ERR_OK)
  {
    tempFile.close ();
    setGlobalError(GlobalError, __FILE__, __LINE__);
    return GlobalError;
  }

  if (tempFile.write (replyBytes) == -1)
    setGlobalError(CG_ERR_WRITE_FILE, __FILE__, __LINE__);
  else if (tempFile.size () != 0)
  {
    if (GlobalProgressBar != NULL)
      GlobalProgressBar->setValue (66);

    if (task == "DOWNLOAD")
      err = csv2sqlite (&entry, "CREATE");
    else
      err = csv2sqlite (&entry, "UPDATE");

    if (err != CG_ERR_OK)
      setGlobalError(err, __FILE__, __LINE__);
  }
  
  if (GlobalProgressBar != NULL)
    GlobalProgressBar->setValue (100);

  result = GlobalError;
  tempFile.close ();
  return result;
}

// callback to update the network transfer counters
void 
GoogleFeed::updateNetworkTransferCounters (qint64 rBytes, qint64 tBytes)
{
  readBytes = rBytes;
  totalBytes = tBytes;
}

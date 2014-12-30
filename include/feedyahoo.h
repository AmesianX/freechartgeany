/*
 * feedyahoo.h
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

#ifndef FEEDYAHOO_H
#define FEEDYAHOO_H

#include <QObject>
#include <QAtomicInt>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTemporaryFile>
#include "defs.h"

namespace Ui
{
  class YahooFeed;
}

class YahooFeed: public QObject
{
  Q_OBJECT
public:
  explicit YahooFeed (); // constructor
  ~YahooFeed ();	     // destructor

  // functions
  bool symbolExistence (QString & symbol,
                        QString & name,
                        QString & market); // check if symbol exists

  CG_ERR_RESULT downloadData (QString symbol,    // download
                              QString timeframe, // historical
                              QString currency,  // data
                              QString task,
                              bool    adjust);

  bool validSymbol (QString symbol);	// validate yahoo symbol


private:
  // variables and classes
  QAtomicInt exists;	// symbol existence: 0 false, 1 true, 2 wait
  QAtomicInt finished;	// download operation: 0 false, 1 true
  QString Symbol;		// symbol
  QString symbolName; 	// symbolname
  QString Market;		// market
  QByteArray replyBytes;				// network reply buffer
  QTemporaryFile tempFile;				// temporary file
  QNetworkAccessManager *checkManager;	// manager to check symbol existence
  QNetworkAccessManager *downloadManager;	// manager to check symbol existence
  SymbolEntry entry;						// symbol entry
  
  qint64 readBytes;							// bytes read
  qint64 totalBytes;						// total bytes to be read

  // functions
  QString symbolURL (QString symbol); // returns symbol check URL
  QString downloadURL (QString symbol, QString timeframe); // download URL
  QString updateURL (QString symbol, QString timeframe, QString datefrom); // update URL

private slots:
  void symbolCheckFinished (QNetworkReply *checkreply); // callback of checkManager
  void downloadDataFinished (QNetworkReply *downloadreply); // callback of downloadManager
  void updateNetworkTransferCounters (qint64 readBytes, qint64 totalBytes); // callback to update the																			// network transfer counters

};

#endif

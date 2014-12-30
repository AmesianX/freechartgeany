/*
 * main.cpp
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

#include "ta_libc.h"
#include <QtGui/QApplication>
#include <QWebSettings>
#include <QTimer>
#include "appdata.h"
#include "mainwindow.h"

int
main (int argc, char *argv[])
{
   MainWindow *ChartGeanyMain;		
   TA_RetCode retCode;
   int retcode;
   retCode = TA_Initialize( );
   if( retCode != TA_SUCCESS )
   {
     printf( "Cannot initialize TA-Lib (%d)!\n", retCode );	
     exit (1);
  }
      
  QApplication::setApplicationName (APPNAME);
  QApplication App (argc, argv);
  QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
  QApplication::setStyle("plastique");

/*  
#ifdef Q_OS_LINUX  
  QApplication::setStyle("plastique");
#endif

#ifdef Q_OS_MAC  
  QApplication::setStyle("macintosh");
#endif

#ifdef Q_OS_WIN  
  QApplication::setStyle("windows");
#endif
*/
  
  QWebSettings *settings = QWebSettings::globalSettings();
  settings->setAttribute (QWebSettings::PluginsEnabled, false);
  
  ChartGeanyMain = new MainWindow;
  ChartGeanyMain->show ();
  retcode = App.exec ();

  delete ChartGeanyMain;  
  return retcode;
}

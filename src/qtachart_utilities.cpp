/*
 * qtachart_utilities.cpp
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

#ifdef Q_WS_WIN
#include <windows.h>
#endif

#include <QtCore/qmath.h>
#include "qtachart_core.h"

// return the decimal digits of a number
static qint32
decdig (qreal r)
{

  QString s;
  QStringList values;

  s = QString::number (r);
  values = s.split (".", QString::KeepEmptyParts);
  if (values.size () > 1)
    return values[1].size ();

  return 0;
}

// max decimal points
qreal
maxdecimals (FrameVector *HLOC)
{
  FrameVector hloc = *HLOC;	
  qint32 maxd = 0.1;

  foreach (const QTAChartFrame Frame, hloc)
  {
    maxd = qMax (maxd, decdig (Frame.Open));
    maxd = qMax (maxd, decdig (Frame.High));
    maxd = qMax (maxd, decdig (Frame.Low));
    maxd = qMax (maxd, decdig (Frame.Close));
  }

  return (qreal) (1 / qPow (10, (qreal) maxd));
}

// get chart's data
void *
getData (QTAChart * chart)
{
  return (void *) chart->chartdata;
}

/********************************************************/
/** logarithmic price scale:                           **/
/** x'i = (log(xi)-log(xmin)) / (log(xmax)-log(xmin))  **/
/** x'i = relative position (0 <= x'i <=1)             **/
/********************************************************/

// find the corresponding y of a price
qreal
yOnPrice (QTAChartCore * chart, qreal price)
{
  if (price == 0)
    return (qreal) chart->chartbottomost;
    	
  if (chart->linear)
  {
	qreal y, k, points;  
    points = (qreal) chart->charttop;
    k = price / chart->points;
    points -= k;
    points = points * chart->dots_per_point;
    y = (qreal) chart->charttopmost;
    y += points;
    return y;
  }
  else
  {
	qreal relpos, y, bottom;
	
	bottom = qLog10 (((qreal) chart->chartbottom) * chart->points);
	relpos = (qLog10 (price) - bottom) /
	         (qLog10 (((qreal) chart->charttop) * chart->points) -
	          bottom);
	y = ((qreal) chart->chartbottomost) -
	      ((qreal) (chart->chartbottomost - chart->charttopmost) * relpos);         
	         
    return y;         
  }
}

// find the corresponding price of y
qreal
priceOnY (QTAChartCore * chart, int y)
{
  if (chart->linear)
  {
	qreal price, k;
    qint64 diff;  
    diff = (y - chart->charttopmost);
    k = (qreal) diff / chart->dots_per_point;
    price = chart->points;
    price = price * ((qreal) chart->charttop - k);
    return price;
  }  
   else
  {
	qreal relpos, bottom, k, price; 
	
	bottom = qLog10 ((qreal) chart->chartbottom * chart->points);
	relpos = ((qreal) y - (qreal) chart->chartbottomost) /
	         ((qreal) (chart->chartbottomost - chart->charttopmost));
	relpos = qAbs (relpos);
	k =  (relpos *
	     ((qLog10 (((qreal) chart->charttop) * chart->points) - 
	     bottom))) + bottom;
	      
	 
	price = qPow (10,k);
	return price;
   }
   
}

// find the corresponding bar number of x
// returns -1 if not applicable
int
barOnX (QTAChartCore * chart, int x)
{
  qreal a, b, c;	
  int bar;
  
  if (chart->nbars_on_chart == 0)	
    return -1;
  
  if (x < chart->chartleftmost)
    return -1;
  
  if (x > chart->chartrightmost)
    return -1;  
  
  b = chart->framewidth * 1.5;
  a = x - chart->chartleftmost;
  c = chart->chartwidth - a;
  c /= b;
  bar = qFloor (c);
  if (bar < 0)
    return -1;
  
  if (!((bar + *chart->startbar) > chart->HLOC->size () - 1))
    return  bar + *chart->startbar;
  
  return -1;  
}


// determine if running windows os is vista or later
#ifdef Q_WS_WIN		// windows 
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4996)

#include <windows.h>
#include <windows.h>

bool IsWinVistaOrLater() {
    DWORD version = GetVersion();
    DWORD major = (DWORD) (LOBYTE(LOWORD(version)));
    DWORD minor = (DWORD) (HIBYTE(LOWORD(version)));

    return (major > 6) || ((major == 6) && (minor >= 0));
}
#endif // Q_WS_WIN	


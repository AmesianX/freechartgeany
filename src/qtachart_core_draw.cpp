/*
 * qtachart_core_draw.cpp
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

#include <limits>
#include <QtCore/qmath.h>
#include "qtachart.h"
#include "qtachart_core.h"

// compute the geomerty factors of the chart
static void
geom (QTAChartCore * core)
{
  qint64 price_width, keepbottom, keeptop;
  qreal chart_high, chart_low, chart_height_prc = 1.0,  max_volume = 0;
  int listsize, counter, i;

  // find the size of the bar list
  listsize = core->HLOC->size ();
  if (listsize == 0)
    return;

  // width
  core->chartwidth =
    core->chartrightmost - *core->excess_drag_width - core->chartleftmost;

  // height
  if (core->nsubcharts > 0)
  {
    chart_height_prc = 1 - (core->nsubcharts * 0.175);
    if (chart_height_prc < 0.45)
      chart_height_prc = 0.45;
  }

  core->charttopmost = core->title_height + core->chartframe;
  core->chartheight = core->height -
                      ((core->chartframe * 2) + core->title_height + core->bottomline_height);
  core->chartheight *= chart_height_prc;
  core->chartbottomost = core->charttopmost + core->chartheight;

  // compute the number of bars on the chart
  core->nbars_on_chart = core->chartwidth / (core->framewidth * 1.5);

  // initialize chart high and low
  chart_low = std::numeric_limits < qreal >::max ();
  chart_high = std::numeric_limits < qreal >::min ();

  counter = 0;
  do
  {
    i = counter + *core->startbar;
    chart_high = qMax (chart_high, core->HIGH[i]);
    chart_low = qMin (chart_low, core->LOW[i]);
    max_volume = qMax (max_volume, core->VOLUME[i]);
    counter++;
  }
  while ((counter < (core->nbars_on_chart + 2)) &&
         ((counter + *core->startbar) < listsize));

  if (!core->always_redraw)
  {
    if (core->max_high >= chart_high && core->min_low <= chart_low )
    {
      core->redraw = false;
      if (!core->recalc &&
          (chart_high * 1.1 > core->max_high) &&
          (chart_low * 0.8 < core->min_low))
        return;
    }
  }

  core->redraw = true;
  core->recalc = true;
  core->max_high = chart_high;
  core->min_low = chart_low;

  // compute the charttop and the chartbottom
  core->charttop = (qint64) (chart_high / core->points);
  core->chartbottom = (qint64) (chart_low / core->points);
  
  // compute the gridstep
  core->gridstep =
    qPow (10, qCeil (qLog10 (core->charttop - core->chartbottom) - 1));

  // recompute the charttop and the chartbottom (in cents or pips)
  keepbottom = core->chartbottom;
  keeptop = core->charttop;
  
  core->charttop = (keeptop / core->gridstep) * core->gridstep;
  if (core->charttop < keeptop)
    core->charttop += core->gridstep;
           
  core->chartbottom = (keepbottom / core->gridstep) * core->gridstep;
  if (core->chartbottom <= 0)
  {
    core->chartbottom = core->gridstep;
    while (core->chartbottom > keepbottom && core->chartbottom > 4)
      core->chartbottom /= 2;
    
    core->chartbottom = qPow (10, qCeil (qLog10 (core->chartbottom) - 1));
    core->chartbottom = qCeil (keepbottom / core->chartbottom) * core->chartbottom;  
  }
  
  // at least 4 grid lines  
  while ((((qint64) (chart_high / core->points) - core->chartbottom) / core->gridstep) < 4)
    core->gridstep /= 2;
  
  // no more than 15 lines
  while (((core->charttop - core->chartbottom) / core->gridstep) > 15)
    core->gridstep *= 2;
  
  
  while (((qreal) qAbs (core->charttop - keeptop) / (qreal) core->gridstep) >= 2)
    core->charttop -= core->gridstep;  
    
  while (((qreal) qAbs (keepbottom - core->chartbottom) / (qreal) core->gridstep) >= 2)
    core->chartbottom += core->gridstep;
  
    
  // compute the dots_per_point and the dots_per_traded_unit
  price_width = core->charttop - core->chartbottom;
  core->dots_per_point = core->chartheight / (qreal) price_width;
}

// draw the chartchartheight
void
QTAChartCore::draw (void)
{
  QWidget *gqw;
  QTACObject *object, *delobject;
  
  if (Q_UNLIKELY (!tfinit))
  {
	QString title;  
	createTFButtons ();  
	HLOC = &TIMEFRAME[0].HLOC;
    HEIKINASHI = &TIMEFRAME[0].HEIKINASHI; 
    startbar = &TIMEFRAME[0].TFStartBar;
    excess_drag_width = &TIMEFRAME[0].TFExcess_Drag_Width;
    title = Symbol + " - " + TIMEFRAME[0].TFName;
    currenttf = TIMEFRAME[0].TFName;
    setTitle (title, QString::fromUtf8 (subtitletext));
    tfinit = true;
  }  
  
  // collect garbage objects
  do
  { 
	delobject = NULL;  
    foreach (object, Object)
      if (object->deleteit)
        delobject = object;
  
    if (delobject != NULL)
      deleteObject (delobject);
  }
  while (delobject != NULL);
  
  
  // collect garbase QWidgets
  if (garbageQWidget.size () > 0)
  {
	gqw = garbageQWidget[0];
	gqw->deleteLater ();
	garbageQWidget.remove (0); 
  }
  
  if (reloaded)
  {
    points = maxdecimals (HLOC);
    decimals = qCeil (qAbs (qLog10 (points)));
    OPEN.clear ();
    CLOSE.clear ();
    HIGH.clear ();
    LOW.clear ();
    VOLUME.clear ();
    foreach (const QTAChartFrame &frame, *HLOC)
    {
      OPEN += frame.Open;
      CLOSE += frame.Close;
      HIGH += frame.High;
      LOW += frame.Low;
      VOLUME += frame.Volume;
    }
    reloaded = false;
  }

  // geometry
  geom (this);
  
  // draw the objects
  foreach (object, Object)
    object->draw ();
  
  // draw the chart frame, grid, bottom text 
  ruller_cursor_x = chartrightmost + 2;
  ruller_cursor->setPos (ruller_cursor_x, ruller_cursor_y);
  bottom_text->setPos (chartleftmost, height - (bottomline_height + 5));

  topedge->setLine (chartleftmost, charttopmost, chartrightmost + 3, charttopmost);
  bottomedge->setLine (chartleftmost, chartbottomost + 5, width, chartbottomost + 5);
  rightedge->setLine (chartrightmost + 3, 0, chartrightmost + 3, height);
  leftedge->setLine (chartleftmost, 0, chartleftmost, height);
  
  if (show_grid)
    drawGRID ();
  else
    clearGRID ();
      
  if (linear)
    scaletitle->setPlainText ("Linear");
  else  
    scaletitle->setPlainText ("Logarithmic");
    
  if (chart_style == QTACHART_CANDLE)
  {
    typetitle->setPlainText ("Candle");
    drawCandleChart ();
  }  
  else  if (chart_style == QTACHART_HEIKINASHI)  
  {
    typetitle->setPlainText ("Heikin-Ashi");
    drawCandleChart ();
  }  
  else if (chart_style == QTACHART_BAR)
  {
    typetitle->setPlainText ("Bar");
    drawBarChart ();
  }  
  else if (chart_style == QTACHART_LINE)
  {
    typetitle->setPlainText ("Line");
    drawPriceLine (linecolor, linethickness);
  }  
  
  clearITEMS ();
  setRullerCursor (ruller_cursor_y);
}

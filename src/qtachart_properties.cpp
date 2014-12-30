/*
 * qtachart_properties.cpp
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


// Full implementation of QTACProperties

#include <QResizeEvent>
#include "common.h"
#include "qtachart_properties.h"
#include "qtachart.h"
#include "ui_qtacproperties.h"

// constructor
QTACProperties::QTACProperties (QWidget * parent):
  QWidget (parent), ui (new Ui::QTACProperties)
{
  QString checkbstylesheet, colorbstylesheet;
  ui->setupUi (this);
  showVolumes = ui->showVolumes;
  showGrid = ui->showGrid;
  lineChart = ui->lineChart;
  candleChart = ui->candleChart;
  barChart = ui->barChart;
  heikinAshi = ui->heikinAshi;
  lineColorButton = ui->lineColorButton;
  barColorButton = ui->barColorButton;
  linearScale = ui->linearScale;
  frame = ui->frame;
  
  linecolor = Qt::green;
  barcolor = Qt::green;
  lineColorDialog = NULL;
  barColorDialog = NULL;
  lineicon = NULL;
  baricon = NULL;
  linepixmap = NULL;
  barpixmap = NULL;

  checkbstylesheet =
    "background: transparent; border: 1px solid transparent;border-color: darkgray; padding-left: 5px";
  colorbstylesheet =
    "background: transparent; border: 1px solid transparent;border-color: darkgray;";  
  showVolumes->setStyleSheet (checkbstylesheet);
  lineChart->setStyleSheet (checkbstylesheet);
  candleChart->setStyleSheet (checkbstylesheet);
  barChart->setStyleSheet (checkbstylesheet);
  heikinAshi->setStyleSheet (checkbstylesheet);
  showGrid->setStyleSheet (checkbstylesheet);
  lineColorButton->setStyleSheet (colorbstylesheet);
  barColorButton->setStyleSheet (colorbstylesheet);
  linearScale->setStyleSheet (checkbstylesheet);

  styleGroup = new QButtonGroup (this);
  styleGroup->addButton (candleChart);
  styleGroup->addButton (lineChart);
  styleGroup->addButton (heikinAshi);
  styleGroup->addButton (barChart);
  styleGroup->setExclusive (true);

  frame->move ((width () - frame->width ()) / 2,
               ((height () - frame->height ()) / 2));

  connect (lineColorButton, SIGNAL (clicked ()), this,
           SLOT (lineColorButton_clicked ()));
  connect (barColorButton, SIGNAL (clicked ()), this,
           SLOT (barColorButton_clicked ()));
  
  correctWidgetFonts (this);         
}

// destructor
QTACProperties::~QTACProperties ()
{
  delete styleGroup;
  
  if (lineicon != NULL)
    delete lineicon;
  
  if (linepixmap != NULL)  
    delete linepixmap;
  
  if (baricon != NULL)  
    delete baricon;
  
  if (barpixmap != NULL)  
    delete barpixmap;
  
  if (lineColorDialog != NULL)
    delete lineColorDialog;
  
  if (barColorDialog != NULL)
    delete barColorDialog;    
    
  delete ui;
}

// set chart's style
void
QTACProperties::setChartStyle (int style)
{
  if (style == QTACHART_CANDLE)
  {
    lineChart->setCheckState (Qt::Unchecked);
    candleChart->setCheckState (Qt::Checked);
    heikinAshi->setCheckState (Qt::Unchecked);
    barChart->setCheckState (Qt::Unchecked);
  }

  if (style == QTACHART_HEIKINASHI)
  {
    lineChart->setCheckState (Qt::Unchecked);
    candleChart->setCheckState (Qt::Unchecked);
    heikinAshi->setCheckState (Qt::Checked);
    barChart->setCheckState (Qt::Unchecked);
  }

  if (style == QTACHART_LINE)
  {
    lineChart->setCheckState (Qt::Checked);
    candleChart->setCheckState (Qt::Unchecked);
    heikinAshi->setCheckState (Qt::Unchecked);
    barChart->setCheckState (Qt::Unchecked);
  }

  if (style == QTACHART_BAR)
  {
    lineChart->setCheckState (Qt::Unchecked);
    candleChart->setCheckState (Qt::Unchecked);
    heikinAshi->setCheckState (Qt::Unchecked);
    barChart->setCheckState (Qt::Checked);
  }
}

// get chart style
int
QTACProperties::ChartStyle (void)
{
  if (lineChart->checkState () == Qt::Checked)
    return QTACHART_LINE;

  if (candleChart->checkState () == Qt::Checked)
    return QTACHART_CANDLE;

  if (heikinAshi->checkState () == Qt::Checked)
    return QTACHART_HEIKINASHI;

  if (barChart->checkState () == Qt::Checked)
    return QTACHART_BAR;

  return QTACHART_CANDLE;

}

// set grid on/off
void
QTACProperties::setGrid (bool boolean)
{
  if (boolean)
    showGrid->setCheckState (Qt::Checked);
  else
    showGrid->setCheckState (Qt::Unchecked);
}

// get grid
bool QTACProperties::Grid (void)
{
  if (showGrid->checkState () == Qt::Checked)
    return true;

  return false;
}

// get line color
QColor QTACProperties::lineColor (void)
{
  return linecolor;
}

// get bar color
QColor QTACProperties::barColor (void)
{
  return barcolor;
}

// set volumes on/off
void
QTACProperties::setVolumes (bool boolean)
{
  if (boolean)
    showVolumes->setCheckState (Qt::Checked);
  else
    showVolumes->setCheckState (Qt::Unchecked);
}

// set linear scale
void
QTACProperties::setLinearScale (bool boolean)
{
  if (boolean)
    linearScale->setCheckState (Qt::Checked);
  else
    linearScale->setCheckState (Qt::Unchecked);
}

// get linear scale
bool
QTACProperties::LinearScale (void)
{
  if (linearScale->checkState () == Qt::Checked)
    return true;
 
  return false;
}

// set the reference chart
void
QTACProperties::setReferenceChart (void *chart)
{
  referencechart = chart;
}

// get volumes
bool 
QTACProperties::Volumes (void)
{
  if (showVolumes->checkState () == Qt::Checked)
    return true;

  return false;
}

// line color button clicked
void
QTACProperties::lineColorButton_clicked (void)
{
  lineColorDialog->setCurrentColor (linecolor);
  lineColorDialog->move ((width () - lineColorDialog->width ()) / 2,
                         (height () - lineColorDialog->height ()) / 2);
  lineColorDialog->show ();
  lineColorDialog->open ();
  linecolor = lineColorDialog->selectedColor ();
  linepixmap->fill (linecolor);
}

// bar color button clicked
void
QTACProperties::barColorButton_clicked (void)
{
  barColorDialog->setCurrentColor (barcolor);
  barColorDialog->move ((width () - barColorDialog->width ()) / 2,
                        (height () - barColorDialog->height ()) / 2);
  barColorDialog->show ();
  barColorDialog->open ();
  barcolor = barColorDialog->selectedColor ();
  barpixmap->fill (barcolor);
}

// line color dialog done
void
QTACProperties::lineColorDialog_finished ()
{
  linecolor = lineColorDialog->currentColor ();
  linepixmap->fill (linecolor);
  lineicon->addPixmap (*linepixmap, QIcon::Normal, QIcon::On);
  lineColorButton->setIcon (*lineicon);
}

// bar color dialog done
void
QTACProperties::barColorDialog_finished ()
{
  barcolor = barColorDialog->currentColor ();
  barpixmap->fill (barcolor);
  baricon->addPixmap (*barpixmap, QIcon::Normal, QIcon::On);
  barColorButton->setIcon (*baricon);
}

// resize
void
QTACProperties::resizeEvent (QResizeEvent * event)
{
  QSize newsize;
  
  newsize = event->size ();	
  frame->move ((newsize.width () - frame->width ()) / 2,
               ((newsize.height () - frame->height ()) / 2));

  if (barColorDialog != NULL)
  {
    barColorDialog->move ((newsize.width () - barColorDialog->width ()) / 2,
                          (newsize.height () - barColorDialog->height ()) / 2);
  } 
  
  if (lineColorDialog != NULL)
  {
    lineColorDialog->move ((newsize.width () - barColorDialog->width ()) / 2,
                          (newsize.height () - barColorDialog->height ()) / 2);
  }                       
}

// show 
void 
QTACProperties::showEvent (QShowEvent * event)
{
  QTAChart *chart = (QTAChart *) referencechart;	
  
  if (lineColorDialog == NULL)
  {
#ifdef Q_OS_WIN  
    lineColorDialog = new QColorDialog (chart);
#else
    lineColorDialog = new QColorDialog; // (chart);
#endif	
    lineColorDialog->setModal (true);
    linepixmap = new QPixmap (16, 16);
    lineicon = new QIcon;
    linepixmap->fill (linecolor);
    lineicon->addPixmap (*linepixmap, QIcon::Normal, QIcon::On);
    lineColorButton->setIcon (*lineicon);
    lineColorDialog->setStyleSheet ("background-color: lightgray; color: black"); 
    connect (lineColorDialog, SIGNAL (finished (int)), this,
           SLOT (lineColorDialog_finished ())); 
  }	
  
  if (barColorDialog == NULL)
  {
#ifdef Q_OS_WIN  
    barColorDialog = new QColorDialog (chart);
#else
    barColorDialog = new QColorDialog; // (chart);
#endif	
    barColorDialog->setModal (true);
    barpixmap = new QPixmap (16, 16);
    baricon = new QIcon;
    barpixmap->fill (barcolor);
    baricon->addPixmap (*barpixmap, QIcon::Normal, QIcon::On);
    barColorButton->setIcon (*baricon);   
    barColorDialog->setStyleSheet ("background-color: lightgray; color: black");
    connect (barColorDialog, SIGNAL (finished (int)), this,
           SLOT (barColorDialog_finished ()));
  }
}

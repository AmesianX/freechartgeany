/*
 * qtachart_properties.h
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

#ifndef QTACHART_PROPERTIES_H
#define QTACHART_PROPERTIES_H

#include <QWidget>
#include <QFrame>
#include <QPixmap>
#include <QIcon>
#include <QToolButton>
#include <QCheckBox>
#include <QColorDialog>

namespace Ui
{
  class QTACProperties;
}

class QTACProperties:public QWidget
{
Q_OBJECT public:
  explicit QTACProperties (QWidget * parent = 0);
  ~QTACProperties ();

  void setChartStyle (int);	// set chart's style
  int ChartStyle (void);	// return's chart style

  void setGrid (bool);		// set grid on/off
  bool Grid (void);		// returns grid state
  
  void setLinearScale (bool);		// set price scale to linear on/off
  bool LinearScale (void);	// return linear price scale state 

  QColor lineColor (void); // return the line color
  QColor barColor (void); // return the bar color

  void setVolumes (bool);	// set volumes on off
  bool Volumes (void);		// returns volumes state
  
  void setReferenceChart (void *chart);		// sets the reference chart

private:
  Ui::QTACProperties * ui;

  QColor linecolor;
  QColor barcolor;
  QButtonGroup *styleGroup;
  QCheckBox *showVolumes;
  QCheckBox *showGrid;
  QCheckBox *lineChart;
  QCheckBox *candleChart;
  QCheckBox *barChart;
  QCheckBox *heikinAshi;
  QCheckBox *linearScale;
  QPushButton *lineColorButton;
  QPushButton *barColorButton;
  QPixmap *linepixmap, *barpixmap;
  QIcon *lineicon, *baricon;
  QColorDialog *lineColorDialog;
  QColorDialog *barColorDialog;
  QFrame *frame;
  void *referencechart;						// reference chart

private slots:
  void lineColorButton_clicked (void);
  void barColorButton_clicked (void);
  void lineColorDialog_finished ();
  void barColorDialog_finished ();

protected:
  virtual void resizeEvent (QResizeEvent * event);  
  virtual void showEvent (QShowEvent * event); 

};

#endif // QTACHART_PROPERTIES_H

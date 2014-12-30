/*
 * qtachart_functions.cpp
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

// Full implementation of QTACFunctions
#include "common.h"
#include "dynparamsdialog.h"
#include "qtachart_core.h"
#include "ui_qtacfunctions.h"
#include "qtachart_functions.h"

// constructor
QTACFunctions::QTACFunctions (QWidget * parent):
  QWidget (parent), ui (new Ui::QTACFunctions)
{
  ui->setupUi (this);
  
  button_width = 200;
  button_height = 40;
  
  layout = new QGridLayout(this);
  this->setLayout(layout);
}

void
QTACFunctions::createButtons (void)
{  
  QPushButton *btn;
  QColorDialog *colorDialog;
  DynParamsDialog *ParamDialog;
  	
  btn = addButton ("SMA");
  connect (btn, SIGNAL (clicked ()), this, SLOT (button_clicked ()));
  ParamDialog = new DynParamsDialog (btn);
#ifdef Q_OS_WIN  
  colorDialog = new QColorDialog (parentchart);
#else
  colorDialog = new QColorDialog; // (parentchart);
#endif  
  ParamDialog->setColorDialog (colorDialog);
  ParamDialog->addParam ("Period", "Period", DPT_INT, 14.0);
  ParamDialog->addParam ("Color", "Color", DPT_COLOR, (qreal) QColor (Qt::cyan).rgb ());
  ParamDialog->setObjectName ("ParamDialog"); 
  ParamDialog->setParent (btn);
  connect(ParamDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect(ParamDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));
  
  btn = addButton ("EMA");
  connect (btn, SIGNAL (clicked ()), this, SLOT (button_clicked ()));
  ParamDialog = new DynParamsDialog (btn);
#ifdef Q_OS_WIN  
  colorDialog = new QColorDialog (parentchart);
#else
  colorDialog = new QColorDialog; // (parentchart);
#endif    
  ParamDialog->setColorDialog (colorDialog);
  ParamDialog->addParam ("Period", "Period", DPT_INT, 14.0);
  ParamDialog->addParam ("Color", "Color", DPT_COLOR, (qreal) QColor (Qt::cyan).rgb ());
  ParamDialog->setObjectName ("ParamDialog"); 
  ParamDialog->setParent (btn);
  connect(ParamDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect(ParamDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));
  
  btn = addButton ("MACD");
  connect (btn, SIGNAL (clicked ()), this, SLOT (button_clicked ()));
  ParamDialog = new DynParamsDialog (btn);
#ifdef Q_OS_WIN  
  colorDialog = new QColorDialog (parentchart);
#else
  colorDialog = new QColorDialog; // (parentchart);
#endif  
  ParamDialog->setColorDialog (colorDialog);
  ParamDialog->addParam ("Period", "Period", DPT_INT, 9.0);
  ParamDialog->addParam ("Color", "Color", DPT_COLOR, (qreal) QColor (Qt::red).rgb ());
  ParamDialog->setObjectName ("ParamDialog"); 
  ParamDialog->setParent (btn);
  connect(ParamDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect(ParamDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));
  
  btn = addButton ("RSI");
  connect (btn, SIGNAL (clicked ()), this, SLOT (button_clicked ()));
  ParamDialog = new DynParamsDialog (btn);
#ifdef Q_OS_WIN  
  colorDialog = new QColorDialog (parentchart);
#else
  colorDialog = new QColorDialog; // (parentchart);
#endif  
  ParamDialog->setColorDialog (colorDialog);
  ParamDialog->addParam ("Period", "Period", DPT_INT, 14.0);
  ParamDialog->addParam ("High level", "High level", DPT_INT, 70);
  ParamDialog->addParam ("Low level", "Low level", DPT_INT, 30);
  ParamDialog->addParam ("Color", "Color", DPT_COLOR, (qreal) QColor (Qt::cyan).rgb ());
  ParamDialog->setObjectName ("ParamDialog"); 
  ParamDialog->setParent (btn);
  connect(ParamDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect(ParamDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));
  
  btn = addButton ("Bollinger Bands");
  connect (btn, SIGNAL (clicked ()), this, SLOT (button_clicked ()));
  ParamDialog = new DynParamsDialog (btn);
#ifdef Q_OS_WIN  
  colorDialog = new QColorDialog (parentchart);
#else
  colorDialog = new QColorDialog; // (parentchart);
#endif    
  ParamDialog->setColorDialog (colorDialog);
  ParamDialog->addParam ("Period", "Period", DPT_INT, 20.0);
  ParamDialog->addParam ("Color", "Color", DPT_COLOR, (qreal) QColor (Qt::magenta).rgb ());
  ParamDialog->setObjectName ("ParamDialog"); 
  ParamDialog->setParent (btn);
  connect(ParamDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect(ParamDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));
  
  btn = addButton ("Parabolic SAR");
  connect (btn, SIGNAL (clicked ()), this, SLOT (button_clicked ()));
  ParamDialog = new DynParamsDialog (btn);
#ifdef Q_OS_WIN  
  colorDialog = new QColorDialog (parentchart);
#else
  colorDialog = new QColorDialog; // (parentchart);
#endif  
  ParamDialog->setColorDialog (colorDialog);
  ParamDialog->addParam ("Color", "Color", DPT_COLOR, (qreal) QColor (Qt::cyan).rgb ());
  ParamDialog->setObjectName ("ParamDialog"); 
  ParamDialog->setParent (btn);
  connect(ParamDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect(ParamDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));

}

// destructor
QTACFunctions::~QTACFunctions ()
{
  delete ui;
}

// add a button
QPushButton *
QTACFunctions::addButton (QString text)
{
  QFont fnt;
  QPushButton *btn;
  QString stylesheet;

  stylesheet =
    "background: transparent; border: 1px solid transparent;border-color: darkgray;";
  
  btn = new QPushButton (text, this);
  btn->setFixedSize (QSize (button_width, button_height));
  fnt = btn->font ();
  fnt.setPixelSize (16);
  fnt.setBold (true);
  btn->setFont (fnt);
  btn->setStyleSheet (stylesheet);
  btn->setAutoFillBackground (false);
  btn->setFocusPolicy (Qt::NoFocus);
  Button += btn;
  return btn;
}

// set the reference chart
void
QTACFunctions::setReferenceChart (void *chart)
{
  referencechart = chart;
  parentchart = (QTAChart *) chart; 
  createButtons ();
}

// get the reference chart
void *
QTACFunctions::getReferenceChart (void)
{
  return referencechart;
}

// resize
void
QTACFunctions::resizeEvent (QResizeEvent * event)
{
  DynParamsDialog *paramDialog;
  QSize newsize;
  int w, h, counter;
  
  newsize = event->size ();
  w = newsize.width () - 2;
  h = newsize.height () - 2;
  layout->setGeometry (QRect (0, 0, w, h));
  for (counter = 0; counter < Button.size (); counter ++)
  {
	layout->setRowMinimumHeight (counter % 6, Button[counter]->height ());
    layout->setColumnMinimumWidth (counter/6, Button[counter]->width ());  
    layout->addWidget (Button[counter], counter % 6, counter / 6, Qt::AlignHCenter);
    paramDialog = Button[counter]->findChild<DynParamsDialog *>("ParamDialog");
    if (paramDialog != NULL)
    {
	  paramDialog->move ((width () - paramDialog->width ()) / 2, 25);	
    }
  }  
}

// button clicked
void
QTACFunctions::button_clicked (void)
{
  DynParamsDialog *paramDialog;	
  QPushButton *btn;	
  btn = (QPushButton *) QObject::sender();
  paramDialog = btn->findChild<DynParamsDialog *>("ParamDialog");
  if (paramDialog != NULL)
  {
    paramDialog->setReferenceChart (referencechart);  
	paramDialog->move ((width () - paramDialog->width ()) / 2, 25);	  
    paramDialog->show ();
  }  
}

// function accepted
void
QTACFunctions::function_accepted (void)
{
  QTAChart *chart = (QTAChart *) referencechart;
  QTAChartCore *core = (QTAChartCore *) getData (chart);
  QTACObject *indicator = NULL, *childobj;
  DynParamsDialog *paramDialog;		
  QString fname;
  qint32 period;
  QColor color;
  
  paramDialog = (DynParamsDialog *) QObject::sender()->parent ();
  fname = paramDialog->getTitle ();
  
  if (fname == "SMA")
  {
	period = paramDialog->getParam ("Period");
	if (period < 1)
	  return;
	color = paramDialog->getParam ("Color");   
	indicator = new QTACObject (core, QTACHART_OBJ_CURVE);
	indicator->setAttributes (QTACHART_CLOSE, period, "Period", SMA, 0, 0, color, "Color");
	indicator->setTitle (fname);
  }
  
  if (fname == "EMA")
  {
	period = paramDialog->getParam ("Period");
	if (period < 1)
	  return;
	color = paramDialog->getParam ("Color");   
	indicator = new QTACObject (core, QTACHART_OBJ_CURVE);
	indicator->setAttributes (QTACHART_CLOSE, period, "Period", EMA, 0, 0, color, "Color");
	indicator->setTitle (fname);
  }
  
  if (fname == "Parabolic SAR")
  {
	color = paramDialog->getParam ("Color");   
	indicator = new QTACObject (core, QTACHART_OBJ_DOT);
	indicator->setAttributes (QTACHART_CLOSE, 1, "", PSAR, 0, 0, color, "Color");
	indicator->setTitle (fname);  
  }
  
  if (fname == "RSI")
  {
	period = paramDialog->getParam ("Period");
	if (period < 1)
	  return;  
	color = paramDialog->getParam ("Color");   
	indicator = new QTACObject (core, QTACHART_OBJ_SUBCHART);
	indicator->setAttributes (QTACHART_CLOSE, period, "Period", DUMMY, 0, 100, color, "");
	indicator->setTitle (fname);  
	childobj = new QTACObject (indicator, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, "Period", RSI, 0, 100, color, "Color");
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam ("High level"));
    childobj->setAttributes (QTACHART_CLOSE, period, "High level", DUMMY, 0, 100, color, "");
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam ("Low level"));
    childobj->setAttributes (QTACHART_CLOSE, period, "Low level", DUMMY, 0, 100, color, "");
  }
  
  if (fname == "MACD")
  {
	period = paramDialog->getParam ("Period");
	if (period < 1)
	  return;  
	 
	color = paramDialog->getParam ("Color");
	indicator = new QTACObject (core, QTACHART_OBJ_SUBCHART);
	indicator->setAttributes (QTACHART_CLOSE, period, "Period", MACDSIGNAL, QREAL_MIN, QREAL_MAX, QColor (Qt::white).rgb (), "");
	indicator->setTitle (fname); 
	childobj = new QTACObject (indicator, QTACHART_OBJ_VBARS);
    childobj->setAttributes (QTACHART_CLOSE, period, "Period", MACD, QREAL_MIN, QREAL_MAX, QColor (Qt::white).rgb (), ""); 
	childobj = new QTACObject (indicator, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, "Period", MACDSIGNAL, QREAL_MIN, QREAL_MAX, color, "Color");
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, 0);
    childobj->setAttributes (QTACHART_CLOSE, 0, "", DUMMY, 0, 0, color, "");
  }
  
  if (fname == "Bollinger Bands")
  {
	period = paramDialog->getParam ("Period");
	if (period < 1)
	  return;
	color = paramDialog->getParam ("Color");   
	indicator = new QTACObject (core, QTACHART_OBJ_CURVE);
	indicator->setAttributes (QTACHART_CLOSE, period, "Period", BBANDSMIDDLE, 0, 0, color, "Color");
	indicator->setTitle (fname);  
	childobj = new QTACObject (indicator, QTACHART_OBJ_CURVE);
	childobj->setAttributes (QTACHART_CLOSE, period, "Period", BBANDSUPPER, 0, 0, color, "Color");
	childobj->setParamDialog (paramDialog->getPVector (), fname);
	childobj = new QTACObject (indicator, QTACHART_OBJ_CURVE);
	childobj->setAttributes (QTACHART_CLOSE, period, "Period", BBANDSLOWER, 0, 0, color, "Color");
	childobj->setParamDialog (paramDialog->getPVector (), fname);
  }
  
  if (indicator != NULL)
    indicator->setParamDialog (paramDialog->getPVector (), fname);
  chart->goBack ();
}

// function rejected
void
QTACFunctions::function_rejected (void)
{
  
}


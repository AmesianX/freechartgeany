/*
 * qtachart_objects.cpp
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

// Full implementation of QTACObjects

#include <QStringList>
#include <QResizeEvent>
#include <QScrollBar>
#include "qtachart_core.h"
#include "qtachart_object.h"
#include "ui_qtacobjects.h"

#define NCOLUMNS 		2

// constructor
QTACObjects::QTACObjects (QWidget * parent):
  QWidget (parent), ui (new Ui::QTACObjects)
{
  QStringList cheadersList;
  QString stylesheet = "color black; background-color: lightgray; border-color: black";
  cheadersList << "Object" << "Action";	
  
  ui->setupUi (this);
  ui->editButton->setStyleSheet ("background: transparent; border: 1px solid transparent;border-color: darkgray;color: white;");
  ui->listWidget->setSelectionMode (QAbstractItemView::SingleSelection);
  ui->upToolButton->setToolTip (TOOLTIP + "Scroll Up</span>");  
  ui->downToolButton->setToolTip (TOOLTIP + "Scroll Down</span>");  
  ui->editButton->setToolTip (TOOLTIP + "Edit Object</span>");  
  
  connect (ui->downToolButton, SIGNAL (clicked ()), this,
           SLOT (downButton_clicked ()));
  connect (ui->upToolButton, SIGNAL (clicked ()), this,
           SLOT (upButton_clicked ())); 
  connect (ui->editButton, SIGNAL (clicked ()), this,
           SLOT (editButton_clicked ())); 
  connect (ui->listWidget, SIGNAL (doubleClicked (const QModelIndex &)), this,
           SLOT (listWidget_doubleClicked (const QModelIndex &)));                 	
           
  correctWidgetFonts (this);         
}

// destructor
QTACObjects::~QTACObjects ()
{
  delete ui;
}

// set the reference chart
void
QTACObjects::setReferenceChart (void *chart)
{
  referencechart = chart;
}


void 
QTACObjects::loadObjectVector ()
{
  QTAChart *chart = (QTAChart *) referencechart;
  QTAChartCore *core = (QTAChartCore *) getData (chart);
  QTACObject *obj;
  
  while (ui->listWidget->count () > 0)
    delete ui->listWidget->takeItem(0);
  
  Object.clear ();
  foreach (obj, core->Object)
  {
	if (!obj->deleteit)
	{
	  QString title;
	  title = obj->getTitle ();
	  title.remove (':');
	  if (!title.contains ("Volume"))	
	  {  
        ui->listWidget->addItem (title);
        Object += obj;
      }  
    }  
  }  
    
  if (ui->listWidget->count () == 0)
    ui->listWidget->addItem ("No objects");    	
}
 
/// slots
// down
void 
QTACObjects::downButton_clicked (void)
{
  QScrollBar *vScrollBar = ui->listWidget->verticalScrollBar();
  vScrollBar->triggerAction(QAbstractSlider::SliderSingleStepAdd);
  ui->listWidget->setFocus (Qt::MouseFocusReason);
}

// up
void 
QTACObjects::upButton_clicked (void)
{
  QScrollBar *vScrollBar = ui->listWidget->verticalScrollBar();
  vScrollBar->triggerAction(QAbstractSlider::SliderSingleStepSub);	
  ui->listWidget->setFocus (Qt::MouseFocusReason);
}

// up
void 
QTACObjects::editButton_clicked (void)
{
  QTAChart *chart = (QTAChart *) referencechart;
  QTAChartCore *core = (QTAChartCore *) getData (chart);	
  QTACObject *object;	
  int nobj;
  
  if (Object.size () == 0)
    return;
    
  nobj = ui->listWidget->row (ui->listWidget->selectedItems ().at (0));
  if (nobj < 0)
    return;
    
  object = Object[nobj];
  
  if (object->type == QTACHART_OBJ_CURVE || object->type == QTACHART_OBJ_DOT)
  {
	bool modrslt;	
	modrslt = object->modifyIndicator ();
	if (modrslt == false)
	  object->deleteit = true;
  }  
	    
  if (object->type == QTACHART_OBJ_HLINE ||  
	  object->type == QTACHART_OBJ_VLINE)
  {
	bool modrslt;
	modrslt  = core->lineobjectdialog->modify (object);
	if (modrslt == false)
	  object->deleteit = true;
  }
    	  
  if (object->type == QTACHART_OBJ_LABEL ||  
	  object->type == QTACHART_OBJ_TEXT)
  {
	bool modrslt;	
	modrslt  = core->textobjectdialog->modify (object->text);
	if (modrslt == false)
	  object->deleteit = true;
  }	
    
  if (object->type == QTACHART_OBJ_LINE ||
      object->type == QTACHART_OBJ_FIBO)
  {
	bool modrslt;
	modrslt  = core->lineobjectdialog->modify (object);
	if (modrslt == false)
      object->deleteit = true;
  }
  
  if (object->type == QTACHART_OBJ_SUBCHART)
  {
    bool modrslt;	
	modrslt = object->modifyIndicator ();
	if (modrslt == false)
	  object->deleteit = true;
  }	
  
  loadObjectVector ();  
}

void 
QTACObjects::listWidget_doubleClicked ( const QModelIndex & index )
{
  editButton_clicked ();
}

/// events
// resize
void
QTACObjects::resizeEvent (QResizeEvent * event)
{
  ui->listWidget->resize (570, height () - 20);	
  ui->listWidget->move ((width () - 620) / 2,
                      ((height () - ui->listWidget->height ()) / 2) - 10);
  ui->upToolButton->move (((width () - 620) / 2) + 580,        
                            ((height () - ui->listWidget->height ()) / 2) - 10);        
  ui->downToolButton->move (((width () - 620) / 2) + 580,        
                            (((height () - ui->listWidget->height ()) / 2) - 10) + 60);
  ui->editButton->move (((width () - 620) / 2) + 580,        
                            (((height () - ui->listWidget->height ()) / 2) - 10) + 120);                          
  
}

// show 
void 
QTACObjects::showEvent (QShowEvent * event)
{
  if (this->isVisible () == false)
    return;
    
  loadObjectVector ();  
}

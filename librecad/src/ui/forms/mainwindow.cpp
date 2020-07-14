/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "treemodel.h"
#include <QStandardItemModel>

#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);
	/*
    QStringList headers;
    headers << tr("Title") << tr("Description");

    QFile file(":/main/default.txt");
    file.open(QIODevice::ReadOnly);
    TreeModel *model = new TreeModel(headers, file.readAll());
    file.close();

    view->setModel(model);
    for (int column = 0; column < model->columnCount(); ++column)
        view->resizeColumnToContents(column);
	*/
	QStandardItemModel* model = new QStandardItemModel(1,3);
	QStandardItem* col0Item = new QStandardItem(QIcon(":/main/building.png"), tr("mw_Building Name"));
	QStandardItem* col1Item = new QStandardItem("");
	QStandardItem* col2Item = new QStandardItem(QIcon(":/main/greenlight.png"),"");
	col0Item->setEditable(true);
    col1Item->setEditable(false);
    col2Item->setEditable(false);
	model->setItem(0, 0, col0Item);
	model->setItem(0, 1, col1Item);
	model->setItem(0, 2, col2Item);

	QStandardItem* header0 = new QStandardItem(QIcon(":/main/building.png"), tr("mw_header0_Building"));
	QStandardItem* heaader1 = new QStandardItem(QIcon(":/actions/viewmap.png"), tr("mw_header1_Floor Map"));
	QStandardItem* heaader2 = new QStandardItem(QIcon(":/main/trafficlight.png"), tr("mw_header2_supervising status"));
	model->setHorizontalHeaderItem(0,header0);
	model->setHorizontalHeaderItem(1,heaader1);
	model->setHorizontalHeaderItem(2,heaader2);
	
	view->setModel(model);
	for (int column = 0; column < model->columnCount(); ++column)
        view->resizeColumnToContents(column);

    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    connect(view->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &,
                                    const QItemSelection &)),
            this, SLOT(updateActions()));

    connect(actionsMenu, SIGNAL(aboutToShow()), this, SLOT(updateActions()));
    connect(insertRowAction, SIGNAL(triggered()), this, SLOT(insertRow()));
    //connect(insertColumnAction, SIGNAL(triggered()), this, SLOT(insertColumn()));
    connect(removeRowAction, SIGNAL(triggered()), this, SLOT(removeRow()));
    //connect(removeColumnAction, SIGNAL(triggered()), this, SLOT(removeColumn()));
    connect(removeRowAction_2, SIGNAL(triggered()), this, SLOT(removeRow()));
    connect(insertChildAction_2, SIGNAL(triggered()), this, SLOT(insertChild()));
	

    updateActions();
}

void MainWindow::initMenu()
{
	QMenu* menu;
  
    // File actions:
    menu = menuBar()->addMenu(tr("&File"));
    menu->setObjectName("File");
    fileToolBar->setWindowTitle(tr("File"));
}

void MainWindow::initToolBar()
{

}

bool MainWindow::insertColumn(const QModelIndex &parent)
{
    QAbstractItemModel *model = view->model();
    int column = view->selectionModel()->currentIndex().column();

    // Insert a column in the parent item.
    bool changed = model->insertColumn(column + 1, parent);
    if (changed)
        model->setHeaderData(column + 1, Qt::Horizontal, QVariant(QIcon(":/main/building.png")), Qt::DecorationRole|Qt::EditRole);

    updateActions();

    return changed;
}

void MainWindow::insertRow()
{
    /*
    QModelIndex child;
    QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel *model = view->model();

    if (!model->insertRow(index.row()+1, index.parent()))
        return;

    updateActions();

    child = model->index(index.row()+1, 0, index.parent());
    model->setData(child,QVariant(QIcon(":/main/building.png")), Qt::DecorationRole);
    child = model->index(index.row()+1, 1, index.parent());
    model->setData(child,QVariant(""), Qt::DisplayRole);
    child = model->index(index.row()+1, 2, index.parent());
    model->setData(child,QVariant(QIcon(":/main/greenlight.png")), Qt::DecorationRole);
    */
    QStandardItemModel *model = (QStandardItemModel*)view->model();
    QModelIndex index = view->selectionModel()->currentIndex();
    if (!model->insertRow(index.row()+1, index.parent()))
        return;

    updateActions();
    QStandardItem* col0Item = new QStandardItem(QIcon(":/main/building.png"), tr("mw_Building Name"));
    QStandardItem* col1Item = new QStandardItem(QIcon(":/actions/viewmap.png"),"");
    QStandardItem* col2Item = new QStandardItem(QIcon(":/main/redlight.png"),"");
    col0Item->setEditable(true);
    col1Item->setEditable(false);
    col2Item->setEditable(false);
    model->setItem(index.row()+1, 0, col0Item);
    model->setItem(index.row()+1, 1, col1Item);
    model->setItem(index.row()+1, 2, col2Item);

   
}

void MainWindow::insertChild()
{
    /*
	QModelIndex child;
    QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel *model = view->model();

	
	int rows = 0;//model->rowCount(index);
    if (!model->insertRow(rows, index))
        return;

	if(!model->insertColumn(0, index))
			return;
	if(!model->insertColumn(1, index))
			return;
		

    child = model->index(rows, 0, index);
    model->setData(child, QVariant(QIcon(":/main/floor.png")), Qt::DecorationRole);
	child = model->index(rows, 1, index);
    model->setData(child, QVariant(tr("Floor map not yet set.")), Qt::EditRole);
	
    
    
    //view->selectionModel()->setCurrentIndex(model->index(rows, 0, index),
    //                                        QItemSelectionModel::ClearAndSelect);
    updateActions();
	*/
    QStandardItemModel *model = (QStandardItemModel*)view->model();
    QModelIndex index = view->selectionModel()->currentIndex();
	QStandardItem* parentItem = model->item(index.row(),0);
    

    QStandardItem* col0Item = new QStandardItem(QIcon(":/main/floor.png"),tr("mw_Floor"));
    QStandardItem* col1Item = new QStandardItem(QIcon(":/actions/viewmap.png"),tr("mw_Click here to add map"));
    //QStandardItem* col2Item = new QStandardItem("");
	col0Item->setEditable(true);
    col1Item->setEditable(false);
    //col2Item->setEditable(false);
	
	parentItem->appendRow(QList<QStandardItem *>() << col0Item << col1Item );
	view->selectionModel()->setCurrentIndex(index,QItemSelectionModel::ClearAndSelect);
	view->expand(index);
	updateActions();

}


bool MainWindow::removeColumn(const QModelIndex &parent)
{
    QAbstractItemModel *model = view->model();
    int column = view->selectionModel()->currentIndex().column();

    // Insert columns in each child of the parent item.
    bool changed = model->removeColumn(column, parent);

    if (!parent.isValid() && changed)
        updateActions();

    return changed;
}

void MainWindow::removeRow()
{
    QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel *model = view->model();
    if (model->removeRow(index.row(), index.parent()))
        updateActions();
}

void MainWindow::updateActions()
{
    bool hasSelection = !view->selectionModel()->selection().isEmpty();
    removeRowAction->setEnabled(hasSelection);
    //removeColumnAction->setEnabled(hasSelection);

    bool hasCurrent = view->selectionModel()->currentIndex().isValid();
    insertRowAction->setEnabled(hasCurrent);
    //insertColumnAction->setEnabled(hasCurrent);

    if (hasCurrent) {
        view->closePersistentEditor(view->selectionModel()->currentIndex());

        int row = view->selectionModel()->currentIndex().row();
        int column = view->selectionModel()->currentIndex().column();
        if (view->selectionModel()->currentIndex().parent().isValid()){
			insertChildAction_2->setEnabled(false);
			insertRowAction->setEnabled(false);
            statusBar()->showMessage(tr("Position: (%1,%2)").arg(row).arg(column));
        }else{
        	insertChildAction_2->setEnabled(true);
			insertRowAction->setEnabled(true);
            statusBar()->showMessage(tr("Position: (%1,%2) in top level").arg(row).arg(column));
        }
    }
}


#include "sv_floorselection.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStandardItemModel>
#include <QMessageBox>

#include "mw_window.h"

SV_FloorSelection::SV_FloorSelection(QWidget *parent) :
    QDialog(parent)
{
	mapName = "";
    setWindowTitle("選擇樓層圖");
}

void SV_FloorSelection::closeEvent(QCloseEvent *event)
{
	clearAllItems();
		
	
	
	QStandardItemModel *model = (QStandardItemModel*)view->model();
	if(model != NULL){
		model->clear();
		delete model;
	}
	
	delete view;
}

SV_FloorSelection::~SV_FloorSelection()
{
	
}

void SV_FloorSelection::init(QMap<QString,int>& r, QMap<QString,int>& g,
						  QMap<QString,int>& b, QMap<QString,int>& m)
{
	QString style = "QTreeView::item:!selected "
    "{ "
      "border: 0.5px solid lavender;"
      "border-left: none; "
      "border-top: none; "
    "}"
    "QTreeView::item:selected {}"
    ;

	redAlertMap = r;
	greenAlertMap = g;
	blueAlertMap = b;
	magentaAlertMap = m;
	
    setMinimumSize(450,600);
    view = new MV_TreeView(this);
    view->setIconSize(QSize(48,48));
	view->setStyleSheet(style);
	connect(view, SIGNAL(doubleClicked(const QModelIndex& )),
			this, SLOT(Ondoubleclicktree(const QModelIndex&))); 

	QPushButton* button = new QPushButton();
    button->setIcon(QIcon(":/main/check.png"));
	button->setIconSize(QSize(48,48));
	button->setMaximumWidth(50);
	button->setMaximumHeight(50);
	connect(button, SIGNAL(clicked()), this, SLOT(quit()));
	QVBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *layoutH = new QHBoxLayout;
	layoutH->addWidget(button,0,Qt::AlignRight);
    layout->addWidget(view);
	layout->addLayout(layoutH);
	
    setLayout(layout);
	readConfig();
	updateAlertStatus();
	
}

void SV_FloorSelection::quit()
{
	QModelIndex index = view->selectionModel()->currentIndex();
	if(!index.parent().isValid()){
        QMessageBox::about( NULL , "監控", "請選擇一個樓層圖檔");
		return;
	}
	QString fileName;
	if(index.column() != 1){
		QModelIndex indexTmp = index.sibling(index.row(),1);
		fileName = indexTmp.data().toString();
	}else
		fileName = index.data().toString();
	
    mapName = MW_Window::getScadaSystemFolder() + MW_Window::getProjectFolder() + fileName;
	emit this->getMapName(mapName);
    //qDebug() << mapName;
    setResult(QDialog::Accepted);
	close();
}



void SV_FloorSelection::Ondoubleclicktree(const QModelIndex& index)
{
	if(!index.parent().isValid()){
        QMessageBox::about( NULL , "監控", "請選擇一個樓層圖檔");
		return;
	}

	
	QString fileName;
	if(index.column() != 1){
		QModelIndex indexTmp = index.sibling(index.row(),1);
		fileName = indexTmp.data().toString();
	}else
		fileName = index.data().toString();
	
    mapName = MW_Window::getScadaSystemFolder() + MW_Window::getProjectFolder() + fileName;
	emit this->getMapName(mapName);
    setResult(QDialog::Accepted);
	close();
	return ;

}

void SV_FloorSelection::insertChild(QModelIndex& index, QString col0, QString col1)
{
   
    QStandardItemModel *model = (QStandardItemModel*)view->model();
    QStandardItem* parentItem = model->item(index.row(),0);
    

    QStandardItem* col0Item = new QStandardItem(QIcon(":/main/floor.png"),col0);
    QStandardItem* col1Item = new QStandardItem(QIcon(":/actions/viewmap.png"),col1);
    QStandardItem* col2Item = new QStandardItem("");
    //QStandardItem* col2Item = new QStandardItem(QIcon(":/main/greenlight.png"),"");
	col0Item->setEditable(true);
    col1Item->setEditable(false);
    col2Item->setEditable(false);
	
    parentItem->appendRow(QList<QStandardItem *>() << col0Item << col1Item << col2Item );
	view->selectionModel()->setCurrentIndex(index,QItemSelectionModel::ClearAndSelect);
	view->expand(index);
	for (int column = 0; column < model->columnCount(); ++column)
        view->resizeColumnToContents(column);

}


void SV_FloorSelection::insertRow(QModelIndex& index,int rowCount,QString text)
{
    
    QStandardItemModel *model = (QStandardItemModel*)view->model();
    if(!model->insertRow(index.row()+rowCount, index.parent()))
        return;

    
    QStandardItem* col0Item = new QStandardItem(QIcon(":/main/building.png"), text);
    QStandardItem* col1Item = new QStandardItem("");
    //QStandardItem* col2Item = new QStandardItem(QIcon(":/main/greenlight.png"),"");
    col0Item->setEditable(true);
    col1Item->setEditable(false);
   // col2Item->setEditable(false);
    model->setItem(index.row()+rowCount, 0, col0Item);
    model->setItem(index.row()+rowCount, 1, col1Item);
   // model->setItem(index.row()+rowCount, 2, col2Item);
	for (int column = 0; column < model->columnCount(); ++column)
        view->resizeColumnToContents(column);

   	
}

void SV_FloorSelection::readConfig()
{
	// read data
	bool fileNotFound = false;
    QStringList dataList;
	QString fileName = MW_Window::getScadaSystemFolder() + MW_Window::getProjectFolder() + "building.cfg";
    QFile fIn(fileName);
    if (fIn.open(QFile::ReadOnly | QFile::Text)) {
      QTextStream sIn(&fIn);
      while (!sIn.atEnd())
        dataList += sIn.readLine();
    } else {
      fileNotFound = true;
      qDebug() << "error opening building config file\n";
      //return ;
    }
	fIn.close();

    if(fileNotFound){
        QMessageBox::about( NULL , "監控", "樓層圖檔尚未設定.");
        return;
    }
	if(view != NULL){ //clear all items
		QStandardItemModel *model = (QStandardItemModel*)view->model();
		if(model != NULL)
			model->clear();
	}
    QStandardItemModel* model = new QStandardItemModel(1,2);

	QStandardItem* header0;
    QStandardItem* header1;
    QStandardItem* header2;
	
    header0 = new QStandardItem(QIcon(":/main/building.png"), "大樓名稱");
    header1 = new QStandardItem(QIcon(":/actions/viewmap.png"), "樓層圖");
    header2 = new QStandardItem(QIcon(":/main/trafficlight.png"), "狀態");
	model->setHorizontalHeaderItem(0,header0);
    model->setHorizontalHeaderItem(1,header1);
    model->setHorizontalHeaderItem(2,header2);
	QStandardItem* col0Item;
	if(fileNotFound)
        col0Item = new QStandardItem(QIcon(":/main/building.png"), "大樓名稱");
	else
		col0Item = new QStandardItem(QIcon(":/main/building.png"), dataList[0]);
	
	QStandardItem* col1Item = new QStandardItem("");
    QStandardItem* col2Item = new QStandardItem("");
    col0Item->setEditable(false);
    col1Item->setEditable(false);
    col2Item->setEditable(false);
	model->setItem(0, 0, col0Item);
	model->setItem(0, 1, col1Item);
    model->setItem(0, 2, col2Item);
	view->setModel(model);
    for (int column = 0; column < model->columnCount(); ++column)
        view->resizeColumnToContents(column);


	
	QModelIndex	rootIndex = model->indexFromItem(col0Item);
	QModelIndex	nodeIndex = rootIndex;
	int rowCount = 0;
	for(int i=1;i<dataList.size();i++){
		if(dataList[i].contains("\t")){
			insertChild(nodeIndex,
				       dataList[i].right(dataList[i].length()-1),
				       dataList[i+1].right(dataList[i+1].length()-1));
			
            i += 1;
			
		}else{
		
			insertRow(rootIndex, ++rowCount, dataList[i]);
            QStandardItem* tmpItem = model->item(rowCount,0);
			nodeIndex = model->indexFromItem(tmpItem);
		}
		
	}
	for (int column = 0; column < model->columnCount(); ++column)
        view->resizeColumnToContents(column);
	/*
	QMapIterator<QString, QString> iterator(rtuMap);
	while(iterator.hasNext()) {
	    iterator.next();
        qDebug() << iterator.key() << ":" << iterator.value() ;

	}
	*/
	
}

void SV_FloorSelection::updateAlertStatus()
{
	
	foreach(QString key, magentaAlertMap.keys()){
		if(magentaAlertMap.value(key,-1) > 0){
			AlertNotification(key,Qt::magenta);
			continue;
    	}
	}
	foreach(QString key, greenAlertMap.keys()){
		if(greenAlertMap.value(key,-1) > 0){
			AlertNotification(key,Qt::green);
			continue;
    	}
	}
	foreach(QString key, blueAlertMap.keys()){
		if(blueAlertMap.value(key,-1) > 0){
			AlertNotification(key,Qt::blue);
			continue;
    	}
	}
	foreach(QString key, redAlertMap.keys()){
		if(redAlertMap.value(key,-1) > 0){
			AlertNotification(key,Qt::red);
			continue;
    	}
	}
	
		
}

void SV_FloorSelection::AlertNotification(QString fn, Qt::GlobalColor color)
{
    QModelIndex parent;
	
	
	QStandardItemModel *model = (QStandardItemModel*)view->model();
	QModelIndex index = view->selectionModel()->currentIndex();

	//check top level
	while(index.parent().isValid())
		index = index.parent();

	parent = index.parent();

	bool found = false;
    int rowCount = model->rowCount(parent);
    for(int i = 0; i < rowCount; ++i){
		QModelIndex idx = model->index(i, 0, parent);
    	if(!idx.isValid()){
			break;
	    }
		int rowCountChild = model->rowCount(idx);
		for(int rowChild=0 ; rowChild<rowCountChild ; rowChild++){
			QModelIndex indexChild = model->index(rowChild, 1, idx);
			QString text = indexChild.data(Qt::DisplayRole).toString();
			QModelIndex indexChild2 = model->index(rowChild, 2, idx);
			QStandardItem* item = model->itemFromIndex(indexChild2);
			if(text == fn){
				if(color == Qt::white){
					item->setIcon(QIcon(":/main/idlelight.png"));
				}else
				if(color == Qt::red){
					item->setIcon(QIcon(":/main/redlight.png"));
	                    
				}else
				if(color == Qt::green){
					item->setIcon(QIcon(":/main/greenlight.png"));
						
				}else
				if(color == Qt::blue){
					item->setIcon(QIcon(":/main/bluelight.png"));
	                
						
				}else
				if(color == Qt::yellow){
					item->setIcon(QIcon(":/main/yellowlight.png"));
	                
				}else
				if(color == Qt::magenta){
					item->setIcon(QIcon(":/main/pinklight.png"));
	                
				}
				found = true;
				break;
			}
		}
		if(found == true){
			update();
			break;
		}
    }
	update();
}

void SV_FloorSelection::clearAllItems()
{
    QModelIndex parent;
	QStandardItem *item;
	
	QStandardItemModel *model = (QStandardItemModel*)view->model();
	QModelIndex index = view->selectionModel()->currentIndex();

	
	
	//check top level
	while(index.parent().isValid())
		index = index.parent();

	parent = index.parent();
    int rowCount = model->rowCount(parent);
    for(int i = 0; i < rowCount; ++i){
		item = model->item(i,0);
		if(item)
			delete item;
		
		item = model->item(i,1);
		if(item)
			delete item;
		
		item = model->item(i,2);
		if(item)
			delete item;
		
    }
	
}



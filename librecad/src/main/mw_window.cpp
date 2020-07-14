#include "MW_Window.h"
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QWidget>
#include <QFileDialog>
#include <QDebug>
#include <QApplication>
#include <QLabel>
#include <QPixmap>
#include <QBitmap>
#include <QSplashScreen>
#include <QCryptographicHash>
#include <QTextCodec>
#include "sv_database.h"
#include <QMessageBox>
#include <QDir>
#include <QThread>

QString MW_Window::scadaSystemFolder;
QString MW_Window::projectFolder;

extern QSplashScreen *splash;

MW_Window::~MW_Window()
{
	svWindow->slotExit();
	SV_database::closeConnection();
	if(svWindow != NULL)
		delete svWindow;

}

MW_Window::MW_Window()
{
	QString homeDir = QDir::homePath();
	
#if defined(Q_OS_WIN32)
    MW_Window::setScadaSystemFolder(QString(homeDir + "/Documents/SCADA"));
#elif defined(Q_OS_MAC)
    MW_Window::setScadaSystemFolder(QString(homeDir + "/SCADA"));
#endif
    
    
    if(!QDir(scadaSystemFolder).exists()){
        QDir().mkdir(scadaSystemFolder);
    }
	
	readAuthProfile();
	
	svWindow = NULL;
	mapWindow = NULL;
	view = NULL;
	
    setMinimumSize(800,600);
    //setupUi(this);
    initToolBar();
	initView();
	initActions();
	initMenu();

	projectNameDock = new QDockWidget(tr("Project Name"), this);
    projectNameDock->setAllowedAreas(Qt::TopDockWidgetArea);
	projectNameDock->setTitleBarWidget(new QWidget(this));
	leProjectName = new QLabel(projectName);
	leProjectName->setStyleSheet("QLabel { background-color : rgba(0, 51, 102, 75%); color : white; border: 1px solid black;}");
    leProjectName->setFont(QFont("Verdana", 20, QFont::Bold));
	leProjectName->setAlignment(Qt::AlignHCenter);
    leProjectName->setMinimumHeight(15);
    projectNameDock->setWidget(leProjectName);
    addDockWidget(Qt::TopDockWidgetArea, projectNameDock);
	
	connect(view->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &,const QItemSelection &)),
            this, SLOT(updateActions()));
	
	connect(view,
		    SIGNAL(doubleClicked(const QModelIndex& )),
			this, SLOT(Ondoubleclicktree(const QModelIndex&))); 

	
	//connect(view,
	//	    SIGNAL(clicked(QModelIndex)),
	//	    this,SLOT(Onclickedtree(QModelIndex)));


/*
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    

    
    connect(insertRowAction, SIGNAL(triggered()), this, SLOT(insertRow()));
    //connect(insertColumnAction, SIGNAL(triggered()), this, SLOT(insertColumn()));
    connect(removeRowAction, SIGNAL(triggered()), this, SLOT(removeRow()));
    //connect(removeColumnAction, SIGNAL(triggered()), this, SLOT(removeColumn()));
    connect(removeRowAction_2, SIGNAL(triggered()), this, SLOT(removeRow()));
    connect(insertChildAction_2, SIGNAL(triggered()), this, SLOT(insertChild()));
	*/
	
    updateActions();
	
	
	
	QLabel *logo1 = new QLabel;
	logo1 ->setPixmap(QPixmap(":/main/logo1.png"));
    logo1 ->setScaledContents(false);
	logo1 ->setAlignment(Qt::AlignRight);
    statusBar()->insertPermanentWidget(0, logo1, 0);

	QLabel *logo = new QLabel;
	logo ->setPixmap(QPixmap(":/main/logo.png"));
    logo ->setScaledContents(false);
	logo ->setAlignment(Qt::AlignRight);
    statusBar()->insertPermanentWidget(0, logo, 0);

	
	QTimer::singleShot(1000, this, SLOT(slotLogin()));
	//9/17
	//mapWindow = new QC_ApplicationWindow();
    //connect(mapWindow,SIGNAL(qc_AppWindowlose()),this,SLOT(slotQC_AppWindowClosed()));
    //mapWindow->setWindowTitle("SCADA");
	//mapWindow->slotRunStartScript();
	

	
}

void MW_Window::closeEvent(QCloseEvent *event)
{	
	qDebug() << "MW_Window::closeEvent";
}

void MW_Window::setScadaSystemFolder(const QString& str)
{
	scadaSystemFolder = str + "/";
}

const QString& MW_Window::getScadaSystemFolder()
{
	return scadaSystemFolder;
}

void MW_Window::setProjectFolder(const QString& str)
{
	projectFolder = str + "/";
}

const QString& MW_Window::getProjectFolder()
{
	return projectFolder;
}


void MW_Window::slotLogin()
{
	loginDlg = new MW_loginDialog(this);
    loginDlg->setModal(true);

	
    loginDlg->setAuthorityInfo(userList);
    int rc = loginDlg->exec();
	if(rc != QDialog::Accepted){
		close();
		return;
	}
	
	loginUser = loginDlg->getLoginUser();
	if(loginUser != "scada"){
		userManagerAction->setDisabled(true);
	}
    if(userList.size() == 0){ //no user profile
	    //admin
    	//static const char pdData[] = {0x21,0x23,0x2f,0x29,0x7a,0x57,0xa5,0xa7,0x43,0x89,
        //                            0x4a,0x0e,0x4a,0x80,0x1f,0xc3};
        //scada
        static const unsigned char pdData[] = {0x54,0x79,0xbf,0x0e,0xbb,0x6d,0xc9,0x2d,
                                   0xfc,0x06,0xcb,0x80,0x32,0x06,0x89,0xb9};
		
        QByteArray pdArray = QByteArray::fromRawData((char*)pdData, sizeof(pdData));
    	userList.append(loginUser);
		userList.append(pdArray.toBase64());
		loginUserAuth = "xxx";
		userList.append(loginUserAuth);
        setLoginUserAuth(loginUserAuth);
		writeAuthProfile();
    }else{
        int index = userList.indexOf(loginUser);
        loginUserAuth = userList[index+2];
        setLoginUserAuth(loginUserAuth);
    }
	initProfile();
	delete loginDlg;
}

void MW_Window::initProfile()
{
	
	QString fileName = MW_Window::getScadaSystemFolder() + "project.profile";
    if(!QFile::exists(fileName)){
  		slotSetProjectName();
	}
	readProjectProfile();
	//connect database
	if(!SV_database::createConnection())
		qDebug() << "Database connection failed.";

	readConfig();
	
}

void MW_Window::setLoginUserAuth(QString& auth)
{
	if(auth.at(0) == QChar('h')){ //disable building editing
		insertBuildingAction->setDisabled(true);
		removeBuildingAction->setDisabled(true);
		insertFloorAction->setDisabled(true);
		removeFloorAction->setDisabled(true);
	}
	if(auth.at(1) == QChar('h')){ //disable map editing
		editMapAction->setDisabled(true);
	}
	if(auth.at(2) == QChar('h')){ //disable supevising
		svStartAction->setDisabled(true);
		svStopAction->setDisabled(true);
	}
	 
}

/**
 * Shows the main application window and a splash screen.
 */
void MW_Window::show() {
#ifdef QSPLASHSCREEN_H
    if (splash) {
        splash->raise();
        }
#endif

    QMainWindow::show();
#ifdef QSPLASHSCREEN_H
    if (splash) {
        splash->raise();
        qApp->processEvents();
        splash->clearMessage();
# ifdef QC_DELAYED_SPLASH_SCREEN
        QTimer::singleShot(1000, this, SLOT(finishSplashScreen()));
# else
        finishSplashScreen();
# endif
    }
#endif
	
	
}



/**
 * Called when the splash screen has to terminate.
 */
void MW_Window::finishSplashScreen() {
#ifdef QSPLASHSCREEN_H
    if (splash) {
        splash->finish(this);
        delete splash;
        splash = 0;
    }
#endif
}

void MW_Window::initView()
{
	QString style = "QTreeView::item:!selected "
    "{ "
      "border: 0.5px solid lavender;"
      "border-left: none; "
      "border-top: none; "
    "}"
    "QTreeView::item:selected {}"
    ;
	
    view = new MV_TreeView(this);
	view->setIconSize(QSize(64,64));
	view->setStyleSheet(style);
	connect(view, SIGNAL(dataChanged()), this, SLOT(dataChanged()));
	connect(view, SIGNAL(currentTopLevel(bool)), this, SLOT(currentTopLevel(bool)));
	setCentralWidget(view);
	
	
}

void MW_Window::readProjectProfile()
{
	QStringList dataList;
	QString fileName = MW_Window::getScadaSystemFolder() + "project.profile";
	
    QFile fIn(fileName);
    if (fIn.open(QFile::ReadOnly | QFile::Text)) {
      QTextStream sIn(&fIn);
      while (!sIn.atEnd())
        dataList += sIn.readLine();
    	
    } else {
      return ;
    }
	fIn.close();
	
    projectName = dataList[0];
    setPorjectName(projectName);
	
}

void MW_Window::setPorjectName(QString text)
{
	projectName = text;
	leProjectName->setText(projectName);
    MW_Window::setProjectFolder(projectName);
	
}

void MW_Window::initToolBar()
{
	toolBar = addToolBar(tr("File"));
	toolBar->setIconSize(QSize(32,32));
	toolBar->setFloatable(false);
	toolBar->setMovable(false);

}

void MW_Window::initMenu()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->setObjectName("mwFile");
    fileMenu->addAction(saveAction);
    fileMenu->addAction(exitAction);

	buildingMenu = menuBar()->addMenu(tr("&Building"));
    buildingMenu->setObjectName("mwBuilding");
    buildingMenu->addAction(insertBuildingAction);
    buildingMenu->addAction(removeBuildingAction);

	floorMenu = menuBar()->addMenu(tr("F&Loor"));
    floorMenu->setObjectName("mwFloor");
    floorMenu->addAction(insertFloorAction);
    floorMenu->addAction(removeFloorAction);
	floorMenu->addAction(editMapAction);
	

	setupMenu = menuBar()->addMenu(tr("&Setup"));
    setupMenu->setObjectName("mwSetup");
    setupMenu->addAction(userManagerAction);
    //setupMenu->addAction(setPorjectNameAction);

	superviseMenu = menuBar()->addMenu(tr("Super&Vise"));
    superviseMenu->setObjectName("mwSupervise");
    superviseMenu->addAction(svStartAction);
    superviseMenu->addAction(svStopAction);

	//aboutMenu = menuBar()->addMenu(tr("About"));
    //aboutMenu->setObjectName("mwAbout");
    //aboutMenu->addAction(aboutAction);
    
	
	
}

void MW_Window::initActions()
{
		 
	saveAction = new QAction(tr("Save"), this);
    saveAction->setIcon(QIcon(":/actions/filesave.png")); 
    saveAction->setStatusTip(tr("mw_save building configuration"));
	saveAction->setShortcuts(QKeySequence::Save);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveFile()));
	toolBar->addAction(saveAction);
	toolBar->addSeparator();

	exitAction = new QAction(tr("Exit"), this);
    exitAction->setStatusTip(tr("mw_exit"));
	exitAction->setShortcuts(QKeySequence::Quit);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(mwexit()));
	

	insertBuildingAction = new QAction(tr("Insert Building"), this);
	insertBuildingAction->setIcon(QIcon(":/actions/insertbuilding.png")); 
    insertBuildingAction->setStatusTip(tr("mw_insert building"));
    insertBuildingAction->setShortcut(QKeySequence(tr("Ctrl+B")));
    connect(insertBuildingAction, SIGNAL(triggered()), this, SLOT(insertBuilding()));
	toolBar->addAction(insertBuildingAction);

	removeBuildingAction = new QAction(tr("Remove Building"), this);
	removeBuildingAction->setIcon(QIcon(":/actions/removebuilding.png")); 
    removeBuildingAction->setStatusTip(tr("mw_remove building"));
	
    connect(removeBuildingAction, SIGNAL(triggered()), this, SLOT(removeBuilding()));
	toolBar->addAction(removeBuildingAction);
	toolBar->addSeparator();

	insertFloorAction = new QAction(tr("Insert Floor"), this);
	insertFloorAction->setIcon(QIcon(":/actions/insertfloor.png")); 
    insertFloorAction->setStatusTip(tr("mw_insert floor"));
    insertFloorAction->setShortcut(QKeySequence(tr("Ctrl+F")));
    connect(insertFloorAction, SIGNAL(triggered()), this, SLOT(insertFloor()));
	toolBar->addAction(insertFloorAction);

	removeFloorAction = new QAction(tr("Remove Floor"), this);
	removeFloorAction->setIcon(QIcon(":/actions/removefloor.png")); 
    removeFloorAction->setStatusTip(tr("mw_remove floor"));
    connect(removeFloorAction, SIGNAL(triggered()), this, SLOT(removeFloor()));
	toolBar->addAction(removeFloorAction);
	
	editMapAction = new QAction(tr("Edit map"), this);
	editMapAction->setIcon(QIcon(":/actions/editmap.png")); 
    editMapAction->setStatusTip(tr("mw_edit map"));
    editMapAction->setShortcut(QKeySequence(tr("Ctrl+E")));
    connect(editMapAction, SIGNAL(triggered()), this, SLOT(editMap()));
	toolBar->addAction(editMapAction);
	toolBar->addSeparator();
	
		
	userManagerAction = new QAction(tr("User Manager"), this);
	userManagerAction->setIcon(QIcon(":/actions/usermanager.png")); 
    userManagerAction->setStatusTip(tr("mw_user management"));
    userManagerAction->setShortcut(QKeySequence(tr("Ctrl+U")));
    connect(userManagerAction, SIGNAL(triggered()), this, SLOT(userManagement()));
	toolBar->addAction(userManagerAction);

/*
	setPorjectNameAction = new QAction(tr("Set Project Name"), this);
	setPorjectNameAction->setIcon(QIcon(":/actions/setprojectname.png")); 
    setPorjectNameAction->setStatusTip(tr("Set Project Name"));
    setPorjectNameAction->setShortcut(QKeySequence(tr("Ctrl+P")));
    connect(setPorjectNameAction, SIGNAL(triggered()), this, SLOT(slotSetProjectName()));
	toolBar->addAction(setPorjectNameAction);
*/
	
	svStartAction = new QAction(tr("Supervising Start"), this);
	svStartAction->setIcon(QIcon(":/actions/svstart.png")); 
    svStartAction->setStatusTip(tr("mw_start supervising"));
    svStartAction->setShortcut(QKeySequence(tr("Ctrl+V")));
    connect(svStartAction, SIGNAL(triggered()), this, SLOT(svStart()));
	toolBar->addAction(svStartAction);

	svStopAction = new QAction(tr("Supervising Stop"), this);
	svStopAction->setIcon(QIcon(":/actions/svstop.png")); 
    svStopAction->setStatusTip(tr("mw_stop supervising"));
	svStopAction->setEnabled(false);
    connect(svStopAction, SIGNAL(triggered()), this, SLOT(svStop()));
	toolBar->addAction(svStopAction);
	toolBar->addSeparator();

	//aboutAction = new QAction(tr("About"), this);
    //aboutAction->setStatusTip(tr("mw_about"));
    //connect(aboutAction, SIGNAL(triggered()), this, SLOT(mwAbout()));
}


void MW_Window::insertRow()
{
    
    QStandardItemModel *model = (QStandardItemModel*)view->model();
    QModelIndex index = view->selectionModel()->currentIndex();
    if (!model->insertRow(index.row()+1, index.parent()))
        return;

    
    QStandardItem* col0Item = new QStandardItem(QIcon(":/main/building.png"), tr("mw_Building Name"));
    QStandardItem* col1Item = new QStandardItem("");
    QStandardItem* col2Item = new QStandardItem(QIcon(":/main/greenlight.png"),"");
    col0Item->setEditable(true);
    col1Item->setEditable(false);
    col2Item->setEditable(false);
    model->setItem(index.row()+1, 0, col0Item);
    model->setItem(index.row()+1, 1, col1Item);
    model->setItem(index.row()+1, 2, col2Item);
	for (int column = 0; column < model->columnCount(); ++column)
        view->resizeColumnToContents(column);
	
	updateActions();
	saveFile();

   
}

void MW_Window::insertRow(QModelIndex& index,int rowCount,QString text)
{
    
    QStandardItemModel *model = (QStandardItemModel*)view->model();
    if(!model->insertRow(index.row()+rowCount, index.parent()))
        return;

    
    QStandardItem* col0Item = new QStandardItem(QIcon(":/main/building.png"), text);
    QStandardItem* col1Item = new QStandardItem("");
    QStandardItem* col2Item = new QStandardItem(QIcon(":/main/greenlight.png"),"");
    col0Item->setEditable(true);
    col1Item->setEditable(false);
    col2Item->setEditable(false);
    model->setItem(index.row()+rowCount, 0, col0Item);
    model->setItem(index.row()+rowCount, 1, col1Item);
    model->setItem(index.row()+rowCount, 2, col2Item);
	for (int column = 0; column < model->columnCount(); ++column)
        view->resizeColumnToContents(column);

   	saveFile();
}

void MW_Window::insertChild()
{
   
    QStandardItemModel *model = (QStandardItemModel*)view->model();
	QModelIndex index; 
	if(view->selectionModel()->currentIndex().parent().isValid()){
		index = view->selectionModel()->currentIndex().parent();
	}else{
		index = view->selectionModel()->currentIndex();
	}
		
    
	QStandardItem* parentItem = model->item(index.row(),0);
    

    QStandardItem* col0Item = new QStandardItem(QIcon(":/main/floor.png"),tr("mw_Floor"));
    QStandardItem* col1Item = new QStandardItem(QIcon(":/actions/viewmap.png"),tr("mw_Click here to add map"));
    QStandardItem* col2Item = new QStandardItem("");
	col0Item->setEditable(true);
    col1Item->setEditable(false);
    col2Item->setEditable(false);
	
	parentItem->appendRow(QList<QStandardItem *>() << col0Item << col1Item << col2Item );
	view->selectionModel()->setCurrentIndex(index,QItemSelectionModel::ClearAndSelect);
	view->expand(index);
	for (int column = 0; column < model->columnCount(); ++column)
        view->resizeColumnToContents(column);
	updateActions();
	saveFile();

}

void MW_Window::insertChild(QModelIndex& index, QString col0, QString col1)
{
   
    QStandardItemModel *model = (QStandardItemModel*)view->model();
    QStandardItem* parentItem = model->item(index.row(),0);
    

    QStandardItem* col0Item = new QStandardItem(QIcon(":/main/floor.png"),col0);
    QStandardItem* col1Item = new QStandardItem(QIcon(":/actions/viewmap.png"),col1);
    //QStandardItem* col2Item = new QStandardItem(QIcon(":/main/greenlight.png"),"");
    QStandardItem* col2Item = new QStandardItem("");
	col0Item->setEditable(true);
    col1Item->setEditable(false);
    col2Item->setEditable(false);
	
	parentItem->appendRow(QList<QStandardItem *>() << col0Item << col1Item << col2Item );
	view->selectionModel()->setCurrentIndex(index,QItemSelectionModel::ClearAndSelect);
	view->expand(index);
	for (int column = 0; column < model->columnCount(); ++column)
        view->resizeColumnToContents(column);
	updateActions();
	saveFile();
}



void MW_Window::removeRow()
{
    QModelIndex index = view->selectionModel()->currentIndex();
	//if(!index.parent().isValid())
	//	return;
	
    QAbstractItemModel *model = view->model();
	if (model->removeRow(index.row(), index.parent()))
        updateActions();

	saveFile();
}

void MW_Window::updateActions()
{
    if(view->selectionModel() == NULL)
		return;
	
    bool hasSelection = !view->selectionModel()->selection().isEmpty();
	bool hasCurrent = view->selectionModel()->currentIndex().isValid();

	//check status of disabling building editing
	if(loginUserAuth.at(0) == QChar('x')){
		insertBuildingAction->setEnabled(hasCurrent);
		insertFloorAction->setEnabled(hasCurrent);
		removeBuildingAction->setEnabled(hasSelection);
		removeFloorAction->setEnabled(hasSelection);
	}

    if (hasCurrent) {
        view->closePersistentEditor(view->selectionModel()->currentIndex());
		/*
        int row = view->selectionModel()->currentIndex().row();
        int column = view->selectionModel()->currentIndex().column();
        if (view->selectionModel()->currentIndex().parent().isValid()){
            insertFloorAction->setEnabled(false);
            insertBuildingAction->setEnabled(false);
            //statusBar()->showMessage(tr("Position: (%1,%2)").arg(row).arg(column));
        }else{
            insertFloorAction->setEnabled(true);
            insertBuildingAction->setEnabled(true);
            //statusBar()->showMessage(tr("Position: (%1,%2) in top level").arg(row).arg(column));
        }*/
    }
	
	
}

void MW_Window::readAuthProfile()
{
	userList.clear();
	//format : name\n passwd\n auth\n
	//auth format : xxx  3 bytes,x or h , h:no,x:yes, B0:building,B1:sensor editing,B2:supervise
	QString fileName = MW_Window::getScadaSystemFolder()  + "user.profile";
	
    QFile fIn(fileName);
    if(fIn.open(QFile::ReadOnly | QFile::Text)){
		QTextStream sIn(&fIn);
	    while(!sIn.atEnd()){
	       	userList << sIn.readLine();
	   	}
    }else{
    	return ;
    }
	fIn.close();

	
}

void MW_Window::writeAuthProfile()
{
	//check if update user data
	QString fileName = MW_Window::getScadaSystemFolder()  + "user.profile";
    QFile fOut(fileName);
    if(fOut.open(QFile::WriteOnly | QFile::Text)){
		QTextStream s(&fOut);
		for(int i=0;i<userList.size();i+=3){
			s << userList[i]  << "\n";
			s << userList[i+1]<< "\n";
			s << userList[i+2]<< "\n";
		}
      	
    }else{

      return ;
    }
	fOut.flush();
    fOut.close();
}

void MW_Window::readConfig()
{
	// read data
	bool fileNotFound = false;
    QStringList dataList;
	QString fileName = MW_Window::getScadaSystemFolder() + MW_Window::getProjectFolder() + "building.cfg";
    QFile fIn(fileName);
    /*
    QTime t;
	t.start();
	while(t.elapsed()<5000){
    	QCoreApplication::processEvents();
    	if(fIn.exists() == true){
            qDebug() << "readConfig:" << fileName << " found.";
    		break;
    	}else{
            qDebug() << "readConfig:" << fileName << " not found.";
    	}
	}
	*/
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

	rtuMap.clear();
	if(view != NULL){ //clear all items
		QStandardItemModel *model = (QStandardItemModel*)view->model();
		if(model != NULL)
			model->clear();
	}
	QStandardItemModel* model = new QStandardItemModel(1,3);
	QStandardItem* header0 = new QStandardItem(QIcon(":/main/building.png"), tr("mw_header0_Building"));
	QStandardItem* heaader1 = new QStandardItem(QIcon(":/actions/viewmap.png"), tr("mw_header1_Floor Map"));
	QStandardItem* heaader2 = new QStandardItem(QIcon(":/main/trafficlight.png"), tr("mw_header2_supervising status"));
	model->setHorizontalHeaderItem(0,header0);
	model->setHorizontalHeaderItem(1,heaader1);
	model->setHorizontalHeaderItem(2,heaader2);
	QStandardItem* col0Item;
	if(fileNotFound)
		col0Item = new QStandardItem(QIcon(":/main/building.png"), tr("mw_Building Name"));
	else
		col0Item = new QStandardItem(QIcon(":/main/building.png"), dataList[0]);
	
	QStandardItem* col1Item = new QStandardItem("");
	//QStandardItem* col2Item = new QStandardItem(QIcon(":/main/greenlight.png"),"");
	QStandardItem* col2Item = new QStandardItem("");
	col0Item->setEditable(true);
    col1Item->setEditable(false);
    col2Item->setEditable(false);
	model->setItem(0, 0, col0Item);
	model->setItem(0, 1, col1Item);
	model->setItem(0, 2, col2Item);
	view->setModel(model);
    for (int column = 0; column < model->columnCount(); ++column)
        view->resizeColumnToContents(column);

	if(fileNotFound){
		saveFile();
		return;
	}
	mapsFileList.clear();
	QModelIndex	rootIndex = model->indexFromItem(col0Item);
	QModelIndex	nodeIndex = rootIndex;
	int rowCount = 0;
	for(int i=1;i<dataList.size();i++){
		if(dataList[i].contains("\t")){
			insertChild(nodeIndex,
				       dataList[i].right(dataList[i].length()-1),
				       dataList[i+1].right(dataList[i+1].length()-1));
			readRtuFile(dataList[i+1].right(dataList[i+1].length()-1));
			//add file to maps file list
			mapsFileList << dataList[i+1].right(dataList[i+1].length()-1);
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

void MW_Window::writeConfig()
{
	QStringList cfgList;
	QModelIndex parent;
	
	
	QStandardItemModel *model = (QStandardItemModel*)view->model();
	QModelIndex index = view->selectionModel()->currentIndex();

	//check top level
	while(index.parent().isValid())
		index = index.parent();

	parent = index.parent();

	
    int rowCount = model->rowCount(parent);
    for(int i = 0; i < rowCount; ++i){
		QModelIndex idx = model->index(i, 0, parent);
    	if(idx.isValid()){
	        //retval << idx.data(Qt::DisplayRole).toString();
			cfgList << idx.data(Qt::DisplayRole).toString() << "\n";
	    }
		int rowCountChild = model->rowCount(idx);
		for(int rowChild=0 ; rowChild<rowCountChild ; rowChild++){
			
			for (int colChild = 0; colChild < 2; ++colChild){
				QModelIndex indexChild = model->index(rowChild, colChild, idx);
				cfgList << "\t" << indexChild.data(Qt::DisplayRole).toString() << "\n";
			}
		}
		
    }
    
	//write config
	// write data
	QString fileName = MW_Window::getScadaSystemFolder() + MW_Window::getProjectFolder() + "building.cfg";
    QFile fOut(fileName);
    if(fOut.open(QFile::WriteOnly | QFile::Text)){
		QTextStream s(&fOut);
      	for (int i = 0; i < cfgList.size(); ++i)
        	s << cfgList.at(i);
    }else{
      qDebug() << "error opening output file\n";
      return ;
    }
	fOut.flush();
    fOut.close();
	
    return ;

}

/*
//rtumap format :
//rtu_address:rtu name
//rtuaddress+"_file":file name
//
*/
void MW_Window::readRtuFile(QString fileName)
{
	QStringList dataList;
    QString rtuFile = fileName;
    QString str = rtuFile.right(3);
    //if(str != "dxf" && str != "DXF")
	//	return;


    int len = rtuFile.size();
    rtuFile.replace(len-3,3,"rtu");
	
    QString rtuFn = MW_Window::getScadaSystemFolder() + MW_Window::getProjectFolder() + rtuFile;
	QFile fIn(rtuFn);
	if(fIn.open(QFile::ReadOnly | QFile::Text)){
		QTextCodec *codec=QTextCodec::codecForName("UTF-8");
   		QTextStream sIn(&fIn);
	    sIn.setCodec(codec); 
        while (!sIn.atEnd())
            dataList += codec->fromUnicode(sIn.readLine());
	}else{
	      qDebug() << "error opening" << fileName << " rtu file\n";
	      fIn.close();
	      return ;
	}
	fIn.close();

	QString key;
	QString val;
	for(int i=0;i<dataList.size();i+=4){
		if(dataList[i].contains("102")){
			if(dataList[i+2].contains("103")){
				val = dataList[i+1];
				key = dataList[i+3];
				if(key!=NULL && key.size() != 0){
					rtuMap.insert(key,val);
					//qDebug() << "rtu:" << key << val;
					key += "_file";
					rtuMap.insert(key,fileName);
					//qDebug() << "rtu:" << key << fileName;
				}
			}
		}
	}
    return;
		
}

const QMultiMap<QString, QString>& MW_Window::getRtuInfo()
{
	return rtuMap;

}

void MW_Window::slotSetProjectName()
{

	projectNameDlg = new MW_setProjectNameDialog();
    //connect(projectNameDlg, SIGNAL(accepted()), this, SLOT(slotUpdatePorjectName()));
	projectNameDlg->setModal(true);
    projectNameDlg->exec();
    int rc = projectNameDlg->result();
    qDebug() << "slotSetProjectName:rc=" << rc;
    if(projectNameDlg->result() == 1){  //accept
    	slotUpdatePorjectName();
    }
}



void MW_Window::slotUpdatePorjectName()
{
    projectName = projectNameDlg->getProjectName();
	setPorjectName(projectName);
    MW_Window::setProjectFolder(projectName);
	QString folder = MW_Window::getScadaSystemFolder() + MW_Window::getProjectFolder();
	if(!QDir(folder).exists()){
        QDir().mkdir(folder);
    }
    
	/*
	QString fileName = MW_Window::getScadaSystemFolder() + "projects";
	QFile fOut(fileName);
    if(fOut.open(QFile::WriteOnly | QFile::Text)){
		QTextStream s(&fOut);
		s << projectName;
  	}
	fOut.flush();
    fOut.close();
	*/
	//SV_database::createConnection();
}

void MW_Window::saveFile()
{
	writeConfig();
}

void MW_Window::mwexit()
{

}

void MW_Window::insertBuilding()
{
	insertRow();
}

void MW_Window::removeBuilding()
{
	removeRow();
}


void MW_Window::insertFloor()
{
	insertChild();
}

void MW_Window::removeFloor()
{
	removeRow();
}

void MW_Window::userManagement()
{
	userManDlg = new MW_userManagerDialog();
	connect(userManDlg, SIGNAL(accepted()), this, SLOT(slotAddUser()));
	userManDlg->setModal(true);
    userManDlg->exec();
}

void MW_Window::slotAlertNotification(QString fn, Qt::GlobalColor color, int count)
{
    //qDebug() << "MW_Window:" <<fn << ":" << color <<":" << count ;
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
    	if(idx.isValid()){
	        //retval << idx.data(Qt::DisplayRole).toString();
			//cfgList << idx.data(Qt::DisplayRole).toString() << "\n";
	    }
		int rowCountChild = model->rowCount(idx);
		for(int rowChild=0 ; rowChild<rowCountChild ; rowChild++){
			QModelIndex indexChild = model->index(rowChild, 1, idx);
			QString text = indexChild.data(Qt::DisplayRole).toString();
			QModelIndex indexChild2 = model->index(rowChild, 2, idx);
			QStandardItem* item = model->itemFromIndex(indexChild2);
			//qDebug() << "data=" << text;			
			if(text == fn){
				//qDebug() << "text = fn:";
				if(color == Qt::red){
					//qDebug() << "red";
					if(count > 0)
						item->setIcon(QIcon(":/main/redlight.png"));
	                else
						item->setIcon(QIcon(":/main/greenlight.png"));
	                    
				}else
				if(color == Qt::green){
					//qDebug() << "green";
					item->setIcon(QIcon(":/main/greenlight.png"));
						
				}else
				if(color == Qt::blue){
					//qDebug() << "blue";
					if(count > 0)
						item->setIcon(QIcon(":/main/bluelight.png"));
	                else
						item->setIcon(QIcon(":/main/greenlight.png"));
						
				}else
				if(color == Qt::yellow){
					//qDebug() << "yellow";
					if(count > 0)
						item->setIcon(QIcon(":/main/yellowlight.png"));
	                else
						item->setIcon(QIcon(":/main/greenlight.png"));
				
				}else
				if(color == Qt::magenta){
					//qDebug() << "magenta";
					if(count > 0)
						item->setIcon(QIcon(":/main/magentalight.png"));
	                else
						item->setIcon(QIcon(":/main/greenlight.png"));
					
				}
				found = true;
				break;
			}
		}
		if(found == true)
			break;
    }
	update();
}


void MW_Window::slotAddUser()
{

}
void MW_Window::slotSvWindowClosed()
{
	//qDebug() << "MW_Window::slotSvWindowClosed";
	show();
	svStartAction->setEnabled(true);
	editMapAction->setEnabled(true);
	//delete svWindow;
	//svWindow = NULL;
}

void MW_Window::slotQC_AppWindowClosed()
{
	qDebug() << "MW_Window::slotQC_AppWindowClosed.";
	editMapAction->setEnabled(true);
	//delete mapWindow;
	//mapWindow = NULL;
	readConfig();
}


void MW_Window::svStart()
{
    if(svWindow != NULL){
		delete svWindow;
		svWindow = NULL;
    }
	svWindow = new SV_Window();
    connect(svWindow,SIGNAL(svclose()),this,SLOT(slotSvWindowClosed()));
	connect(svWindow,SIGNAL(svFloorAlertNotification(QString,Qt::GlobalColor,int)),
		    this,SLOT(slotAlertNotification(QString,Qt::GlobalColor,int)));
	
    //svWindow->setMapWindow(alarmWindow);
    svWindow->setRtuMap(rtuMap);
	svWindow->setMapsFileList(mapsFileList);
	svWindow->init();
	svWindow->setActive();
	svWindow->setProjectName(projectName);
	
	
    svStartAction->setEnabled(false);
	svStopAction->setEnabled(true);
	editMapAction->setEnabled(false);
	//hide();
	
}

void MW_Window::svStop()
{
	svWindow->slotExit();
	delete svWindow;
	svWindow = NULL;
    svStartAction->setEnabled(true);
	editMapAction->setEnabled(true);
}

void MW_Window::mwAbout()
{
	
}

void MW_Window::editMap()
{
	saveFile();
    //QStandardItemModel *model = (QStandardItemModel*)view->model();
    QModelIndex index = view->selectionModel()->currentIndex();
	//QStandardItem* parentItem = model->item(index.row(),0);
    qDebug() << "editMap 1";
	if(!index.parent().isValid())
		return;
	
	if(index.column() != 1)
		return;
	
	qDebug() << "editMap 2";

    QString mapName = MW_Window::getScadaSystemFolder() + MW_Window::getProjectFolder() + index.data().toString();
	statusBar()->showMessage(mapName);
	qDebug() << "editMap 3";
	//
	if(mapWindow != NULL){
		delete mapWindow;
		mapWindow = NULL;
	}

	qDebug() << "editMap 4";
	mapWindow = new QC_ApplicationWindow();
    connect(mapWindow,SIGNAL(qc_AppWindowlose()),this,SLOT(slotQC_AppWindowClosed()));
    mapWindow->setWindowTitle("SCADA");
	mapWindow->slotRunStartScript();

	qDebug() << "editMap 5";
 	mapWindow->slotFileOpen(mapName, RS2::FormatDXFRW);
	
	mapWindow->show();
    mapWindow->setEdMode();
	QApplication::setActiveWindow(mapWindow);
    mapWindow->activateWindow();
	mapWindow->slotZoomAuto();
	mapWindow->slotViewDraft(false);
	//mapWindow->setGridMode(false);
	editMapAction->setEnabled(false);

	qDebug() << "editMap 6";

	/* test findChild()
	QTimer*  coloseTimer= new QTimer(this);
    connect(coloseTimer, SIGNAL(timeout()), this, SLOT(slotCloseTimer()));
	coloseTimer->setSingleShot(true);
    coloseTimer->start(10000);
	*/
}

void MW_Window::slotCloseTimer()
{
	/*
	qDebug() << "slotCloseTimer++++++++";
	QDialog * dlg = mapWindow->findChild<QDialog *>();
	if(dlg == NULL)
		qDebug() << "slotCloseTimer: list=null";
	else{
		qDebug() << "slotCloseTimer: list got children";
		dlg->reject();
	}
	*/
}

void MW_Window::dataChanged()
{
	saveFile();
}


void MW_Window::currentTopLevel(bool topLevel)
{
	//check status of disabling building editing
	if(loginUserAuth.at(0) == QChar('h')) 
		return;
		
	if(topLevel == true){
        insertBuildingAction->setEnabled(true);
        removeBuildingAction->setEnabled(true);
	}else{
        insertBuildingAction->setEnabled(false);
        removeBuildingAction->setEnabled(false);
	}
}

void MW_Window::Ondoubleclicktree(const QModelIndex& index)
{
	if(!index.parent().isValid())
		return;

	if(index.column() != 1)
		return;

	QString mapName;
	QStandardItemModel *model = (QStandardItemModel*)view->model();
	QString fn = QFileDialog::getOpenFileName(this, tr("mw_Open File..."),
                                              QString(), tr("JPG-Files (*.jpg);;All Files (*)"));
	if(fn.isEmpty())
		return;

    mapName = index.parent().data().toString();
    mapName += "-" + index.sibling(index.row(),0).data().toString() +".jpg";
    model->setData(index,mapName);

	QString fileName = MW_Window::getScadaSystemFolder() + MW_Window::getProjectFolder() + mapName;

	//check if fileName exists.
	QFile fout(fileName);
	if(!fout.exists()){       
	  QFile::copy(fn, fileName);
	}  
    else{
		QMessageBox message(QMessageBox::Warning,
			               "Information",
                           "檔案已存在,要取代嗎 ?",
			               QMessageBox::Yes|QMessageBox::No,NULL);  
        if (message.exec()==QMessageBox::Yes){
			fout.remove();
            QFile::copy(fn, fileName);
	    }  

	} 
	
	statusBar()->showMessage(mapName);
	saveFile();
}
/*
void MW_Window::Onclickedtree(QModelIndex index)
{
	statusBar()->showMessage(tr("Single clicked: (%1,%2)").arg(index.row()).arg(index.column()));
}
*/

#ifndef MW_WINDOW_H
#define MW_NWINDOW_H


#include <QMainWindow>
#include <QModelIndex>
#include <QSplitter>
#include <QTreeView>
#include <QToolBar>
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QMultiMap>
#include <QMap>
#include <QDockWidget>

#include "sv_window.h"
#include "qc_applicationwindow.h"
#include "mw_setprojectnamedialog.h"
#include "mw_usermanagerdialog.h"
#include "mw_logindialog.h"
#include "mv_treeview.h"

class MW_Window : public QMainWindow
{
    Q_OBJECT
	
public:
    MW_Window();
	~MW_Window();
	void show();
	void readAuthProfile();
	void writeAuthProfile();
	void readProjectProfile();
	void initActions();
	void initMenu();
	void initToolBar();
	void initView();
	void insertRow();
	void removeRow();
	void insertChild();
	void readConfig();
	void writeConfig();
	void insertChild(QModelIndex& index, QString col0, QString col1);
	void insertRow(QModelIndex& index,int rowCount, QString text);
	void readRtuFile(QString fileName);
	const QMultiMap<QString, QString>& getRtuInfo();
    void setLoginUserAuth(QString&);
	void initProfile();
	
	static void setScadaSystemFolder(const QString& str);	
    static const QString& getScadaSystemFolder();
	static void setProjectFolder(const QString& str);	
    static const QString& getProjectFolder();
	static QString scadaSystemFolder;
	static QString projectFolder;

	QStringList			mapsFileList;
	MW_loginDialog		*loginDlg;
	MW_userManagerDialog	*userManDlg;
	MW_setProjectNameDialog *projectNameDlg;
	QLabel *leProjectName;
	QDockWidget *projectNameDock;
	QString projectName;
	QMultiMap<QString, QString> rtuMap;
	QC_ApplicationWindow* mapWindow;
	SV_Window *svWindow;
	MV_TreeView* view;
	QToolBar* toolBar;
	QMenu* fileMenu;
    QMenu* buildingMenu;
    QMenu* floorMenu;
    QMenu* setupMenu;
    QMenu* superviseMenu;
    QMenu* aboutMenu;

	QAction* saveAction;
	QAction* exitAction;
	QAction* insertBuildingAction;
	QAction* removeBuildingAction;
	QAction* insertFloorAction;
	QAction* removeFloorAction;
	QAction* userManagerAction;
	QAction* svStartAction;
	QAction* svStopAction;
	QAction* aboutAction;
	QAction* editMapAction;
	QAction* setPorjectNameAction;

protected:
	void closeEvent(QCloseEvent *event);

private:
	QStringList userList;
    QString loginUser;
    QString loginUserAuth;

public slots:
	void slotLogin();
    void updateActions();
	void finishSplashScreen();
	void slotSetProjectName();
	void setPorjectName(QString);
	void slotUpdatePorjectName();
	void slotAddUser();
	void slotSvWindowClosed();
	void slotQC_AppWindowClosed();
	void dataChanged();
	void currentTopLevel(bool);
	void slotAlertNotification(QString,Qt::GlobalColor,int);
	void slotCloseTimer();
	
private slots:
    void saveFile();
	void mwexit();
	void insertBuilding();
	void removeBuilding();
	void insertFloor();
	void removeFloor();
	void userManagement();
	void svStart();
	void svStop();
	void mwAbout();
	void editMap();
	void Ondoubleclicktree(const QModelIndex&);
	
	
	
	//void Onclickedtree(QModelIndex index);
	
};

#endif 


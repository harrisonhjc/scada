#ifndef MV_TREEVIEW_H
#define MV_TREEVIEW_H

#include <QTreeView>

class MV_TreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit MV_TreeView(QWidget *parent = 0);

	virtual void dataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int> &);

protected:
	virtual void currentChanged(const QModelIndex & current, const QModelIndex & previous);
	
signals:
	void dataChanged();
	void currentTopLevel(bool);
	
public slots:

};

#endif // MV_TREEVIEW_H

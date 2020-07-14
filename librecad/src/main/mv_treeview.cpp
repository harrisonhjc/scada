#include "mv_treeview.h"
#include <QTreeView>

MV_TreeView::MV_TreeView(QWidget *parent) :
    QTreeView(parent)
{
}

void MV_TreeView::currentChanged(const QModelIndex & current, const QModelIndex & previous)
{
	if(!current.parent().isValid())
		emit currentTopLevel(true);
	else
		emit currentTopLevel(false);
}


void MV_TreeView::dataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int> & roles)
{
	emit dataChanged();
}

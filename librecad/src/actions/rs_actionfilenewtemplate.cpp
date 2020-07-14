/****************************************************************************
**
** This file is part of the LibreCAD project, a 2D CAD program
**
** Copyright (C) 2010 R. van Twisk (librecad@rvt.dds.nl)
** Copyright (C) 2001-2003 RibbonSoft. All rights reserved.
**
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software 
** Foundation and appearing in the file gpl-2.0.txt included in the
** packaging of this file.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**
** This copyright notice MUST APPEAR in all copies of the script!  
**
**********************************************************************/

#include "rs_actionfilenewtemplate.h"

#include <QAction>


RS_ActionFileNewTemplate::RS_ActionFileNewTemplate(RS_EntityContainer& container,
                                   RS_GraphicView& graphicView)
        :RS_ActionInterface("File New", container, graphicView) {}


QAction* RS_ActionFileNewTemplate::createGUIAction(RS2::ActionType /*type*/, QObject* parent) {
        // tr("New Drawing")
    QAction* action = new QAction(tr("New From &Template"), parent);
#if QT_VERSION >= 0x040600
        action->setIcon(QIcon::fromTheme("document-new", QIcon(":/actions/filenew.png")));
#else
        action->setIcon(QIcon(":/actions/filenew.png"));
#endif
//	action->setShortcut(QKeySequence::New);
    //action->zetStatusTip(tr("Creates a new drawing"));
	
    return action;
}


void RS_ActionFileNewTemplate::trigger() {
    /*
    // Not supported currently
    RS_DEBUG->print("RS_ActionFileNewTemplate::trigger");

    QString fileName; //= RS_DIALOGFACTORY->requestFileNewDialog();
    if (graphic!=NULL && !fileName.isEmpty()) {
        graphic->open(fileName, );
}
    */
    finish(false);
}


void RS_ActionFileNewTemplate::init(int status) {
    RS_ActionInterface::init(status);
    trigger();
}

// EOF

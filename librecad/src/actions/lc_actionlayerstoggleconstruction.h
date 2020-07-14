/****************************************************************************
**
 * Toggle whether a layer is a construction layer
 * Construction layer doesn't appear on printout
 * and have straight lines of infinite length

Copyright (C) 2015 A. Stebich (librecad@mail.lordofbikes.de)
Copyright (C) 2011 Dongxu Li (dongxuli2011@gmail.com)
Copyright (C) 2011 R. van Twisk (librecad@rvt.dds.nl)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**********************************************************************/


#ifndef LC_ACTIONLAYERSTOGGLECONSTRUCTION_H
#define LC_ACTIONLAYERSTOGGLECONSTRUCTION_H

#include "rs_actioninterface.h"


/**
 * whether a layer is a construction layer or not
 * construction layers doesn't appear on printout,
 * and have straight lines of infinite length
 *
 * @author Armin Stebich
 */
class LC_ActionLayersToggleConstruction : public RS_ActionInterface {
    Q_OBJECT
public:
    LC_ActionLayersToggleConstruction(RS_EntityContainer& container,
                                      RS_GraphicView& graphicView);
    ~LC_ActionLayersToggleConstruction() {}

    static QAction* createGUIAction(RS2::ActionType /*type*/, QObject* /*parent*/);

    virtual void init(int status=0);
    virtual void trigger();

};

#endif

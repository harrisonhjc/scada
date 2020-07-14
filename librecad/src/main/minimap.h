/*
 * minimap.h
 * Copyright 2012, Christoph Schnackenberg <bluechs@gmx.de>
 * Copyright 2012, Thorbjørn Lindeijer <thorbjorn@lindeijer.nl>
 *
 * This file is part of Tiled.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include <QFrame>
#include <QImage>
#include <QTimer>
#include "qc_mdiwindow.h"

class QG_GraphicView;

class MiniMap : public QFrame
{
    Q_OBJECT

public:
    MiniMap(QWidget *parent);

    void setMap(QC_MDIWindow*);

    QSize sizeHint() const override;
    QSize getMapSize();

public slots:
    /** Schedules a redraw of the minimap image. */
    void scheduleMapImageUpdate();

protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    void redrawTimeout();

private:
    bool mFirstRender;
    QRectF mViewRect;
    QRectF mMapRect;
    QRect mViewPortRect;
    QImage mMapImage;
    QRect mImageRect;
    QTimer mMapImageUpdateTimer;
    bool mDragging;
    QPoint mDragOffset;
    bool mMouseMoveCursorState;
    bool mRedrawMapImage;
    QC_MDIWindow* mMDIWindow;
    
    QRect viewportRect() ;
    QPointF mapToScene(QPoint p) const;
    void updateImageRect();
    void renderMapToImage();
    void centerViewOnLocalPixel(QPoint centerPos, int delta = 0);
    void getOriginalMap();
    
    
};


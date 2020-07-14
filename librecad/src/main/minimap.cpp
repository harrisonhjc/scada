/*
 * minimap.cpp
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

#include "minimap.h"

#include <QCursor>
#include <QResizeEvent>
#include <QScrollBar>
#include <QUndoStack>
#include <QGuiApplication>
#include <QPainter>
#include <QPen>
#include <cmath>
#include "qg_graphicview.h"
#include "rs_debug.h"
#include "rs_actionzoomin.h"

MiniMap::MiniMap(QWidget *parent)
    : QFrame(parent)
    , mDragging(false)
    , mMouseMoveCursorState(false)
    , mRedrawMapImage(false)
    
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setMinimumSize(50, 50);

    // for cursor changes
    setMouseTracking(true);

    mMapImageUpdateTimer.setSingleShot(true);
    connect(&mMapImageUpdateTimer, &QTimer::timeout, this, &MiniMap::redrawTimeout);
    mFirstRender = true;

    //mViewPortRect = QRect(100,100, 60,60);
}

void MiniMap::setMap(QC_MDIWindow* w)
{
    mMDIWindow = w;
    QG_GraphicView* mapView =  mMDIWindow->getGraphicView();
    if(mapView != nullptr){
        connect(mapView, SIGNAL(horizontalScrollBar()), this, SLOT(update()) );
        connect(mapView, SIGNAL(verticalScrollBar()), this, SLOT(update()) );
        connect(mapView, SIGNAL(zoomChanged()), this, SLOT(update()) );
    }
    //mImageRect = QRect(0,0, width(), height());
    scheduleMapImageUpdate();

    

}

QSize MiniMap::sizeHint() const
{
    //return defaultDpiScale(QSize(200, 200));
    return QSize(500, 500);
}



void MiniMap::scheduleMapImageUpdate()
{
    mMapImageUpdateTimer.start(100);
}

void MiniMap::getOriginalMap()
{
    QG_GraphicView* mapView =  mMDIWindow->getGraphicView();
    if(mapView == nullptr)
        return;

    mViewRect= QRectF(0, 0, mapView->getWidth(), mapView->getHeight());
    mMapRect = QRectF(mapView->posPaper.x, mapView->posPaper.y,
                                   mapView->sizePaper.x, mapView->sizePaper.x);
    /*
    QString s;
    s.sprintf("MiniMap::setMap:getOriginalMap:mMapRect:x.y.w.h=%lf:%lf:%lf:%lf",
              mMapRect.x(),mMapRect.y(),mMapRect.width(),mMapRect.height());
    qDebug() << s;
    s.sprintf("MiniMap::setMap:getOriginalMap:mViewRect:x.y.w.h=%lf:%lf:%lf:%lf",
              mViewRect.x(),mViewRect.y(),mViewRect.width(),mViewRect.height());
    qDebug() << s;
    */
}

void MiniMap::paintEvent(QPaintEvent *pe)
{
    QFrame::paintEvent(pe);

    
    //RS_DEBUG->print(RS_Debug::D_WARNING, "MiniMap::paintEvent ++");
    if(mFirstRender){
        //get original paper size
        getOriginalMap();
        mFirstRender = false;
    }
    if (mRedrawMapImage) {
        //RS_DEBUG->print(RS_Debug::D_WARNING, "MiniMap::paintEvent mRedrawMapImage");
        renderMapToImage();
        mRedrawMapImage = false;
    }

    if (mMapImage.isNull() || mImageRect.isEmpty()){
        //RS_DEBUG->print(RS_Debug::D_WARNING, "MiniMap::paintEvent mMapImage=null");
        return;
    }

    QPainter p(this);
    
    QRect r = mImageRect;
    //p.fillRect(r, QColor(255, 255, 255));
    r.moveTo(0,0);
    //int h = r.height() + 100;
    //r.setHeight(h);
    p.drawImage(r, mMapImage);
    
    QRect viewRect = viewportRect();
    int h = viewRect.height() - 10;
    viewRect.setHeight(h);
    //p.setBrush(Qt::NoBrush);
    //p.setPen(QColor(0, 0, 0, 128));
    //p.translate(1, 1);
    //p.drawRect(viewRect);

    QPen outLinePen(QColor(255, 0, 0), 2);
    outLinePen.setJoinStyle(Qt::MiterJoin);
    //p.translate(3, 20);
    p.translate(0, 0);
    p.setPen(outLinePen);
    p.drawRect(viewRect);

    
    //RS_DEBUG->print(RS_Debug::D_WARNING, "MiniMap::paintEvent --");
}

QRect MiniMap::viewportRect() 
{
    
    QG_GraphicView* mapView =  mMDIWindow->getGraphicView();
    if (!mapView)
        return QRect(0, 0, 1, 1);

    const QRectF viewRect= QRectF(0, 0, mapView->getWidth(), mapView->getHeight());
    const QRectF mapRect = QRectF(mapView->posPaper.x, mapView->posPaper.y,
                                   mapView->sizePaper.x, mapView->sizePaper.x);
/*
    QString s;

    s.sprintf("MiniMap::viewportRect:mImageRect:x.y.w.h=%d:%d:%d:%d",
              mImageRect.x(),mImageRect.y(),mImageRect.width(),mImageRect.height());
    qDebug() << s;
    s.sprintf("MiniMap::viewportRect:mapRect:x.y.w.h=%lf:%lf:%lf:%lf",
              mapRect.x(),mapRect.y(),mapRect.width(),mapRect.height());
    qDebug() << s;
    s.sprintf("MiniMap::viewportRect:viewRect:x:y:w:h=%lf:%lf:%lf:%lf",
              viewRect.x(),viewRect.y(),viewRect.width(),viewRect.height());
    qDebug() << s;
*/
    int x,y,w,h;
    int dx = mMapRect.x() - mapRect.x();
    int dy = mMapRect.y() - mapRect.y();
    qreal ratioX = mMapRect.width() / mapRect.width();
    qreal ratioY = mMapRect.height() / mapRect.height();
    
    x = dx / viewRect.width() * mImageRect.width() + mImageRect.x();
    y = dy / viewRect.height() * mImageRect.height() + mImageRect.y();
    w = ratioX * mImageRect.width();
    h = ratioY * mImageRect.height();
    
    /*  
    QString s;
    s.sprintf("MiniMap::viewportRect:width->mMapRect:mapRect=%lf:%lf", mMapRect.width(), mapRect.width() );
    qDebug() << s;
    s.sprintf("MiniMap::viewportRect:QRect=:x:y:w:h=%d:%d:%d:%d",x,y,w,h);
    qDebug() << s;
    */
    return QRect(x,y,w,h);
    /*
    return QRect((viewRect.x() - mapRect.x()) / mapRect.width() * mImageRect.width() + mImageRect.x(),
                 (viewRect.y() - mapRect.y()) / mapRect.height() * mImageRect.height() + mImageRect.y(),
                 viewRect.width() / mapRect.width() * mImageRect.width(),
                viewRect.height() / mapRect.height() * mImageRect.height());
    */            
    
}

QPointF MiniMap::mapToScene(QPoint p) const
{
    if (mImageRect.isEmpty())
        return QPointF();

    QG_GraphicView* mapView =  mMDIWindow->getGraphicView();
    if (!mapView)
        return QPointF();

    const QRectF mapRect = QRectF(0, 0, mapView->getWidth(), mapView->getHeight());
    p -= mImageRect.topLeft();
    return QPointF(p.x() * (mapRect.width() / mImageRect.width()) + mapRect.x(),
                   p.y() * (mapRect.height() / mImageRect.height()) + mapRect.y());
    
}

void MiniMap::resizeEvent(QResizeEvent *)
{
    //qDebug() << "MiniMap::resizeEvent++";
    updateImageRect();
    scheduleMapImageUpdate();
}

void MiniMap::updateImageRect()
{
    QG_GraphicView* mapView =  mMDIWindow->getGraphicView();
    if (!mapView)
        return;
    //int wImg = width();
    //int h = wImg * mapView->height() / mapView->getWidth();
    mImageRect = QRect(0,0,280, 350);
    //setFixedSize(wImg, h);
    //qDebug() << "MiniMap::updateImageRect:w:h=" << wImg <<":"<< h;
    //QPoint imgPt = QPoint(550, h);
    //QPoint pa = mapToParent(imgPt);
    //parentWidget()->setFixedSize(pa.x(), pa.y());
 
}

QSize MiniMap::getMapSize()
{
    return mMapImage.size();
}

void MiniMap::renderMapToImage()
{
    //RS_DEBUG->print(RS_Debug::D_WARNING, "MiniMap::renderMapToImage++");
    QG_GraphicView* mapView =  mMDIWindow->getGraphicView();
    if(mapView != nullptr){
        //RS_DEBUG->print(RS_Debug::D_WARNING, "MiniMap::renderMapToImage：got gv");
        /*
        QSize viewSize = contentsRect().size() * devicePixelRatioF();
        QSize mapSize = QSize(mapView->getWidth(), mapView->getWidth());
        qreal scale = qMin((qreal) viewSize.width() / mapSize.width(),
                       (qreal) viewSize.height() / mapSize.height());
        const QSize imageSize = mapSize * scale;
        */
            


        QSize mapSize = QSize(mapView->getWidth(), mapView->getWidth());
        const QSize imageSize = QSize(mImageRect.width(), mImageRect.height());
        if (mMapImage.size() != imageSize) {
            mMapImage = QImage(imageSize, QImage::Format_ARGB32_Premultiplied);
            //updateImageRect();
        }
        mapView->renderToImage(mMapImage, mapSize);
        //RS_DEBUG->print(RS_Debug::D_WARNING, "MiniMap::renderMapToImage:viewSize:w.h=%d.%d",
        //                viewSize.width(),viewSize.height());
        //RS_DEBUG->print(RS_Debug::D_WARNING, "MiniMap::renderMapToImage:mapSize:w.h=%d.%d",
        //                mapSize.width(),mapSize.height());

        //RS_DEBUG->print(RS_Debug::D_WARNING, "MiniMap::renderMapToImage:w.h=%d.%d",
        //                mMapImage.width(),mMapImage.height());
    }
    //RS_DEBUG->print(RS_Debug::D_WARNING, "MiniMap::renderMapToImage--");
}

void MiniMap::centerViewOnLocalPixel(QPoint centerPos, int delta)
{
    QG_GraphicView* mapView =  mMDIWindow->getGraphicView();
    if (!mapView)
        return;

    QRectF viewRect= QRectF(0, 0, mapView->getWidth(), mapView->getHeight());
    QRectF mapRect = QRectF(mapView->posPaper.x, mapView->posPaper.y,
                                   mapView->sizePaper.x, mapView->sizePaper.x);

    qDebug() << "centerViewOnLocalPixel:centerPos:x:y=" << centerPos.x() << ":" << centerPos.y();

    //qreal x = (centerPos.x() - mImageRect.x()) * viewRect.width() / mImageRect.width() + mapRect.x();
    //qreal y = (centerPos.y() - mImageRect.y()) * viewRect.height() / mImageRect.height() + mapRect.y();
    //mapRect.translate(centerPos.x(), centerPos.y());
    mapView->setOffset(-centerPos.x(), centerPos.y());
    mapView->update();

    //QString s;
    //s.sprintf("centerViewOnLocalPixel:x.y=%lf,%lf",x,y);
    //qDebug() << s;
    //mapView->forceCenterOn(mapToScene(centerPos));
}



void MiniMap::redrawTimeout()
{
    mRedrawMapImage = true;
    update();
}

void MiniMap::wheelEvent(QWheelEvent *e)
{
    QG_GraphicView* mapView =  mMDIWindow->getGraphicView();
    if (!mapView)
        return;

    if (e->orientation() == Qt::Vertical) {
        if (e->delta() != 0){
            RS_Vector mouse = mapView->toGraph(RS_Vector(e->x(), e->y()));
            if (e->delta() <= -120) {
                mapView->zoomOutMap(mouse);
            } else if (e->delta() >= 120) {
                mapView->zoomInMap(mouse);
            } 
        }

        return;
    }

    QFrame::wheelEvent(e);
}

void MiniMap::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QPoint cursorPos = event->pos();
        QRect viewPort = viewportRect();

        if (viewPort.contains(cursorPos)) {
            //qDebug() << "mousePressEvent:viewPort.contains";
            mDragOffset = viewPort.center() - cursorPos + QPoint(1, 1);
            cursorPos += mDragOffset;
        } else {
            //qDebug() << "mousePressEvent:viewPort.contains:else";
            mDragOffset = QPoint();
            centerViewOnLocalPixel(cursorPos);
        }

        mDragging = true;
        setCursor(Qt::ClosedHandCursor);

        return;
    }

    QFrame::mouseReleaseEvent(event);
}

void MiniMap::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && mDragging) {
        mDragging = false;

        QRect viewPort = viewportRect();
        if (viewPort.contains(event->pos())) {
            setCursor(Qt::OpenHandCursor);
            mMouseMoveCursorState = true;
        } else if (rect().contains(event->pos())) {
            unsetCursor();
            mMouseMoveCursorState = false;
        }

        return;
    }

    QFrame::mouseReleaseEvent(event);
}

void MiniMap::mouseMoveEvent(QMouseEvent *event)
{
    if (mDragging) {
        qDebug() << "mouseMoveEvent:mDragging";
        centerViewOnLocalPixel(event->pos() + mDragOffset);
        return;
    }

    if (viewportRect().contains(event->pos())) {
        //qDebug() << "mouseMoveEvent:viewportRect().contains";
        if (!mMouseMoveCursorState) {
            setCursor(Qt::OpenHandCursor);
            mMouseMoveCursorState = true;
        }
    } else {
        if (mMouseMoveCursorState) {
            unsetCursor();
            mMouseMoveCursorState = false;
        }
    }

    QFrame::mouseMoveEvent(event);
}




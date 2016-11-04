/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "widgethellper.h"

#include <QMatrix3x3>
#include <QImage>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsBlurEffect>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <QWidget>

QT_BEGIN_NAMESPACE
extern Q_WIDGETS_EXPORT void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);
QT_END_NAMESPACE

namespace WidgetHelper
{

QImage applyEffectToImage(const QImage src, QGraphicsEffect *effect, int extent = 0)
{
    if (src.isNull()) { return QImage(); }
    if (!effect) { return src; }
    QGraphicsScene scene;
    QGraphicsPixmapItem item;
    item.setPixmap(QPixmap::fromImage(src));
    item.setGraphicsEffect(effect);
    scene.addItem(&item);
    QImage res(src.size() + QSize(extent * 2, extent * 2), QImage::Format_ARGB32);
    res.fill(Qt::transparent);
    QPainter ptr(&res);
    scene.render(&ptr, QRectF(),
                 QRectF(-extent, -extent, src.width() + extent * 2,
                        src.height() + extent * 2));
    return res;
}

QPixmap blurImage(const QImage &image, int radius)
{
    QGraphicsBlurEffect *blur = new QGraphicsBlurEffect;
    blur->setBlurRadius(radius);
    QImage result = applyEffectToImage(image, blur);
    int cropWidth = 20;
    QRect rect(cropWidth, cropWidth,
               result.width() - cropWidth * 2,
               result.height() - cropWidth * 2);
    QImage cropped = result.copy(rect);
    return QPixmap::fromImage(cropped);
}

QImage cropRect(const QImage &image, QSize sz)
{
    QImage newImage = image.scaled(sz, Qt::KeepAspectRatioByExpanding);
    auto center = QRectF(newImage.rect()).center();
    auto topLeft = QPointF(center.x() - sz.width() / 2.0,
                           center.y() - sz.height()/2.0);
    auto bottomRight = QPointF(center.x() + sz.width() / 2.0,
                               center.y() + sz.height()/2.0);
    QRect crop(topLeft.toPoint(), bottomRight.toPoint());
    return newImage.copy(crop);
}


void slideRight2LeftWidget(QWidget *left, QWidget *right, int delay)
{
    right->show();
    left->show();
    right->resize(left->size());

    QRect leftStart = QRect(0, 0, left->width(), left->height());
    QRect leftEnd = leftStart;
    leftEnd.moveLeft(-left->width());

    QPropertyAnimation *animation = new QPropertyAnimation(left, "geometry");
    animation->setDuration(delay);
    animation->setEasingCurve(QEasingCurve::InOutCubic);
    animation->setStartValue(leftStart);
    animation->setEndValue(leftEnd);
    animation->start();

    QRect rightStart = QRect(left->width(), 0, right->width(), right->height());
    QRect rightEnd = rightStart;
    rightEnd.moveRight(left->width() - 1);

    QPropertyAnimation *animation2 = new QPropertyAnimation(right, "geometry");
    animation2->setEasingCurve(QEasingCurve::InOutCubic);
    animation2->setDuration(delay);
    animation2->setStartValue(rightStart);
    animation2->setEndValue(rightEnd);
    animation2->start();

    animation->connect(animation, &QPropertyAnimation::finished,
                       animation, &QPropertyAnimation::deleteLater);
    animation2->connect(animation2, &QPropertyAnimation::finished,
                        animation2, &QPropertyAnimation::deleteLater);
    animation2->connect(animation2, &QPropertyAnimation::finished,
                        left, &QWidget::hide);

}

void slideBottom2TopWidget(QWidget *top, QWidget *bottom, int delay)
{
    bottom->show();
    top->show();
    bottom->resize(top->size());

    QRect topStart = QRect(0, 0, top->width(), top->height());
    QRect topEnd = topStart;
    topEnd.moveTop(-top->height());

    QPropertyAnimation *animation = new QPropertyAnimation(top, "geometry");
    animation->setDuration(delay);
    animation->setEasingCurve(QEasingCurve::InOutCubic);
    animation->setStartValue(topStart);
    animation->setEndValue(topEnd);
    animation->start();

    QRect bottomStart = QRect(0, top->height(), bottom->width(), bottom->height());
    QRect bottomEnd = bottomStart;
    bottomEnd.moveTo(0, 0);

    QPropertyAnimation *animation2 = new QPropertyAnimation(bottom, "geometry");
    animation2->setEasingCurve(QEasingCurve::InOutCubic);
    animation2->setDuration(delay);
    animation2->setStartValue(bottomStart);
    animation2->setEndValue(bottomEnd);
    animation2->start();

    auto bottomOpacity = new QGraphicsOpacityEffect(bottom);
    bottom->setGraphicsEffect(bottomOpacity);
    bottomOpacity->setOpacity(0);

    QPropertyAnimation *animation3 = new QPropertyAnimation(bottomOpacity, "opacity");
    animation3->setEasingCurve(QEasingCurve::InCubic);
    animation3->setDuration(delay);
    animation3->setStartValue(0);
    animation3->setEndValue(1);
    animation3->start();
    animation->connect(animation3, &QPropertyAnimation::finished,
                       animation3, &QPropertyAnimation::deleteLater);
    animation->connect(animation3, &QPropertyAnimation::finished,
                       bottomOpacity, &QGraphicsOpacityEffect::deleteLater);

    auto topOpacity = new QGraphicsOpacityEffect(top);
    top->setGraphicsEffect(topOpacity);
    topOpacity->setOpacity(1);

    QPropertyAnimation *animation4 = new QPropertyAnimation(topOpacity, "opacity");
    animation4->setEasingCurve(QEasingCurve::InCubic);
    animation4->setDuration(delay);
    animation4->setStartValue(1);
    animation4->setEndValue(0);
    animation4->start();
    animation->connect(animation4, &QPropertyAnimation::finished,
                       animation4, &QPropertyAnimation::deleteLater);
    animation->connect(animation4, &QPropertyAnimation::finished,
                       topOpacity, &QGraphicsOpacityEffect::deleteLater);

    animation->connect(animation, &QPropertyAnimation::finished,
                       animation, &QPropertyAnimation::deleteLater);
    animation->connect(animation2, &QPropertyAnimation::finished,
                       animation2, &QPropertyAnimation::deleteLater);
    animation->connect(animation2, &QPropertyAnimation::finished,
                       top, &QWidget::hide);

}

void slideTop2BottomWidget(QWidget *top, QWidget *bottom, int delay)
{
    bottom->show();
    top->show();
    bottom->resize(top->size());

    QRect topStart = QRect(0, 0, top->width(), top->height());
    QRect topEnd = topStart;
    topEnd.moveTo(0, top->height());

    QPropertyAnimation *animation = new QPropertyAnimation(top, "geometry");
    animation->setDuration(delay);
    animation->setEasingCurve(QEasingCurve::InOutCubic);
    animation->setStartValue(topStart);
    animation->setEndValue(topEnd);
    animation->start();

    QRect bottomStart = QRect(0, -top->height(), bottom->width(), bottom->height());
    QRect bottomEnd = bottomStart;
    bottomEnd.moveBottom(top->height() - 1);

    QPropertyAnimation *animation2 = new QPropertyAnimation(bottom, "geometry");
    animation2->setEasingCurve(QEasingCurve::InOutCubic);
    animation2->setDuration(delay);
    animation2->setStartValue(bottomStart);
    animation2->setEndValue(bottomEnd);
    animation2->start();

    auto bottomOpacity = new QGraphicsOpacityEffect(bottom);
    bottom->setGraphicsEffect(bottomOpacity);
    bottomOpacity->setOpacity(0);

    QPropertyAnimation *animation3 = new QPropertyAnimation(bottomOpacity, "opacity");
    animation3->setEasingCurve(QEasingCurve::InCubic);
    animation3->setDuration(delay);
    animation3->setStartValue(0);
    animation3->setEndValue(1);
    animation3->start();
    animation->connect(animation3, &QPropertyAnimation::finished,
                       animation3, &QPropertyAnimation::deleteLater);
    animation->connect(animation3, &QPropertyAnimation::finished,
                       bottomOpacity, &QGraphicsOpacityEffect::deleteLater);

    auto topOpacity = new QGraphicsOpacityEffect(top);
    top->setGraphicsEffect(topOpacity);
    topOpacity->setOpacity(0.99);

    QPropertyAnimation *animation4 = new QPropertyAnimation(topOpacity, "opacity");
    animation4->setEasingCurve(QEasingCurve::InCubic);
    animation4->setDuration(delay);
    animation4->setStartValue(0.99);
    animation4->setEndValue(0);
    animation4->start();
    animation->connect(animation4, &QPropertyAnimation::finished,
                       animation4, &QPropertyAnimation::deleteLater);
    animation->connect(animation4, &QPropertyAnimation::finished,
                       topOpacity, &QGraphicsOpacityEffect::deleteLater);

    animation->connect(animation, &QPropertyAnimation::finished,
                       animation, &QPropertyAnimation::deleteLater);
    animation->connect(animation2, &QPropertyAnimation::finished,
                       animation2, &QPropertyAnimation::deleteLater);
    animation->connect(animation2, &QPropertyAnimation::finished,
                       top, &QWidget::hide);
}

void slideEdgeWidget(QWidget *right, QRect start, QRect end, int delay, bool hide)
{
    right->show();

    QPropertyAnimation *animation2 = new QPropertyAnimation(right, "geometry");
    animation2->setEasingCurve(QEasingCurve::InCurve);
    animation2->setDuration(delay);
    animation2->setStartValue(start);
    animation2->setEndValue(end);
    animation2->start();
    animation2->connect(animation2, &QPropertyAnimation::finished,
                        animation2, &QPropertyAnimation::deleteLater);
    if (hide)
        animation2->connect(animation2, &QPropertyAnimation::finished,
                            right, &QWidget::hide);

}


};
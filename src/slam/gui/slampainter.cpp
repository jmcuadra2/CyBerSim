/***************************************************************************
 *   Copyright (C) 2010 by Jose Manuel Cuadra Troncoso   *
 *   jmcuadra@dia.uned.es   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "slampainter.h"
#include "../../neuraldis/ndmath.h"
#include "qgraphicsitem.h"
#include <QTransform>
#include <cmath>
#include <QtDebug>
#include <QFileDialog>
#include <QtSvg/QtSvg>
#include <QMessageBox>
#include <QPushButton>
#include <boost/concept_check.hpp>

SlamPainter::SlamPainter()
    : QGraphicsView()
{
  
  minx = 10000;
  miny = 10000;
  maxx = -10000;
  maxy = -10000;
  maxSensorVal = 5000;
  resize(800,800);
  setBackgroundBrush(Qt::white);
//   setStyleSheet("background-color:white;");
//   setBackgroundRole(QPalette::Base);
  setAttribute(Qt::WA_DeleteOnClose);
  scale = 1;
  erase = false;
  sizePoint = 5;
  zoom = 1;
  
  countColor = 0;
  colors.append(Qt::red);
  colors.append(Qt::darkYellow);
  colors.append(Qt::yellow);
  colors.append(Qt::green);
  colors.append(Qt::cyan);
  colors.append(Qt::blue);
  colors.append(Qt::magenta);
  totalColors = colors.size();
  rotation = 0;
}


SlamPainter::~SlamPainter()
{
}

void SlamPainter::setParams(double minx, double miny, double width, double height, int maxSensorVal, double robot_x, double robot_y, double robot_th)
{
  this->minx = minx;
  this->miny = miny;
  this->maxx = minx + width;
  this->maxy = miny + height;
  this->maxSensorVal = maxSensorVal;

  scale = min(720/width, 720/height); // 10% de margen
//   offsetX = round(minx) >= 0 ? -maxx/2*720/width : minx*800/width;
//   offsetX = abs(offsetX) < 10 ? -100: offsetX/2;
//   offsetY = miny*800/height;
  offsetX = robot_x;
  offsetY = robot_y;
  rotation = robot_th; // grados
  offset = QPointF(offsetX, offsetY);
  NDMath::selfRotateGrad(offset, -rotation);
//   scene.setSceneRect(QRect(offsetX, offsetY, width, height));
  QRectF rect(minx - width/2 - offsetX, -maxy - height/2 + offsetY, 2 * width, 2 * height);
  QPointF center = rect.center();
//   QPointF robot(robot_x, robot_y);
//   rect.translate(robot - center);
  NDMath::scaleRect(rect, scale);
  scene.setSceneRect(rect); 
  ensureVisible(rect);
  scene.addRect(rect);
  
  scene.addLine(-200, 0, 200, 0)->setVisible(true);
  scene.addLine(0, -200, 0, 200)->setVisible(true);  
  
  setScene(&scene);

}

void SlamPainter::paintSlam(QVector< pair<QPointF,QPointF> > endPoints, QVector<int> segSizes, int sizePoint)
{
  int sizeP = endPoints.size();
  this->sizePoint = sizePoint;
  this->segSizes += segSizes;
  double xx, yy;
  
  QPen pen;
  pen.setWidth(2); 
  QColor penColor(colors.at(countColor));
  pen.setColor(penColor);   

  if(sizeP) {  
    for(int i = 0;i < sizeP; i++) {
      NDMath::selfRotateGrad(endPoints[i].first, rotation/*, QPointF(offsetX, offsetY)*/); 
      xx = endPoints[i].first.x()*scale;
      yy = -endPoints[i].first.y()*scale;
      endPoints[i].first.setX(xx);
      endPoints[i].first.setY(yy);
      
      NDMath::selfRotateGrad(endPoints[i].second, rotation/*, QPointF(offsetX, offsetY)*/);
      xx = endPoints[i].second.x()*scale;
      yy = -endPoints[i].second.y()*scale;
      endPoints[i].second.setX(xx);
      endPoints[i].second.setY(yy);

      this->endPoints.append(pair<int, pair<QPointF,QPointF> >(countColor, endPoints[i]));
      
      if(endPoints.at(i).first == endPoints.at(i).second) { 
	if(sizePoint) {
	  QSize s(sizePoint, sizePoint);
	  scene.addEllipse(QRectF(endPoints.at(i).first, s), pen)->setVisible(true);
// 	  p.drawEllipse(endPoints.at(i).second.first, sizePoint, sizePoint);
	}
	else {
	  QSize s(2, 2);
	  scene.addEllipse(QRectF(endPoints.at(i).first, s), pen)->setVisible(true);
// 	  p.drawPoint(endPoints.at(i).second.first);
	}
      }
      else   
	scene.addLine(QLineF(endPoints.at(i).first, endPoints.at(i).second), pen)->setVisible(true);
    }    
    
    countColor++;
    countColor = countColor % totalColors;
    
//     this->endPoints += endPoints;
    scene.update();
  }
}

// void SlamPainter::paintEvent(QPaintEvent * event)
// {
//   if(erase) {
//     p.eraseRect(0, 0, width(), height());
//     erase = false;
//   }
//   else
//     paintOnDevice(this);
// }
// 
// void SlamPainter::paintOnDevice(QPaintDevice* device)
// {
//   p.begin(device);
// //   p.scale(zoom, zoom);
//   p.rotate(0);
// 
// /*  p.translate(clickPoint)*/;
//   QPen pen;
// //   QColor penColor(colors.at(countColor));
// //   pen.setColor(penColor);
//   pen.setWidth(2);  
//   p.setPen(pen);
//   int numSegments = endPoints.size();
//   
// 
//   bool first_color = true;
//   for(int i = 0; i < numSegments; i++) {
// //     if(first_color)
// //       pen.setColor(Qt::blue);
// //     else
// //       pen.setColor(Qt::magenta);
// //     p.setPen(pen);
// //     int level = 0/*255 - min(255, (int)(255 * segSizes[i]/60.0))*/;
// //     pen.setColor(QColor(level, level, level/*, level*/));
// //     p.setPen(pen);
//     
// //     QColor penColor(colors.at(endPoints.at(i).first));
// //     pen.setColor(penColor);
//     p.setPen(pen);
//     
//     if(endPoints.at(i).second.first == endPoints.at(i).second.second) { 
//       p.save();
//       if(sizePoint)
// 	p.drawEllipse(endPoints.at(i).second.first, sizePoint, sizePoint);
//       else
// 	p.drawPoint(endPoints.at(i).second.first);
//       p.restore();
//     }
//     else 
//       p.drawLine(endPoints.at(i).second.first, endPoints.at(i).second.second);
//   }
//   pen.setColor(Qt::black);
//   p.setPen(pen);  
//   p.drawLine(-100, 0, 100, 0);
//   p.drawLine(0, -100, 0, 100);
//   
//   p.end();
// }

void SlamPainter::mouseDoubleClickEvent(QMouseEvent *event)
{
//  int resp = QMessageBox::information(nullptr, QObject::tr("Slam results"), "", QObject::tr("&Save"), QObject::tr("C&lear"), QObject::tr("&Cancel"));
  QMessageBox msgBox(QMessageBox::Information, tr("Slam results"), "", QMessageBox::Save | QMessageBox::Cancel);
  QPushButton *button = msgBox.addButton(QObject::tr("C&lear"), QMessageBox::ActionRole);
  msgBox.exec();
  
//  if(resp == 1) {
  if(msgBox.clickedButton() == button) {
    endPoints.clear();
    erase = true;
    scene.clear();
    scene.addLine(-200, 0, 200, 0)->setVisible(true);
    scene.addLine(0, -200, 0, 200)->setVisible(true);
    scene.update();
    return;
  }
//  else if(resp == 2)
  else if(msgBox.clickedButton() == msgBox.button(QMessageBox::Cancel))
    return;
  
  QString filename = QFileDialog::getSaveFileName(0, tr("Export svg"), "./", tr("") + "Svg (*.svg);;" + tr("All") + "(*.*)");

  if(filename.isEmpty())
    return;

  QSvgGenerator generator;
  QString svgDescription = "";
  generator.setFileName(filename);
  generator.setSize(sceneRect().toRect().size());
  generator.setViewBox(QRect(QPoint(0, 0), sceneRect().toRect().size()));
  generator.setTitle(tr("SVG Generator Cybersim"));
  generator.setDescription(svgDescription);
  
  QPainter painter( &generator );
  scene.render( &painter );  

  double prev_zoom = zoom;
  zoom = 1;
//   paintOnDevice(&generator);
  zoom = prev_zoom;

}

void SlamPainter::setZoom(int delta)
{
  double zoom_rate = 1 + delta/120.0/25; // 120 punto de rueda en Qt
  if(zoom_rate) {
    zoom *= zoom_rate;
//     p.scale(zoom, zoom);
    QTransform m = transform();
    m.scale( zoom_rate, zoom_rate );
    m = QTransform(m.m11(), m.m12(), m.m21(), m.m22(), m.dx()*zoom_rate, m.dy()*zoom_rate);
    QPointF mapped = m.inverted().map(clickPoint);
    m.translate(mapped.x(), mapped.y());
    setTransform(m);

//     centerOn(m.map(clickPoint));
    update();
  }
}

void SlamPainter::wheelEvent ( QWheelEvent * e )
{
  clickPoint = e->position();
  setZoom(e->angleDelta().y());
//   QPoint prevCenter = clickPoint;
//   p.setTransform(p.transform().traclickPoint);
  
//   p.transform().
  e->accept();
}

void SlamPainter::setRotation(double rotation) {
  this->rotation = rotation;
  endPoints.clear();
  erase = true;
  update();
}

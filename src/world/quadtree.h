/*
    Copyright (c) 2014 <copyright holder> <email>

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef QUADTREE_H
#define QUADTREE_H

#include "quadnode.h"


#include <QtGlobal>
#include <QRect>
#include <QObject>
#include <QPair>

class QuadTree : public QObject
{
   Q_OBJECT

public:
    QuadTree(QRect worldRect, int cellSize);
    virtual ~QuadTree();

    QRect calculateGrid(uint& levels);

    void addCollisionSolver(CollisionSolver* collisionSolver);
    QList<CollisionSolver *> *getCollisionSolvers(uint row, uint col);

    QRect getWorldRect() const;
    // para gráfico
    uint getSceneScale() const;
    void setSceneScale(uint value);
    QVector<QPair<QPoint, uint> > *nodesForDrawing();


signals:
    // para gráfico
    void drawCell(QPoint , uint );

private:
    void getNode(uint &node, const uint &row, const uint &col);
     void recursiveWalk(QuadNode *currentNode, uint level, uint xPos, uint yPos);

private:
    QuadNode * rootNode;
    QVector<QPair<QPoint, uint> > treeNodes;
    QRect worldRect;
    QRect rootRect;
    uint cellSize;
    uint size;
    uint N1;
    // para gráfico
    uint sceneScale;
    
};

#endif // QUADTREE_H

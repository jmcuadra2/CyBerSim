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

#include "quadtree.h"
#include <algorithm>
#include <QtGui>


QuadTree::QuadTree(QRect worldRect, int cellSize) : QObject()
{
    N1 = 16;
    rootNode = new QuadNode();
    this->worldRect = worldRect;
    this->cellSize = cellSize;
}
QuadTree::~QuadTree()
{
    delete rootNode;
}


void QuadTree::addCollisionSolver(CollisionSolver* collisionSolver)
{
    for(int j = 0; j<collisionSolver->getLista().size(); j++) {

        uint row = (uint)(collisionSolver->getLista().value(j).x());
        uint col = (uint)(collisionSolver->getLista().value(j).y());
        QuadNode * currentNode = rootNode;
        QuadNode * childNode = 0;

        uint n_2, child_pos;

        uint xPos = 0; // para gráfico
        uint yPos = 0;

        for(int i = N1 - 1; i >= 0 ; i--) {
            n_2 = 1 << i;
            child_pos = (row & n_2 ? 1 : 0) * 2 + (col & n_2 ? 1 : 0);
            childNode = currentNode->getChild(child_pos);

            if(!childNode) {
                childNode = new QuadNode();
                currentNode->setChild(child_pos, childNode);

                // para gráfico
                QPoint center;
                center.setX(xPos + n_2);
                center.setY(yPos + n_2);
            }

            // para gráfico
            yPos += (row & n_2 ? 1 : 0)*n_2;
            xPos += (col & n_2 ? 1 : 0)*n_2;


            if(i == 0) // celdas finales
                childNode->addCollisionSolver(collisionSolver);
            else
                currentNode = childNode;
        }
    }
}

QVector<QPair<QPoint, uint> > *QuadTree::nodesForDrawing()
{
    treeNodes.clear();

    recursiveWalk(rootNode, N1, 0, 0);

    return &treeNodes;

}

void QuadTree::recursiveWalk(QuadNode * currentNode, uint level, uint xPos, uint yPos)
{
    QuadNode * childNode = 0;
    quint64 n_2 = 1 << (level - 1);

    for(int j = 0; j < 4; j++) {
        childNode = currentNode->getChild(j);

        if(childNode) {
            QPoint center;
            center.setX(xPos + n_2);
            center.setY(yPos + n_2);
            treeNodes.append(QPair<QPoint, uint>(center, n_2));

            if(level > 1)
                recursiveWalk(childNode, level - 1, xPos + (j%2)*n_2, yPos + (j/2)*n_2);
        }

    }
}

QList<CollisionSolver *>* QuadTree::getCollisionSolvers(uint row, uint col)
{
    QuadNode * currentNode = rootNode;
    QuadNode * childNode = 0;
    int i = N1 - 1;
    uint n_2, child_pos;
    while(i >= 0) {
        n_2 = 1 << i;
        child_pos = (row & n_2 ? 1 : 0) * 2 + (col & n_2 ? 1 : 0);
        childNode = currentNode->getChild(child_pos);

        if(!childNode)
            break;
        else
            currentNode = childNode;
        i--;
    }
    return currentNode->getCollisionSolvers();
}

QRect QuadTree::calculateGrid(uint &levels)
{
    int dimension = std::max(worldRect.width()/cellSize + (worldRect.width()%cellSize ? 1 : 0), worldRect.height()/cellSize + (worldRect.height()%cellSize ? 1 : 0));
    levels = 0;
    while(dimension >= 1 << levels)
        levels++;
    N1 = levels;

    size = 1 << N1;
    rootRect.setWidth(size * cellSize);
    rootRect.setHeight(size * cellSize);
    rootRect.translate(-rootRect.center() + worldRect.center());

    return rootRect;
}

void QuadTree::getNode(uint &node, const uint& row, const uint& col)
{
    uint n_2;
    uint b_row;
    uint b_col;
    node = 0;
    for(int i = 2*N1 - 1; i >= 1; i-=2) {
        n_2 = 1 << i/2;
        b_row = row & n_2 ? 1 : 0;
        b_col = col & n_2 ? 1 : 0;

        node |= b_row << i;
        node |= b_col << (i-1);
    }
}
uint QuadTree::getSceneScale() const
{
    return sceneScale;
}

void QuadTree::setSceneScale(uint value)
{
    sceneScale = value;
}

QRect QuadTree::getWorldRect() const
{
    return worldRect;
}

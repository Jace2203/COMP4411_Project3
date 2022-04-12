#ifndef AABB_H
#define AABB_H

#include "vecmath/vecmath.h"

#include "scene/ray.h"

class BoundingBox;
class Geometry;

class Node
{
public:
    Node();
    Node(Geometry* obj);

    ~Node()
    { delete m_Box; }

    bool IsLeaf() const;
    BoundingBox GetBoundingBox() const;
    Geometry* GetGeometry() const;
    double GetSurfaceArea() const;

    void SetBoundingBox(const BoundingBox& box);
private:
    BoundingBox* m_Box;

    bool m_IsLeaf;
    Geometry* m_Obj;
};

class Tree
{
public:
    Tree();

    ~Tree();

    void InsertLeaf(Geometry* obj);

    BoundingBox GetBoundingBox() const;
    double GetSurfaceArea() const;

    bool Intersect(const ray& r, isect& i) const;
private:
    void Update();

    Tree* BestSibling(double cost, double& best, Node* obj);

    Tree* m_Parent;
    Node* m_Node;

    Tree *m_Left, *m_Right;
};

double ComputeSurfaceArea(const BoundingBox& a);
BoundingBox Union(const BoundingBox& a, const BoundingBox& b);

#endif
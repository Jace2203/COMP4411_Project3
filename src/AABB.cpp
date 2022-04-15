#include "AABB.h"

#include <limits>

#include "scene/scene.h"

Node::Node()
: m_Box(nullptr), m_IsLeaf(false), m_Obj(nullptr)
{
}

Node::Node(Geometry* obj)
: m_Box(new BoundingBox(obj->getBoundingBox())), m_IsLeaf(true), m_Obj(obj)
{
}

bool Node::IsLeaf() const
{
    return m_IsLeaf;
}

BoundingBox Node::GetBoundingBox() const
{
    return *m_Box;
}

Geometry* Node::GetGeometry() const
{
    return m_Obj;
}

double Node::GetSurfaceArea() const
{
    return ComputeSurfaceArea(*m_Box);
}

void Node::SetBoundingBox(const BoundingBox& box)
{
    if (m_Box)
        delete m_Box;

    m_Box = new BoundingBox(box);
}

Tree::Tree()
: m_Parent(nullptr), m_Node(nullptr), m_Left(nullptr), m_Right(nullptr)
{
}

Tree::~Tree()
{
    delete m_Node;
    delete m_Left;
    delete m_Right;
}

void Tree::InsertLeaf(Geometry* obj)
{
    if (m_Node == nullptr)
    {
        m_Node = new Node(obj);
        return;
    }

    Node* leaf_node = new Node(obj);
    Tree* leaf = new Tree();
    leaf->m_Node = leaf_node;
    double best = std::numeric_limits<double>::max();
    Tree* target = BestSibling(0, best, leaf_node);

    Tree* old_parent = target->m_Parent;
    if (old_parent)
    {
        Tree* new_parent = new Tree();
        new_parent->m_Parent = old_parent;
        new_parent->m_Node = new Node();
        new_parent->m_Left = target;
        new_parent->m_Right = leaf;
        target->m_Parent = new_parent;
        leaf->m_Parent = new_parent;

        if (old_parent->m_Left == target)
        {
            old_parent->m_Left = new_parent;
        }
        else
        {
            old_parent->m_Right = new_parent;
        }
    }
    else
    {
        m_Left = new Tree();
        m_Left->m_Node = m_Node;
        m_Right = leaf;
        m_Left->m_Parent = this;
        m_Right->m_Parent = this;

        m_Node = new Node();
    }

    leaf->Update();
}

BoundingBox Tree::GetBoundingBox() const
{
    return m_Node->GetBoundingBox();
}

double Tree::GetSurfaceArea() const
{
    return m_Node->GetSurfaceArea();
}

bool Tree::Intersect(const ray& r, isect& i) const
{
    if (m_Node->IsLeaf())
    {
        return m_Node->GetGeometry()->intersect(r, i);
    }

    double tMin, tMax;
    if (m_Node->GetBoundingBox().intersect(r, tMin, tMax))
    {
        isect a, b;
        bool A = m_Left->Intersect(r, a);
        bool B = m_Right->Intersect(r, b);

        if (A && B)
        {
            if (a.t < b.t)
            {
                i = a;
                return true;
            }
            else
            {
                i = b;
                return true;
            }
        }
        else if (A)
        {
            i = a;
            return true;
        }
        else if (B)
        {
            i = b;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

void Tree::IntersectAll(const ray& r, std::vector<isect*>& is, double t) const
{
    isect i;
    if (Intersect(r, i))
    {
        i.t += t;
        is.push_back(new isect(i));
        vec3f p = r.getPosition();
        vec3f d = r.getDirection();

        ray rr = ray(r.at(i.t - t) + d * RAY_EPSILON, d);
        IntersectAll(rr, is, i.t);
    }
}

void Tree::Update()
{
    if (!m_Node->IsLeaf())
    {
        BoundingBox box = Union(m_Left->m_Node->GetBoundingBox(), m_Right->m_Node->GetBoundingBox());
        m_Node->SetBoundingBox(box);
    }
    else
    {
        BoundingBox box = m_Node->GetGeometry()->getBoundingBox();
        m_Node->SetBoundingBox(box);
    }

    if (m_Parent)
        m_Parent->Update();
}

Tree* Tree::BestSibling(double cost, double& best, Node* obj)
{
    if (m_Node->IsLeaf())
    {
        best = cost;
        return this;
    }

    cost += ComputeSurfaceArea(Union(m_Node->GetBoundingBox(), obj->GetBoundingBox()));
    if (cost < best)
    {
        cost -= ComputeSurfaceArea(m_Node->GetBoundingBox());

        double left_best, right_best;
        left_best = right_best = best;
        Tree* left = m_Left->BestSibling(cost, left_best, obj);
        Tree* right = m_Right->BestSibling(cost, right_best, obj);
        if (left_best < right_best)
        {
            best = left_best;
            return left;
        }
        else
        {
            best = right_best;
            return right;
        }
    }

    return nullptr;
}

double ComputeSurfaceArea(const BoundingBox& a)
{
    vec3f d = a.max - a.min;
    return 2 * (d[0] * d[1] + d[1] * d[2] + d[0] * d[2]);
}

BoundingBox Union(const BoundingBox& a, const BoundingBox& b)
{
    BoundingBox result;
    vec3f a_min = a.min;
    vec3f a_max = a.max;
    vec3f b_min = b.min;
    vec3f b_max = b.max;

    result.min = vec3f(min(a_min[0], b_min[0]), min(a_min[1], b_min[1]), min(a_min[2], b_min[2]));
    result.max = vec3f(max(a_max[0], b_max[0]), max(a_max[1], b_max[1]), max(a_max[2], b_max[2]));

    return result;
}
#include "csg.h"

#include <algorithm>

CSGTree::CSGTree()
: m_Left(nullptr), m_Right(nullptr), m_Index(-1), m_Opt(OPT::NONE)
{
}

CSGTree::CSGTree(int index)
: m_Left(nullptr), m_Right(nullptr), m_Index(index), m_Opt(OPT::NONE)
{
}

CSGTree::CSGTree(OPT opt, CSGTree* left, CSGTree* right)
: m_Left(left), m_Right(right), m_Index(-1), m_Opt(opt)
{
}

isect* CSGTree::GetIsect(const std::vector<isect*>& is, const std::vector<int> indices)
{
    std::vector<double> line;
    // for (int i = 0; i < is.size(); i++)
    // {
    //     std::cout << is[i]->t << ' ' <<  indices[i] << ", ";
    // }
    // std::cout << std::endl;
    TraverseTree(line, is, indices);

    if (line.size() > 0)
    {
        for (auto it = is.begin(); it != is.end(); it++)
            if (abs((*it)->t - line[0]) < RAY_EPSILON)
                return *it;
    }

    return nullptr;
}

void CSGTree::TraverseTree(std::vector<double>& line, const std::vector<isect*>& is, const std::vector<int> indicies)
{
    if (m_Opt == OPT::NONE)
    {
        for (int i = 0; i < is.size(); i++)
        {
            if (indicies[i] == m_Index)
                line.push_back(is[i]->t);
        }
        return;
    }
    
    std::vector<double> left, right;
    m_Left->TraverseTree(left, is, indicies);
    m_Right->TraverseTree(right, is, indicies);

    if (m_Opt == OPT::UNION)
    {
        bool leftIn = false, rightIn = false;
        auto ll = left.begin(), rr = right.begin();
        while (ll != left.end() || rr != right.end())
        {
            if (ll == left.end())
            {
                line.push_back(*rr);
                rr++;
            }
            else if (rr == right.end())
            {
                line.push_back(*ll);
                ll++;;
            }
            else if (*ll < *rr)
            {
                if (!rightIn)
                    line.push_back(*ll);
                    
                leftIn = !leftIn;
                ll++;
            }
            else
            {
                if (!leftIn)
                    line.push_back(*rr);
                    
                rightIn = !rightIn;
                rr++;
            }
        }
    }
    else if (m_Opt == OPT::INTERSECT)
    {
        if (left.size() == 0)
            return;

        if (right.size() == 0)
            return;
        
        bool leftIn = false, rightIn = false;
        auto ll = left.begin(), rr = right.begin();
        while (ll != left.end() || rr != right.end())
        {
            if (ll == left.end())
            {
                rr++;
            }
            else if (rr == right.end())
            {
                ll++;
            }
            else if (*ll < *rr)
            {
                if (leftIn ^ rightIn)
                    line.push_back(*ll);

                leftIn = !leftIn;
                ll++;
            }
            else
            {
                if (leftIn ^ rightIn)
                    line.push_back(*rr);

                rightIn = !rightIn;
                rr++;
            }
        }
    }
    else if (m_Opt == OPT::DIFF)
    {
        if (left.size() == 0)
        {
            for (auto it = right.begin(); it != right.end(); it++)
                line.push_back(*it);
            return;
        }

        if (right.size() == 0)
        {
            for (auto it = left.begin(); it != left.end(); it++)
                line.push_back(*it);
            return;
        }

        bool leftIn = false, rightIn = false;
        auto ll = left.begin(), rr = right.begin();
        while (ll != left.end() || rr != right.end())
        {
            if (ll == left.end())
            {
                line.push_back(*rr);
                rr++;
            }
            else if (rr == right.end())
            {
                line.push_back(*ll);
                ll++;
            }
            else if (*ll < *rr)
            {
                if (rightIn)
                    line.push_back(*ll);
                
                leftIn = !leftIn;
                ll++;
            }
            else
            {
                if (leftIn)
                    line.push_back(*rr);
                
                rightIn = !rightIn;
                rr++;
            }
        }
    }
    else if (m_Opt == OPT::MINUS)
    {
        if (left.size() == 0)
            return;

        if (right.size() == 0)
        {
            for (auto it = left.begin(); it != left.end(); it++)
                line.push_back(*it);
            return;
        }

        bool leftIn = false, rightIn = false;
        auto ll = left.begin(), rr = right.begin();
        while (ll != left.end())
        {
            if (rr == right.end())
            {
                line.push_back(*ll);
                ll++;
            }
            else if (*ll < *rr)
            {
                if (!rightIn)
                    line.push_back(*ll);
                
                leftIn = !leftIn;
                ll++;
            }
            else
            {
                if (leftIn)
                    line.push_back(*rr);
                
                rightIn = !rightIn;
                rr++;
            }
        }
    }
}

CSG::CSG(Scene* scene)
: SceneObject(scene), m_Mat(new Material()), aabbTree(new Tree())
{
}

CSG::~CSG()
{
    delete m_Mat;
    for (auto it = m_Objs.begin(); it != m_Objs.end(); it++)
        delete *it;

    delete aabbTree;
}

bool CSG::intersectLocal( const ray& r, isect& i) const
{
    std::vector<isect*> is;
    aabbTree->IntersectAll(r, is);

    bool result = false;
    if (is.size() > 0)
    {
        isect* cur = GetIsect(is);

        if (cur)
        {
            i = isect(*cur);
            result = true;
        }
    }
    
    for (auto it = is.begin(); it != is.end(); it++)
        delete *it;

    return result;
}

void CSG::addPrimitive(MaterialSceneObject* obj)
{
    m_Objs.push_back(obj);
    aabbTree->InsertLeaf(obj);
}

isect* CSG::GetIsect(const std::vector<isect*>& is) const
{
    std::vector<int> order;

    for (auto it = is.begin(); it != is.end(); it++)
    {
        int index = std::find(m_Objs.begin(), m_Objs.end(), (*it)->obj) - m_Objs.begin();
        order.push_back(index);
    }

    return m_Tree->GetIsect(is, order);
}
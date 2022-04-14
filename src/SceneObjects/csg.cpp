#include "csg.h"

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

void CSG::addPrimitive(MaterialSceneObject* obj)
{
    m_Objs.push_back(obj);
    aabbTree->InsertLeaf(obj);
}
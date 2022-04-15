#ifndef CSG_H
#define CSG_H

#include <vector>

#include "../scene/scene.h"
#include "../AABB.h"

enum class OPT {
    NONE = -1,
    UNION,
    INTERSECT,
    DIFF,
    MINUS,
};

class CSGTree
{
public:
    CSGTree();
    CSGTree(int index);
    CSGTree(OPT opt, CSGTree* left, CSGTree* right);

    isect* GetIsect(const std::vector<isect*>& is, const std::vector<int> indices);

private:
    void TraverseTree(std::vector<double>& line, const std::vector<isect*>& is, const std::vector<int> indicies);

    CSGTree* m_Left;
    CSGTree* m_Right;

    int m_Index;
    OPT m_Opt;
};

class CSG
    : public SceneObject
{
public:
    CSG(Scene* scene);
    ~CSG();

    const Material& getMaterial() const { return *m_Mat; }
    void setMaterial( Material *m ) {}

    void addPrimitive(MaterialSceneObject* obj);
    void addCSGTree(CSGTree* tree) { m_Tree = tree; }

	virtual bool intersectLocal( const ray& r, isect& i ) const;
	virtual bool hasBoundingBoxCapability() const { return true; }
    virtual BoundingBox ComputeLocalBoundingBox()
    {
        auto it = m_Objs.begin();
        BoundingBox box = (*it)->getBoundingBox();
        it++;

        for (it; it != m_Objs.end(); it++)
        {
            box = Union(box, (*it)->getBoundingBox());
        }

        return box;
    };

private:
    isect* GetIsect(const std::vector<isect*>& is) const;

    vector<SceneObject*> m_Objs;

    Material* m_Mat;
    CSGTree* m_Tree;

    Tree* aabbTree;
};

#endif
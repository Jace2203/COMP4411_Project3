#ifndef CSG_H
#define CSG_H

#include <vector>

#include "../scene/scene.h"

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

private:
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
private:
    vector<MaterialSceneObject*> m_Objs;
    Material* m_Mat;
    CSGTree* m_Tree;
};

#endif
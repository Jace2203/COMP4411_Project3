#ifndef CSG_H
#define CSG_H

#include <vector>

#include "../scene/scene.h"

class CSG
    : public SceneObject
{
public:
    CSG();
    ~CSG();

    const Material& getMaterial() const;
    void setMaterial( Material *m ) {}

private:
    vector<MaterialSceneObject*> m_Objs;
};

#endif
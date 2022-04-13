#include "csg.h"

CSG::~CSG()
{
    for (auto it = m_Objs.begin(); it != m_Objs.end(); it++)
        delete *it;
}
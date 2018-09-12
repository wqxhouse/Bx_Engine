#pragma once

#include "Primitive.h"
#include "../../Math/Vector3.h"

class Line : public Primitive
{
public:
    Line();
    ~Line();

    inline void SetLineData(const Math::Vector3& p1, const Math::Vector3& p2)
    {
        m_lineData[0] = p1;
        m_lineData[1] = p2;
    }

private:
    Math::Vector3 m_lineData[2];
};
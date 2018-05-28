#include "Trans.h"

void Trans::initialize()
{
    translationMatrix = glm::translate(glm::mat4(), pos);
    scaleMatrix       = glm::scale(glm::mat4(), scale);

    transMatrix = translationMatrix * scaleMatrix;

    viewMatrix = glm::lookAt(pos, pos + front, up);
    
    // TODO: Rotation matrix
}

void Trans::update()
{
    glm::mat4 translate = GetTranslateMatrix();
    glm::mat4 scale = GetScaleMatrix();

    // TODO: Rotation matrix

    if (m_transFlags.bits.transFlag == 1)
    {
        transMatrix = translate * scale;
        m_transFlags.bits.transFlag = 0;
    }
}

glm::mat4 Trans::GetTransMatrix()
{
    update();

    return transMatrix;
}

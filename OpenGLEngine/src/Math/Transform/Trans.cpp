#include "Trans.h"
#include "../Math.h"

void Trans::initialize()
{
    translationMatrix = glm::translate(glm::mat4(), pos);
    scaleMatrix       = glm::scale(glm::mat4(), scale);

    //glm::vec3 axis = glm::cross(front, glm::vec3(0.0f, 0.0f, 1.0f));
    //float cosTheta = glm::dot(front, glm::vec3(0.0f, 0.0f, 1.0f));
    //float theta = std::acos(cosTheta);    
    //rotationMatrix = glm::rotate(glm::mat4(), theta, axis);

    transMatrix = translationMatrix * rotationMatrix * scaleMatrix;

    viewMatrix = glm::lookAt(pos, pos + front, up);

    // Math::Mat4 testViewMat = Math::viewMatrix(pos, front, up);
    
    // TODO: Rotation matrix
}

void Trans::update()
{
    if (m_transFlags.flags != 0)
    {
        glm::mat4 translate = GetTranslateMatrix();
        glm::mat4 scale     = GetScaleMatrix();

        // TODO: Rotation matrix

        if (m_transFlags.bits.transFlag == 1)
        {
            transMatrix = translate * scale;
        }

        if (m_transFlags.bits.viewFlag == 1)
        {
            viewMatrix = glm::lookAt(pos, pos + front, up);
        }

        // Reset all transform matrix flags
        m_transFlags.flags = 0;
    }
}

glm::mat4 Trans::GetTransMatrix()
{
    update();

    return transMatrix;
}

/*
    The Utilities specified for OpenGL
*/

#pragma once

#include "Utility.h"

#include "TypeDef.h"
#include "GLFW/glfw3.h"

inline BOOL CheckError()
{
#if _DEBUG
    BOOL result = TRUE;

    GLenum error = glGetError();

    if (error != GL_NO_ERROR)
    {
        printf("GL Error: %u\n", error);
        result = FALSE;
    }

    assert(error == GL_NO_ERROR);

    return result;
#endif
}

#define CHECK_GL_ERROR CheckError();
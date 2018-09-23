/*
    The Utilities specified for OpenGL
*/

#pragma once

#include "Utility.h"

#include "TypeDef.h"
#include "GLFW/glfw3.h"

inline BOOL CheckError()
{
    BOOL result = TRUE;

#if _DEBUG
    GLenum error = glGetError();

    if (error != GL_NO_ERROR)
    {
        printf("GL Error: %x\n", error);
        result = FALSE;
    }

    assert(error == GL_NO_ERROR);

#endif
    return result;
}

#define CHECK_GL_ERROR CheckError();
#pragma once

#include "Context/OpenGLContext.h"

class OpenGLTemplate
{
public:
    OpenGLTemplate(
        Setting*       pSetting,
        const BOOL     defaultScene = TRUE);
    ~OpenGLTemplate();

    void run();

    void createDefaultScene();

    Setting*      m_pSetting;
    OpenGLContext m_context;
};
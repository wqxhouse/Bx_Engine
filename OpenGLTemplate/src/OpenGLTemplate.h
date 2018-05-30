#pragma once

#include "Context/OpenGLContext.h"

class OpenGLTemplate
{
public:
    OpenGLTemplate(
        const Setting& setting, 
        const BOOL     defaultScene = TRUE);
    ~OpenGLTemplate();

    void run();

    void createDefaultScene();

    Setting       m_setting;
    OpenGLContext m_context;
};
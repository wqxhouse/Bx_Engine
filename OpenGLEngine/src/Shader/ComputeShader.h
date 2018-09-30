#pragma once

#include "Shader.h"

class ComputeShader : public Shader
{
public:
    ComputeShader();
    ~ComputeShader();

    BOOL compileShaderProgram();

    void compute();

    inline void setShader(char* const computeShaderFile) { m_computeShaderFile = computeShaderFile; }
    void setTotalThreadSize(const UINT x, const UINT y, const UINT z);
    void setThreadGroupSize(const UINT x, const UINT y, const UINT z);

private:
    void calGroupNum();

    char* m_computeShaderFile;

    UINT totalInvocationSize[3]; // Total thread numbers
    UINT groupSize[3];           // Number of threads in every group
    UINT groupNum[3];            // Number of groups

    union
    {
        UINT threadGroupChangeFlags;

        struct
        {
            BOOL totalInvocationSizeChange : 1;
            BOOL groupSizeChange           : 1;
            UINT reserve                   : 30;
        }bits;
    };
};
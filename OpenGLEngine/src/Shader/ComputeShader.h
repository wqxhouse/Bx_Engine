#pragma once

#include "Shader.h"

class ComputeShader : public Shader
{
public:
    ComputeShader();
    ~ComputeShader();

    BOOL compileShaderProgram();

    void compute();

    void setTotalThreadSize(const UINT x, const UINT y, const UINT z);
    void setThreadGroupSize(const UINT x, const UINT y, const UINT z);

    inline void setShader(char* const computeShaderFile) { m_computeShaderFile = computeShaderFile; }
    inline void enableCSVariableGroupSize()              { useComputeVariableGroupSize = TRUE;      }
    inline void disableCSVariableGroupSize()             { useComputeVariableGroupSize = FALSE;     }

private:
    void calGroupNum();

    char* m_computeShaderFile;

    UINT m_totalInvocationSize[3]; // Total thread numbers
    UINT m_groupSize[3];           // Number of threads in every group
    UINT m_groupNum[3];            // Number of groups

    BOOL useComputeVariableGroupSize;

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
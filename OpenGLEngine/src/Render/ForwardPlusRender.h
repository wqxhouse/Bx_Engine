#pragma once

#include "../Shader/GraphicsShader.h"
#include "../Shader/ComputeShader.h"
#include "../Math/Vector4.h"

class Scene;

class ForwardPlusRender
{
public:
    ForwardPlusRender(Scene* pScene);
    ~ForwardPlusRender();

    BOOL initialize();
    void update();
    void draw();

private:
    BOOL initGridFrustums();
    void calGridFrustums();

    BOOL initTileLightList();
    void lightCulling();

    Scene* m_pScene;

    // Grid Frustum
    ComputeShader  m_gridFrustumComputeShader;
    GraphicsShader m_renderShader;

    GLuint m_frustumVerticesSsbo;

    Resolution m_resolution;
    UINT       m_frustumNum[2];
    UINT       m_frustumSize[2];

    struct SimpleFrustum
    {
        Plane planes[4];
    };

    std::vector<SimpleFrustum> m_frustums;

    // Light culling

    // Light grid structure design: http://www.cse.chalmers.se/~uffe/tiled_shading_preprint.pdf
    struct TiledLightList
    {
        struct LightTile
        {
            UINT offset = 0;
            UINT lightNum = 0;
        };

        void* m_pLightListData;
        std::vector<UINT> m_lightIndexList;
        std::vector<LightTile> m_lightGrid;

        GLuint m_lightIndexListSSBO;
        GLuint m_lightGridSSBO;

    } m_tiledLightList;

    union RenderFlags
    {
        UINT value;

        struct
        {
            UINT skipCalGridFrustums   : 1;
            UINT reserve               : 31;
        } bits;
    } m_renderFlags;
};
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

    inline void UpdateLightListBit() { m_renderFlags.bits.lightListUpdate = 1; }

private:
    BOOL initGridFrustums();
    void calGridFrustums();

    BOOL initTileLightList();
    void updateLightData();
    void lightCulling();

    Scene* m_pScene;

    /// Grid Frustum
    ComputeShader  m_gridFrustumComputeShader;
    GraphicsShader m_renderShader;

    GLuint m_gridFrustumSsboHandle;
    UINT m_gridFrustumBindingPoint;

    Resolution m_resolution;
    UINT       m_frustumNum[2];
    UINT       m_frustumSize[2];

    UINT globalSizeUboIndex;

    struct SimpleFrustum
    {
        Plane planes[4];
    };

    std::vector<SimpleFrustum> m_frustums;

    /// Light culling
    ComputeShader m_lightCullingComputeShader;

    // Light grid structure design: http://www.cse.chalmers.se/~uffe/tiled_shading_preprint.pdf
    struct TiledLightList
    {
        struct LightTile
        {
            UINT offset = 0;
            UINT size   = 0;
        };

        void* m_pLightListData;
        UINT  m_lightNum;

        std::vector<UINT>      m_lightIndexList;
        std::vector<LightTile> m_lightGrid;

        UINT m_lightIndexListBindingPoint;
        UINT m_lightGridBindingPoint;

        GLuint m_lightGridCounterHandle; // Automic counter for light grid
    } m_tiledLightList;

    union RenderFlags
    {
        UINT value;

        struct
        {
            BOOL skipCalGridFrustums     : 1;
            BOOL skipgenerateFrustumSsbo : 1;
            BOOL lightListUpdate         : 1;
            UINT reserve                 : 29;
        } bits;
    } m_renderFlags;
};
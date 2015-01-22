
#include "common/app.hh"

#include <memory>

#define GLM_FORCE_PURE
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class CubeApplication : public Application
{
public:

    struct CommonVertex
    {
        enum { MaxBones = 4 };
        float position[3];
        float texcoord0[2];
        float texcoord1[2];
        float normal[3];

        // not used
        uint8_t boneIDs[MaxBones];
        float   boneWeights[MaxBones];
        uint8_t color[4];
    };

    struct ConstantBuffer
    {
        float mvp[16];
    };

    sgfx::VertexShaderHandle  vsHandle;
    sgfx::PixelShaderHandle   psHandle;
    sgfx::SurfaceShaderHandle ssHandle;

    sgfx::BufferHandle         cubeVertexBuffer;
    sgfx::BufferHandle         cubeIndexBuffer;
    sgfx::ConstantBufferHandle constantBuffer;
    sgfx::VertexFormatHandle   vertexFormat;

    sgfx::PipelineStateHandle pipelineState;
    sgfx::DrawQueueHandle     drawQueue;

public:

    void* operator new(size_t size)
    {
        return _mm_malloc(size, 32);
    }

    void operator delete(void* ptr)
    {
        _mm_free(ptr);
    }

    virtual void loadSampleData() override
    {
        // setup Sigrlinn
        sgfx::initD3D11(g_pd3dDevice, g_pImmediateContext, g_pSwapChain);

        // constant buffer
        constantBuffer = sgfx::createConstantBuffer(nullptr, sizeof(ConstantBuffer));

        // mesh data
        const size_t stride1 = 0;                             // Position
        const size_t stride2 = stride1 + 3 * sizeof(float);   // uv0
        const size_t stride3 = stride2 + 2 * sizeof(float);   // uv1
        const size_t stride4 = stride3 + 2 * sizeof(float);   // normal
        const size_t stride5 = stride4 + 3 * sizeof(float);   // boneIDs
        const size_t stride6 = stride5 + 4 * sizeof(uint8_t); // boneWeights
        const size_t stride7 = stride6 + 4 * sizeof(float);   // color
        sgfx::VertexElementDescriptor vfElements[] = {
            { "POSITION",    0, sgfx::DataFormat::RGB32F,  0, stride1, sgfx::VertexElementType::PerVertex },
            { "TEXCOORDA",   0, sgfx::DataFormat::RG32F,   0, stride2, sgfx::VertexElementType::PerVertex },
            { "TEXCOORDB",   0, sgfx::DataFormat::RG32F,   0, stride3, sgfx::VertexElementType::PerVertex },
            { "NORMAL",      0, sgfx::DataFormat::RGB32F,  0, stride4, sgfx::VertexElementType::PerVertex },
            { "BONEIDS",     0, sgfx::DataFormat::R32U,    0, stride5, sgfx::VertexElementType::PerVertex },
            { "BONEWEIGHTS", 0, sgfx::DataFormat::RGBA32F, 0, stride6, sgfx::VertexElementType::PerVertex },
            { "VCOLOR",      0, sgfx::DataFormat::R32U,    0, stride7, sgfx::VertexElementType::PerVertex }
        };
        size_t vfSize = sizeof(vfElements) / sizeof(sgfx::VertexElementDescriptor);

        vertexFormat = loadVF(vfElements, vfSize, "shaders/sample0.hlsl");

        CommonVertex cubeVertices[] = {
            {
                { 1.000000, 1.000000, -1.000000 }, // Vertex
                { 0.000000, 0.000000 }, // Texcoord 0
                { 0, 0 }, // Texcoord 1
                { 0.000000, 1.000000, 0.000000 }  // Normal
            },
            {
                {-1.000000, 1.000000, -1.000000 }, // Vertex
                { 0.000000, 1.000000 }, // Texcoord 0
                { 0, 0 }, // Texcoord 1
                { 0.000000, 1.000000, 0.000000 }  // Normal
            },
            {
                { -1.000000, 1.000000, 1.000000 }, // Vertex
                { 1.000000, 1.000000 }, // Texcoord 0
                { 0, 0 }, // Texcoord 1
                { 0.000000, 1.000000, 0.000000 }  // Normal
            },
            {
                { 1.000000, 1.000000, 1.000000 }, // Vertex
                { 1.000000, 0.000000 }, // Texcoord 0
                { 0, 0 }, // Texcoord 1
                { 0.000000, 1.000000, 0.000000 }  // Normal
            },
            {
                { 1.000000, -1.000000, 1.000000 }, // Vertex
                { 0.000000, 0.000000 }, // Texcoord 0
                { 0, 0 }, // Texcoord 1
                { 0.000000, -1.000000, 0.000000 }  // Normal
            },
            {
                { -1.000000, -1.000000, 1.000000 }, // Vertex
                { 0.000000, 1.000000 }, // Texcoord 0
                { 0, 0 }, // Texcoord 1
                { 0.000000, -1.000000, 0.000000 }  // Normal
            },
            {
                { -1.000000, -1.000000, -1.000000 }, // Vertex
                { 1.000000, 1.000000 }, // Texcoord 0
                { 0, 0 }, // Texcoord 1
                { 0.000000, -1.000000, 0.000000 }  // Normal
            },
            {
                { 1.000000, -1.000000, -1.000000 }, // Vertex
                { 1.000000, 0.000000 }, // Texcoord 0
                { 0, 0 }, // Texcoord 1
                { 0.000000, -1.000000, 0.000000 }  // Normal
            },
            {
                { 1.000000, 1.000000, 1.000000 }, // Vertex
                { 0.000000, 0.000000 }, // Texcoord 0
                { 0, 0 }, // Texcoord 1
                { 0.000000, 0.000000, 1.000000 }  // Normal
            },
            {
                { -1.000000, 1.000000, 1.000000 }, // Vertex
                { 0.000000, 1.000000 }, // Texcoord 0
                { 0, 0 }, // Texcoord 1
                { 0.000000, 0.000000, 1.000000 }  // Normal
            },
            {
                { -1.000000, -1.000000, 1.000000 }, // Vertex
                { 1.000000, 1.000000 }, // Texcoord 0
                { 0, 0 }, // Texcoord 1
                { 0.000000, 0.000000, 1.000000 }  // Normal
            },
            {
                { 1.000000, -1.000000, 1.000000 }, // Vertex
                { 1.000000, 0.000000 }, // Texcoord 0
                { 0, 0 }, // Texcoord 1
                { 0.000000, 0.000000, 1.000000 }  // Normal
            },
            {
                { 1.000000, -1.000000, -1.000000 }, // Vertex
                { 0.000000, 0.000000 }, // Texcoord 0
                { 0, 0 }, // Texcoord 1
                { 0.000000, 0.000000, -1.000000 }  // Normal
            },
            {
                { -1.000000, -1.000000, -1.000000 }, // Vertex
                { 0.000000, 1.000000 }, // Texcoord 0
                { 0, 0 }, // Texcoord 1
                { 0.000000, 0.000000, -1.000000 }  // Normal
            },
            {
                { -1.000000, 1.000000, -1.000000 }, // Vertex
                { 1.000000, 1.000000 }, // Texcoord 0
                { 0, 0 }, // Texcoord 1
                { 0.000000, 0.000000, -1.000000 }  // Normal
            },
            {
                { 1.000000, 1.000000, -1.000000 }, // Vertex
                { 1.000000, 0.000000 }, // Texcoord 0
                { 0, 0 }, // Texcoord 1
                { 0.000000, 0.000000, -1.000000 }  // Normal
            },
            {
                { -1.000000, 1.000000, 1.000000 }, // Vertex
                { 0.000000, 0.000000 }, // Texcoord 0
                { 0, 0 }, // Texcoord 1
                { -1.000000, 0.000000, 0.000000 }  // Normal
            },
            {
                { -1.000000, 1.000000, -1.000000 }, // Vertex
                { 0.000000, 1.000000 }, // Texcoord 0
                { 0, 0 }, // Texcoord 1
                { -1.000000, 0.000000, 0.000000 }  // Normal
            },
            {
                { -1.000000, -1.000000, -1.000000 }, // Vertex
                { 1.000000, 1.000000 }, // Texcoord 0
                { 0, 0 }, // Texcoord 1
                { -1.000000, 0.000000, 0.000000 }  // Normal
            },
            {
                { -1.000000, -1.000000, 1.000000 }, // Vertex
                { 1.000000, 0.000000 }, // Texcoord 0
                { 0, 0 }, // Texcoord 1
                { -1.000000, 0.000000, 0.000000 }  // Normal
            },
            {
                { 1.000000, 1.000000, -1.000000 }, // Vertex
                { 0.000000, 0.000000 }, // Texcoord 0
                { 0, 0 }, // Texcoord 1
                { 1.000000, 0.000000, 0.000000 }  // Normal
            },
            {
                { 1.000000, 1.000000, 1.000000 }, // Vertex
                { 0.000000, 1.000000 }, // Texcoord 0
                { 0, 0 }, // Texcoord 1
                { 1.000000, 0.000000, 0.000000 }  // Normal
            },
            {
                { 1.000000, -1.000000, 1.000000 }, // Vertex
                { 1.000000, 1.000000 }, // Texcoord 0
                { 0, 0 }, // Texcoord 1
                { 1.000000, 0.000000, 0.000000 }  // Normal
            },
            {
                { 1.000000, -1.000000, -1.000000 }, // Vertex
                { 1.000000, 0.000000 }, // Texcoord 0
                { 0, 0 }, // Texcoord 1
                { 1.000000, 0.000000, 0.000000 }  // Normal
            }
        };
        size_t verticesSize = sizeof(cubeVertices) / sizeof(CommonVertex);

        static uint32_t cubeIndices[] = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11, 8, 12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20 };
        size_t indicesSize = sizeof(cubeIndices) / sizeof(uint32_t);

        cubeVertexBuffer = sgfx::createBuffer(sgfx::BufferFlags::VertexBuffer, cubeVertices, sizeof(CommonVertex) * verticesSize, sizeof(CommonVertex));
        cubeIndexBuffer  = sgfx::createBuffer(sgfx::BufferFlags::IndexBuffer, cubeIndices, sizeof(uint32_t) * indicesSize, sizeof(uint32_t));

        vsHandle = loadVS("shaders/sample0.hlsl");
        psHandle = loadPS("shaders/sample0.hlsl");

        if (vsHandle != sgfx::VertexShaderHandle::invalidHandle() && psHandle != sgfx::PixelShaderHandle::invalidHandle()) {
            ssHandle = sgfx::linkSurfaceShader(
                vsHandle,
                sgfx::HullShaderHandle::invalidHandle(),
                sgfx::DomainShaderHandle::invalidHandle(),
                sgfx::GeometryShaderHandle::invalidHandle(),
                psHandle
            );
        }

        if (ssHandle != sgfx::SurfaceShaderHandle::invalidHandle()) {
            sgfx::PipelineStateDescriptor desc;

            desc.rasterizerState.fillMode                           = sgfx::FillMode::Solid;
            desc.rasterizerState.cullMode                           = sgfx::CullMode::Back;
            desc.rasterizerState.counterDirection                   = sgfx::CounterDirection::CW;

            desc.blendState.blendDesc.blendEnabled                  = false;
            desc.blendState.blendDesc.writeMask                     = sgfx::ColorWriteMask::All; // not implemented
            desc.blendState.blendDesc.srcBlend                      = sgfx::BlendFactor::One;
            desc.blendState.blendDesc.dstBlend                      = sgfx::BlendFactor::Zero;
            desc.blendState.blendDesc.blendOp                       = sgfx::BlendOp::Add;
            desc.blendState.blendDesc.srcBlendAlpha                 = sgfx::BlendFactor::One;
            desc.blendState.blendDesc.dstBlendAlpha                 = sgfx::BlendFactor::Zero;
            desc.blendState.blendDesc.blendOpAlpha                  = sgfx::BlendOp::Add;

            desc.depthStencilState.depthEnabled                     = true;
            desc.depthStencilState.writeMask                        = sgfx::DepthWriteMask::All;
            desc.depthStencilState.depthFunc                        = sgfx::DepthFunc::Less;

            desc.depthStencilState.stencilEnabled                   = false;
            desc.depthStencilState.stencilRef                       = 0;
            desc.depthStencilState.stencilReadMask                  = 0;
            desc.depthStencilState.stencilWriteMask                 = 0;

            desc.depthStencilState.frontFaceStencilDesc.stencilFunc = sgfx::StencilFunc::Always;
            desc.depthStencilState.frontFaceStencilDesc.failOp      = sgfx::StencilOp::Keep;
            desc.depthStencilState.frontFaceStencilDesc.depthFailOp = sgfx::StencilOp::Keep;
            desc.depthStencilState.frontFaceStencilDesc.passOp      = sgfx::StencilOp::Keep;

            desc.depthStencilState.backFaceStencilDesc.stencilFunc  = sgfx::StencilFunc::Always;
            desc.depthStencilState.backFaceStencilDesc.failOp       = sgfx::StencilOp::Keep;
            desc.depthStencilState.backFaceStencilDesc.depthFailOp  = sgfx::StencilOp::Keep;
            desc.depthStencilState.backFaceStencilDesc.passOp       = sgfx::StencilOp::Keep;

            desc.shader       = ssHandle;
            desc.vertexFormat = vertexFormat;

            pipelineState = sgfx::createPipelineState(desc);
            if (pipelineState != sgfx::PipelineStateHandle::invalidHandle()) {
                drawQueue = sgfx::createDrawQueue(pipelineState);
            } else {
                OutputDebugString("Failed to create pipeline state!");
            }
        }
    }

    virtual void releaseSampleData() override
    {
        OutputDebugString("Cleanup\n");
        sgfx::releaseBuffer(cubeVertexBuffer);
        sgfx::releaseBuffer(cubeIndexBuffer);
        sgfx::releaseConstantBuffer(constantBuffer);
        sgfx::releaseVertexFormat(vertexFormat);
        sgfx::releaseVertexShader(vsHandle);
        sgfx::releasePixelShader(psHandle);
        sgfx::releaseSurfaceShader(ssHandle);
        sgfx::releasePipelineState(pipelineState);
        sgfx::releaseDrawQueue(drawQueue);

        sgfx::shutdown();
    }

    virtual void renderSample() override
    {
        // Update our time
        static float t = 0.0f;

        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount();
        if (dwTimeStart == 0)
            dwTimeStart = dwTimeCur;
        t = (dwTimeCur - dwTimeStart) / 1000.0f;

        glm::mat4 projection = glm::perspective(glm::pi<float>() / 2.0F, width / (FLOAT)height, 0.01f, 100.0f);
        glm::mat4 view       = glm::lookAt(glm::vec3(0.0F, 1.0F, -5.0F), glm::vec3(0.0F, 1.0F, 0.0F), glm::vec3(0.0F, 1.0F, 0.0F));
        glm::mat4 world      = glm::rotate(glm::mat4(1.0F), t, glm::vec3(0.0F, 1.0F, 0.0F));

        glm::mat4 mvp = projection * view * world;

        ConstantBuffer constants;
        std::memcpy(constants.mvp, glm::value_ptr(mvp), sizeof(constants.mvp));
        sgfx::updateConstantBuffer(constantBuffer, &constants);

        // actually draw some stuff
        sgfx::setPrimitiveTopology(drawQueue, sgfx::PrimitiveTopology::TriangleList);
        sgfx::setConstantBuffer(drawQueue, 0, constantBuffer);
        sgfx::setVertexBuffer(drawQueue, cubeVertexBuffer);
        sgfx::setIndexBuffer(drawQueue, cubeIndexBuffer);
        sgfx::drawIndexed(drawQueue, 36, 0, 0);

        sgfx::submit(drawQueue);
    }
};

void sampleApplicationMain()
{
    ApplicationInstance = new CubeApplication;
}

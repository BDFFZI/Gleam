#pragma once
#include "LightGL/Runtime/Resource/GLBuffer.h"
#include "LightGL/Runtime/Resource/GLImageSampler.h"
#include "LightGL/Runtime/Resource/GLImageView.h"

struct MeshData
{
    GLBuffer* glVertexBuffer;
    GLBuffer* glIndexBuffer;
};

struct TextureData
{
    GLImageView* glImageView;
    GLImageSampler* glImageSampler;
};

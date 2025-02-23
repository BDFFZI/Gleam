#pragma once
#include "GleamGraphics/Runtime/Resource/GMesh.h"
#include "GleamRendering/Runtime/RenderingConfig.h"

namespace Gleam
{
    class Mesh : public GMeshT<Vertex>
    {
    public:
        explicit Mesh(bool readwrite = false);
        explicit Mesh(const RawMesh& rawMesh, bool readwrite = false);

        void GetPositions(std::vector<float3>& buffer) const;
        void GetColors(std::vector<float4>& buffer) const;
        void GetNormals(std::vector<float3>& buffer) const;
        void GetTangents(std::vector<float3>& buffer) const;
        void GetUVs(std::vector<float2>& buffer) const;

        void SetPositions(const std::vector<float3>& data);
        void SetColors(const std::vector<float4>& data);
        void SetNormals(const std::vector<float3>& data);
        void SetTangents(const std::vector<float3>& data);
        void SetUVs(const std::vector<float2>& data);
    };
}
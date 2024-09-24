#pragma once

#include <memory>

#include "LightGL/Runtime/Pipeline/GLRenderPass.h"

class RenderPass
{
public:
    RenderPass(
        const std::vector<GLAttachmentInfo>& glAttachmentInfos,
        const std::vector<GLSubpass>& glSubpasses,
        const std::vector<VkSubpassDependency>& glSubpassDependencies
    )
    {
        glRenderPass = std::make_unique<GLRenderPass>(glAttachmentInfos, glSubpasses, glSubpassDependencies);
        passCount = static_cast<int>(glSubpasses.size());
    }
    RenderPass(const RenderPass&) = delete;

    int GetPassCount() const { return passCount; }
    const GLRenderPass& GetGLRenderPass() const { return *glRenderPass; }

private:
    std::unique_ptr<GLRenderPass> glRenderPass;
    int passCount;
};

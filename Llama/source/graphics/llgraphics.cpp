#include "llcore.h"
#include "graphics/llgraphics.h"

#include "llvulkangraphics.h"

llama::GraphicsDevice llama::createGraphicsDevice()
{
    return std::make_shared<GraphicsDevice_IVulkan>();
}

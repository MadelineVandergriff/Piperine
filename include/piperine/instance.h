//
// Created by jack on 8/13/21.
//

#ifndef PIPERINE_INSTANCE_H
#define PIPERINE_INSTANCE_H

#include "vulkan_types.h"
#include "interface.h"

namespace pip::detail {
    InstanceRef createInstance(const WindowCreateInfo& windowInfo, const ExtensionLayerInfo& layerInfo);
}

#endif //PIPERINE_INSTANCE_H

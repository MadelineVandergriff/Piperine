//
// Created by jack on 7/28/21.
//

#ifndef PIPERINE_DEFINITIONS_H
#define PIPERINE_DEFINITIONS_H

#include "common.h"

#include <cstdint>
#include <memory>

typedef uint32_t PIPERINE_ENUM;
typedef char PIPERINE_LONG_STRING[64];
typedef char PIPERINE_MEDIUM_STRING[32];
typedef char PIPERINE_SHORT_STRING[16];

namespace pip {
    namespace detail {
        using ErasedRef = std::shared_ptr<void>;
    }
}

#endif //PIPERINE_DEFINITIONS_H

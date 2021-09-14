//
// Created by jack on 8/12/21.
//

#include "piperine/public_utility.h"

using namespace pip;
using namespace pip::detail;

void detail::null_deleter(void*) {
    // It is null after all
}

erased_ptr detail::getEmptyErasedPtr() {
    return {nullptr, null_deleter};
}

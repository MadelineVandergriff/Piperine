//
// Created by jack on 6/21/21.
//

#include "piperine/utility.h"

#include <fstream>

using namespace pip;

void pip::log(const char *msg, LogType type, LogSeverity severity) {
#ifdef PIPERINE_DEBUG
    auto* output = &std::cerr;
    if (severity == LogSeverity::LOG) {
        output = &std::cout;
    }

    switch (type) {
        case LogType::CREATION:
            *output << "[ooo] " << msg << '\n';
            break;
        case LogType::DESTRUCTION:
            *output << "[xxx] " << msg << '\n';
            break;
        case LogType::VALIDATION:
            *output << "[KHR] " << msg << '\n';
            break;
        case LogType::BASIC:
            *output << "[---] " << msg << '\n';
            break;
    }
#endif
}

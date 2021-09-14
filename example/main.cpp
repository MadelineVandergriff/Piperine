//
// Created by jack on 7/28/21.
//

#include <piperine/interface_builder.h>
#include <piperine/public_utility.h>

#include <cassert>
#include <cstring>

using namespace pip;

int main() {
    auto interface = BasicBuilder()
            .addPipeline<ShortString{"Main"}>()
                .setProperty<ShaderCreateInfo{.type = ShaderTypes::VERTEX, .id = "main.vert"}>()
                .setProperty<ShaderCreateInfo{.type = ShaderTypes::FRAGMENT, .id = "main.frag"}>()
            .build();
    sleep(5);

    detail::pack_array<int, 3, std::array{1, 2, 5}> pack{};
}
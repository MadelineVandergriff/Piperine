//
// Created by jack on 7/7/21.
//

#ifndef PIPERINE_INPUT_GENERATOR_H
#define PIPERINE_INPUT_GENERATOR_H

namespace pip {
    namespace detail {
        template<typename... Members> struct flat_tuple;
    }

    template<typename Main, typename... Members>
    class TupleUnion {
    public:
    private:
        union {Main main, detail::flat_tuple<Members...> members} data;
    };
}

#endif //PIPERINE_INPUT_GENERATOR_H

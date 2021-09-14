//
// Created by jack on 7/15/21.
//

#ifndef PIPERINE_MACROS_H
#define PIPERINE_MACROS_H

#define CAT(a, ...) PRIMITIVE_CAT(a, __VA_ARGS__)
#define PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__
#define PRIMITIVE_CAT_ONE(a, b, ...) a ## b

#define INC_HX_DIGIT(h) PRIMITIVE_CAT(INC_H_, h)
#define INC_H_0 1
#define INC_H_1 2
#define INC_H_2 3
#define INC_H_3 4
#define INC_H_4 5
#define INC_H_5 6
#define INC_H_6 7
#define INC_H_7 8
#define INC_H_8 9
#define INC_H_9 a
#define INC_H_a b
#define INC_H_b c
#define INC_H_c d
#define INC_H_d e
#define INC_H_e f
#define INC_H_f 0

#define EMPTY()
#define DEFER(x) x EMPTY()
#define EXPAND(...) __VA_ARGS__
#define EXPAND_FIRST(e, ...) EXPAND e
#define EXPAND_SECOND(x, e, ...) EXPAND e

#define CHECK_N(x, n, ...) n
#define CHECK(...) CHECK_N(__VA_ARGS__, 0,)
#define PROBE(x) x, 1,

#define COMPL(b) PRIMITIVE_CAT(COMPL_, b)
#define COMPL_0 1
#define COMPL_1 0

#define IS_EMPTY(...) CHECK(PRIMITIVE_CAT_ONE(IS_EMPTY_PROBE, __VA_ARGS__))
#define IS_EMPTY_PROBE PROBE(~)
#define NOT(x) CHECK(PRIMITIVE_CAT(NOT_, x))
#define NOT_0 PROBE(~)
#define BOOL(x) COMPL(NOT(x))

#define HAS_PARENS(...) CHECK(PRIMITIVE_CAT_ONE(HAS_PARENS_PROBE, __VA_ARGS__))
#define HAS_PARENS_PROBE(...) PROBE(~)

#define IIF(c) PRIMITIVE_CAT(IIF_, c)
#define IIF_0(t, ...) __VA_ARGS__
#define IIF_1(t, ...) t
#define IF(c) IIF(BOOL(c))
#define EAT(...)
#define WHEN(c) IF(c)(EXPAND, EAT)
#define IF_THEN(c, e) WHEN(c) e
#define TERNARY(c, e1, e2) IF(c)(EXPAND_FIRST, EXPAND_SECOND)(e1, e2)

#define EVAL(...)  EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL1(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL2(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL3(...) EVAL4(EVAL4(EVAL4(__VA_ARGS__)))
#define EVAL4(...) EVAL5(EVAL5(EVAL5(__VA_ARGS__)))
#define EVAL5(...) __VA_ARGS__

#define FLIP_ID() FLIP_IMPL
#define FLIP_IMPL(x, ...) IF_THEN(NOT(IS_EMPTY(__VA_ARGS__)), (DEFER(FLIP_ID)()(__VA_ARGS__),)) x
#define FLIP(...) EVAL(FLIP_IMPL(__VA_ARGS__))

#define INC(x) x + 1
#define COUNT_ID() COUNT_IMPL
#define COUNT_IMPL(x, f, ...) TERNARY(IS_EMPTY(__VA_ARGS__), (x), (DEFER(COUNT_ID)()(INC(x), __VA_ARGS__)))
#define COUNT(...) EVAL(COUNT_IMPL(1, __VA_ARGS__))

#define STRIP_PARENS(...) EXPAND(TERNARY(HAS_PARENS(__VA_ARGS__), (EXPAND __VA_ARGS__), (__VA_ARGS__)))
#define IS_NULL(x) CHECK(PRIMITIVE_CAT_ONE(NULL_PROBE_, x))
#define NULL_PROBE_n PROBE(~)

#define CALL(f, p) EXPAND f p
#define CALL_N_DEEP(f, p, n)
#define COMPOSE_ID() COMPOSE_IMPL
#define COMPOSE_IMPL(f, ...) TERNARY()
#define COMPOSE(...)

#define FOR_ID() FOR_IMPL
#define FOR_IMPL(x, c, i, b, e) TERNARY(CALL(c, x), (DEFER(FOR_ID)()(CALL(i, x), c, i, CALL(e, b), b)), b)
#define FOR(x, c, i, b, e) EVAL(FOR_IMPL(x, c, i, b, e))

#define WHILE_ID() WHILE_IMPL
#define WHILE_IMPL(x, c, e) TERNARY(CALL(c, x), (DEFER(WHILE_ID)()(CALL(e, x), c, e)), x)
#define WHILE(x, c, e) EVAL(WHILE_IMPL(x, c, e))

#define STRIP_FIRST(x, ...) __VA_ARGS__
#define GET_FIRST(x, ...) x
#define ROLL_LEFT(x, ...) __VA_ARGS__, x
#define WRAP_ROLL_LEFT(x) (ROLL_LEFT x)
#define APPEND(l, x) EXPAND l, x

#define NULL_TERMINATE(l) (APPEND(l, n))
#define IS_NULL_TERMINATED(l) IS_NULL(GET_FIRST(FLIP(EXPAND l)))
#define NOT_NULL_TERMINATED(l) NOT(IS_NULL_TERMINATED(l))
#define UNROLL_NT(h) WHILE((h), (NOT_NULL_TERMINATED), (WRAP_ROLL_LEFT))

WRAP_ROLL_LEFT(WRAP_ROLL_LEFT(WRAP_ROLL_LEFT((1, 2, 3, 4, 5))))

//UNROLL_NT((1, 2, n, 5, 7))

// body is (input, output)
#define DECODE_HEX_ID() DECODE_HEX_IMPL
#define DECODE_HEX_FUNC(h) ((CALL((STRIP_FIRST), (EXPAND_FIRST h))), (CAT(EXPAND_SECOND h, CALL((GET_FIRST), (EXPAND_FIRST h)))))
#define DECODE_HEX_IMPL(h) TERNARY(CALL((IS_EMPTY), (EXPAND_FIRST h)), (EXPAND_SECOND h), (DEFER(DECODE_HEX_ID)()(DECODE_HEX_FUNC(h))))
#define HEX_TEST(h) TERNARY(CALL((IS_EMPTY), (EXPAND_FIRST h)), 4, (5))
#define DECODE_HEX(h) EVAL(DECODE_HEX_IMPL(((FLIP(EXPAND h)), (0x)))

DECODE_HEX_IMPL(((1, a, c, 5), (0x)))
DECODE_HEX_FUNC(((1, a, c, 5), (0x)))
HEX_TEST(((1, a, c, 5), (0x)))
DECODE_HEX((1, a, c, 5))


COUNT(1, 2, 3, 4, 5)
FOR((1, 2, 3, 4, 5), (IS_EMPTY), (STRIP_FIRST), (1), (INC))
CALL((INC), (1))

#define SHIFT_ONE(x) x << 1

#define PIPERINE_GENERATE_FLAGS_ENUM(name, ...) \
namespace name {                                \
    enum {                                      \
    };                                          \
}

#undef SHIFT_ONE

#endif //PIPERINE_MACROS_H

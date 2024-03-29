#ifndef INCLUDE_CUTE_H
#define INCLUDE_CUTE_H

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __GNUC__
#    define CUTE_GNU_ATTR(attr) __attribute__(attr)
#else
#    define CUTE_GNU_ATTR(attr)
#endif // __GNUC__

#ifndef CUTE_NO_ALIASES
#    define TEST(tname) CUTE_TEST(tname)
#    define BEFORE_EACH(setup) CUTE_BEFORE_EACH(setup)
#    define AFTER_EACH(teardown) CUTE_AFTER_EACH(teardown)
#    define SUBTEST(...) CUTE_SUBTEST(__VA_ARGS__)
#    define EXPECT(actual, ...) CUTE_EXPECT(actual, __VA_ARGS__)
#    define ASSERT(actual, ...) CUTE_ASSERT(actual, __VA_ARGS__)
#    define FAIL(...) CUTE_FAIL(__VA_ARGS__)
#    define RUN_ALL() CUTE_RUN_ALL()
#endif // CUTE_NO_ALIASES

#ifndef CUTE_VERBOSE
#    define CUTE_VERBOSE 0
#endif // CUTE_VERBOSE

typedef struct cute_test_def_t cute_test_def_t;
typedef struct cute_test_list_t cute_test_list_t;
typedef struct cute_result_t cute_result_t;
typedef struct cute_tester_t cute_tester_t;
typedef struct cute_str_span_t cute_str_span_t;

typedef void cute_test_func_t(cute_tester_t *);
typedef void cute_setup_func_0_t(void);
typedef void cute_setup_func_1_t(const char *tname);
typedef void cute_teardown_func_0_t(void);
typedef void cute_teardown_func_1_t(const char *tname);

struct cute_test_def_t {
    cute_test_def_t *next;
    const char *testname;
    const char *file;
    unsigned long long line;
    cute_test_func_t *run;
};

struct cute_test_list_t {
    cute_test_def_t *head, **tail;

    struct {
        bool enabled;
        unsigned char argc;
        union {
            cute_setup_func_0_t *_0;
            cute_setup_func_1_t *_1;
        } func;
    } setup;

    struct {
        bool enabled;
        unsigned char argc;
        union {
            cute_teardown_func_0_t *_0;
            cute_teardown_func_1_t *_1;
        } func;
    } teardown;
};

typedef enum cute_result_type_t {
    cute_result_type_subtest,
    cute_result_type_assert,
} cute_result_type_t;

struct cute_result_t {
    cute_result_type_t type;
    cute_result_t *parent;
    cute_result_t *next;
    const char *file;
    unsigned long long line;
    char *message;

    union {
        struct {
            cute_result_t *head;
            clock_t start_time, end_time;
            int total, ok;
        } subtest;

        struct {
            const char *expr;
            bool ok;
        } assert;
    } _;
};

struct cute_tester_t {
    cute_result_t *current;
    cute_result_t **tail;
};

struct cute_str_span_t {
    const char *ptr;
    size_t len;
};

extern cute_test_list_t cute_test_list;

static inline void cute_register(cute_test_def_t *test);
static inline void cute_before_each_0(cute_setup_func_0_t *setup);
static inline void cute_before_each_1(cute_setup_func_1_t *setup);
static inline void cute_after_each_0(cute_teardown_func_0_t *teardown);
static inline void cute_after_each_1(cute_teardown_func_1_t *teardown);

static inline cute_result_t *cute_result_new_subtest(cute_result_t *parent, const char *file, unsigned long long line, const char *format, ...) CUTE_GNU_ATTR((format(printf, 4, 5)));
static inline cute_result_t *cute_result_new_subtest_v(cute_result_t *parent, const char *file, unsigned long long line, const char *format, va_list args);
static inline cute_result_t *cute_result_new_assert_v(cute_result_t *parent, const char *file, unsigned long long line, const char *expr, bool ok, const char *format, va_list args);
static inline void cute_result_delete(cute_result_t *r);
static inline void cute_result_subtest_start(cute_result_t *r);
static inline void cute_result_subtest_finish(cute_result_t *r);
static inline double cute_result_subtest_duration(const cute_result_t *r);
static inline int cute_result_ok(const cute_result_t *r);
static inline void cute_result_dump_i(const cute_result_t *r, FILE *out, unsigned int depth);
static inline void cute_result_dump(const cute_result_t *r, FILE *out);

static inline void cute_tester_init(cute_tester_t *t, cute_result_t *subtest);
static inline cute_tester_t *cute_tester_subtest_start(cute_tester_t *t, cute_tester_t *subtester, const char *file, unsigned long long line, const char *format, ...)
    CUTE_GNU_ATTR((format(printf, 5, 6)));
static inline void cute_tester_subtest_finish(cute_tester_t *subtester);
static inline void cute_tester_push_result(cute_tester_t *t, cute_result_t *result);
static inline void cute_tester_expect(cute_tester_t *t, const char *file, unsigned long long line, const char *expr, bool ok, const char *format, ...) CUTE_GNU_ATTR((format(printf, 6, 7)));
static inline bool cute_tester_assert(cute_tester_t *t, const char *file, unsigned long long line, const char *expr, bool ok, const char *format, ...) CUTE_GNU_ATTR((format(printf, 6, 7)));
static inline void cute_tester_fail(cute_tester_t *t, const char *file, unsigned long long line, const char *format, ...) CUTE_GNU_ATTR((format(printf, 4, 5)));

#define CUTE_TESTER _cute_tester
#define CUTE_TEST_FUNC(tname) cute_test_##tname##_run
#define CUTE_TEST_INIT(tname) cute_test_##tname##_init

#define CUTE_TEST(tname) CUTE_TEST_I(CUTE_TESTER, tname)
#define CUTE_TEST_I(t, tname)                                                                                                                                                                          \
    static void CUTE_TEST_FUNC(tname)(cute_tester_t * (t));                                                                                                                                            \
    CUTE_GNU_ATTR((constructor)) static void CUTE_TEST_INIT(tname)(void) {                                                                                                                             \
        static cute_test_def_t test_def = {                                                                                                                                                            \
            .next = NULL,                                                                                                                                                                              \
            .testname = #tname,                                                                                                                                                                        \
            .file = __FILE__,                                                                                                                                                                          \
            .line = __LINE__,                                                                                                                                                                          \
            .run = &CUTE_TEST_FUNC(tname),                                                                                                                                                             \
        };                                                                                                                                                                                             \
        cute_register(&test_def);                                                                                                                                                                      \
    }                                                                                                                                                                                                  \
    static void CUTE_TEST_FUNC(tname)(CUTE_GNU_ATTR((unused)) cute_tester_t * (t))

#if __STDC_VERSION__ >= 201112L
// C11 support
#    define CUTE_BEFORE_EACH(setup) _Generic(&(setup), cute_setup_func_0_t *: cute_before_each_0, cute_setup_func_1_t *: cute_before_each_1)(&(setup))
#    define CUTE_AFTER_EACH(teardown) _Generic(&(teardown), cute_teardown_func_0_t *: cute_after_each_0, cute_teardown_func_1_t *: cute_after_each_1)(&(teardown))
#else
#    define CUTE_BEFORE_EACH(setup) cute_before_each_1((cute_setup_func_1_t *)&(setup))
#    define CUTE_AFTER_EACH(teardown) cute_after_each_1((cute_teardown_func_1_t *)&(teardown))
#endif

#define CUTE_SUBTEST(...) CUTE_SUBTEST_I(CUTE_TESTER, __VA_ARGS__)
#define CUTE_SUBTEST_I(t, ...)                                                                                                                                                                         \
    for (cute_tester_t *_cute_tester_saved = (t), _cute_tester_subtester, (*t) = cute_tester_subtest_start(_cute_tester_saved, &_cute_tester_subtester, __FILE__, __LINE__, __VA_ARGS__); (t);         \
         cute_tester_subtest_finish(&_cute_tester_subtester), (t) = NULL)

#define CUTE_PP_NO_MSG(...) CUTE_PP_NO_MSG_I(__VA_ARGS__, )
#define CUTE_PP_NO_MSG_I(m, ...) CUTE_PP_NO_MSG_II(__VA_OPT__(0, ) 1, )
#define CUTE_PP_NO_MSG_II(x, ...) x

#define CUTE_PP_IF(c, x, y) CUTE_PP_IF_I(c, x, y)
#define CUTE_PP_IF_I(c, x, y) CUTE_PP_IF_##c(x, y)
#define CUTE_PP_IF_0(x, y) y
#define CUTE_PP_IF_1(x, y) x

#define CUTE_PP_IS_TUPLE(x) CUTE_PP_IS_TUPLE_II(CUTE_PP_IS_TUPLE_I x)
#define CUTE_PP_IS_TUPLE_I(...) 1
#define CUTE_PP_IS_TUPLE_II(x) CUTE_PP_IS_TUPLE_III(x)
#define CUTE_PP_IS_TUPLE_III(x) CUTE_PP_IS_TUPLE_IV(CUTE_PP_IS_TUPLE_##x)
#define CUTE_PP_IS_TUPLE_IV(x) CUTE_PP_IS_TUPLE_V(x)
#define CUTE_PP_IS_TUPLE_V(r, _) r
#define CUTE_PP_IS_TUPLE_1 1,
#define CUTE_PP_IS_TUPLE_CUTE_PP_IS_TUPLE_I 0,

#define CUTE_STR_SPAN(x) CUTE_STR_SPAN_I x
#define CUTE_STR_SPAN_I(p, n) ((cute_str_span_t){.ptr = p, .len = n})

#define CUTE_MATCHER_DESC(mt) CUTE_MATCHER_DESC_I mt
#define CUTE_MATCHER_DESC_I(desc, prepare, pred, data) desc
#define CUTE_MATCHER_PREPARE(mt) CUTE_MATCHER_PREPARE_I mt
#define CUTE_MATCHER_PREPARE_I(desc, prepare, pred, data) prepare
#define CUTE_MATCHER_PRED(mt) CUTE_MATCHER_PRED_I mt
#define CUTE_MATCHER_PRED_I(desc, prepare, pred, data) pred
#define CUTE_MATCHER_DATA(mt) CUTE_MATCHER_DATA_I mt
#define CUTE_MATCHER_DATA_I(desc, prepare, pred, data) data
#define CUTE_MATCHER_ARGS(t, actual, data) CUTE_MATCHER_ARGS_II(t, actual, CUTE_MATCHER_ARGS_I data)
#define CUTE_MATCHER_ARGS_I(...) __VA_ARGS__
#define CUTE_MATCHER_ARGS_II(t, actual, ...) (t, actual __VA_OPT__(, ) __VA_ARGS__)

#define CUTE_MATCHER_PREPARE_0(t, actual) const __typeof(actual) _0 = (actual);
#define CUTE_MATCHER_PREPARE_1(t, actual, a1)                                                                                                                                                          \
    const __typeof(actual) _0 = (actual);                                                                                                                                                              \
    const __typeof(a1) _1 = (a1);
#define CUTE_MATCHER_PREPARE_2(t, actual, a1, a2)                                                                                                                                                      \
    const __typeof(actual) _0 = (actual);                                                                                                                                                              \
    const __typeof(a1) _1 = (a1);                                                                                                                                                                      \
    const __typeof(a2) _2 = (a2);
#define CUTE_MATCHER_PREPARE_3(t, actual, a1, a2, a3)                                                                                                                                                  \
    const __typeof(actual) _0 = (actual);                                                                                                                                                              \
    const __typeof(a1) _1 = (a1);                                                                                                                                                                      \
    const __typeof(a2) _2 = (a2);                                                                                                                                                                      \
    const __typeof(a3) _3 = (a3);
#define CUTE_MATCHER_PREPARE_4(t, actual, a1, a2, a3, a4)                                                                                                                                              \
    const __typeof(actual) _0 = (actual);                                                                                                                                                              \
    const __typeof(a1) _1 = (a1);                                                                                                                                                                      \
    const __typeof(a2) _2 = (a2);                                                                                                                                                                      \
    const __typeof(a3) _3 = (a3);                                                                                                                                                                      \
    const __typeof(a4) _4 = (a4);
#define CUTE_MATCHER_PREPARE_STR_1(t, actual, a1) CUTE_PP_IF(CUTE_PP_IS_TUPLE(actual), CUTE_MATCHER_PREPARE_STR_1_SPAN, CUTE_MATCHER_PREPARE_STR_1_STR)(t, actual, a1)
#define CUTE_MATCHER_PREPARE_STR_1_STR(t, actual, a1)                                                                                                                                                  \
    const char *const _0 = (actual);                                                                                                                                                                   \
    const char *const _1 = (a1);
#define CUTE_MATCHER_PREPARE_STR_1_SPAN(t, actual, a1)                                                                                                                                                 \
    const cute_str_span_t _0 = CUTE_STR_SPAN(actual);                                                                                                                                                  \
    const char *const _1 = (a1);

#define CUTE_MATCH(t, f, actual, ...) CUTE_PP_IF(CUTE_PP_NO_MSG(__VA_ARGS__), CUTE_MATCH_I, CUTE_MATCH_II)(t, f, actual, __VA_ARGS__)
#define CUTE_MATCH_I(t, f, actual, m) CUTE_MATCH_II(t, f, actual, m, NULL)
#define CUTE_MATCH_II(t, f, actual, m, ...) CUTE_MATCH_III(t, f, actual, CUTE_MATCHER_##m, __VA_ARGS__)
#define CUTE_MATCH_III(t, f, actual, mt, ...)                                                                                                                                                          \
    CUTE_MATCH_IV(t, f, CUTE_MATCHER_DESC(mt), CUTE_MATCHER_PREPARE(mt), CUTE_MATCHER_PRED(mt), CUTE_MATCHER_ARGS(t, actual, CUTE_MATCHER_DATA(mt)), __VA_ARGS__)
#define CUTE_MATCH_IV(t, f, desc, prepare, pred, args, ...) f(t, desc args, prepare args, pred args, __VA_ARGS__)

#define CUTE_EXPECT(actual, ...) CUTE_EXPECT_I(CUTE_TESTER, actual, __VA_ARGS__)
#define CUTE_EXPECT_I(t, actual, ...) CUTE_MATCH(t, CUTE_EXPECT_II, actual, __VA_ARGS__)
#define CUTE_EXPECT_II(t, desc, prepare, pred, ...)                                                                                                                                                    \
    do {                                                                                                                                                                                               \
        prepare;                                                                                                                                                                                       \
        cute_tester_expect((t), __FILE__, __LINE__, (desc), (pred), __VA_ARGS__);                                                                                                                      \
    } while (0)

#define CUTE_ASSERT(actual, ...) CUTE_ASSERT_I(CUTE_TESTER, actual, __VA_ARGS__)
#define CUTE_ASSERT_I(t, actual, ...) CUTE_MATCH(t, CUTE_ASSERT_II, actual, __VA_ARGS__)
#define CUTE_ASSERT_II(t, desc, prepare, pred, ...)                                                                                                                                                    \
    do {                                                                                                                                                                                               \
        prepare;                                                                                                                                                                                       \
        if (!cute_tester_assert((t), __FILE__, __LINE__, (desc), (pred), __VA_ARGS__)) {                                                                                                               \
            return;                                                                                                                                                                                    \
        }                                                                                                                                                                                              \
    } while (0)

#define CUTE_FAIL(...) CUTE_FAIL_I(CUTE_TESTER __VA_OPT__(, ) __VA_ARGS__)
#define CUTE_FAIL_I(...) CUTE_PP_IF(CUTE_PP_NO_MSG(__VA_ARGS__), CUTE_FAIL_II, CUTE_FAIL_III)(__VA_ARGS__)
#define CUTE_FAIL_II(t) CUTE_FAIL_III(t, NULL)
#define CUTE_FAIL_III(t, ...)                                                                                                                                                                          \
    do {                                                                                                                                                                                               \
        cute_tester_fail((t), __FILE__, __LINE__, __VA_ARGS__);                                                                                                                                        \
        return;                                                                                                                                                                                        \
    } while (0)

// matchers
// EXPECT(x, is_true)
#define CUTE_MATCHER_is_true (CUTE_MATCHER_is_true_DESC, CUTE_MATCHER_PREPARE_0, CUTE_MATCHER_is_true_PRED, ())
#define CUTE_MATCHER_is_true_DESC(t, actual) #actual " == true"
#define CUTE_MATCHER_is_true_PRED(t, actual) cute_matcher_is_true(t, _0)

static inline bool cute_matcher_is_true(cute_tester_t *t, bool actual) {
    (void)t;
    return actual;
}

// EXPECT(x, is_false)
#define CUTE_MATCHER_is_false (CUTE_MATCHER_is_false_DESC, CUTE_MATCHER_PREPARE_0, CUTE_MATCHER_is_false_PRED, ())
#define CUTE_MATCHER_is_false_DESC(t, actual) #actual " == false"
#define CUTE_MATCHER_is_false_PRED(t, actual) cute_matcher_is_false(t, _0)

static inline bool cute_matcher_is_false(cute_tester_t *t, bool actual) {
    (void)t;
    return !actual;
}

// EXPECT(x, is_null)
#define CUTE_MATCHER_is_null (CUTE_MATCHER_is_null_DESC, CUTE_MATCHER_PREPARE_0, CUTE_MATCHER_is_null_PRED, ())
#define CUTE_MATCHER_is_null_DESC(t, actual) #actual " == NULL"
#define CUTE_MATCHER_is_null_PRED(t, actual) (_0 == NULL)

// EXPECT(x, is_not_null)
#define CUTE_MATCHER_is_not_null (CUTE_MATCHER_is_not_null_DESC, CUTE_MATCHER_PREPARE_0, CUTE_MATCHER_is_not_null_PRED, ())
#define CUTE_MATCHER_is_not_null_DESC(t, actual) #actual " != NULL"
#define CUTE_MATCHER_is_not_null_PRED(t, actual) (_0 != NULL)

// EXPECT(actual, is(op, x))
#define CUTE_MATCHER_is(op, x) (CUTE_MATCHER_is_DESC, CUTE_MATCHER_is_PREPARE, CUTE_MATCHER_is_PRED, (op, x))
#define CUTE_MATCHER_is_DESC(t, actual, op, x) #actual " " #op " " #x
#define CUTE_MATCHER_is_PREPARE(t, actual, op, x)                                                                                                                                                      \
    const __typeof(actual) _0 = (actual);                                                                                                                                                              \
    const __typeof(x) _1 = (x);
#define CUTE_MATCHER_is_PRED(t, actual, op, x) ((_0)op(_1))

// EXPECT(actual, eq(expected))
#define CUTE_MATCHER_eq(expected) CUTE_MATCHER_is(==, expected)

// EXPECT(actual, ne(x))
#define CUTE_MATCHER_ne(x) CUTE_MATCHER_is(!=, x)

// EXPECT(actual, lt(x))
#define CUTE_MATCHER_lt(x) CUTE_MATCHER_is(<, x)

// EXPECT(actual, le(x))
#define CUTE_MATCHER_le(x) CUTE_MATCHER_is(<=, x)

// EXPECT(actual, gt(x))
#define CUTE_MATCHER_gt(x) CUTE_MATCHER_is(>, x)

// EXPECT(actual, ge(x))
#define CUTE_MATCHER_ge(x) CUTE_MATCHER_is(>=, x)

// EXPECT(actual, near(expected, eps))
#define CUTE_MATCHER_near(expected, eps) (CUTE_MATCHER_near_DESC, CUTE_MATCHER_PREPARE_2, CUTE_MATCHER_near_PRED, (expected, eps))
#define CUTE_MATCHER_near_DESC(t, actual, expected, eps) #actual " == " #expected " ± " #eps
#define CUTE_MATCHER_near_PRED(t, actual, expected, eps) ((_1 - _2) <= _0 && _0 <= (_1 + _2))

// EXPECT(actual, eq_str(x)) or
// EXPECT((actual_ptr, actual_len), eq_str(x))
#define CUTE_MATCHER_eq_str(expected) (CUTE_MATCHER_eq_str_DESC, CUTE_MATCHER_PREPARE_STR_1, CUTE_MATCHER_eq_str_PRED, (expected))
#define CUTE_MATCHER_eq_str_DESC(t, actual, expected) CUTE_PP_IF(CUTE_PP_IS_TUPLE(actual), CUTE_MATCHER_eq_str_DESC_SPAN, CUTE_MATCHER_eq_str_DESC_STR)(t, actual, expected)
#define CUTE_MATCHER_eq_str_DESC_STR(t, actual, expected) #actual " == " #expected
#define CUTE_MATCHER_eq_str_DESC_SPAN(t, actual, expected) CUTE_MATCHER_eq_str_DESC_SPAN_I actual " == " #expected
#define CUTE_MATCHER_eq_str_DESC_SPAN_I(p, n) #p "[:" #n "]"
#define CUTE_MATCHER_eq_str_PRED(t, actual, expected) CUTE_PP_IF(CUTE_PP_IS_TUPLE(actual), CUTE_MATCHER_eq_str_PRED_SPAN, CUTE_MATCHER_eq_str_PRED_STR)(t, actual, expected)
#define CUTE_MATCHER_eq_str_PRED_STR(t, actual, expected) cute_matcher_eq_str(t, _0, _1)
#define CUTE_MATCHER_eq_str_PRED_SPAN(t, actual, expected) cute_matcher_eq_str_span(t, _0, _1)

static inline bool cute_matcher_eq_str(cute_tester_t *t, const char *actual, const char *expected) {
    assert(expected != NULL);
    (void)t;

    if (actual == NULL) {
        return 0;
    }
    return strcmp(actual, expected) == 0;
}

static inline bool cute_matcher_eq_str_span(cute_tester_t *t, cute_str_span_t actual, const char *expected) {
    assert(expected != NULL);
    (void)t;

    if (actual.ptr == NULL) {
        return 0;
    }
    if (actual.len != strlen(expected)) {
        return 0;
    }
    return strncmp(actual.ptr, expected, actual.len) == 0;
}

// EXPECT(actual, contains(s))
#define CUTE_MATCHER_contains(s) (CUTE_MATCHER_contains_DESC, CUTE_MATCHER_PREPARE_STR_1, CUTE_MATCHER_contains_PRED, (s))
#define CUTE_MATCHER_contains_DESC(t, actual, s) #actual " contains " #s
#define CUTE_MATCHER_contains_PRED(t, actual, s) cute_matcher_contains(t, _0, _1)

static inline bool cute_matcher_contains(cute_tester_t *t, const char *actual, const char *s) {
    assert(s != NULL);
    (void)t;

    if (actual == NULL) {
        return 0;
    }
    return strstr(actual, s) != NULL;
}

// EXPECT(actual, not(matcher))
#define CUTE_MATCHER_not(matcher) CUTE_MATCHER_not_I(CUTE_MATCHER_##matcher)
#define CUTE_MATCHER_not_I(m) (CUTE_MATCHER_not_DESC, CUTE_MATCHER_not_PREPARE, CUTE_MATCHER_not_PRED, m)
#define CUTE_MATCHER_not_DESC(t, actual, desc, prepare, pred, data) "not " desc CUTE_MATCHER_ARGS(t, actual, data)
#define CUTE_MATCHER_not_PREPARE(t, actual, desc, prepare, pred, data) prepare CUTE_MATCHER_ARGS(t, actual, data)
#define CUTE_MATCHER_not_PRED(t, actual, desc, prepare, pred, data) (!(pred CUTE_MATCHER_ARGS(t, actual, data)))

#ifdef CUTE_MAIN
#    define CUTE_RUN_ALL() cute_run_all(__FILE__, __LINE__)
#endif // CUTE_MAIN

static inline void cute_register(cute_test_def_t *test) {
    if (cute_test_list.tail == NULL) {
        cute_test_list.tail = &cute_test_list.head;
    }

    test->next = NULL;
    *cute_test_list.tail = test;
    cute_test_list.tail = &test->next;
}

static inline void cute_before_each_0(cute_setup_func_0_t *setup) {
    assert(setup != NULL);
    cute_test_list.setup.enabled = true;
    cute_test_list.setup.argc = 0;
    cute_test_list.setup.func._0 = setup;
}

static inline void cute_before_each_1(cute_setup_func_1_t *setup) {
    assert(setup != NULL);
    cute_test_list.setup.enabled = true;
    cute_test_list.setup.argc = 1;
    cute_test_list.setup.func._1 = setup;
}

static inline void cute_after_each_0(cute_teardown_func_0_t *teardown) {
    assert(teardown != NULL);
    cute_test_list.teardown.enabled = true;
    cute_test_list.teardown.argc = 0;
    cute_test_list.teardown.func._0 = teardown;
}

static inline void cute_after_each_1(cute_teardown_func_1_t *teardown) {
    assert(teardown != NULL);
    cute_test_list.teardown.enabled = true;
    cute_test_list.teardown.argc = 1;
    cute_test_list.teardown.func._1 = teardown;
}

static inline cute_result_t *cute_result_new_subtest(cute_result_t *parent, const char *file, unsigned long long line, const char *format, ...) {
    va_list args;
    va_start(args, format);
    cute_result_t *r = cute_result_new_subtest_v(parent, file, line, format, args);
    va_end(args);
    return r;
}

static inline cute_result_t *cute_result_new_subtest_v(cute_result_t *parent, const char *file, unsigned long long line, const char *format, va_list args) {
    cute_result_t *r = (cute_result_t *)malloc(sizeof(cute_result_t));
    r->type = cute_result_type_subtest;
    r->parent = parent;
    r->next = NULL;
    r->file = file;
    r->line = line;
    r->message = NULL;
    r->_.subtest.head = NULL;
    r->_.subtest.start_time = 0;
    r->_.subtest.end_time = 0;
    r->_.subtest.total = -1;
    r->_.subtest.ok = -1;

    if (format != NULL) {
        va_list args_copy;
        va_copy(args_copy, args);
        const int size = vsnprintf(NULL, 0, format, args_copy);
        va_end(args_copy);

        if (size >= 0) {
            r->message = (char *)malloc(size + 1);
            if (r->message != NULL) {
                vsnprintf(r->message, size + 1, format, args);
            }
        }
    }

    return r;
}

static inline cute_result_t *cute_result_new_assert_v(cute_result_t *parent, const char *file, unsigned long long line, const char *expr, bool ok, const char *format, va_list args) {
    cute_result_t *r = (cute_result_t *)malloc(sizeof(cute_result_t));
    r->type = cute_result_type_assert;
    r->parent = parent;
    r->next = NULL;
    r->file = file;
    r->line = line;
    r->message = NULL;
    r->_.assert.expr = expr;
    r->_.assert.ok = ok;

    if (format != NULL) {
        va_list args_copy;
        va_copy(args_copy, args);
        const int size = vsnprintf(NULL, 0, format, args_copy);
        va_end(args_copy);

        if (size >= 0) {
            r->message = (char *)malloc(size + 1);
            if (r->message != NULL) {
                vsnprintf(r->message, size + 1, format, args);
            }
        }
    }

    return r;
}

static inline void cute_result_delete(cute_result_t *r) {
    while (r != NULL) {
        cute_result_t *next = r->next;

        switch (r->type) {
        case cute_result_type_subtest:
            cute_result_delete(r->_.subtest.head);
            break;
        case cute_result_type_assert:
            break;
        default:
            break;
        }

        free(r->message);
        free(r);

        r = next;
    }
}

static inline void cute_result_subtest_start(cute_result_t *r) {
    assert(r != NULL);
    assert(r->type == cute_result_type_subtest);

    r->_.subtest.end_time = r->_.subtest.start_time = clock();
}

static inline void cute_result_subtest_finish(cute_result_t *r) {
    assert(r != NULL);
    assert(r->type == cute_result_type_subtest);

    r->_.subtest.end_time = clock();

    r->_.subtest.total = 0;
    r->_.subtest.ok = 0;

    for (const cute_result_t *child = r->_.subtest.head; child != NULL; child = child->next) {
        r->_.subtest.total += 1;
        r->_.subtest.ok += cute_result_ok(child) ? 1 : 0;
    }
}

static inline double cute_result_subtest_duration(const cute_result_t *r) {
    assert(r != NULL);
    assert(r->type == cute_result_type_subtest);

    return 1e3 * (r->_.subtest.end_time - r->_.subtest.start_time) / CLOCKS_PER_SEC;
}

static inline int cute_result_ok(const cute_result_t *r) {
    assert(r != NULL);

    switch (r->type) {
    case cute_result_type_subtest:
        return r->_.subtest.ok == r->_.subtest.total;
    case cute_result_type_assert:
        return r->_.assert.ok;
    default:
        return 0;
    }
}

#define CUTE_ANSI_RED "\x1b[31m"
#define CUTE_ANSI_GREEN "\x1b[32m"
#define CUTE_ANSI_YELLOW "\x1b[33m"
#define CUTE_ANSI_BLUE "\x1b[34m"
#define CUTE_ANSI_RESET "\x1b[m"

#define CUTE_COLOR_INFO(s) CUTE_ANSI_BLUE s CUTE_ANSI_RESET
#define CUTE_COLOR_ERROR(s) CUTE_ANSI_RED s CUTE_ANSI_RESET
#define CUTE_COLOR_WARN(s) CUTE_ANSI_YELLOW s CUTE_ANSI_RESET
#define CUTE_COLOR_SUCCESS(s) CUTE_ANSI_GREEN s CUTE_ANSI_RESET

static inline void cute_result_dump_i(const cute_result_t *r, FILE *out, unsigned int depth) {
    assert(r != NULL);

    const bool ok = cute_result_ok(r);

    for (unsigned int i = 0; i < depth; i++) {
        fprintf(out, "  ");
    }

    switch (r->type) {
    case cute_result_type_subtest: {
        if (ok) {
            fprintf(out, CUTE_COLOR_SUCCESS("[ PASS ] %s"), r->message);
            fprintf(out, " ");
            fprintf(out, CUTE_COLOR_INFO("[%d/%d]"), r->_.subtest.ok, r->_.subtest.total);
            fprintf(out, " - ");
            fprintf(out, CUTE_COLOR_INFO("%.1f ms"), cute_result_subtest_duration(r));
            fprintf(out, "\n");

#if CUTE_VERBOSE
            for (const cute_result_t *child = r->_.subtest.head; child != NULL; child = child->next) {
                cute_result_dump_i(child, out, depth + 1);
            }
#endif
        } else {
            fprintf(out, CUTE_COLOR_WARN("[FAILED] %s"), r->message);
            fprintf(out, " ");
            fprintf(out, CUTE_COLOR_INFO("[%d/%d]"), r->_.subtest.ok, r->_.subtest.total);
            fprintf(out, " - ");
            fprintf(out, CUTE_COLOR_INFO("%.1f ms"), cute_result_subtest_duration(r));
            fprintf(out, " at %s:%llu", r->file, r->line);
            fprintf(out, "\n");

            for (const cute_result_t *child = r->_.subtest.head; child != NULL; child = child->next) {
                cute_result_dump_i(child, out, depth + 1);
            }
        }
        break;
    }
    case cute_result_type_assert: {
        if (ok) {
            fprintf(out, CUTE_COLOR_SUCCESS("[ PASS ] %s"), r->_.assert.expr);
            if (r->message != NULL) {
                fprintf(out, CUTE_COLOR_SUCCESS(": %s"), r->message);
            }
        } else {
            fprintf(out, CUTE_COLOR_ERROR("[FAILED] %s"), r->_.assert.expr);
            if (r->message != NULL) {
                fprintf(out, CUTE_COLOR_ERROR(": %s"), r->message);
            }
            fprintf(out, " at %s:%llu", r->file, r->line);
        }
        fprintf(out, "\n");
        break;
    }
    default:
        break;
    }
}

static inline void cute_result_dump(const cute_result_t *r, FILE *out) {
    assert(r != NULL);
    assert(r->type == cute_result_type_subtest);

    for (const cute_result_t *child = r->_.subtest.head; child != NULL; child = child->next) {
        cute_result_dump_i(child, out, 0);
    }

    if (cute_result_ok(r)) {
        fprintf(out, CUTE_COLOR_SUCCESS("[%d/%d]"), r->_.subtest.ok, r->_.subtest.total);
    } else {
        fprintf(out, CUTE_COLOR_ERROR("[%d/%d]"), r->_.subtest.ok, r->_.subtest.total);
    }

    fprintf(out, " ");
    fprintf(out, CUTE_COLOR_INFO("total %.1f ms"), cute_result_subtest_duration(r));
    fprintf(out, "\n");
}

static inline void cute_tester_init(cute_tester_t *t, cute_result_t *subtest) {
    assert(t != NULL);
    assert(subtest != NULL);
    assert(subtest->type == cute_result_type_subtest);

    t->current = subtest;
    t->tail = &t->current->_.subtest.head;
}

static inline cute_tester_t *cute_tester_subtest_start(cute_tester_t *t, cute_tester_t *subtester, const char *file, unsigned long long line, const char *format, ...) {
    assert(t != NULL);
    assert(subtester != NULL);

    va_list args;
    va_start(args, format);
    cute_result_t *subtest = cute_result_new_subtest_v(t->current, file, line, format, args);
    va_end(args);

    cute_tester_push_result(t, subtest);

    cute_tester_init(subtester, subtest);

    cute_result_subtest_start(subtest);

    return subtester;
}

static inline void cute_tester_subtest_finish(cute_tester_t *subtester) {
    assert(subtester != NULL);

    cute_result_subtest_finish(subtester->current);
}

static inline void cute_tester_push_result(cute_tester_t *t, cute_result_t *result) {
    assert(t != NULL);
    assert(result != NULL);

    result->next = NULL;
    *t->tail = result;
    t->tail = &result->next;
}

static inline void cute_tester_expect(cute_tester_t *t, const char *file, unsigned long long line, const char *expr, bool ok, const char *format, ...) {
    assert(t != NULL);

    va_list args;
    va_start(args, format);
    cute_tester_push_result(t, cute_result_new_assert_v(t->current, file, line, expr, ok, format, args));
    va_end(args);
}

static inline bool cute_tester_assert(cute_tester_t *t, const char *file, unsigned long long line, const char *expr, bool ok, const char *format, ...) {
    assert(t != NULL);

    va_list args;
    va_start(args, format);
    cute_tester_push_result(t, cute_result_new_assert_v(t->current, file, line, expr, ok, format, args));
    va_end(args);

    if (!ok) {
        for (cute_result_t *p = t->current; p != NULL; p = p->parent) {
            cute_result_subtest_finish(p);
        }
    }

    return ok;
}

static inline void cute_tester_fail(cute_tester_t *t, const char *file, unsigned long long line, const char *format, ...) {
    assert(t != NULL);

    va_list args;
    va_start(args, format);
    cute_tester_push_result(t, cute_result_new_assert_v(t->current, file, line, "fail", 0, format, args));
    va_end(args);

    for (cute_result_t *p = t->current; p != NULL; p = p->parent) {
        cute_result_subtest_finish(p);
    }
}

#ifdef CUTE_MAIN

cute_test_list_t cute_test_list;

static inline void cute_run_test(cute_tester_t *t, const cute_test_def_t *test) {
    assert(test != NULL);

    cute_result_t *subtest = cute_result_new_subtest(t->current, test->file, test->line, "%s", test->testname);
    cute_tester_push_result(t, subtest);

    cute_tester_t u;
    cute_tester_init(&u, subtest);

    if (cute_test_list.setup.enabled) {
        if (cute_test_list.setup.argc == 0) {
            cute_test_list.setup.func._0();
        } else {
            cute_test_list.setup.func._1(test->testname);
        }
    }

    cute_result_subtest_start(subtest);
    test->run(&u);
    cute_result_subtest_finish(subtest);

    if (cute_test_list.teardown.enabled) {
        if (cute_test_list.teardown.argc == 0) {
            cute_test_list.teardown.func._0();
        } else {
            cute_test_list.teardown.func._1(test->testname);
        }
    }
}

static inline int cute_run_all(const char *file, unsigned long long line) {
    cute_result_t *root = cute_result_new_subtest(NULL, file, line, NULL);
    cute_tester_t t;
    cute_tester_init(&t, root);

    cute_result_subtest_start(root);

    for (const cute_test_def_t *test = cute_test_list.head; test != NULL; test = test->next) {
        cute_run_test(&t, test);
    }

    cute_result_subtest_finish(root);

    cute_result_dump(root, stderr);

    const bool ok = cute_result_ok(root);

    cute_result_delete(root);

    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

#endif // CUTE_MAIN

#endif // INCLUDE_CUTE_H

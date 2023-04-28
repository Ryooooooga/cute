#ifndef INCLUDE_CUTE_H
#define INCLUDE_CUTE_H

#ifndef __STDC_WANT_LIB_EXT2__
#    define __STDC_WANT_LIB_EXT2__ 1
#endif // __STDC_WANT_LIB_EXT2__

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
#    define TEST CUTE_TEST
#    define REGISTER CUTE_REGISTER
#    define BEFORE_EACH CUTE_BEFORE_EACH
#    define AFTER_EACH CUTE_AFTER_EACH
#    define SUBTEST CUTE_SUBTEST
#    define EXPECT CUTE_EXPECT
#    define ASSERT CUTE_ASSERT
#    define FAIL CUTE_FAIL
#    define ACTUAL CUTE_ACTUAL
#    define RUN_ALL CUTE_RUN_ALL
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
typedef void cute_setup_func_t(void);
typedef void cute_teardown_func_t(void);

struct cute_test_def_t {
    cute_test_def_t *next;
    const char *testname;
    const char *file;
    unsigned long long line;
    cute_test_func_t *run;
};

struct cute_test_list_t {
    cute_test_def_t *head, **tail;
    cute_setup_func_t *setup;
    cute_teardown_func_t *teardown;
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
static inline void cute_before_each(cute_setup_func_t *setup);
static inline void cute_after_each(cute_teardown_func_t *teardown);

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
#define CUTE_ACTUAL _0
#define CUTE_TEST_FUNC(tname) cute_test_##tname##_run
#define CUTE_TEST_INIT(tname) cute_test_##tname##_init
#define CUTE_TEST_DATA(tname) cute_test_##tname##_data

#define CUTE_TEST(tname) CUTE_TEST_I(CUTE_TESTER, tname)
#define CUTE_TEST_I(t, tname)                                                                                                                                                                          \
    static void CUTE_TEST_FUNC(tname)(cute_tester_t * (t));                                                                                                                                            \
    static void CUTE_TEST_INIT(tname)(void) CUTE_GNU_ATTR((constructor));                                                                                                                              \
    static cute_test_def_t CUTE_TEST_DATA(tname) = {                                                                                                                                                   \
        .next = NULL,                                                                                                                                                                                  \
        .testname = #tname,                                                                                                                                                                            \
        .file = __FILE__,                                                                                                                                                                              \
        .line = __LINE__,                                                                                                                                                                              \
        .run = &CUTE_TEST_FUNC(tname),                                                                                                                                                                 \
    };                                                                                                                                                                                                 \
    static void CUTE_TEST_INIT(tname)(void) {                                                                                                                                                          \
        CUTE_REGISTER(tname);                                                                                                                                                                          \
    }                                                                                                                                                                                                  \
    static void CUTE_TEST_FUNC(tname)(CUTE_GNU_ATTR((unused)) cute_tester_t * (t))

#define CUTE_REGISTER(tname) cute_register(&CUTE_TEST_DATA(tname));
#define CUTE_BEFORE_EACH(setup) cute_before_each((setup))
#define CUTE_AFTER_EACH(teardown) cute_after_each((teardown))

#define CUTE_SUBTEST(...) CUTE_SUBTEST_I(CUTE_TESTER, __VA_ARGS__)
#define CUTE_SUBTEST_I(t, ...)                                                                                                                                                                         \
    for (cute_tester_t *_cute_tester_saved = (t), _cute_tester_subtester, (*t) = cute_tester_subtest_start(_cute_tester_saved, &_cute_tester_subtester, __FILE__, __LINE__, __VA_ARGS__); (t);         \
         cute_tester_subtest_finish(&_cute_tester_subtester), (t) = NULL)

#define CUTE_PP_NO_MSG(...) CUTE_PP_NO_MSG_I(__VA_ARGS__, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, )
#define CUTE_PP_NO_MSG_I(x, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, ...) _10

#define CUTE_PP_IF(c, x, y) CUTE_PP_IF_I(c, x, y)
#define CUTE_PP_IF_I(c, x, y) CUTE_PP_IF_##c(x, y)
#define CUTE_PP_IF_0(x, y) y
#define CUTE_PP_IF_1(x, y) x

#define CUTE_MATCHER_DESC(mt) CUTE_MATCHER_DESC_I mt
#define CUTE_MATCHER_DESC_I(desc, trans, placeholder, pred, data) desc
#define CUTE_MATCHER_TRANS(mt) CUTE_MATCHER_TRANS_I mt
#define CUTE_MATCHER_TRANS_I(desc, trans, placeholder, pred, data) trans
#define CUTE_MATCHER_PLACEHOLDER(mt) CUTE_MATCHER_PLACEHOLDER_I mt
#define CUTE_MATCHER_PLACEHOLDER_I(desc, trans, placeholder, pred, data) placeholder
#define CUTE_MATCHER_PRED(mt) CUTE_MATCHER_PRED_I mt
#define CUTE_MATCHER_PRED_I(desc, trans, placeholder, pred, data) pred
#define CUTE_MATCHER_DATA(mt) CUTE_MATCHER_DATA_I mt
#define CUTE_MATCHER_DATA_I(desc, trans, placeholder, pred, data) data
#define CUTE_MATCHER_ARGS(actual, data) CUTE_MATCHER_ARGS_II(actual, CUTE_MATCHER_ARGS_I data)
#define CUTE_MATCHER_ARGS_I(...) __VA_ARGS__
#define CUTE_MATCHER_ARGS_II(actual, ...) (actual __VA_OPT__(, ) __VA_ARGS__)
#define CUTE_MATCHER_TRANS_ID(x) x
#define CUTE_MATCHER_TRANS_STR_SPAN(x) CUTE_MATCHER_TRANS_STR_SPAN_I x
#define CUTE_MATCHER_TRANS_STR_SPAN_I(p, n) ((cute_str_span_t){.ptr = p, .len = n})
#define CUTE_MATCHER_PLACEHOLDER_0(actual)
#define CUTE_MATCHER_PLACEHOLDER_1(actual, a1) const __typeof(a1) _1 = (a1);
#define CUTE_MATCHER_PLACEHOLDER_2(actual, a1, a2)                                                                                                                                                     \
    const __typeof(a1) _1 = (a1);                                                                                                                                                                      \
    const __typeof(a2) _2 = (a2);
#define CUTE_MATCHER_PLACEHOLDER_3(actual, a1, a2, a3)                                                                                                                                                 \
    const __typeof(a1) _1 = (a1);                                                                                                                                                                      \
    const __typeof(a2) _2 = (a2);                                                                                                                                                                      \
    const __typeof(a3) _3 = (a3);
#define CUTE_MATCHER_PLACEHOLDER_4(actual, a1, a2, a3, a4)                                                                                                                                             \
    const __typeof(a1) _1 = (a1);                                                                                                                                                                      \
    const __typeof(a2) _2 = (a2);                                                                                                                                                                      \
    const __typeof(a3) _3 = (a3);                                                                                                                                                                      \
    const __typeof(a4) _4 = (a4);
#define CUTE_MATCHER_PLACEHOLDER_STR_1(actual, a1) const char *const _1 = (a1);

#define CUTE_MATCH(t, f, actual, ...) CUTE_PP_IF(CUTE_PP_NO_MSG(__VA_ARGS__), CUTE_MATCH_I, CUTE_MATCH_II)(t, f, actual, __VA_ARGS__)
#define CUTE_MATCH_I(t, f, actual, m) CUTE_MATCH_II(t, f, actual, m, NULL)
#define CUTE_MATCH_II(t, f, actual, m, ...) CUTE_MATCH_III(t, f, actual, CUTE_MATCHER_##m, __VA_ARGS__)
#define CUTE_MATCH_III(t, f, actual, mt, ...)                                                                                                                                                          \
    CUTE_MATCH_IV(t, f, actual, CUTE_MATCHER_DESC(mt), CUTE_MATCHER_TRANS(mt), CUTE_MATCHER_PLACEHOLDER(mt), CUTE_MATCHER_PRED(mt), CUTE_MATCHER_DATA(mt), __VA_ARGS__)
#define CUTE_MATCH_IV(t, f, actual, desc, trans, placeholder, pred, data, ...)                                                                                                                         \
    f(t, trans(actual), placeholder CUTE_MATCHER_ARGS(actual, data), desc CUTE_MATCHER_ARGS(actual, data), pred CUTE_MATCHER_ARGS((CUTE_ACTUAL), data), __VA_ARGS__)

#define CUTE_EXPECT(actual, ...) CUTE_EXPECT_I(CUTE_TESTER, actual, __VA_ARGS__)
#define CUTE_EXPECT_I(t, actual, ...) CUTE_MATCH(t, CUTE_EXPECT_II, actual, __VA_ARGS__)
#define CUTE_EXPECT_II(t, actual, placeholder, desc, ok, ...)                                                                                                                                          \
    do {                                                                                                                                                                                               \
        const __typeof(actual)(CUTE_ACTUAL) = (actual);                                                                                                                                                \
        placeholder;                                                                                                                                                                                   \
        cute_tester_expect((t), __FILE__, __LINE__, (desc), (ok), __VA_ARGS__);                                                                                                                        \
    } while (0)

#define CUTE_ASSERT(actual, ...) CUTE_ASSERT_I(CUTE_TESTER, actual, __VA_ARGS__)
#define CUTE_ASSERT_I(t, actual, ...) CUTE_MATCH(t, CUTE_ASSERT_II, actual, __VA_ARGS__)
#define CUTE_ASSERT_II(t, actual, placeholder, desc, ok, ...)                                                                                                                                          \
    do {                                                                                                                                                                                               \
        const __typeof(actual)(CUTE_ACTUAL) = (actual);                                                                                                                                                \
        placeholder;                                                                                                                                                                                   \
        if (!cute_tester_assert((t), __FILE__, __LINE__, (desc), (ok), __VA_ARGS__)) {                                                                                                                 \
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
#define CUTE_MATCHER_is_true (CUTE_MATCHER_is_true_DESC, CUTE_MATCHER_TRANS_ID, CUTE_MATCHER_PLACEHOLDER_0, CUTE_MATCHER_is_true_PRED, ())
#define CUTE_MATCHER_is_true_DESC(actual) #actual " == true"
#define CUTE_MATCHER_is_true_PRED(actual) cute_matcher_is_true((actual))

static inline bool cute_matcher_is_true(bool actual) {
    return actual;
}

// EXPECT(x, is_false)
#define CUTE_MATCHER_is_false (CUTE_MATCHER_is_false_DESC, CUTE_MATCHER_TRANS_ID, CUTE_MATCHER_PLACEHOLDER_0, CUTE_MATCHER_is_false_PRED, ())
#define CUTE_MATCHER_is_false_DESC(actual) #actual " == false"
#define CUTE_MATCHER_is_false_PRED(actual) cute_matcher_is_false((actual))

static inline bool cute_matcher_is_false(bool actual) {
    return !actual;
}

// EXPECT(x, is_null)
#define CUTE_MATCHER_is_null (CUTE_MATCHER_is_null_DESC, CUTE_MATCHER_TRANS_ID, CUTE_MATCHER_PLACEHOLDER_0, CUTE_MATCHER_is_null_PRED, ())
#define CUTE_MATCHER_is_null_DESC(actual) #actual " == NULL"
#define CUTE_MATCHER_is_null_PRED(actual) ((actual) == NULL)

// EXPECT(x, is_not_null)
#define CUTE_MATCHER_is_not_null (CUTE_MATCHER_is_not_null_DESC, CUTE_MATCHER_TRANS_ID, CUTE_MATCHER_PLACEHOLDER_0, CUTE_MATCHER_is_not_null_PRED, ())
#define CUTE_MATCHER_is_not_null_DESC(actual) #actual " != NULL"
#define CUTE_MATCHER_is_not_null_PRED(actual) ((actual) != NULL)

// EXPECT(actual, is(op, x))
#define CUTE_MATCHER_is(op, x) (CUTE_MATCHER_is_DESC, CUTE_MATCHER_TRANS_ID, CUTE_MATCHER_is_PLACEHOLDER, CUTE_MATCHER_is_PRED, (op, x))
#define CUTE_MATCHER_is_DESC(actual, op, x) #actual " " #op " " #x
#define CUTE_MATCHER_is_PLACEHOLDER(actual, op, x) const __typeof(x) _1 = (x);
#define CUTE_MATCHER_is_PRED(actual, op, x) ((actual)op(_1))

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

// EXPECT(actual, eq_str(x))
#define CUTE_MATCHER_eq_str(expected) (CUTE_MATCHER_eq_str_DESC, CUTE_MATCHER_TRANS_ID, CUTE_MATCHER_PLACEHOLDER_STR_1, CUTE_MATCHER_eq_str_PRED, (expected))
#define CUTE_MATCHER_eq_str_DESC(actual, expected) #actual " == " #expected
#define CUTE_MATCHER_eq_str_PRED(actual, expected) cute_matcher_eq_str((actual), (_1))

static inline bool cute_matcher_eq_str(const char *actual, const char *expected) {
    assert(expected != NULL);

    if (actual == NULL) {
        return 0;
    }
    return strcmp(actual, expected) == 0;
}

// EXPECT((actual, actual_len), eq_str_n(expected))
#define CUTE_MATCHER_eq_str_n(expected) (CUTE_MATCHER_eq_str_n_DESC, CUTE_MATCHER_TRANS_STR_SPAN, CUTE_MATCHER_PLACEHOLDER_STR_1, CUTE_MATCHER_eq_str_n_PRED, (expected))
#define CUTE_MATCHER_eq_str_n_DESC(actual, expected) CUTE_MATCHER_eq_str_n_DESC_I actual " == " #expected
#define CUTE_MATCHER_eq_str_n_DESC_I(p, n) #p "[:" #n "]"
#define CUTE_MATCHER_eq_str_n_PRED(actual, expected) cute_matcher_eq_str_n((actual), (_1))

static inline bool cute_matcher_eq_str_n(cute_str_span_t actual, const char *expected) {
    assert(expected != NULL);

    if (actual.ptr == NULL) {
        return 0;
    }
    if (actual.len != strlen(expected)) {
        return 0;
    }
    return strncmp(actual.ptr, expected, actual.len) == 0;
}

// EXPECT(actual, contains(s))
#define CUTE_MATCHER_contains(s) (CUTE_MATCHER_contains_DESC, CUTE_MATCHER_TRANS_ID, CUTE_MATCHER_PLACEHOLDER_STR_1, CUTE_MATCHER_contains_PRED, (s))
#define CUTE_MATCHER_contains_DESC(actual, s) #actual " contains " #s
#define CUTE_MATCHER_contains_PRED(actual, s) cute_matcher_contains((actual), (_1))

static inline bool cute_matcher_contains(const char *actual, const char *s) {
    assert(s != NULL);

    if (actual == NULL) {
        return 0;
    }
    return strstr(actual, s) != NULL;
}

// EXPECT(actual, not(matcher))
#define CUTE_MATCHER_not(matcher) CUTE_MATCHER_not_I(CUTE_MATCHER_##matcher)
#define CUTE_MATCHER_not_I(m) (CUTE_MATCHER_not_DESC, CUTE_MATCHER_TRANS(m), CUTE_MATCHER_not_PLACEHOLDER, CUTE_MATCHER_not_PRED, m)
#define CUTE_MATCHER_not_DESC(actual, desc, trans, placeholder, pred, data) CUTE_MATCHER_not_DESC_I(desc, CUTE_MATCHER_ARGS(actual, data))
#define CUTE_MATCHER_not_DESC_I(desc, args) "not " desc args
#define CUTE_MATCHER_not_PLACEHOLDER(actual, desc, trans, placeholder, pred, data) CUTE_MATCHER_not_PLACEHOLDER_I(placeholder, CUTE_MATCHER_ARGS(actual, data))
#define CUTE_MATCHER_not_PLACEHOLDER_I(placeholder, args) placeholder args
#define CUTE_MATCHER_not_PRED(actual, desc, trans, placeholder, pred, data) CUTE_MATCHER_not_PRED_I(pred, CUTE_MATCHER_ARGS(actual, data))
#define CUTE_MATCHER_not_PRED_I(pred, args) (!(pred args))

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

static inline void cute_before_each(cute_setup_func_t *setup) {
    cute_test_list.setup = setup;
}

static inline void cute_after_each(cute_teardown_func_t *teardown) {
    cute_test_list.teardown = teardown;
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
        if (vasprintf(&r->message, format, args) == -1) {
            r->message = NULL;
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
        if (vasprintf(&r->message, format, args) == -1) {
            r->message = NULL;
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

    if (cute_test_list.setup != NULL) {
        cute_test_list.setup();
    }

    cute_result_subtest_start(subtest);
    test->run(&u);
    cute_result_subtest_finish(subtest);

    if (cute_test_list.teardown != NULL) {
        cute_test_list.teardown();
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

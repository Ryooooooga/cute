#ifndef INCLUDE_CUTE_H
#define INCLUDE_CUTE_H

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef CUTE_NO_ALIASES
#    define TEST CUTE_TEST
#    define RUN_TESTS CUTE_RUN_TESTS
#    define DUMP_RESULT CUTE_DUMP_RESULT
#    define RUN CUTE_RUN
#    define GROUP CUTE_GROUP
#    define FAIL CUTE_FAIL
#    define FAIL_MSG CUTE_FAIL_MSG
#    define EXPECT CUTE_EXPECT
#    define EXPECT_MSG CUTE_EXPECT_MSG
#    define ASSERT CUTE_ASSERT
#    define ASSERT_MSG CUTE_ASSERT_MSG
#endif // CUTE_NO_ALIASES

#ifdef __GNUC__
#    define CUTE_ATTRIBUTE(attr) __attribute__((attr))
#else // __GNUC__
#    define CUTE_ATTRIBUTE(attr)
#endif // __GNUC__

#define CUTE_ANSI_RED "\x1b[31m"
#define CUTE_ANSI_GREEN "\x1b[32m"
#define CUTE_ANSI_BLUE "\x1b[34m"
#define CUTE_ANSI_RESET "\x1b[m"

#define CUTE_COLOR_INFO(s) CUTE_ANSI_BLUE s CUTE_ANSI_RESET
#define CUTE_COLOR_ERROR(s) CUTE_ANSI_RED s CUTE_ANSI_RESET
#define CUTE_COLOR_SUCCESS(s) CUTE_ANSI_GREEN s CUTE_ANSI_RESET

#define CUTE_PP_STRINGIZE(x) CUTE_PP_STRINGIZE_I(x)
#define CUTE_PP_STRINGIZE_I(x) #x
#define CUTE_PP_SPREAD(tup) CUTE_PP_SPREAD_I tup
#define CUTE_PP_SPREAD_I(...) __VA_ARGS__

#define CUTE_TESTING cute_testing

#define CUTE_TEST_FUNCTION(testname) CUTE_TEST_FUNCTION_I(testname)
#define CUTE_TEST_FUNCTION_I(testname) cute_test_##testname

#define CUTE_TEST(testname) CUTE_TEST_I(CUTE_TESTING, testname)
#define CUTE_TEST_I(t, testname) void CUTE_TEST_FUNCTION(testname)(cute_testing_t * (t))

#define CUTE_RUN_TESTS() CUTE_RUN_TESTS_I(CUTE_TESTING)
#define CUTE_RUN_TESTS_I(t) for (cute_testing_t * (t) = cute_testing_new(__FILE__, __LINE__); (t); cute_testing_delete((t)), (t) = NULL)

#define CUTE_DUMP_RESULT() CUTE_DUMP_RESULT_I(CUTE_TESTING)
#define CUTE_DUMP_RESULT_I(t) cute_dump_result((t))

#define CUTE_RUN(testname) CUTE_RUN_I(CUTE_TESTING, testname)
#define CUTE_RUN_I(t, testname)                                                                                                                                                                        \
    do {                                                                                                                                                                                               \
        extern void CUTE_TEST_FUNCTION(testname)(cute_testing_t *);                                                                                                                                    \
        cute_run_test((t), &CUTE_TEST_FUNCTION(testname), CUTE_PP_STRINGIZE(testname), __FILE__, __LINE__);                                                                                            \
    } while (0)

#define CUTE_GROUP(...) CUTE_GROUP_I(CUTE_TESTING, __VA_ARGS__)
#define CUTE_GROUP_I(t, ...) for (cute_testing_t *_cute_tmp = (t), (*t) = CUTE_GROUP_START(_cute_tmp, __VA_ARGS__); (t); CUTE_GROUP_FINISH((t)), (t) = NULL)
#define CUTE_GROUP_START(t, ...) cute_group_start((t), __FILE__, __LINE__, __VA_ARGS__)
#define CUTE_GROUP_FINISH(t) cute_group_finish((t))

#define CUTE_FAIL() CUTE_FAIL_I(CUTE_TESTING)
#define CUTE_FAIL_I(t)                                                                                                                                                                                 \
    do {                                                                                                                                                                                               \
        cute_fail((t), __FILE__, __LINE__);                                                                                                                                                            \
        return;                                                                                                                                                                                        \
    } while (0)

#define CUTE_FAIL_MSG(...) CUTE_FAIL_MSG_I(CUTE_TESTING, __VA_ARGS__)
#define CUTE_FAIL_MSG_I(t, ...)                                                                                                                                                                        \
    do {                                                                                                                                                                                               \
        cute_fail_msg((t), __FILE__, __LINE__, __VA_ARGS__);                                                                                                                                           \
        return;                                                                                                                                                                                        \
    } while (0)

#define CUTE_EXPECT(actual, pred) CUTE_EXPECT_I(CUTE_TESTING, actual, CUTE_PRED_##pred)
#define CUTE_EXPECT_I(t, actual, p) CUTE_EXPECT_II(t, actual, p)
#define CUTE_EXPECT_II(t, actual, cond, desc, ...) CUTE_EXPECT_III(t, cond(actual, __VA_ARGS__), desc(actual, __VA_ARGS__))
#define CUTE_EXPECT_III(t, ok, s)                                                                                                                                                                      \
    do {                                                                                                                                                                                               \
        cute_expect((t), (ok), (s), __FILE__, __LINE__);                                                                                                                                               \
    } while (0)

#define CUTE_EXPECT_MSG(actual, pred, ...) CUTE_EXPECT_MSG_I(CUTE_TESTING, actual, (__VA_ARGS__), CUTE_PRED_##pred)
#define CUTE_EXPECT_MSG_I(t, actual, fmt, p) CUTE_EXPECT_MSG_II(t, actual, fmt, p)
#define CUTE_EXPECT_MSG_II(t, actual, fmt, cond, desc, ...) CUTE_EXPECT_MSG_III(t, fmt, cond(actual, __VA_ARGS__), desc(actual, __VA_ARGS__))
#define CUTE_EXPECT_MSG_III(t, fmt, ok, s)                                                                                                                                                             \
    do {                                                                                                                                                                                               \
        cute_expect_msg((t), (ok), (s), __FILE__, __LINE__, CUTE_PP_SPREAD(fmt));                                                                                                                      \
    } while (0)

#define CUTE_ASSERT(actual, pred) CUTE_ASSERT_I(CUTE_TESTING, actual, CUTE_PRED_##pred)
#define CUTE_ASSERT_I(t, actual, p) CUTE_ASSERT_II(t, actual, p)
#define CUTE_ASSERT_II(t, actual, cond, desc, ...) CUTE_ASSERT_III(t, cond(actual, __VA_ARGS__), desc(actual, __VA_ARGS__))
#define CUTE_ASSERT_III(t, ok, s)                                                                                                                                                                      \
    do {                                                                                                                                                                                               \
        if (!cute_assert((t), (ok), (s), __FILE__, __LINE__)) {                                                                                                                                        \
            return;                                                                                                                                                                                    \
        }                                                                                                                                                                                              \
    } while (0)

#define CUTE_ASSERT_MSG(actual, pred, ...) CUTE_ASSERT_MSG_I(CUTE_TESTING, actual, (__VA_ARGS__), CUTE_PRED_##pred)
#define CUTE_ASSERT_MSG_I(t, actual, fmt, p) CUTE_ASSERT_MSG_II(t, actual, fmt, p)
#define CUTE_ASSERT_MSG_II(t, actual, fmt, cond, desc, ...) CUTE_ASSERT_MSG_III(t, fmt, cond(actual, __VA_ARGS__), desc(actual, __VA_ARGS__))
#define CUTE_ASSERT_MSG_III(t, fmt, ok, s)                                                                                                                                                             \
    do {                                                                                                                                                                                               \
        if (!cute_assert_msg((t), (ok), (s), __FILE__, __LINE__, CUTE_PP_SPREAD(fmt))) {                                                                                                               \
            return;                                                                                                                                                                                    \
        }                                                                                                                                                                                              \
    } while (0)

// EXPECT(actual, is_true)
#define CUTE_PRED_is_true CUTE_PRED_is_true_COND, CUTE_PRED_is_true_DESC, _
#define CUTE_PRED_is_true_COND(actual, _) ((_Bool)(actual))
#define CUTE_PRED_is_true_DESC(actual, _) CUTE_PP_STRINGIZE(actual) " == true"

// EXPECT(actual, is_false)
#define CUTE_PRED_is_false CUTE_PRED_is_false_COND, CUTE_PRED_is_false_DESC, _
#define CUTE_PRED_is_false_COND(actual, _) (!(_Bool)(actual))
#define CUTE_PRED_is_false_DESC(actual, _) CUTE_PP_STRINGIZE(actual) " == false"

// EXPECT(actual, is_null)
#define CUTE_PRED_is_null CUTE_PRED_is_null_COND, CUTE_PRED_is_null_DESC, _
#define CUTE_PRED_is_null_COND(actual, _) ((actual) == NULL)
#define CUTE_PRED_is_null_DESC(actual, _) CUTE_PP_STRINGIZE(actual) " == NULL"

// EXPECT(actual, is_not_null)
#define CUTE_PRED_is_not_null CUTE_PRED_is_not_null_COND, CUTE_PRED_is_not_null_DESC, _
#define CUTE_PRED_is_not_null_COND(actual, _) ((actual) != NULL)
#define CUTE_PRED_is_not_null_DESC(actual, _) CUTE_PP_STRINGIZE(actual) " != NULL"

// EXPECT(actual, is(op, x))
#define CUTE_PRED_is(op, x) CUTE_PRED_is_COND, CUTE_PRED_is_DESC, op, x
#define CUTE_PRED_is_COND(actual, op, x) ((actual)op(x))
#define CUTE_PRED_is_DESC(actual, op, x) CUTE_PP_STRINGIZE(actual) " " CUTE_PP_STRINGIZE(op) " " CUTE_PP_STRINGIZE(expected)

// EXPECT(actual, eq(x))
#define CUTE_PRED_eq(x) CUTE_PRED_is(==, x)

// EXPECT(actual, ne(x))
#define CUTE_PRED_ne(x) CUTE_PRED_is(!=, x)

// EXPECT(actual, lt(x))
#define CUTE_PRED_lt(x) CUTE_PRED_is(<, x)

// EXPECT(actual, le(x))
#define CUTE_PRED_le(x) CUTE_PRED_is(<=, x)

// EXPECT(actual, gt(x))
#define CUTE_PRED_gt(x) CUTE_PRED_is(>, x)

// EXPECT(actual, ge(x))
#define CUTE_PRED_ge(x) CUTE_PRED_is(>=, x)

// EXPECT(actual, eq_str(expected))
#define CUTE_PRED_eq_str(expected) CUTE_PRED_eq_str_COND, CUTE_PRED_eq_str_DESC, expected
#define CUTE_PRED_eq_str_COND(actual, expected) (cute_pred_eq_str((actual), (expected)))
#define CUTE_PRED_eq_str_DESC(actual, expected) CUTE_PP_STRINGIZE(actual) " == " CUTE_PP_STRINGIZE(expected)

// EXPECT((actual, actual_len), eq_str_n(expected))
#define CUTE_PRED_eq_str_n(expected) CUTE_PRED_eq_str_n_COND, CUTE_PRED_eq_str_n_DESC, expected
#define CUTE_PRED_eq_str_n_COND(actual, expected) (cute_pred_eq_str_n((CUTE_PRED_eq_str_n_STR actual), (CUTE_PRED_eq_str_n_LEN actual), (expected)))
#define CUTE_PRED_eq_str_n_DESC(actual, expected) CUTE_PP_STRINGIZE(CUTE_PRED_eq_str_n_STR actual) " == " CUTE_PP_STRINGIZE(expected)
#define CUTE_PRED_eq_str_n_STR(str, len) str
#define CUTE_PRED_eq_str_n_LEN(str, len) len

// EXPECT(actual, contains(expected))
#define CUTE_PRED_contains(expected) CUTE_PRED_contains_COND, CUTE_PRED_contains_DESC, expected
#define CUTE_PRED_contains_COND(actual, expected) (cute_pred_contains((actual), (expected)))
#define CUTE_PRED_contains_DESC(actual, expected) CUTE_PP_STRINGIZE(actual) " contains " CUTE_PP_STRINGIZE(expected)

// EXPECT(actual, not(pred))
#define CUTE_PRED_not(pred) CUTE_PRED_not_COND, CUTE_PRED_not_DESC, CUTE_PRED_##pred
#define CUTE_PRED_not_COND(actual, cond, desc, ...) (!cond((actual), __VA_ARGS__))
#define CUTE_PRED_not_DESC(actual, cond, desc, ...) "not " desc(actual, __VA_ARGS__)

typedef enum cute_testing_type_t {
    cute_testing_type_group,
    cute_testing_type_assertion,
} cute_testing_type_t;

typedef struct cute_testing_t {
    struct cute_testing_t *parent;
    struct cute_testing_t *next;
    cute_testing_type_t type;
    const char *file;
    unsigned long long line;

    union {
        struct {
            char *name;
            struct cute_testing_t *head, **tail;
            clock_t start_time, end_time;
        } group;

        struct {
            int ok;
            const char *desc;
            char *message;
        } assertion;
    } _;
} cute_testing_t;

static inline cute_testing_t *cute_testing_new(const char *file, unsigned long long line) {
    cute_testing_t *t = malloc(sizeof(cute_testing_t));
    t->parent = NULL;
    t->next = NULL;
    t->type = cute_testing_type_group;
    t->file = file;
    t->line = line;
    t->_.group.name = NULL;
    t->_.group.head = NULL;
    t->_.group.tail = &t->_.group.head;
    t->_.group.start_time = clock();
    t->_.group.end_time = t->_.group.start_time;

    return t;
}

static inline void cute_testing_delete(cute_testing_t *t) {
    while (t != NULL) {
        cute_testing_t *next = t->next;

        switch (t->type) {
        case cute_testing_type_group:
            cute_testing_delete(t->_.group.head);
            free(t->_.group.name);
            break;
        case cute_testing_type_assertion:
            free(t->_.assertion.message);
            break;
        default:
            break;
        }
        free(t);

        t = next;
    }
}

static inline int cute_testing_result(const cute_testing_t *t) {
    assert(t != NULL);

    switch (t->type) {
    case cute_testing_type_group:
        for (const cute_testing_t *u = t->_.group.head; u != NULL; u = u->next) {
            if (cute_testing_result(u)) {
                return 1;
            }
        }
        return 0;
    case cute_testing_type_assertion:
        if (t->_.assertion.ok) {
            return 0;
        }
        return 1;
    default:
        return 2;
    }
}

static inline void cute_testing_dump_i(const cute_testing_t *t, FILE *fp, unsigned depth) {
    assert(t != NULL);

    for (unsigned i = 0; i < depth; i++) {
        fprintf(fp, "  ");
    }

    switch (t->type) {
    case cute_testing_type_group: {
        unsigned total = 0;
        unsigned failed = 0;
        for (const cute_testing_t *u = t->_.group.head; u != NULL; u = u->next) {
            if (cute_testing_result(u)) {
                failed++;
            }
            total++;
        }

        const double duration = 1e3 * (t->_.group.end_time - t->_.group.start_time) / CLOCKS_PER_SEC;
        if (failed == 0) {
            fprintf(fp, CUTE_COLOR_SUCCESS("[  OK  ] %s"), t->_.group.name);
            fprintf(fp, " ");
            fprintf(fp, CUTE_COLOR_INFO("[%u/%u]"), total - failed, total);
            fprintf(fp, " - ");
            fprintf(fp, CUTE_COLOR_INFO("%.1f ms"), duration);
            fprintf(fp, "\n");
        } else {
            fprintf(fp, CUTE_COLOR_ERROR("[FAILED] %s"), t->_.group.name);
            fprintf(fp, " ");
            fprintf(fp, CUTE_COLOR_INFO("[%u/%u]"), total - failed, total);
            fprintf(fp, " - ");
            fprintf(fp, CUTE_COLOR_INFO("%.1f ms"), duration);
            fprintf(fp, " at %s:%llu", t->file, t->line);
            fprintf(fp, "\n");

            for (const cute_testing_t *u = t->_.group.head; u != NULL; u = u->next) {
                cute_testing_dump_i(u, fp, depth + 1);
            }
        }
        break;
    }
    case cute_testing_type_assertion: {
        if (t->_.assertion.ok) {
            fprintf(fp, CUTE_COLOR_SUCCESS("[  OK  ] %s"), t->_.assertion.desc);
            if (t->_.assertion.message != NULL) {
                fprintf(fp, CUTE_COLOR_SUCCESS(": %s"), t->_.assertion.message);
            }
        } else {
            fprintf(fp, CUTE_COLOR_ERROR("[FAILED] %s"), t->_.assertion.desc);
            if (t->_.assertion.message != NULL) {
                fprintf(fp, CUTE_COLOR_ERROR(": %s"), t->_.assertion.message);
            }
            fprintf(fp, " at %s:%llu", t->file, t->line);
        }
        fprintf(fp, "\n");
        break;
    }
    default:
        break;
    }
}

static inline void cute_testing_dump(const cute_testing_t *t) {
    assert(t != NULL);
    assert(t->type == cute_testing_type_group);

    for (const cute_testing_t *u = t->_.group.head; u != NULL; u = u->next) {
        cute_testing_dump_i(u, stderr, 0);
    }
}

static inline int cute_dump_result(cute_testing_t *t) {
    assert(t != NULL);
    assert(t->type == cute_testing_type_group);

    t->_.group.end_time = clock();

    const int result = cute_testing_result(t);
    cute_testing_dump(t);

    cute_testing_delete(t);

    return result;
}

static inline void cute_run_test(cute_testing_t *t, void (*test_function)(cute_testing_t *), const char *name, const char *file, unsigned long long line) {
    assert(t != NULL);
    assert(t->type == cute_testing_type_group);
    assert(test_function != NULL);
    assert(name != NULL);

    cute_testing_t *g = malloc(sizeof(cute_testing_t));
    g->type = cute_testing_type_group;
    g->parent = t;
    g->next = NULL;
    g->file = file;
    g->line = line;
    g->_.group.name = strdup(name);
    g->_.group.head = NULL;
    g->_.group.tail = &g->_.group.head;
    g->_.group.start_time = clock();
    g->_.group.end_time = g->_.group.start_time;

    *t->_.group.tail = g;
    t->_.group.tail = &g->next;

    test_function(g);

    g->_.group.end_time = clock();
}

static inline cute_testing_t *cute_group_start(cute_testing_t *t, const char *file, unsigned long long line, const char *format, ...) CUTE_ATTRIBUTE(format(printf, 4, 5));

static inline cute_testing_t *cute_group_start(cute_testing_t *t, const char *file, unsigned long long line, const char *format, ...) {
    assert(t != NULL);
    assert(t->type == cute_testing_type_group);

    cute_testing_t *g = malloc(sizeof(cute_testing_t));
    g->type = cute_testing_type_group;
    g->parent = t;
    g->next = NULL;
    g->file = file;
    g->line = line;
    g->_.group.name = NULL;

    va_list args;
    va_start(args, format);
    vasprintf(&g->_.group.name, format, args);
    va_end(args);

    g->_.group.head = NULL;
    g->_.group.tail = &g->_.group.head;
    g->_.group.start_time = clock();
    g->_.group.end_time = g->_.group.start_time;

    *t->_.group.tail = g;
    t->_.group.tail = &g->next;

    return g;
}

static inline void cute_group_finish(cute_testing_t *t) {
    assert(t != NULL);
    assert(t->type == cute_testing_type_group);

    t->_.group.end_time = clock();
}

static inline void cute_push_assert(cute_testing_t *t, int ok, const char *desc, const char *file, unsigned long long line, const char *format, va_list args) {
    assert(t != NULL);
    assert(t->type == cute_testing_type_group);

    cute_testing_t *a = malloc(sizeof(cute_testing_t));
    a->type = cute_testing_type_assertion;
    a->parent = t;
    a->next = NULL;
    a->file = file;
    a->line = line;
    a->_.assertion.ok = ok;
    a->_.assertion.desc = desc;
    a->_.assertion.message = NULL;

    if (format != NULL) {
        vasprintf(&a->_.assertion.message, format, args);
    }

    *t->_.group.tail = a;
    t->_.group.tail = &a->next;

    if (!ok) {
        for (cute_testing_t *p = t; p != NULL; p = p->parent) {
            t->_.group.end_time = clock();
        }
    }
}

static inline void cute_fail_msg(cute_testing_t *t, const char *file, unsigned long long line, const char *format, ...) CUTE_ATTRIBUTE(format(printf, 4, 5));
static inline void cute_expect_msg(cute_testing_t *t, int ok, const char *desc, const char *file, unsigned long long line, const char *format, ...) CUTE_ATTRIBUTE(format(printf, 6, 7));
static inline int cute_assert_msg(cute_testing_t *t, int ok, const char *desc, const char *file, unsigned long long line, const char *format, ...) CUTE_ATTRIBUTE(format(printf, 6, 7));

static inline void cute_fail(cute_testing_t *t, const char *file, unsigned long long line) { cute_push_assert(t, 0, "fail", file, line, NULL, NULL); }

static inline void cute_fail_msg(cute_testing_t *t, const char *file, unsigned long long line, const char *format, ...) {
    va_list args;
    va_start(args, format);
    cute_push_assert(t, 0, "fail", file, line, format, args);
    va_end(args);
}

static inline void cute_expect(cute_testing_t *t, int ok, const char *desc, const char *file, unsigned long long line) { cute_push_assert(t, ok, desc, file, line, NULL, NULL); }

static inline void cute_expect_msg(cute_testing_t *t, int ok, const char *desc, const char *file, unsigned long long line, const char *format, ...) {
    va_list args;
    va_start(args, format);
    cute_push_assert(t, ok, desc, file, line, format, args);
    va_end(args);
}

static inline int cute_assert(cute_testing_t *t, int ok, const char *desc, const char *file, unsigned long long line) {
    cute_push_assert(t, ok, desc, file, line, NULL, NULL);
    return ok;
}

static inline int cute_assert_msg(cute_testing_t *t, int ok, const char *desc, const char *file, unsigned long long line, const char *format, ...) {
    va_list args;
    va_start(args, format);
    cute_push_assert(t, ok, desc, file, line, format, args);
    va_end(args);

    return ok;
}

static inline int cute_pred_eq_str(const char *actual, const char *expected) {
    assert(expected != NULL);
    if (actual == NULL) {
        return 0;
    }
    return strcmp(actual, expected) == 0;
}

static inline int cute_pred_eq_str_n(const char *actual, size_t len, const char *expected) {
    assert(expected != NULL);
    if (actual == NULL) {
        return 0;
    }
    if (len != strlen(expected)) {
        return 0;
    }
    return strncmp(actual, expected, len) == 0;
}

static inline int cute_pred_contains(const char *actual, const char *expected) {
    assert(expected != NULL);
    if (actual == NULL) {
        return 0;
    }
    return strstr(actual, expected) != NULL;
}

#endif // INCLUDE_CUTE_H

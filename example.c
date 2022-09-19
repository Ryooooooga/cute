#include "cute.h"

int factorial(int n) {
    if (n > 0) {
        return n * factorial(n - 1);
    }
    return 1;
}

TEST(factorial) {
    EXPECT(factorial(0), eq(1));
    EXPECT(factorial(1), eq(1));
    EXPECT_MSG(factorial(5), eq(120), "5! == 120 (actual %d)", factorial(5));
}

TEST(string) {
    const char *s = "Hello, world!";

    ASSERT(s, is_not_null);
    ASSERT(s, not(is_null));
    EXPECT(s, eq_str("Hello, world!"));
    EXPECT((s, 4), eq_str_n("Hell"));
    EXPECT(s, contains("world"));
    EXPECT(s, not(contains("nya")));
}

TEST(boolean) {
    EXPECT(1, is_true);
    EXPECT(0, is_false);
}

TEST(numeric) {
    GROUP("integer") {
        int n = 42;

        EXPECT(n, eq(42));
        EXPECT(n, ne(41));
        EXPECT(n, lt(43));
        EXPECT(n, le(42));
        EXPECT(n, gt(41));
        EXPECT(n, ge(42));
        // same as
        EXPECT(n, is(==, 42));
        EXPECT(n, is(!=, 41));
        EXPECT(n, is(<, 43));
        EXPECT(n, is(<=, 42));
        EXPECT(n, is(>, 41));
        EXPECT(n, is(>=, 42));
    }

    GROUP("floating point") {
        double f = 3.14159265;

        EXPECT(f, ne(0));
        EXPECT(f, lt(3.15));
        EXPECT(f, le(3.15));
        EXPECT(f, gt(3.14));
        EXPECT(f, ge(3.14));
        // same as
        EXPECT(f, is(!=, 0));
        EXPECT(f, is(<, 3.15));
        EXPECT(f, is(<=, 3.15));
        EXPECT(f, is(>, 3.14));
        EXPECT(f, is(>=, 3.14));
    }
}

// TEST(fail) { FAIL(); }

static void setup(void) {}

static void teardown(void) {}

int main(void) {
    RUN_TESTS() {
        BEFORE_EACH(setup);
        AFTER_EACH(teardown);

        RUN(factorial);
        RUN(string);
        RUN(boolean);
        RUN(numeric);
        // RUN(fail);
        return DUMP_RESULT();
    }
}

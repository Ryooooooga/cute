#define CUTE_MAIN
#include "cute.h"

#include <math.h>

int factorial(int n) {
    if (n > 0) {
        return n * factorial(n - 1);
    }
    return 1;
}

TEST(factorial) {
    EXPECT(factorial(0), eq(1));
    EXPECT(factorial(1), eq(1));
    EXPECT(factorial(5), eq(120), "5! == %d (actual %d)", _1, ACTUAL);
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
    SUBTEST("integer") {
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

    SUBTEST("floating point") {
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

        EXPECT(sqrt(2.0), near(1.4142, 1e-4));
    }
}

TEST(fail) {
    // FAIL();
}

static void setup(void) {
}

static void teardown(void) {
}

int main(void) {
    BEFORE_EACH(setup);
    AFTER_EACH(teardown);

    return RUN_ALL();
}

# CUTE

❤️ Simple header only Unit Testing Framework for C99 ❤️

## Examples

```c
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
    EXPECT(s, eq_str("Hello, world!"));
    EXPECT((s, 4), eq_str_n("Hell"));
    EXPECT(s, contains("world"));
    EXPECT(s, not(contains("nya")));
}

int main(void) {
    RUN_TESTS() {
        RUN(factorial);
        RUN(string);
        return DUMP_RESULT();
    }
}
```

See [example.c](example.c).

## Usage

### via CMake

```cmake
include(FetchContent)

FetchContent_Declare(cute
    GIT_REPOSITORY "https://github.com/Ryooooooga/cute.git"
    GIT_TAG        "main"
)

FetchContent_MakeAvailable(cute)

add_executable(your_project your_src.c)
target_link_library(your_project cute)
```

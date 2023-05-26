# CUTE

❤️ Simple header only Unit Testing Framework for C99 ❤️

## Examples

```c
#define CUTE_MAIN
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
    EXPECT(factorial(5), eq(120), "5! == 120 (actual %d)", ACTUAL);
}

TEST(string) {
    const char *s = "Hello, world!";

    ASSERT(s, is_not_null);
    ASSERT(s, not(is_null));
    EXPECT(s, eq_str("Hello, world!"));
    EXPECT((s, 4), eq_str("Hell"));
    EXPECT(s, contains("world"));
    EXPECT(s, not(contains("nya")));
}

int main(void) {
    return RUN_ALL();
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

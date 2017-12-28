#include "Compare.hpp"

#include "Logging.hpp"

namespace komb {
namespace {

bool isDigit(const char c)
{
    return std::isdigit(static_cast<unsigned char>(c));
}

const char* findNextNonDigit(const char* ptr)
{
    while (isDigit(*ptr)) { ++ptr; }
    return ptr;
}

} // namespace

int numberAwareStrcmp(const char* a, const char* b)
{
    CHECK_NOTNULL(a);
    CHECK_NOTNULL(b);
    for (;;)
    {
        if (!*a && *b)  { return -1; } // a is shorter than b
        if (!*a && !*b) { return  0; } // equal length
        if (*a  && !*b) { return +1; } // a is longer than b

        if (isDigit(*a) && isDigit(*b))
        {
            // Start of a number!
            // Ignore leading zeros:
            while (*a == '0') { ++a; }
            while (*b == '0') { ++b; }

            const char* non_digit_a = findNextNonDigit(a);
            const char* non_digit_b = findNextNonDigit(b);
            const long a_len = non_digit_a - a;
            const long b_len = non_digit_b - b;
            if (a_len < b_len) { return -1; } // Shorter number is always smaller
            if (a_len > b_len) { return +1; } // Longer number is always larger

            // Same length numbers. Compare them:
            for (long i = 0; i < a_len; ++i)
            {
                if (a[i] != b[i])
                {
                    return a[i] < b[i] ? -1 : +1;
                }
            }

            // Identical!
            a += a_len;
            b += b_len;
            continue;
        }

        if (*a  == *b)
        {
            // identical non-digits.
            ++a;
            ++b;
            continue;
        }

        return *a < *b ? -1 : +1;
    }
}

} // namespace komb

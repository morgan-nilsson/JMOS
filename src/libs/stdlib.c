#include "libs/stdlib.h"

int atoi(const char* nstr) {
    int result = 0;
    int sign = 1;

    // Skip leading whitespace
    while (*nstr == ' ' || *nstr == '\t' || *nstr == '\n' || *nstr == '\r' || *nstr == '\f' || *nstr == '\v') {
        nstr++;
    }

    // Handle optional sign
    if (*nstr == '-') {
        sign = -1;
        nstr++;
    } else if (*nstr == '+') {
        nstr++;
    }

    // Convert digits to integer
    while (*nstr >= '0' && *nstr <= '9') {
        result = result * 10 + (*nstr - '0');
        nstr++;
    }

    return sign * result;
}

long int atol(const char* nptr) {
    long int result = 0;
    int sign = 1;

    // Skip leading whitespace
    while (*nptr == ' ' || *nptr == '\t' || *nptr == '\n' || *nptr == '\r' || *nptr == '\f' || *nptr == '\v') {
        nptr++;
    }

    // Handle optional sign
    if (*nptr == '-') {
        sign = -1;
        nptr++;
    } else if (*nptr == '+') {
        nptr++;
    }

    // Convert digits to long integer
    while (*nptr >= '0' && *nptr <= '9') {
        result = result * 10 + (*nptr - '0');
        nptr++;
    }

    return sign * result;
}

long long int atoll(const char* nptr) {
    long long int result = 0;
    int sign = 1;

    // Skip leading whitespace
    while (*nptr == ' ' || *nptr == '\t' || *nptr == '\n' || *nptr == '\r' || *nptr == '\f' || *nptr == '\v') {
        nptr++;
    }

    // Handle optional sign
    if (*nptr == '-') {
        sign = -1;
        nptr++;
    } else if (*nptr == '+') {
        nptr++;
    }

    // Convert digits to long long integer
    while (*nptr >= '0' && *nptr <= '9') {
        result = result * 10 + (*nptr - '0');
        nptr++;
    }

    return sign * result;
}

long int strtol(const char* restrict nptr, char** restrict endptr, int base) {
    long int result = 0;
    int sign = 1;

    // Skip leading whitespace
    while (*nptr == ' ' || *nptr == '\t' || *nptr == '\n' || *nptr == '\r' || *nptr == '\f' || *nptr == '\v') {
        nptr++;
    }

    // Handle optional sign
    if (*nptr == '-') {
        sign = -1;
        nptr++;
    } else if (*nptr == '+') {
        nptr++;
    }

    // Handle optional base prefix
    if (base == 0) {
        if (*nptr == '0') {
            if (nptr[1] == 'x' || nptr[1] == 'X') {
                base = 16;
                nptr += 2;
            } else {
                base = 8;
                nptr++;
            }
        } else {
            base = 10;
        }
    } else if (base == 16) {
        if (nptr[0] == '0' && (nptr[1] == 'x' || nptr[1] == 'X')) {
            nptr += 2;
        }
    }

    // Convert digits to long integer
    while (*nptr) {
        int digit;
        if (*nptr >= '0' && *nptr <= '9') {
            digit = *nptr - '0';
        } else if (*nptr >= 'a' && *nptr <= 'z') {
            digit = *nptr - 'a' + 10;
        } else if (*nptr >= 'A' && *nptr <= 'Z') {
            digit = *nptr - 'A' + 10;
        } else {
            break;
        }

        if (digit >= base) {
            break;
        }

        result = result * base + digit;
        nptr++;
    }

    if (endptr != NULL) {
        *endptr = (char*)nptr;
    }

    return sign * result;
}

long long int strtoll(const char* restrict nptr, char** restrict endptr, int base) {
    long long int result = 0;
    int sign = 1;

    // Skip leading whitespace
    while (*nptr == ' ' || *nptr == '\t' || *nptr == '\n' || *nptr == '\r' || *nptr == '\f' || *nptr == '\v') {
        nptr++;
    }

    // Handle optional sign
    if (*nptr == '-') {
        sign = -1;
        nptr++;
    } else if (*nptr == '+') {
        nptr++;
    }

    // Handle optional base prefix
    if (base == 0) {
        if (*nptr == '0') {
            if (nptr[1] == 'x' || nptr[1] == 'X') {
                base = 16;
                nptr += 2;
            } else {
                base = 8;
                nptr++;
            }
        } else {
            base = 10;
        }
    } else if (base == 16) {
        if (nptr[0] == '0' && (nptr[1] == 'x' || nptr[1] == 'X')) {
            nptr += 2;
        }
    }

    // Convert digits to long long integer
    while (*nptr) {
        int digit;
        if (*nptr >= '0' && *nptr <= '9') {
            digit = *nptr - '0';
        } else if (*nptr >= 'a' && *nptr <= 'z') {
            digit = *nptr - 'a' + 10;
        } else if (*nptr >= 'A' && *nptr <= 'Z') {
            digit = *nptr - 'A' + 10;
        } else {
            break;
        }

        if (digit >= base) {
            break;
        }

        result = result * base + digit;
        nptr++;
    }

    if (endptr != NULL) {
        *endptr = (char*)nptr;
    }

    return sign * result;
}

unsigned long int strtoul(const char* restrict nptr, char** restrict endptr, int base) {
    unsigned long int result = 0;
    int negative = 0;

    // Skip leading whitespace
    while (*nptr == ' ' || *nptr == '\t' || *nptr == '\n' || *nptr == '\r' || *nptr == '\f' || *nptr == '\v') {
        nptr++;
    }

    if (*nptr == '-') {
        negative = 1;
        nptr++;
    } else if (*nptr == '+') {
        nptr++;
    }

    // Handle optional base prefix
    if (base == 0) {
        if (*nptr == '0') {
            if (nptr[1] == 'x' || nptr[1] == 'X') {
                base = 16;
                nptr += 2;
            } else {
                base = 8;
                nptr++;
            }
        } else {
            base = 10;
        }
    } else if (base == 16) {
        if (nptr[0] == '0' && (nptr[1] == 'x' || nptr[1] == 'X')) {
            nptr += 2;
        }
    }

    // Convert digits to unsigned long integer
    while (*nptr) {
        int digit;
        if (*nptr >= '0' && *nptr <= '9') {
            digit = *nptr - '0';
        } else if (*nptr >= 'a' && *nptr <= 'z') {
            digit = *nptr - 'a' + 10;
        } else if (*nptr >= 'A' && *nptr <= 'Z') {
            digit = *nptr - 'A' + 10;
        } else {
            break;
        }

        if (digit >= base) {
            break;
        }

        result = result * base + digit;
        nptr++;
    }

    if (endptr != NULL) {
        *endptr = (char*)nptr;
    }

    return negative ? (0UL - result) : result;
}

unsigned long long int strtoull(const char* restrict nptr, char** restrict endptr, int base) {
    unsigned long long int result = 0;
    int negative = 0;

    // Skip leading whitespace
    while (*nptr == ' ' || *nptr == '\t' || *nptr == '\n' || *nptr == '\r' || *nptr == '\f' || *nptr == '\v') {
        nptr++;
    }

    if (*nptr == '-') {
        negative = 1;
        nptr++;
    } else if (*nptr == '+') {
        nptr++;
    }

    // Handle optional base prefix
    if (base == 0) {
        if (*nptr == '0') {
            if (nptr[1] == 'x' || nptr[1] == 'X') {
                base = 16;
                nptr += 2;
            } else {
                base = 8;
                nptr++;
            }
        } else {
            base = 10;
        }
    } else if (base == 16) {
        if (nptr[0] == '0' && (nptr[1] == 'x' || nptr[1] == 'X')) {
            nptr += 2;
        }
    }

    // Convert digits to unsigned long long integer
    while (*nptr) {
        int digit;
        if (*nptr >= '0' && *nptr <= '9') {
            digit = *nptr - '0';
        } else if (*nptr >= 'a' && *nptr <= 'z') {
            digit = *nptr - 'a' + 10;
        } else if (*nptr >= 'A' && *nptr <= 'Z') {
            digit = *nptr - 'A' + 10;
        } else {
            break;
        }

        if (digit >= base) {
            break;
        }

        result = result * base + digit;
        nptr++;
    }

    if (endptr != NULL) {
        *endptr = (char*)nptr;
    }

    return negative ? (0ULL - result) : result;
}

char* itoa(int value, char* str, int base) {
    if (base < 2 || base > 36) {
        str[0] = '\0';
        return str;
    }

    char* ptr = str;
    unsigned int num;

    if (base == 10 && value < 0) {
        *ptr++ = '-';
        num = -(unsigned int)value;
    } else {
        num = (unsigned int)value;
    }

    // Convert number to string in reverse order
    char* start = ptr;
    do {
        unsigned int digit = num % (unsigned int)base;
        *ptr++ = (digit < 10) ? (char)('0' + digit) : (char)('a' + digit - 10);
        num /= (unsigned int)base;
    } while (num > 0);

    *ptr = '\0';

    // Reverse the string
    char* end = ptr - 1;
    while (start < end) {
        char temp = *start;
        *start++ = *end;
        *end-- = temp;
    }

    return str;
}

[[noreturn]] void abort(void) {
    while (1) {
#if defined(__x86_64__)
        __asm__ volatile ("hlt");
#endif
    }
}

int abs(int j) {
    return (j < 0) ? -j : j;
}

long int labs(long int j) {
    return (j < 0) ? -j : j;
}

long long int llabs(long long int j) {
    return (j < 0) ? -j : j;
}
#if __STDC_LIB_EXT1__ && defined(__STDC_WANT_LIB_EXT1__)

constraint_handler_t set_constraint_handler_s( constraint_handler_t handler ) {
    if (handler == NULL) {
        handler = DEFAULT_CONSTRAINT_HANDLER;
    }
    constraint_handler_t old_handler = contraint_handler;
    contraint_handler = handler;
    return old_handler;
}

// TODO: Implement abort_handler_s logging
void abort_handler_s(const char* restrict msg, void* restrict ptr, errno_t error) {
    (void)msg;
    (void)ptr;
    (void)error;
    abort();
}

void ignore_handler_s(const char* restrict msg, void* restrict ptr, errno_t error) {
    (void)msg;
    (void)ptr;
    (void)error;
    // Do nothing
}


#endif
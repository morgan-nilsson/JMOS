#include "libs/string.h"
#include "libs/errorno.h"
#include "libs/stdlib.h"

char* strcpy(char* restrict dest, const char* restrict src) {
    char* original_dest = dest;
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
    return original_dest;
}

char* strncpy(char* restrict dest, const char* restrict src, size_t n) {
    char* original_dest = dest;
    while (n > 0 && *src) {
        *dest++ = *src++;
        n--;
    }
    if (n > 0) {
        *dest = '\0';
    }
    return original_dest;
}

char* strcat(char* restrict dest, const char* restrict src) {
    char* original_dest = dest;
    while (*dest) {
        dest++;
    }
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
    return original_dest;
}

char* strncat(char* restrict dest, const char* restrict src, size_t n) {
    char* original_dest = dest;
    while (*dest) {
        dest++;
    }
    while (n > 0 && *src) {
        *dest++ = *src++;
        n--;
    }
    if (n > 0) {
        *dest = '\0';
    }
    return original_dest;
}

size_t strlen(const char* s) {
    const char* start = s;
    while (*s) {
        s++;
    }
    return s - start;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    while (n > 0 && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) {
        return 0;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

char* strchr(const char* s, int c) {
    while (*s) {
        if (*s == (char)c) {
            return (char*)s;
        }
        s++;
    }
    return NULL;
}

char* strrchr(const char* s, int c) {
    const char* last = NULL;
    while (*s) {
        if (*s == (char)c) {
            last = s;
        }
        s++;
    }
    return (char*)last;
}

size_t strcspn(const char* s, const char* reject) {
    const char* start = s;
    while (*s) {
        const char* r = reject;
        while (*r) {
            if (*s == *r) {
                return s - start;
            }
            r++;
        }
        s++;
    }
    return s - start;
}

size_t strspn(const char* s, const char* accept) {
    const char* start = s;
    while (*s) {
        const char* a = accept;
        bool found = false;
        while (*a) {
            if (*s == *a) {
                found = true;
                break;
            }
            a++;
        }
        if (!found) {
            return s - start;
        }
        s++;
    }
    return s - start;
}

char* strpbrk(const char* s, const char* accept) {
    while (*s) {
        const char* a = accept;
        while (*a) {
            if (*s == *a) {
                return (char*)s;
            }
            a++;
        }
        s++;
    }
    return NULL;
}

char* strstr(const char* haystack, const char* needle) {
    if (*needle == '\0') {
        return (char*)haystack;
    }
    while (*haystack) {
        const char* h = haystack;
        const char* n = needle;
        while (*h && *n && (*h == *n)) {
            h++;
            n++;
        }
        if (*n == '\0') {
            return (char*)haystack;
        }
        haystack++;
    }
    return NULL;
}

char* strtok(char* restrict str, const char* restrict delim) {
    static char* next_token = NULL;
    if (str) {
        next_token = str;
    } else if (next_token == NULL) {
        return NULL;
    }

    // Skip leading delimiters
    while (*next_token) {
        const char* d = delim;
        bool is_delim = false;
        while (*d) {
            if (*next_token == *d) {
                is_delim = true;
                break;
            }
            d++;
        }
        if (!is_delim) {
            break;
        }
        next_token++;
    }

    if (*next_token == '\0') {
        next_token = NULL;
        return NULL;
    }

    char* token_start = next_token;

    // Find the end of the token
    while (*next_token) {
        const char* d = delim;
        bool is_delim = false;
        while (*d) {
            if (*next_token == *d) {
                is_delim = true;
                break;
            }
            d++;
        }
        if (is_delim) {
            *next_token = '\0';
            next_token++;
            break;
        }
        next_token++;
    }

    return token_start;
}

void* memchr(const void* ptr, int ch, size_t n) {
    const unsigned char* p = (const unsigned char*)ptr;
    while (n--) {
        if (*p == (unsigned char)ch) {
            return (void*)p;
        }
        p++;
    }
    return NULL;
}

int memcmp(const void* ptr1, const void* ptr2, size_t n) {
    const unsigned char* p1 = (const unsigned char*)ptr1;
    const unsigned char* p2 = (const unsigned char*)ptr2;
    while (n--) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    return 0;
}

void* memset(void* ptr, int value, size_t n) {
    unsigned char* p = (unsigned char*)ptr;
    while (n--) {
        *p++ = (unsigned char)value;
    }
    return ptr;
}

void* memcpy(void* restrict dest, const void* restrict src, size_t n) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

void* memmove(void* dest, const void* src, size_t n) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    if (d < s) {
        while (n--) {
            *d++ = *s++;
        }
    } else {
        d += n;
        s += n;
        while (n--) {
            *--d = *--s;
        }
    }
    return dest;
}

char* strerror(errno_t errnum) {
    switch (errnum) {
        case 0: return "No error";
        case EPERM: return "Operation not permitted";
        case ENOENT: return "No such file or directory";
        case ESRCH: return "No such process";
        case EINTR: return "Interrupted system call";
        case EIO: return "Input/output error";
        case ENXIO: return "No such device or address";
        case E2BIG: return "Argument list too long";
        case ENOEXEC: return "Exec format error";
        case EBADF: return "Bad file descriptor";
        case ECHILD: return "No child processes";
        case EAGAIN: return "Resource temporarily unavailable";
        case ENOMEM: return "Out of memory";
        case EACCES: return "Permission denied";
        case EFAULT: return "Bad address";
        case ENOTBLK: return "Block device required";
        case EBUSY: return "Device or resource busy";
        case EEXIST: return "File exists";
        case EXDEV: return "Cross-device link";
        case ENODEV: return "No such device";
        case ENOTDIR: return "Not a directory";
        case EISDIR: return "Is a directory";
        case EINVAL: return "Invalid argument";
        case ENFILE: return "Too many open files in system";
        case EMFILE: return "Too many open files";
        case ENOTTY: return "Inappropriate ioctl for device";
        case ETXTBSY: return "Text file busy";
        case EFBIG: return "File too large";
        case ENOSPC: return "No space left on device";
        case ESPIPE: return "Illegal seek";
        case EROFS: return "Read-only file system";
        case EMLINK: return "Too many links";
        case EPIPE: return "Broken pipe";
        case EDOM: return "Math argument out of domain of func";
        case ERANGE: return "Math result not representable";
        default: return "Unknown error";
    }
}

#ifndef __STDC_LIB_EXT1__
#if defined(__STDC_WANT_LIB_EXT1__)

errno_t strcpy_s(char* restrict dest, size_t destsz, const char* restrict src) {
    if (dest == NULL || src == NULL) {
        constraint_handler("strcpy_s: dest and src must not be NULL", NULL, EINVAL);
        return EINVAL;
    }

    if (destsz == 0) {
        constraint_handler("strcpy_s: destsz must be greater than 0", NULL, ERANGE);
        return ERANGE;
    }

    if (destsz > RSIZE_MAX) {
        constrain_handler("strcpy_s: destsz exceeds RSIZE_MAX", NULL, ERANGE);
        return EINVAL;
    }

    if (destsz <= strnlen_s(src, destsz)) {
        constraint_handler("strcpy_s: destsz is too small to hold src", NULL, ERANGE);
        return ERANGE;
    }

    // overlap would occur
    if ((src < dest && src + strnlen_s(src, destsz) > dest) ||
        (dest < src && dest + strnlen_s(src, destsz) > src)) {
        constraint_handler("strcpy_s: source and destination buffers overlap", NULL, EINVAL);
        return EINVAL;
    }

    strcpy(dest, src);

    return 0;
}

#endif
#endif
#ifndef _errorno_h
#define _errorno_h

typedef int errno_t;

// Operation not permitted
#define EPERM 1
// No such file or directory
#define ENOENT 2
// No such process
#define ESRCH 3
// Interrupted system call
#define EINTR 4
// Input/output error
#define EIO 5
// No such device or address
#define ENXIO 6
// Argument list too long
#define E2BIG 7
// Exec format error
#define ENOEXEC 8
// Bad file descriptor
#define EBADF 9
// No child processes
#define ECHILD 10
// Resource temporarily unavailable
#define EAGAIN 11
// Out of memory
#define ENOMEM 12
// Permission denied
#define EACCES 13
// Bad address
#define EFAULT 14
// Block device required
#define ENOTBLK 15
// Device or resource busy
#define EBUSY 16
// File exists
#define EEXIST 17
// Cross-device link
#define EXDEV 18
// No such device
#define ENODEV 19
// Not a directory
#define ENOTDIR 20
// Is a directory
#define EISDIR 21
// Invalid argument
#define EINVAL 22
// Too many open files in system
#define ENFILE 23
// Too many open files
#define EMFILE 24
// Inappropriate ioctl for device
#define ENOTTY 25
// Text file busy
#define ETXTBSY 26
// File too large
#define EFBIG 27
// No space left on device
#define ENOSPC 28
// Illegal seek
#define ESPIPE 29
// Read-only file system
#define EROFS 30
// Too many links
#define EMLINK 31
// Broken pipe
#define EPIPE 32
// Numerical argument out of domain
#define EDOM 33
// Result too large
#define ERANGE 34

#endif
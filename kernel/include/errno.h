#ifndef _ERRNO_H
#define _ERRNO_H

extern int errno;

// Standard POSIX errno definitions
#define EPERM           1       // Operation not permitted
#define ENOENT          2       // No such file or directory
#define ESRCH           3       // No such process
#define EINTR           4       // Interrupted system call
#define EIO             5       // I/O error
#define ENXIO           6       // No such device or address
#define E2BIG           7       // Argument list too long
#define EBADF           9       // Bad file descriptor
#define ECHILD          10      // No child processes
#define EAGAIN          11      // Try again
#define ENOMEM          12      // Out of memory
#define EACCES          13      // Permission denied
#define EFAULT          14      // Bad address
#define EBUSY           16      // Device or resource busy
#define EEXIST          17      // File exists
#define ENODEV          19      // No such device
#define ENOTDIR         20      // Not a directory
#define EISDIR          21      // Is a directory
#define EINVAL          22      // Invalid argument
#define EMFILE          24      // Too many open files
#define EFBIG           27      // File too large
#define ENOSPC          28      // No space left on device
#define ESPIPE          29      // Illegal seek
#define EROFS           30      // Read-only file system
#define EMLINK          31      // Too many links
#define EDOM            33      // Math argument out of domain
#define ERANGE          34      // Math result not representable
#define ENAMETOOLONG    36      // File name too long
#define ENOSYS          38      // Function not implemented
#define ENOTEMPTY       39      // Directory not empty
#define EDEADLK         45      // Resource deadlock would occur
#define ENOLCK          46      // No record locks available
#define EOVERFLOW       75      // Value too large for defined data type
#define EILSEQ          84      // Illegal byte sequence
#define EDESTADDRREQ    89      // Destination address required
#define EPROTOTYPE      91      // Protocol wrong type for socket
#define ENOPROTOOPT     92      // Protocol not available
#define EAFNOSUPPORT    97      // Address family not supported
#define EADDRINUSE      98      // Address already in use
#define EADDRNOTAVAIL   99      // Cannot assign requested address
#define ENETDOWN        100     // Network is down
#define ENETUNREACH     101     // Network is unreachable
#define ECONNRESET      104     // Connection reset by peer
#define EISCONN         106     // Transport endpoint is already connected
#define ENOTCONN        107     // Transport endpoint is not connected
#define ETIMEDOUT       110     // Connection timed out
#define ECONNREFUSED    111     // Connection refused
#define EHOSTUNREACH    113     // No route to host
#define EALREADY        114     // Operation already in progress
#define EINPROGRESS     115     // Operation now in progress
#define ECANCELED       125     // Operation Canceled
#define ENOTSOCK        88      // Socket operation on non-socket
#define ENOTTY          25      // Not a typewriter
#define ENOSTR          60      // Device not a stream
#define ETIME           62      // Timer expired
#define ENOLINK         67      // Link has been severed
#define EPROTO          71      // Protocol error
#define EDQUOT          122     // Quota exceeded
#define EHOSTDOWN       112     // Host is down
#define ECONNABORTED    103     // Software caused connection abort
#define EPIPE           32      // Broken pipe
#define ETXTBSY         26      // Text file busy
#define ENFILE          23      // File table overflow
#define ENOBUFS         105     // No buffer space available
#define ENODATA         61      // No data available
#define ENOSR           63      // Out of streams resources
#define ENOTSUP         95      // Operation not supported
#define EPROTONOSUPPORT 93      // Protocol not supported
#define EXDEV           18      // Cross-device link
#define ENETRESET       102     // Network dropped connection because of reset

#endif

#ifndef ARIALIB_H
#define ARIALIB_H

/* Defines errors */
typedef int arialib_error;

#define ARIALIB_SUCCESS 1
#define ARIALIB_FILE_INVALID (-1)
#define ARIALIB_FILE_OPEN (-2)
#define ARIALIB_FILE_READ (-3)
#define ARIALIB_FILE_WRITE (-4)
#define ARIALIB_FILE_READONLY (-5)

#define ARIALIB_ALLOC (-6)
#define ARIALIB_NULL (-7)

#define ARIALIB_NEQ (-8)
#define ARIALIB_SAME_REF (-9)

#define ARIALIB_UNKNOWN (-10)

#endif
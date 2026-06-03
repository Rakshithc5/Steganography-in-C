#ifndef TYPES_H
#define TYPES_H

typedef unsigned int uint;

/* Status */
typedef enum
{
    e_success,
    e_failure
} Status;

/* Operation type */
typedef enum
{
    e_encode,
    e_decode,
    e_unsupported
} OperationType;

/* Common macros (single source of truth) */
#define MAX_FILE_SUFFIX 10
#define MAX_MAGIC_LEN 100

#endif
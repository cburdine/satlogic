#ifndef KHASH_INT_2D_H
#define KHASH_INT_2D_H

#include "khash.h"

KHASH_MAP_INIT_INT64(m64, khint64_t)

/* (int32, int32) -> int32 hash table */
typedef struct KhashInt2d {
    khash_t(m64) *htable;
    khint64_t key; /* stores last retrieved key */
    int last_result; /* stores status of last operation */
} KhashInt2d;

void initKhashInt2d(KhashInt2d* h);

int hashPut(KhashInt2d* h, int x_1, int x_2, int val);

int hashGet(KhashInt2d* h, int x_1, int x_2);

inline int hashStatus(KhashInt2d* h){ return h->last_result; }

int checkHashExists(KhashInt2d* h, int x_1, int x_2);

void clear(KhashInt2d* h);

void destroyKhashInt2d(KhashInt2d* h);

#endif /* KHASH_INT_2D_H */


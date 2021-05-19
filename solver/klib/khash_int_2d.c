#include "khash_int_2d.h"


void initKhashInt2d(KhashInt2d* h){
    h->htable = kh_init(m64);   
    h->key = 0;
    h->last_result = 1;
}

int hashPut(KhashInt2d* h, int x_1, int x_2, int val){
    khint64_t pt = x_1;
    pt = (pt<<32) + x_2;
    h->key = kh_put(m64, h->htable, pt, &(h->last_result));
    kh_value(h->htable, h->key) = val;
    return h->last_result;
}

int hashGet(KhashInt2d* h, int x_1, int x_2){
    khint64_t pt = x_1;
    pt = (pt<<32) + x_2;
    
    if((h->key = kh_get(m64, h->htable, pt)) == kh_end(h->htable)){
        h->last_result = 0;
    }else if((h->last_result = kh_exist(h->htable, h->key))){
        return kh_value(h->htable, h->key);
    }
    
    return 0;
}

int checkHashExists(KhashInt2d* h, int x_1, int x_2){
    khint64_t pt = x_1;
    pt = (pt<<32) + x_2;
    h->key = kh_get(m64, h->htable, pt);
    return kh_exist(h->htable, h->key);
}

void clear(KhashInt2d* h){
    kh_clear(m64, h->htable);
}

void destroyKhashInt2d(KhashInt2d* h){
    kh_destroy(m64, h->htable);
}



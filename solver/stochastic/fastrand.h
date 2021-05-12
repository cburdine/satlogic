#ifndef FASTRAND_H
#define FASTRAND_H

#define INIT_GLOBAL_FASTRAND() unsigned int _fastrand_seed
#define INIT_LOCAL_FASTRAND() unsigned int _fastRand_seed
#define seedFastrand(seed) _fastrand_seed = seed;
#define fastrand() ((_fastrand_seed = (214013*_fastrand_seed+2531011))>>16)&0x7FFF 
#define fastrandN(n) (((_fastrand_seed = (214013*_fastrand_seed+2531011))>>16)&0x7FFF) % n

#endif /* FASTRAND_H */

INIT_GLOBAL_FASTRAND();
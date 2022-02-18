# Malloc Implementation in C

Implemented a custom version of malloc, free and realloc. Memory can be malloced using first fit, next fit or best fit allocation algorithms passed during initialization. An explicit free list is maintained and reallocation is done in-place if possible, else the original region is freed and a new region is allocated.

Memperf returns the utilization and throughput of the implementation using random malloc, realloc and free calls.

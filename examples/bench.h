#ifndef BENCH_H
#define BENCH_H

/* hard-coded values for Raspberry Pi 3 Model B */
#define CPU_FREQ ((uint64_t) 1200000000)
#define CNTR_FREQ ((uint64_t) 19200000)

#define READ_CNTR(x) asm volatile("mrrc p15, 1, %Q0, %R0, c14" : "=r" (x));
// #define READ_CNTFREQ(x) asm volatile("mrc p15, 0, %0, c14, c0, 0" : "=r" (x))

#define TO_MICROSEC(before, after) (((after) - (before)) * 1000000 / CNTR_FREQ)
#define TO_CYCLES(before, after) (((after) - (before)) * CPU_FREQ / CNTR_FREQ)

#endif

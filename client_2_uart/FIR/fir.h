/* FIR filter */

#ifndef FIR_H
#define FIR_H

typedef struct FIR_filter {
    int length;
    int count;
    double *h;
    double *delay_line;
} FIR_filter;

FIR_filter* FIR_init(int length, double *h);
double FIR_get_sample(FIR_filter *filter, double input);
void FIR_destroy(FIR_filter *filter);

#endif // FIR_H

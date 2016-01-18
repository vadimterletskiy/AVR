/* Implementation of FIR filter */

#include <stdlib.h>
#include <string.h>
#include "fir.h"

/* Initialize FIR_filter struct
 * Params:
 *  int length -- number of filter coefficients
 *  double *h  -- array of filter coefficients
 * Returns:
 *  FIR_filter *p -- on success
 *  NULL          -- on error
 */
FIR_filter* FIR_init(int length, double *h) {
    FIR_filter *filter;

    filter = malloc(sizeof(FIR_filter));
    if( filter == NULL )
        return NULL;
    filter->length = length;
    filter->count = 0;
    filter->h = h;
    filter->delay_line = malloc(sizeof(double) * length);
    if( filter->delay_line == NULL ) {
        free(filter);
        return NULL;
    }
    memset(filter->delay_line, 0, sizeof(double) * length);
    return filter;
}

/* Get next filtered sample of input signsl
 * Params:
 *  FIR_filter *filter -- pointer to filter structure
 *  double input       -- input signl sample
 * Returns:
 *  double result -- filtered sample
 */
inline double FIR_get_sample(FIR_filter *filter, double input) {
    double result = 0.0;
    int index = filter->count, i = 0;

    filter->delay_line[filter->count] = input;
    for (i=0; i<filter->length; i++) {
        result += filter->h[i] * filter->delay_line[index--];
        if (index < 0)
            index = filter->length-1;
    }
    if (++filter->count >= filter->length)
        filter->count = 0;
    return result;
}

/* Free memory, allocated by filter
 * Params:
 *  FIR_filter *filter -- pointer to FIR_filter struct
 */
void FIR_destroy(FIR_filter *filter) {
    free(filter->delay_line);
    free(filter);
}

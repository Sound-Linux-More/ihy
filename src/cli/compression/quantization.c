#include "quantization.h"

static int sign(float x)
{
    if (x > 0.0f)
	return 1;
    else
	return -1;
}

static int nb_bits(int a)
{
    int nb = 1;
    int max = 1;

    while (abs(a) > max)
    {
	nb++;
	max *= 2;
    }
    return nb;
}

#define ith_bit(number, i)		\
    (((number) >> ((i) - 1)) & 1)

static void *encode(int *x, const size_t n, const int nb_bits, size_t *size)
{
    void *res;
    char *tab;
    size_t i;
    int j;
    char buf;
    int offset;

    *size = /*1 +*/ ((nb_bits * n) / 8);
    res = malloc(*size);
    tab = res;
    offset = 0;
    buf = 0;
    for (i = 0; i < n; i++)
    {
	for (j = 0; j < nb_bits; j++)
	{
	    buf |= ith_bit(x[i], j) << (7 - offset++);
	    if (offset == 8)
	    {
		*tab = buf;
		buf = 0;
		tab++;
		offset = 0;
	    }
	}
    }
    return res;
}

void *quantizate(float *x, size_t *n, const float factor)
{
    size_t i;
    int min, max;
    void *res;
    int *q;

    q = malloc(*n * sizeof(int));
    min = max = 0;
    for (i = 0; i < *n; i++)
    {
	q[i] = sign(x[i]) * floorf(abs(x[i] / factor));
	if (q[i] > max)
	    max = q[i];
	else if (q[i] < min)
	    min = q[i];
    }
    min = abs(min);
    max = max > min ? max : min;
    min = nb_bits(max);
    res = encode(q, *n, min, n);
    return res;
}

void dequantizate(float *x, const size_t n, const float factor)
{
    size_t i;

    for (i = 0; i < n; i++)
    {
	/*if (x[i] != 0.0f)*/
	    x[i] = sign(x[i]) * (abs(x[i]) + 0.5f) * factor;
    }
}

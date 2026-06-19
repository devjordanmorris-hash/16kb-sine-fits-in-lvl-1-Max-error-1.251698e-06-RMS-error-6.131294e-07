#include <stdio.h>
#include <math.h>
#include <time.h>

#define N 1000000
#define LUT_SIZE 2048

static float L0[LUT_SIZE];
static float R0[LUT_SIZE];

// -----------------------------
// linear interpolation helper
// -----------------------------
static inline float interp(const float* table, float x)
{
    const float twopi = 2.0f * M_PI;

    x = fmodf(x, twopi);
    if (x < 0) x += twopi;

    float p = x / twopi * LUT_SIZE;

    int i = (int)p;
    float f = p - i;

    int i1 = (i + 1) & (LUT_SIZE - 1);

    return table[i] * (1.0f - f) + table[i1] * f;
}

// -----------------------------
// build LUT + TRUE residual
// -----------------------------
static void build()
{
    const float twopi = 2.0f * M_PI;

    // base LUT
    for (int i = 0; i < LUT_SIZE; i++)
    {
        float x = twopi * i / LUT_SIZE;
        L0[i] = sinf(x);
    }

    // residual LUT (IMPORTANT FIX)
    for (int i = 0; i < LUT_SIZE; i++)
    {
        float x = twopi * i / LUT_SIZE;

        float approx = interp(L0, x);
        float exact  = sinf(x);

        R0[i] = exact - approx;
    }
}

// -----------------------------
// evaluation
// -----------------------------
static inline float sin_residual(float x)
{
    return interp(L0, x) + interp(R0, x);
}

// -----------------------------
static double now_ms()
{
    return (double)clock() * 1000.0 / CLOCKS_PER_SEC;
}

int main()
{
    build();

    printf("=====================================\n");
    printf(" RESIDUAL LUT v2 (CORRECTED)\n");
    printf("=====================================\n\n");

    double base_rms = 0.0;
    double res_rms  = 0.0;

    double base_max = 0.0;
    double res_max  = 0.0;

    volatile float sink = 0.0f;

    double t0 = now_ms();

    for (int i = 0; i < N; i++)
    {
        float x = (2.0f * M_PI * i) / N;
        float exact = sinf(x);

        float base = interp(L0, x);
        float res  = sin_residual(x);

        float eb = fabsf(exact - base);
        float er = fabsf(exact - res);

        if (eb > base_max) base_max = eb;
        if (er > res_max)  res_max  = er;

        base_rms += eb * eb;
        res_rms  += er * er;

        sink += res;
    }

    double t1 = now_ms();

    base_rms = sqrt(base_rms / N);
    res_rms  = sqrt(res_rms / N);

    printf("Time: %.3f ms\n\n", t1 - t0);

    printf("BASE (L0 only)\n");
    printf("  Max error: %.6e\n", base_max);
    printf("  RMS error: %.6e\n\n", base_rms);

    printf("RESIDUAL v2 (L0 + R0)\n");
    printf("  Max error: %.6e\n", res_max);
    printf("  RMS error: %.6e\n\n", res_rms);

    printf("Memory:\n");
    printf("  L0: %zu bytes\n", sizeof(L0));
    printf("  R0: %zu bytes\n", sizeof(R0));
    printf("  Total: %zu bytes\n\n", sizeof(L0) + sizeof(R0));

    return 0;
}

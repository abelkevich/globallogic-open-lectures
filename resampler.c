#include "resampler.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

static smp_16_t* duplicate(const smp_16_t* signal_src, const smpn_t samples_n)
{
    smp_16_t* signal_duplicate = (smp_16_t*) malloc(samples_n * sizeof(smp_16_t));
    memcpy(signal_duplicate, signal_src, samples_n * sizeof(smp_16_t));
    return signal_duplicate;
}

static smp_16_t* decimation(const uint8_t k, const smp_16_t* signal_original, const smpn_t samples_original, smpn_t* _samples_decimated)
{
    if (k == 0 || k == 1)
    {
        return NULL;
    }

    if (!signal_original)
    {
        return NULL;
    }

    const smpn_t samples_decimated = samples_original / k;
    smp_16_t* signal_decimated = (smp_16_t*) malloc(samples_decimated * sizeof(smp_16_t));

    if (!signal_decimated)
    {
        return NULL;
    }

    for (uint32_t orig_smp_ind = 0, dcmtd_smp_ind = 0; 
         orig_smp_ind < samples_original; 
         orig_smp_ind += k, dcmtd_smp_ind++)
    {
        signal_decimated[dcmtd_smp_ind] = signal_original[orig_smp_ind];
    }

    *_samples_decimated = samples_decimated;
    return signal_decimated;
}

static smp_16_t* interpolation(const uint8_t k, const smp_16_t* signal_original, const smpn_t samples_original, smpn_t* _samples_interpolated)
{
    if (k == 0 || k == 1)
    {
        return NULL;
    }

    if (!signal_original)
    {
        return NULL;
    }

    const smpn_t samples_interpolated = samples_original * k;
    smp_16_t* signal_interpolated = (smp_16_t*)malloc(samples_interpolated * sizeof(smp_16_t));

    if (!signal_interpolated)
    {
        return NULL;
    }

    // interpolation
    for (smpn_t orig_smp_ind = 0, intrp_smp_ind = 0; orig_smp_ind < samples_original; orig_smp_ind++)
    {
        float f0 = signal_original[orig_smp_ind];
        float f1 = (orig_smp_ind != samples_original - 1) ?
                    signal_original[orig_smp_ind + 1] :
                    signal_original[orig_smp_ind];

        float x0 = (float) orig_smp_ind;
        float x1 = (float) orig_smp_ind + k;

        float c1 = (f1 - f0) / (x1 - x0);
        float c2 = c1 * x0 * (-1) + f0;

        for (uint8_t k_ind = 0; k_ind < k; k_ind++)
        {
            float x = x0 + k_ind;
            smp_16_t y = x * c1 + c2;

            signal_interpolated[intrp_smp_ind] = y;
            intrp_smp_ind++;
        }
    }

    *_samples_interpolated = samples_interpolated;
    return signal_interpolated;
}

static float resampleFactor(const freq_t freq_original, const freq_t freq_target)
{
    return (float) freq_target / freq_original;
}

static freq_t find_possible_common(freq_t M, freq_t N)
{
    for (uint32_t possible_common = M > N ? M : N;
        possible_common > 1; possible_common--)
    {
        if (M % possible_common == 0 &&
            N % possible_common == 0)
        {
            return possible_common;
        }
    }

    return 0;
}

smp_16_t* resample(const freq_t freq_original, const freq_t freq_target, const smp_16_t* signal_original, const smpn_t samples_original, smpn_t* _samples_resampled)
{
    freq_t M = freq_target;
    freq_t N = freq_original;

    freq_t common = find_possible_common(M, N);
    while (common != 0)
    {
        M /= common;
        N /= common;

        common = find_possible_common(M, N);
    }

    if (M == 0 || N == 0)
    {
        return NULL;
    }

    smpn_t samples_interpolated = 0;
    smp_16_t* signal_interpolated = nullptr;

    if (M == 1)
    {
        signal_interpolated = duplicate(signal_original, samples_original);
        samples_interpolated = samples_original;
    }
    else
    {
        signal_interpolated = interpolation(M, signal_original, samples_original, &samples_interpolated);
    }

    if (!signal_interpolated)
    {
        return NULL;
    }

    smp_16_t* signal_decimated = nullptr;
    smpn_t samples_decimated = 0;

    if (N == 1)
    {
        signal_decimated = duplicate(signal_interpolated, samples_interpolated);
        samples_decimated = samples_interpolated;
    }
    else
    {
        signal_decimated = decimation(N, signal_interpolated, samples_interpolated, &samples_decimated);
    }

    free(signal_interpolated);

    if (!signal_decimated)
    {
        return NULL;
    }

    *_samples_resampled = samples_decimated;
    return signal_decimated;
}
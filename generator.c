#include <stdlib.h>
#include <math.h>
#include "generator.h"

smp_16_t* generate_signal(ESigType sig_type, ms_t length, freq_t sample_rate, freq_t base_freq, uint8_t harmonics_n, smpn_t *signal_samples)
{
    static const float pi = 3.14;
    smpn_t samples = sample_rate * (length / 1000.0);
    smp_16_t* signal = (smp_16_t*) malloc(samples * sizeof(smp_16_t));

    if (!signal)
    {
        return NULL;
    }

    for (uint8_t harmonic_ind = 0; harmonic_ind < harmonics_n; harmonic_ind++)
    {
        freq_t freq = base_freq * (harmonic_ind + 1);
        float amp = 1.0 / (harmonic_ind + 1);

        for (smpn_t smp_ind = 0; smp_ind < samples; smp_ind++)
        {
            float t = (float) smp_ind / sample_rate;
            signal[smp_ind] = amp * sin(2 * pi * freq * t);
        }
    }

    *signal_samples = samples;
    return signal;
}
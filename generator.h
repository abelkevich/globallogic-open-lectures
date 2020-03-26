#pragma once
#include "cmn_defs.h"

enum ESigType
{
    eSin
};

typedef enum ESigType ESigType;

smp_16_t* generate_signal(ESigType sig_type, ms_t length, freq_t sample_rate, freq_t base_freq, uint8_t harmonics_n, smpn_t *signal_samples);

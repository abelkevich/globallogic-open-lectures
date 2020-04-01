#pragma once
#include "cmn_defs.h"

smp_16_t* resample(freq_t freq_original, freq_t freq_target, const smp_16_t* signal_original, smpn_t samples_original, smpn_t* _samples_resampled);
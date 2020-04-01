#include <stdio.h>
#include <stdlib.h>

#include "cmn_defs.h"
#include "resampler.h"
#include "generator.h"

static int parse_args(int argc, char** argv, ms_t* length, freq_t* sample_rate, freq_t* target_sample_rate)
{
    if (argc != 4)
    {
        printf("Usage: %s length sample_rate target_sample_rate\n", argv[0]);
        printf("\tlength - generated signal length in ms\n");
        printf("\tsample_rate - generated signal sample rate\n");
        printf("\ttarget_sample_rate - resampled signal sample rate\n");

        return 1;
    }

    int arg_length = atoi(argv[1]);

    if (arg_length <= 0)
    {
        printf("length incorrect value\n");
        return 1;
    }

    int arg_sample_rate = atoi(argv[2]);

    if (arg_sample_rate <= 0)
    {
        printf("sample_rate incorrect value\n");
        return 1;
    }

    int arg_target_sample_rate = atoi(argv[3]);

    if (arg_target_sample_rate <= 0)
    {
        printf("target_sample_rate incorrect value\n");
        return 1;
    }

    *length = (smpn_t)arg_length;
    *sample_rate = (freq_t)arg_sample_rate;
    *target_sample_rate = (freq_t)arg_target_sample_rate;

    return 0;
}

void test_backward_linear_resampling(smp_16_t* signal, smpn_t samples_n, freq_t sample_rate, freq_t target_sample_rate)
{
    printf("[Test] backward linear resampling");
    printf("Input: \n\tsamples_n: '%ld'\n\tsample_rate: '%d'\n\ttarget_sample_rate: '%d'\n",
            samples_n, sample_rate, target_sample_rate);
    
    // resample from input to target sample rates
    smpn_t resampled_samples_n = 0;
    smp_16_t* resampled_signal = resample(sample_rate, target_sample_rate, signal, samples_n, &resampled_samples_n);

    if (!resampled_signal)
    {
        printf("Error due resampling from sample_rate to target_sample_rate, exiting\n");
        return;
    }

    // resample from target to input sample rates
    smpn_t backward_resampled_samples_n = 0;
    smp_16_t* backward_resampled_signal = resample(target_sample_rate, sample_rate, resampled_signal, resampled_samples_n, &backward_resampled_samples_n);

    if (!backward_resampled_signal)
    {
        printf("Error due resampling from target_sample_rate to sample_rate, exiting\n");
        free(resampled_signal);
        return;
    }

    // calculate errors
    smpn_t err_n = 0;
    for (smpn_t smp_ind = 0; smp_ind < samples_n; smp_ind++)
    {
        if (signal[smp_ind] != backward_resampled_signal[smp_ind])
        {
            err_n++;
        }
    }

    float err_percent = err_n / (samples_n / 100.0);

    printf("Errors due conversions: %f\n", err_percent);

    free(resampled_signal);
    free(backward_resampled_signal);

    printf("Test done\n");
}

int main(int argc, char** argv)
{
    ms_t length = 0;
    freq_t sample_rate = 0;
    freq_t target_sample_rate = 0;

    if (parse_args(argc, argv, &length, &sample_rate, &target_sample_rate))
    {
        return 0;
    }

    // get 3 sin waves starting logariphmicaly from 1KHz
    // 2s length at sample rate 'input freq'
    smpn_t samples = 0;
    smp_16_t* input_signal = generate_signal(eSin, length, sample_rate, 1000, 3, &samples);

    test_backward_linear_resampling(input_signal, samples, sample_rate, target_sample_rate);

    free(input_signal);

    return 0;
}

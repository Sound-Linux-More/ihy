#ifndef DEF_AO
#define DEF_AO

#include <ao/ao.h>

/* return the device, to play sound in streaming */
ao_device *ao_init_device(int BitsPerSample, int NumChannels, int SampleRate);

/* close the device, and do some cleaning */
void ao_close_device(ao_device *device);

/* play the content of array */
void ao_play_samples(ao_device *device, void *array, int size);

#endif

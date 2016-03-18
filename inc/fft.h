/* Constant Definitions */
#define FFT_BLOCK_LENGTH	64    /* = Number of frequency points in the FFT */
#define LOG2_BLOCK_LENGTH 	6	/* = Number of "Butterfly" Stages in FFT processing */
#define SAMPLING_RATE		10000	/* = Rate at which input signal was sampled */
                                        /* SAMPLING_RATE is used to calculate the frequency*/
                                        /* of the largest element in the FFT output vector*/


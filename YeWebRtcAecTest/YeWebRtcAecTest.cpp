#include <windows.h>
#include <iostream>
#include <stdio.h>

#include "echo_cancellation.h"
using namespace std;
using namespace webrtc;

#define NUM_BANDS 1
#define NUM_SAMPLES 80

int main(int argc, char** argv) {

	short far_frame_s[NUM_SAMPLES];
	short near_frame_s[NUM_SAMPLES];
	short out_frame_s[NUM_SAMPLES];
	float far_frame_f[NUM_SAMPLES];
	float* near_frame_f[NUM_BANDS];
	float* out_frame_f[NUM_BANDS];
	int i = 0;

	for (int i = 0; i < NUM_BANDS; i++)
	{
		near_frame_f[i] = new float[NUM_SAMPLES];
		out_frame_f[i] = new float[NUM_SAMPLES];
	}
	void* pAec = WebRtcAec_Create();
	WebRtcAec_Init(pAec, 8000, 8000);
	AecConfig config;
	config.nlpMode = kAecNlpConservative;
	WebRtcAec_set_config(pAec, config);

	FILE* fEcho = fopen("speaker_16bit_8k.pcm", "rb");
	FILE* fMicin = fopen("micin_16bit_8k.pcm", "rb");
	FILE* fOut = fopen("out_16bit_8k.pcm", "wb");
	if (!fEcho || !fMicin || !fOut)
	{
		goto fail;
	}

	while (1)
	{
		if (NUM_SAMPLES == fread(far_frame_s, sizeof(short), NUM_SAMPLES, fEcho))
		{
			for (i = 0; i < NUM_SAMPLES; i++) //convert short to float
			{
				far_frame_f[i] = far_frame_s[i];
			}
			WebRtcAec_BufferFarend(pAec, far_frame_f, NUM_SAMPLES);

			fread(near_frame_s, sizeof(short), NUM_SAMPLES, fMicin);
			for (i = 0; i < NUM_SAMPLES; i++) //convert short to float
			{
				near_frame_f[0][i] = near_frame_s[i];
			}
			WebRtcAec_Process(pAec, near_frame_f, NUM_BANDS, out_frame_f, NUM_SAMPLES, 70, 0); //Win7 Desktop,use 70
			for (i = 0; i < NUM_SAMPLES; i++) //convert float to short
			{
				out_frame_s[i] = (short)out_frame_f[0][i];
			}
			fwrite(out_frame_s, sizeof(short), NUM_SAMPLES, fOut);
		} else {
			break;
		}	
		
	}

fail:
	fclose(fEcho);
	fclose(fMicin);
	fclose(fOut);

	for (int i = 0; i < NUM_BANDS; i++)
	{
		delete []near_frame_f[i];
		delete []out_frame_f[i];
	}
	WebRtcAec_Free(pAec);
	cout << "AECÍê³É£¡" << endl;
	system("pause");
	return 0;
}
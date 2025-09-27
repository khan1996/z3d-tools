#ifndef DEFINESOUND_H__
#define DEFINESOUND_H__

#include "Common.h"
#include <math.h>
#include <dsound.h>

//====================================
// Sound Property  
//====================================
#define VOLUME_MIN 0.0f
//#define VOLUME_MIN 0.0001f
#define VOLUME_MAX 1.0f
#define PAN_LEFT -1.0f
#define PAN_CENTER 0.0f
#define PAN_RIGHT 1.0f
#define PITCH_ORIGINAL 1.0f
#define MODE_NORMAL	0x00000000
#define MODE_HEADRELATIVE 0x00000001
#define MODE_DISABLE 0x00000002

//====================================
// Sound SampleRate  
//====================================
#define SOUND_SAMPLE_11k 11025		
#define SOUND_SAMPLE_22k 22050		
#define SOUND_SAMPLE_44k 44100

//====================================
// 3DSound Algorithm
//====================================
#define SOUND_ALGORITHM_NO DS3DALG_NO_VIRTUALIZATION
#define SOUND_ALGORITHM_FULL DS3DALG_HRTF_FULL 	
#define SOUND_ALGORITHM_LIGHT DS3DALG_HRTF_LIGHT        

//====================================
// Sound Creation Flag
//====================================
#define SOUND_FLAG_3D DSBCAPS_CTRL3D|DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_CTRLVOLUME|DSBCAPS_MUTE3DATMAXDISTANCE
#define SOUND_FLAG_2D DSBCAPS_CTRLVOLUME|DSBCAPS_GETCURRENTPOSITION2

//====================================
// Macro
//====================================
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

//====================================
// Util Function  
//====================================
static int LinearToLogVol(double fLevel)
{
	// Clamp the value
	if(fLevel <= 0.0f)
		return DSBVOLUME_MIN;
	else if(fLevel >= 1.0f)
		return 0;
    return (long) (-2000.0 * log10(1.0f / fLevel));
}

static float LogToLinearVol(int iLevel)
{
	// Clamp the value
	if(iLevel <= -9600)
		return 0.0f;
	else if(iLevel >= 0)
		return 1.0f;
    return pow((float)10, (float)(double(iLevel + 2000) / 2000.0f)) / 10.0f;
}

template <class T>
inline T Clamp(T var, T min, T max )
{
	if( var < min ) var = min; 
	if( var > max ) var = max; 
	return var; 
}

template<class _Ty>
struct ptr_less : std::binary_function<_Ty, _Ty, bool> 
{
	bool operator()(const _Ty& _X, const _Ty& _Y) const
	{ return (*_X < *_Y); }
};


#endif //DEFINESOUND_H__
//////// Shader Constants Setting
#ifndef __SHADERCONSTANTS_H__
#define __SHADERCONSTANTS_H__


#define CV_ZERO 0
#define CV_ONE 1

#define CV_WORLDVIEWPROJ_0 2
#define CV_WORLDVIEWPROJ_1 3
#define CV_WORLDVIEWPROJ_2 4
#define CV_WORLDVIEWPROJ_3 5

#define CV_WORLDVIEW_0 6
#define CV_WORLDVIEW_1 7
#define CV_WORLDVIEW_2 8
#define CV_WORLDVIEW_3 9

#define CV_WORLDVIEWIT_0 10
#define CV_WORLDVIEWIT_1 11
#define CV_WORLDVIEWIT_2 12

#define CV_WORLD_0 13
#define CV_WORLD_1 14
#define CV_WORLD_2 15
#define CV_WORLD_3 16

#define CV_HALF 18

#define CV_LIGHT_COLOR 19
#define CV_ONE_OVER_LIGHT_RANGE 20

#define CV_LIGHT_POSITION 21
//#define CV_LIGHT_POSITION 22
#define CV_LIGHT_DIRECTION 22
#define CV_BUMP_SCALE 23

#define CV_EYE_POS_WORLD      24
#define CV_FOG_SCALE 25

#define CV_LIGHT1_DIRECTION 26
#define CV_LIGHT1_AMBIENT 27
#define CV_LIGHT1_DIFFUSE 28

#define CV_WORLDIT_0 29
#define CV_WORLDIT_1 30
#define CV_WORLDIT_2 31
#define CV_WORLDIT_3 32

#define CV_SHADOWCONST 33

#define CV_TEXMAT_0 34
#define CV_TEXMAT_1 35
#define CV_TEXMAT_2 36
#define CV_TEXMAT_3 37

#define CV_T0_OFFSET 38
#define CV_T1_OFFSET 39
#define CV_T2_OFFSET 40
#define CV_T3_OFFSET 41

#define CV_CONSTS_1 42
#define CV_ONOFF_1 43

#define CV_WATERBUMPSCALE 44


#define CV_CALC_SXT             51

#define CV_DISPSCALE			52

// for scaling embm offsets overall
#define CV_OFFSETSCALE			53


#define CV_PTS_BASE				54

#define CV_CONSTS_2				55
#define CV_EYE_OBJSPC			56

#define CV_BASISTRANSFORM_0		57
#define CV_BASISTRANSFORM_1		58
#define CV_BASISTRANSFORM_2		59
// Rain

#define CV_RAINDIRECTION		60
#define CV_RAINANI				61

#define CV_REFLECTMAT_0			62
#define CV_REFLECTMAT_1			63
#define CV_REFLECTMAT_2			64

/// Pixel Shader Constants

#define PCN_MULTFACTOR_1		 0
#define PCN_MULTFACTOR_2         1
#define PCN_EQ_REST_FAC          2

#define	PCN_EMBM_SCALE			 3
#define PCN_POSITIONMASK		 4

#define PCN_RED_MASK             5
#define PCN_GREEN_MASK           6

#define PCN_LIGHTCOLOR			 7
#define PCN_AMBIENTCOLOR		 8
#endif

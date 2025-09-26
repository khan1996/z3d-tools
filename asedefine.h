/************************
ase label define
**************************/



#ifndef __ASEDEFINE_H__

#define __ASEDEFINE_H__

#pragma once

#define OBJECT				"*GEOMOBJECT"			// An object tag for new objects
#define LIGHT_OBJECT		"*LIGHTOBJECT"
#define NODE_NAME			"*NODE_NAME"
#define NODE_PARENT			"*NODE_PARENT"
#define NODE_TM				"*NODE_TM"
#define TM_ROW0				"*TM_ROW0"
#define TM_ROW1				"*TM_ROW1"
#define TM_ROW2				"*TM_ROW2"
#define TM_ROW3				"*TM_ROW3"
#define TM_POS				"*TM_POS"
#define TM_ROTAXIS			"*TM_ROTAXIS"
#define TM_ROTANGLE			"*TM_ROTANGLE"
#define TM_SCALE			"*TM_SCALE"
#define TM_SCALEAXIS		"*TM_SCALEAXIS"
#define TM_SCALEAXISANG		"*TM_SCALEAXISANG"

#define BLOCK_START "{"
#define BLOCK_END "}"

#define NUM_VERTEX			"*MESH_NUMVERTEX"		// The number of vertices tag
#define NUM_FACES			"*MESH_NUMFACES"		// The number of faces tag
#define NUM_TVERTEX			"*MESH_NUMTVERTEX"		// The number of texture coordinates
#define VERTEX				"*MESH_VERTEX"			// The list of vertices tag
#define FACE				"*MESH_FACE"			// The list of faces tag
#define NORMALS				"*MESH_NORMALS"			// The list of normals tag (If you want)
#define FACE_NORMAL			"*MESH_FACENORMAL"		// The face normal for the current index
#define NVERTEX				"*MESH_VERTEXNORMAL"	// The list of vertex normals
#define TVERTEX				"*MESH_TVERT"			// The texture coordinate index tag
#define TFACE				"*MESH_TFACE"			// The vertex index tag
#define TEXTURE				"*BITMAP"				// The file name for the object's texture map
#define UTILE				"*UVW_U_TILING"			// The U tiling ratio tag
#define VTILE				"*UVW_V_TILING"			// The V tiling ratio tag
#define UOFFSET				"*UVW_U_OFFSET"			// The U tile offset tag
#define VOFFSET				"*UVW_V_OFFSET"			// The V tile offset tag
#define MATERIAL_ID			"*MATERIAL_REF"			// The material ID tag
#define MATERIAL_COUNT      "*MATERIAL_COUNT"		// The material count tag
#define MATERIAL			"*MATERIAL"				// The material tag
#define MATERIAL_NAME		"*MATERIAL_NAME"		// The material name tag
#define MATERIAL_COLOR		"*MATERIAL_DIFFUSE"		// The material color tag

#define TM_ANIMATION		"*TM_ANIMATION"
#define CONTROL_ROT_TCB		"*CONTROL_ROT_TCB"
#define CONTROL_TCB_ROT_KEY	"*CONTROL_TCB_ROT_KEY" 
#define CONTROL_POS_BEZIER	"*CONTROL_POS_BEZIER"
#define CONTROL_BEZIER_POS_KEY	"*CONTROL_BEZIER_POS_KEY"

///////////////////////////////////
// mesh ani value
//////////////////////////////////
#define MESH_ANIMATION		"*MESH_ANIMATION"
// use check ani frame num.
#define MESH_TIMEVALUE		"*TIMEVALUE"

#endif

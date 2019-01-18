#pragma once
#include <bitset>
#include <cassert>

enum PipelineHW {
	CPU,
	GPU
};

enum PipelineIO {
	// Camera
	CAM_POSITION,
	CAM_DIRECTION,
	CAM_VP,

	/* Scene data */
	// Mesh
	MESH,

	// Lights
	LIGHT_POSITION,
	LIGHT_INTENSITY,

	// Materials
	MAT_AMBIENT,
	MAT_DIFFUSE,
	MAT_SPECULAR,
	MAT_TRANSMISSIVE,

	/* Buffers */
	// G buffer geometry
	G_DEPTH,
	G_POSITION,
	G_NORMAL,

	// G buffer material
	G_MAT_AMBIENT,
	G_MAT_DIFFUSE,
	G_MAT_SPECULAR,
	G_MAT_TRANSMISSIVE,

	// Illumination
	LIGHT_DIRECT,
	LIGHT_INDIRECT,

	RT_COLOR,

	// Filter
	TEMP_COLOR,

	// Constructed color
	COLOR,

	NUM_PIPELINE_IO
};

// CODEHERE - replace with pretty strings
static const char* PipelineIONames[] = {
	"CAM_POSITION",
	"CAM_DIRECTION",
	"CAM_VP",
	"MESH",
	"LIGHT_POSITION",
	"LIGHT_INTENSITY",
	"MAT_AMBIENT",
	"MAT_DIFFUSE",
	"MAT_SPECULAR",
	"MAT_TRANSMISSIVE",
	"G_DEPTH",
	"G_POSITION",
	"G_NORMAL",
	"G_MAT_AMBIENT",
	"G_MAT_DIFFUSE",
	"G_MAT_SPECULAR",
	"G_MAT_TRANSMISSIVE",
	"LIGHT_DIRECT",
	"LIGHT_INDIRECT",
	"RT_COLOR",
	"TEMP_COLOR",
	"COLOR",
};

typedef std::bitset<NUM_PIPELINE_IO> PipelineIOMask;

// assert that the size of names is the number of enum values
static_assert(NUM_PIPELINE_IO == sizeof(PipelineIONames)/sizeof(*PipelineIONames), "PipelineIO and PipelineIONames not same size.");
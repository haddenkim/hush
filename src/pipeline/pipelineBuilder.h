#pragma once

#include "common.h"
#include <initializer_list>

#include "pipeline/pipeline.h"
#include "renderPass/renderPass.h"

class Scene;
class Camera;
class Buffer;
// struct PipelineIOLink;
// struct PipelinePass;
// struct PipelineStage;

struct BuildLink {
	uint fromStage;
	uint fromPass;
	std::initializer_list<PipelineIO> ioTypes;
};

struct BuildPass {
	RenderPassType type;
	std::initializer_list<BuildLink> links;
};

struct BuildStage {
	std::initializer_list<BuildPass> passes;
};

struct BuildPipeline {
	std::initializer_list<BuildStage> stages;
	Scene* scene;
	Camera* camera;
	uint width;
	uint height;
};

class PipelineBuilder {
public:
	PipelineBuilder(Pipeline& pipeline);

	bool createPipeline(const BuildPipeline& buildPipeline);

private:
	bool createStage(const BuildStage& buildStage, uint stageIndex, PipelineStage& pipelineStage);
	bool createPass(const BuildPass& buildPass, uint stageIndex, PipelinePass& pipelinePass);
	bool createLink(const BuildLink& buildLink, PipelineIOLink& pipelineLink);
	bool validateDependencies(const BuildLink& buildLink);

	Buffer* createOutputBuffer(PipelineHW hw, PipelineIO type, PipelinePass& pipelinePass);
	Buffer* requestInputBuffer(PipelineHW hw,
							   PipelineIO type,
							   uint stageIndex,
							   PipelinePass& pipelinePass);
	Buffer* createBufferSync(PipelineHW hw, PipelineIO type, Buffer* fromBuffer, uint stageIndex);

	Buffer* createBuffer(PipelineHW hw, PipelineIO type);
	Buffer* findOutBuffer(PipelineHW hw, PipelineIO type, uint fromStage, uint fromPass);

	// debugging
	void printPipeline();

	// reference
	Pipeline& m_pipeline;
};
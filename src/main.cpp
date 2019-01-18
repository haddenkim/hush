#include "app/viewer.h"
#include "camera/camera.h"
#include "common.h"
#include "pipeline/pipeline.h"
#include "renderPass/renderPass.h"
#include "scene/obj2scene.h"
#include "scene/scene.h"

#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 512

int main(int argc, char** argv)
{
	if(argc < 2) {
		// CODEHERE - proper cmd line parser
		// CODEHERE - print usage message
		return 0;
	}

	Scene scene;
	Obj2scene::LoadScene(&scene, argv[1]);
	scene.setupAdditionalLights();
	scene.setupEmb();

	// TEMP Sponza camera
	// Point3f camPosition(-10.f, 5.f, 0.f);
	// Point3f camCenter(0.f, 5.f, 0.f);
	// Vec3f camUp(0.f, 1.f, 0.f);

	float fovY = 35.f * M_PI / float(180);
	Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, fovY, &scene);

	// initialize viewer
	Viewer viewer(WINDOW_WIDTH, WINDOW_HEIGHT, &scene, &camera);

	// initialize pipelines
	Pipeline simpleRaster(&scene, &camera, { RASTER_GBUFFER, SS_DIRECT_LIGHT, SS_AMBIENT, TO_SCREEN });
	viewer.addPipeline(&simpleRaster);

	Pipeline pathTracer(&scene, &camera, { RT_FULL_GI, TO_SCREEN });
	viewer.addPipeline(&pathTracer);

	// start rendering
	viewer.start();

	return 0;
}

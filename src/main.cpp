#include "app/viewer.h"
#include "camera/camera.h"
#include "common.h"
#include "pipeline/pipeline.h"
#include "pipeline/pipelineBuilder.h"
#include "renderPass/renderPass.h"
#include "renderPass/renderPassType.h"
#include "scene/obj2scene.h"
#include "scene/scene.h"

#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 512

int main(int argc, char** argv)
{
	if (argc < 2) {
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

	Pipeline simpleRaster(BuildPipeline{
		{
			BuildStage{
				{
					BuildPass{
						RASTER_GBUFFER,
						{
							// no links
						} //
					}	 //
				}		  //
			},
			BuildStage{
				{
					BuildPass{
						SS_DIRECT_LIGHT,
						{
							BuildLink{ 0, 0, { G_POSITION, G_NORMAL, G_MAT_DIFFUSE, G_MAT_SPECULAR } } //
						}																			   //
					}																				   //
				}																					   //
			},
			BuildStage{
				{
					BuildPass{
						SS_AMBIENT,
						{
							BuildLink{ 0, 0, { G_MAT_AMBIENT, G_MAT_DIFFUSE } }, //
							BuildLink{ 1, 0, { COLOR } }						 //
						}														 //
					}															 //
				}																 //
			},
			BuildStage{
				{
					BuildPass{
						TO_SCREEN,
						{
							BuildLink{ 2, 0, { COLOR } } //
						}								 //
					}									 //
				}										 //
			}											 //
		},
		&scene,
		&camera,
		WINDOW_WIDTH,
		WINDOW_HEIGHT });

	viewer.addPipeline(&simpleRaster);

	Pipeline pathTracer(BuildPipeline{
		{
			BuildStage{
				{
					BuildPass{
						RT_FULL_GI,
						{
							// no links
						} //
					}	 //
				}		  //
			},
			BuildStage{
				{
					BuildPass{
						DENOISE_ATROUS,
						{
							BuildLink{ 0, 0, { COLOR, G_POSITION, G_NORMAL } } //
						}													   //
					}														   //
				}															   //
			},
			BuildStage{
				{
					BuildPass{
						PP_TONE_MAP,
						{
							BuildLink{ 1, 0, { COLOR } } //
						}								 //
					}									 //
				}										 //
			},											 //
			BuildStage{
				{
					BuildPass{
						TO_SCREEN,
						{
							BuildLink{ 2, 0, { COLOR } } //
						}								 //
					}									 //
				}										 //
			}											 //
		},
		&scene,
		&camera,
		WINDOW_WIDTH,
		WINDOW_HEIGHT });

	viewer.addPipeline(&pathTracer);

	// start rendering
	viewer.start();

	return 0;
}

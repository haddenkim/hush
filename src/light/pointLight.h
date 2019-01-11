#include "light/light.h"

class PointLight : public Light {
public:
	PointLight(const std::string name, Point3f position, Spectrum intensity);

	LightSample sampleLi(const Point3f surfacePosition, Sampler& sampler) const override;

	Spectrum m_intensity;

	// GUI
	void guiRead();
	bool guiEdit();
};
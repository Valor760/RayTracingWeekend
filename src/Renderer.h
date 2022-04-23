#ifndef RENDERER_H
#define RENDERER_H

#include <memory>

#include "graphics/HittableList.h"
#include "graphics/Camera.h"


namespace RTWeekend {
enum class RenderState {
	kRenderStop,
	kRenderRunning
};

class Renderer {
public:
	const int m_samples = 100;
	const int m_depth = 8;
	std::unique_ptr<Graphics::Camera> m_camera;
	const int m_width;
	const int m_height;

public:
	Renderer(int _width, int _height, unsigned char* buff) 
		: m_width(_width), m_height(_height), m_buffer(buff) {}
	void StartRender();
	void Render(unsigned char* buffer);

private:
	Graphics::HittableList randomScene();
	Graphics::Color rayColor(const Graphics::Ray& r, const Graphics::Hittable& world, int depth);
	void WriteToBuffer(unsigned char* buffer, int ix, int iy, Graphics::Color pixel_color);
	double getAspectRatio();
	unsigned char* m_buffer;
};
} // namespace RTWeekend

#endif
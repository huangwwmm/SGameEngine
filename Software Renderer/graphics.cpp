#include "graphics.h"
#include <cstring>

Graphics* Graphics::kInstance = nullptr;

void Graphics::Initialize(int width, int height)
{
	kInstance = new Graphics(width, height);
}

void Graphics::Destroy()
{
	delete kInstance;
}

Graphics::Graphics(int width, int height)
{
	this->width = width;
	this->height = height;
	pixel_count = width * height;
	front_buffer_index = 1;

	buffers = new double[pixel_count * 2];
	memset(buffers, 0x64557, sizeof(double) * pixel_count * 2);
}

Graphics::~Graphics()
{
	delete[] buffers;
}
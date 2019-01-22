#pragma once
#include <windows.h>

class Graphics
{
public:
	static Graphics *kInstance;

public:
	static void Initialize(int width, int height);
	static void Destroy();

public:
	__forceinline void Draw(HDC &hdc) const
	{
		double *front_buffer = GetFrontBuffer();
		for (int i_height = 0; i_height < height; i_height++)
		{
			for (int i_width = 0; i_width < width; i_width++)
			{
				SetPixel(hdc, i_width, i_height, front_buffer[i_height * height + width]);
			}
		}
	}

public:
	__forceinline int GetWidth() const
	{
		return width;
	}

	__forceinline int GetHeight() const
	{
		return height;
	}

	__forceinline double* GetBackBuffer() const
	{
		return &buffers[(!front_buffer_index ^ 1) * pixel_count];
	}

	__forceinline void SwapBuffer()
	{
		front_buffer_index ^= 1;
	}

private:
	__forceinline double* GetFrontBuffer() const
	{
		return &buffers[front_buffer_index * pixel_count];
	}

private:
	Graphics(int width, int height);
	~Graphics();

private:
	// Color[pixel_count * 2]
	// It contains tow buffers, buffer0[pixel_count:0] and buffer1[pixel_count*2:pixel_count]
	double *buffers;
	int width;
	int height;
	// Equal width * height
	int pixel_count;
	int front_buffer_index;
};
#pragma once
#include "ftime.h"
#include <windows.h>
#include <iostream>
#include <wrl/client.h>
#include <d3d11_2.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

class Graphics
{
public:
	static Graphics *kInstance;

public:
	static void Initialize(HINSTANCE hinstance, int ncmdshow, WNDPROC lpfn_wnd_proc);
	static void Destroy();

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

public:
	void Render();

private:
	void CreateGameWindow(HINSTANCE hinstance, int ncmdshow, WNDPROC lpfn_wnd_proc);
	void DestroyGameWindow();
	void InitializeD3DDevice();
	void DestroyD3DDevice();
	void CompileShaderFromFile(WCHAR * file_name, LPCSTR entry_point, LPCSTR shader_model, ID3DBlob ** blob_out);

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

	HWND hwnd = nullptr;

#pragma region For D3D
	D3D_DRIVER_TYPE d3d_driver_type = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL d3d_feature_level = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device *d3d_device = nullptr;
	ID3D11Device *d3d_device_sc = nullptr;
	ID3D11DeviceContext *d3d_device_context = nullptr; 
	ID3D11DeviceContext *d3d_device_context_sc = nullptr; 
	IDXGISwapChain *d3d_swap_chain = nullptr;
	IDXGISwapChain1 *d3d_swap_chain_sc = nullptr;
	ID3D11RenderTargetView *d3d_render_target_view = nullptr;
	ID3D11VertexShader *d3d_vertex_shader = nullptr;
	ID3D11PixelShader *d3d_pixel_shader = nullptr;
	ID3D11InputLayout *d3d_vertex_layout = nullptr;
	ID3D11Buffer *d3d_vertex_buffer = nullptr;
	ID3D11ShaderResourceView* d3d_texture_rv = nullptr;
	ID3D11SamplerState* d3d_sampler_state = nullptr;
#pragma endregion

private:
	struct SimpleVertex
	{
		DirectX::XMFLOAT3 Pos;
	};
};
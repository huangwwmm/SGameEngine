#include "graphics.h"
#include "resource.h"
#include <cstring>
#include <winerror.h>

Graphics* Graphics::kInstance = nullptr;

void Graphics::Initialize(HINSTANCE hinstance, int ncmdshow, WNDPROC lpfn_wnd_proc)
{
	kInstance = new Graphics(800, 600);

	kInstance->CreateGameWindow(hinstance, ncmdshow, lpfn_wnd_proc);
	kInstance->InitializeD3DDevice();
}

void Graphics::Destroy()
{
	kInstance->DestroyGameWindow();

	delete kInstance;
}

void Graphics::Render()
{
	d3d_device_context->ClearRenderTargetView(d3d_render_target_view, DirectX::Colors::MidnightBlue);

	// Render a triangle
	d3d_device_context->VSSetShader(d3d_vertex_shader, nullptr, 0);
	d3d_device_context->PSSetShader(d3d_pixel_shader, nullptr, 0);
	d3d_device_context->Draw(3, 0);

	d3d_swap_chain->Present(0, 0);
}

void Graphics::CreateGameWindow(HINSTANCE hinstance, int ncmdshow, WNDPROC lpfn_wnd_proc)
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = lpfn_wnd_proc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hinstance;
	wcex.hIcon = LoadIcon(hinstance, (LPCTSTR)IDI_ICON1);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"SSSGameEngineClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_ICON1);
	if (!RegisterClassEx(&wcex))
	{
		// UNDONE Assert
		return;
	}

	// Create the window
	hwnd = CreateWindow(L"SSSGameEngineClass", // Window class
		L"SSS Game Engine", // Window text
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // Window style
		CW_USEDEFAULT, CW_USEDEFAULT, // Position
		Graphics::kInstance->GetWidth(), Graphics::kInstance->GetHeight(), // Size
		nullptr, // Parent window    
		nullptr, // Menu
		hinstance, // Instance handle
		nullptr // Additional application data
	);

	if (!hwnd)
	{
		// UNDONE Assert
		return;
	}

	ShowWindow(hwnd, ncmdshow);
}

void Graphics::DestroyGameWindow()
{
	DestroyWindow(hwnd);
	DestroyD3DDevice();
}

void Graphics::InitializeD3DDevice()
{
	HRESULT hr = S_OK;

	UINT create_device_flags = 0;
#ifdef _DEBUG
	create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driver_types[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT num_driver_types = ARRAYSIZE(driver_types);

	D3D_FEATURE_LEVEL feature_levels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT num_feature_levels = ARRAYSIZE(feature_levels);

	for (UINT i_driver_type = 0; i_driver_type < num_driver_types; i_driver_type++)
	{
		d3d_driver_type = driver_types[i_driver_type];
		hr = D3D11CreateDevice(nullptr, d3d_driver_type, nullptr, create_device_flags, feature_levels, num_feature_levels,
			D3D11_SDK_VERSION, &d3d_device, &d3d_feature_level, &d3d_device_context);

		if (hr == E_INVALIDARG)
		{
			// DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
			hr = D3D11CreateDevice(nullptr
				, d3d_driver_type
				, nullptr
				, create_device_flags
				, &feature_levels[1]
				, num_feature_levels - 1
				, D3D11_SDK_VERSION
				, &d3d_device
				, &d3d_feature_level
				, &d3d_device_context);
		}

		if (SUCCEEDED(hr))
		{
			break;
		}
	}
	if (FAILED(hr))
	{
		// UNDONE Assert
		return;
	}

	// Obtain DXGI factory from device (since we used nullptr for pAdapter above)
	IDXGIFactory1* dxgi_factory = nullptr;
	{
		IDXGIDevice* dxgi_device = nullptr;
		hr = d3d_device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgi_device));
		if (SUCCEEDED(hr))
		{
			IDXGIAdapter* adapter = nullptr;
			hr = dxgi_device->GetAdapter(&adapter);
			if (SUCCEEDED(hr))
			{
				hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgi_factory));
				adapter->Release();
			}
			dxgi_device->Release();
		}
	}
	if (FAILED(hr))
	{
		// UNDONE Assert
		return;
	}

	// Create swap chain
	IDXGIFactory2* dxgi_factory_swapchain = nullptr;
	hr = dxgi_factory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgi_factory_swapchain));
	if (dxgi_factory_swapchain)
	{
		// DirectX 11.1 or later
		hr = d3d_device->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&d3d_device_sc));
		if (SUCCEEDED(hr))
		{
			(void)d3d_device_context->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&d3d_device_context_sc));
		}

		DXGI_SWAP_CHAIN_DESC1 sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.Width = width;
		sd.Height = height;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;

		hr = dxgi_factory_swapchain->CreateSwapChainForHwnd(d3d_device, hwnd, &sd, nullptr, nullptr, &d3d_swap_chain_sc);
		if (SUCCEEDED(hr))
		{
			hr = d3d_swap_chain_sc->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&d3d_swap_chain));
		}

		dxgi_factory_swapchain->Release();
	}
	else
	{
		// DirectX 11.0 systems
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hwnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;

		hr = dxgi_factory->CreateSwapChain(d3d_device, &sd, &d3d_swap_chain);
	}

	// Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
	dxgi_factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
	dxgi_factory->Release();

	if (FAILED(hr))
	{
		// UNDONE Assert
		return;
	}

	// Create a render target view
	ID3D11Texture2D* back_buffer = nullptr;
	hr = d3d_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&back_buffer));
	if (FAILED(hr))
	{
		// UNDONE Assert
		return;
	}

	hr = d3d_device->CreateRenderTargetView(back_buffer, nullptr, &d3d_render_target_view);
	back_buffer->Release();
	if (FAILED(hr))
	{
		// UNDONE Assert
		return;
	}

	d3d_device_context->OMSetRenderTargets(1, &d3d_render_target_view, nullptr);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	d3d_device_context->RSSetViewports(1, &vp);

	// Compile the vertex shader
	ID3DBlob* vs_blob = nullptr;
	CompileShaderFromFile(L"draw_image.fx", "VS", "vs_4_0", &vs_blob);

	// Create the vertex shader
	hr = d3d_device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), nullptr, &d3d_vertex_shader);
	if (FAILED(hr))
	{
		// UNDONE Assert
		return;
	}

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT num_elements = ARRAYSIZE(layout);

	// Create the input layout
	hr = d3d_device->CreateInputLayout(layout, num_elements, vs_blob->GetBufferPointer(),
		vs_blob->GetBufferSize(), &d3d_vertex_layout);
	vs_blob->Release();
	if (FAILED(hr))
	{
		// UNDONE Assert
		return;
	}

	// Set the input layout
	d3d_device_context->IASetInputLayout(d3d_vertex_layout);

	// Compile the pixel shader
	ID3DBlob* ps_load = nullptr;
	CompileShaderFromFile(L"draw_image.fx", "PS", "ps_4_0", &ps_load);

	// Create the pixel shader
	hr = d3d_device->CreatePixelShader(ps_load->GetBufferPointer(), ps_load->GetBufferSize(), nullptr, &d3d_pixel_shader);
	ps_load->Release();
	if (FAILED(hr))
	{
		// UNDONE Assert
		return;
	}

	// Create vertex buffer
	SimpleVertex vertices[] =
	{
		DirectX::XMFLOAT3(0.0, 0.5f, 0.5f),
		DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f),
		DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f),
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 3;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	hr = d3d_device->CreateBuffer(&bd, &InitData, &d3d_vertex_buffer);
	if (FAILED(hr))
	{
		// UNDONE Assert
		return;
	}

	// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	d3d_device_context->IASetVertexBuffers(0, 1, &d3d_vertex_buffer, &stride, &offset);

	// Set primitive topology
	d3d_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Graphics::DestroyD3DDevice()
{
	if (d3d_device_context) d3d_device_context->ClearState();

	if (d3d_render_target_view) d3d_render_target_view->Release();
	if (d3d_swap_chain_sc) d3d_swap_chain_sc->Release();
	if (d3d_swap_chain) d3d_swap_chain->Release();
	if (d3d_device_context_sc) d3d_device_context_sc->Release();
	if (d3d_device_context) d3d_device_context->Release();
	if (d3d_device_sc) d3d_device_sc->Release();
	if (d3d_device) d3d_device->Release();
}

void Graphics::CompileShaderFromFile(WCHAR* file_name, LPCSTR entry_point, LPCSTR shader_model, ID3DBlob** blob_out)
{
	HRESULT hr = S_OK;

	DWORD shader_flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	shader_flags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	shader_flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* error_blob = nullptr;
	hr = D3DCompileFromFile(file_name, nullptr, nullptr, entry_point, shader_model,
		shader_flags, 0, blob_out, &error_blob);
	if (FAILED(hr)
		|| error_blob)
	{
		// Assert;
		return;
	}
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
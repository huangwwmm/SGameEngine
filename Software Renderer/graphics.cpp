#include "graphics.h"

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
	int* back_buffer = GetBackBuffer();
	
	// TEST
	int r = 255;
	int g = 0;
	int b = 0;
	int a = 255;
	static int s = 0;
	s = s ^ 1;
	r = s ? 255 : 0;
	g = s ? 0 : 255;

	while (WaitForSingleObject(d3d_thread_semaphore, 0L) != WAIT_OBJECT_0);
	for (int i_height = 0; i_height < height; i_height++)
	{
		for (int i_width = 0; i_width < width; i_width++)
		{
			int color = r | (g << 8) | (b << 16) | (a << 24);
			back_buffer[i_height * width + i_width] = color;
		}
	}
	ReleaseSemaphore(d3d_thread_semaphore, 1, nullptr);

	kInstance->SwapBuffer();
}

DWORD WINAPI Graphics::DrawWithD3D(LPVOID lp_param)
{
	while (true)
	{
		kInstance->d3d_device_context->ClearRenderTargetView(kInstance->d3d_render_target_view, DirectX::Colors::Black);

		// Render a triangle
		kInstance->d3d_device_context->VSSetShader(kInstance->d3d_vertex_shader, nullptr, 0);
		kInstance->d3d_device_context->PSSetShader(kInstance->d3d_pixel_shader, nullptr, 0);

		D3D11_MAPPED_SUBRESOURCE resource;
		UINT subresource = D3D11CalcSubresource(0, 0, 0);
		HRESULT hr = kInstance->d3d_device_context->Map(kInstance->d3d_texture2d, subresource, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		ASSERT_HRESULT(hr);

		while (WaitForSingleObject(kInstance->d3d_thread_semaphore, 0L) != WAIT_OBJECT_0);
		memcpy(resource.pData, kInstance->GetFrontBuffer(), kInstance->pixel_count * sizeof(int));
		ReleaseSemaphore(kInstance->d3d_thread_semaphore, 1, nullptr);
		kInstance->d3d_device_context->Unmap(kInstance->d3d_texture2d, subresource);

		kInstance->d3d_device_context->PSSetShaderResources(0, 1, &kInstance->d3d_texture_rv);
		kInstance->d3d_device_context->PSSetSamplers(0, 1, &kInstance->d3d_sampler_state);
		kInstance->d3d_device_context->DrawIndexed(6, 0, 0);

		kInstance->d3d_swap_chain->Present(0, 0);
	}

	return 0;
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
	RegisterClassEx(&wcex);

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
	ASSERT_HRESULT(hr);

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
	ASSERT_HRESULT(hr);

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
	ASSERT_HRESULT(hr);

	// Create a render target view
	ID3D11Texture2D* back_buffer = nullptr;
	hr = d3d_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&back_buffer));
	ASSERT_HRESULT(hr);

	hr = d3d_device->CreateRenderTargetView(back_buffer, nullptr, &d3d_render_target_view);
	back_buffer->Release();
	ASSERT_HRESULT(hr);

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
	ASSERT_HRESULT(hr);

	// Create the Texture
	D3D11_TEXTURE2D_DESC texture2d_desc;
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.Usage = D3D11_USAGE_DYNAMIC;
	texture2d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2d_desc.MiscFlags = 0;

	hr = d3d_device->CreateTexture2D(&texture2d_desc, NULL, &d3d_texture2d);
	ASSERT_HRESULT(hr);

	// Create shader resource view from texture2d
	D3D11_SHADER_RESOURCE_VIEW_DESC resource_view_desc;
	ZeroMemory(&resource_view_desc, sizeof(resource_view_desc));
	resource_view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resource_view_desc.Texture2D.MipLevels = 1;

	hr = d3d_device->CreateShaderResourceView(d3d_texture2d, &resource_view_desc, &d3d_texture_rv);
	ASSERT_HRESULT(hr);

	// Create the sample state
	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = d3d_device->CreateSamplerState(&sampler_desc, &d3d_sampler_state);
	ASSERT_HRESULT(hr);

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT num_elements = ARRAYSIZE(layout);

	// Create the input layout
	hr = d3d_device->CreateInputLayout(layout, num_elements, vs_blob->GetBufferPointer(),
		vs_blob->GetBufferSize(), &d3d_vertex_layout);
	vs_blob->Release();
	ASSERT_HRESULT(hr);

	// Set the input layout
	d3d_device_context->IASetInputLayout(d3d_vertex_layout);

	// Compile the pixel shader
	ID3DBlob* ps_load = nullptr;
	CompileShaderFromFile(L"draw_image.fx", "PS", "ps_4_0", &ps_load);

	// Create the pixel shader
	hr = d3d_device->CreatePixelShader(ps_load->GetBufferPointer(), ps_load->GetBufferSize(), nullptr, &d3d_pixel_shader);
	ps_load->Release();
	ASSERT_HRESULT(hr);

	// Create vertex buffer
	SimpleVertex vertices[] =
	{
		{ DirectX::XMFLOAT3(-1.0, 1.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 0.0f) },
		{ DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 1.0f) },
		{ DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 1.0f) },
		{ DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 0.0f) },
	};

	D3D11_BUFFER_DESC vertex_buffer_desc = { 0 };
	vertex_buffer_desc.ByteWidth = sizeof(SimpleVertex) * ARRAYSIZE(vertices);
	vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertex_buffer_desc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vertex_buffer_data = { 0 };
	vertex_buffer_data.pSysMem = vertices;

	hr = d3d_device->CreateBuffer(&vertex_buffer_desc, &vertex_buffer_data, &d3d_vertex_buffer);
	ASSERT_HRESULT(hr);

	// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	d3d_device_context->IASetVertexBuffers(0, 1, &d3d_vertex_buffer, &stride, &offset);

	// Create index buffer
	WORD indices[] =
	{
		0,1,2,
		0,3,1
	};
	
	vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vertex_buffer_desc.ByteWidth = sizeof(WORD) * ARRAYSIZE(indices);        // 36 vertices needed for 12 triangles in a triangle list
	vertex_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	vertex_buffer_desc.CPUAccessFlags = 0;
	vertex_buffer_data.pSysMem = indices;
	hr = d3d_device->CreateBuffer(&vertex_buffer_desc, &vertex_buffer_data, &d3d_index_buffer);
	ASSERT_HRESULT(hr);
	// Set index buffer
	d3d_device_context->IASetIndexBuffer(d3d_index_buffer, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	d3d_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create draw thread
	d3d_thread_handle = CreateThread(nullptr, 0, Graphics::DrawWithD3D, nullptr, 0, &d3d_thread_handle_id);
	d3d_thread_semaphore = CreateSemaphore(
		nullptr,    // default security attributes
		1,			// initial count
		2,			// maximum count
		nullptr);   // unnamed semaphore
}

void Graphics::DestroyD3DDevice()
{
	if (d3d_device_context) d3d_device_context->ClearState();

	if (d3d_texture2d) d3d_texture2d->Release();
	if (d3d_sampler_state) d3d_sampler_state->Release();
	if (d3d_texture_rv) d3d_texture_rv->Release();
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

	buffers = new int[pixel_count * 2];
	memset(buffers, 0, sizeof(int) * pixel_count * 2);
}

Graphics::~Graphics()
{
	delete[] buffers;
}
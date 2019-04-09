#include <assert.h>
#include "Graphics.h"
#include "d3d11_1.h"

using namespace DirectX;

Graphics::Graphics(): swapchain(NULL), dev(NULL),devcon(NULL), backbuffer(NULL), camera()
{
	
}

Graphics::~Graphics()
{
}

Graphics& Graphics::GetInstance()
{
	static Graphics graphics;
	return graphics;
}

void Graphics::Initialize(HWND hWnd, int width, int height)
{
	camera.SetFrustum(XMConvertToRadians(45.0f), (float)width / (float)height, 0.3f, 5000.0f);
	camera.SetPosition(XMVectorSet(10.0f, 0.0f, 5.0f,1.0f));
	camera.SetLookDirection(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
	 // create a struct to hold information about the swap chain
    DXGI_SWAP_CHAIN_DESC scd;

    // clear out the struct for use
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	//optional?? check 4x MSAA quality support
	/*UINT m4xMsaaQuality;
	dev->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM,4,&m4xMsaaQuality);
	assert(m4xMsaaQuality>0);*/

    // fill the swap chain description struct
    scd.BufferCount = 1;                                    // one back buffer
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
    scd.OutputWindow = hWnd;                                // the window to be used
    scd.SampleDesc.Count = 1;     
	scd.SampleDesc.Quality = 0;// how many multisamples
    scd.Windowed = TRUE;                                    // windowed/full-screen mode
	scd.BufferDesc.Width = width;							// set the back buffer width
    scd.BufferDesc.Height = height;							// set the back buffer height
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;     // allow full-screen switching
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//scd.SampleDesc.Quality=m4xMsaaQuality-1;

    // create a device, device context and swap chain using the information in the scd struct
    D3D11CreateDeviceAndSwapChain(NULL,
                                  D3D_DRIVER_TYPE_HARDWARE,
                                  NULL,
                                  NULL/*D3D11_CREATE_DEVICE_DEBUG*/,
                                  NULL,
                                  NULL,
                                  D3D11_SDK_VERSION,
                                  &scd,
                                  &swapchain,
                                  &dev,
                                  NULL,
                                  &devcon);

	

	// get the address of the back buffer
    ID3D11Texture2D *pBackBuffer;
    swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    // use the back buffer address to create the render target
    dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
    pBackBuffer->Release();

	//Create deph buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width=width;
	depthStencilDesc.Height=height;
	depthStencilDesc.MipLevels=1;
	depthStencilDesc.ArraySize=1;
	depthStencilDesc.Format= DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count=1;
	depthStencilDesc.SampleDesc.Quality = 0;
	//depthStencilDesc.SampleDesc.Quality=m4xMsaaQuality-1;
	depthStencilDesc.Usage=D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags=D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags=0;
	depthStencilDesc.MiscFlags=0;

	ID3D11Texture2D* pDepthStencilBuffer;
	
	dev->CreateTexture2D(&depthStencilDesc,0,&pDepthStencilBuffer);


	D3D11_DEPTH_STENCIL_DESC dsDesc;

	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	//dsDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;

	// Stencil test parameters
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	ID3D11DepthStencilState * pDSState;
	dev->CreateDepthStencilState(&dsDesc, &pDSState);

	// Bind depth stencil state
	devcon->OMSetDepthStencilState(pDSState, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	dev->CreateDepthStencilView(pDepthStencilBuffer, NULL,&mDepthStencilView);
    // set the render target as the back buffer
    devcon->OMSetRenderTargets(1, &backbuffer, mDepthStencilView);

	CreateViewPort(width, height);
	CreateShaders();
}

void Graphics::CreateShaders()
{
	// load and compile the two shaders
    ID3D10Blob *VS, *PS;
    D3DCompileFromFile(L"VertexShader.shader", 0, 0, "VShader", "vs_4_0", 0, 0, &VS, 0);
    D3DCompileFromFile(L"PixelShader.shader", 0, 0, "PShader", "ps_4_0", 0, 0, &PS, 0);

	// encapsulate both shaders into shader objects
    dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
    dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);


    // set the shader objects
    devcon->VSSetShader(pVS, 0, 0);
    devcon->PSSetShader(pPS, 0, 0);

	 // create the input layout object
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    dev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);

	//set the input layout, these changes if we are gonig to draw with another layout
	devcon->IASetInputLayout(pLayout);

	//this goes here??
	CreateBuffer();
}

void Graphics::CreateBuffer()
{
	// create a triangle using the VERTEX struct
    /*VERTEX OurVertices[] =
    {
        {0.0f, 0.5f, 0.0f, PackedVector::XMCOLOR(1.0f, 0.0f, 0.0f, 1.0f)},
        {0.45f, -0.5, 0.0f, PackedVector::XMCOLOR(0.0f, 1.0f, 0.0f, 1.0f)},
        {-0.45f, -0.5f, 0.0f, PackedVector::XMCOLOR(0.0f, 0.0f, 1.0f, 1.0f)}
    };*/

	
	sphere = MeshGenerator::GenerateSphere(	1.5f, 20, 20);
	std::vector<VERTEX> outVertices(sphere.vertices.size());

	DirectX::XMFLOAT4 black(0.0f, 0.0f, 0.0f, 1.0f);

	for (unsigned i = 0;i < sphere.vertices.size();++i)
	{
		outVertices[i].position = sphere.vertices[i].position;
		outVertices[i].color = black;
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	int numberVertex= sphere.vertices.size();
	bd.Usage = D3D11_USAGE_IMMUTABLE;                // write access access by CPU and GPU
	bd.ByteWidth = sizeof(VERTEX) * numberVertex;             // size is the VERTEX struct * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = 0;    // allow CPU to write in buffer
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vInitData;
	vInitData.pSysMem = &outVertices[0];


	dev->CreateBuffer(&bd, &vInitData, &pVBuffer);       // create the buffer

	//indices
	std::vector<unsigned> indices = sphere.indices;

	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	numberVertex = sphere.indices.size();
	ibd.Usage = D3D11_USAGE_IMMUTABLE;                // write access access by CPU and GPU
	ibd.ByteWidth = sizeof(unsigned) * numberVertex;             // size is the VERTEX struct * 3
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;       // use as a vertex buffer
	ibd.CPUAccessFlags = 0;    // allow CPU to write in buffer
	ibd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iInitData;
	iInitData.pSysMem = &indices[0];
	dev->CreateBuffer(&ibd, &iInitData, &indicesBuffer);

	//temp world matrix
	XMMATRIX scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX rotation = XMMatrixRotationQuaternion(XMQuaternionIdentity());
	XMMATRIX translation = XMMatrixTranslation(0.0f, 3.0f, -1.0f);
	

	XMMATRIX world = scale *rotation*translation;
	//XMMATRIX world = translation * rotation*scale;//XMMatrixMultiply(scale, translation);
	//constant buffer
	VS_CONSTANT_BUFFER VsConstData;
	XMMATRIX wvp = world *camera.GetViewMatrix()*camera.GetProjectionMatrix();
	wvp=XMMatrixTranspose(wvp);
	XMStoreFloat4x4(&VsConstData.worldViewProjection,wvp);
	

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CONSTANT_BUFFER);
	cbDesc.Usage = D3D11_USAGE_IMMUTABLE;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &VsConstData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	dev->CreateBuffer(&cbDesc, &InitData,&pConstantBuffer11);

	


}

void Graphics::Close()
{	
	swapchain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode, since direct x can not close while in full screen

	// close and release all existing COM objects
	pVS->Release();
    pPS->Release();
    swapchain->Release();
	backbuffer->Release();
    dev->Release();
    devcon->Release();
}

void Graphics::CreateViewPort(int width, int height)
{
	// Set the viewport
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = (float)width;
    viewport.Height = (float)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

    devcon->RSSetViewports(1, &viewport);

	

	D3D11_RASTERIZER_DESC rasterizerState;
	ZeroMemory(&rasterizerState, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerState.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerState.CullMode = D3D11_CULL_NONE;
	rasterizerState.FrontCounterClockwise = false;
	rasterizerState.DepthClipEnable = true;
	//rasterizerState.MultisampleEnable = true;

	dev->CreateRasterizerState(&rasterizerState, &g_pRasterState);

	
}

//render the stufff!!!
void Graphics::Update(float dt)
{
	// clear the back buffer to a deep blue
	const float c[4]= { 0.0f, 0.2f, 0.4f, 1.0f };
    devcon->ClearRenderTargetView(backbuffer, c);
	devcon->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0F, 0);
    // select which vertex buffer to display
    UINT stride = sizeof(VERTEX);
    UINT offset = 0;
    devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
	devcon->IASetIndexBuffer(indicesBuffer, DXGI_FORMAT_R32_UINT, 0);
    // select which primtive type we are using
    devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devcon->VSSetConstantBuffers(0, 1, &pConstantBuffer11);
	devcon->RSSetState(g_pRasterState);
    // draw the vertex buffer to the back buffer
	devcon->DrawIndexed(sphere.indices.size(), 0, 0);

    // switch the back buffer and the front buffer
    swapchain->Present(0, 0);
}
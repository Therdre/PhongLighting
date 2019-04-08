#include <assert.h>
#include "Graphics.h"

using namespace DirectX;

Graphics::Graphics(): swapchain(NULL), dev(NULL),devcon(NULL), backbuffer(NULL), camera(Camera())
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
	camera.SetFrustum(XMConvertToRadians(30.0f), (float)width / (float)height, 0.1f, 1000.0f);
	camera.SetPosition(XMVectorSet(0.0f, 0.0f, -5.0f,1.0f));
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
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
    scd.OutputWindow = hWnd;                                // the window to be used
    scd.SampleDesc.Count = 4;                               // how many multisamples
    scd.Windowed = TRUE;                                    // windowed/full-screen mode
	scd.BufferDesc.Width = width;							// set the back buffer width
    scd.BufferDesc.Height = height;							// set the back buffer height
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;     // allow full-screen switching
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
	depthStencilDesc.SampleDesc.Count=4;
	//depthStencilDesc.SampleDesc.Quality=m4xMsaaQuality-1;
	depthStencilDesc.Usage=D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags=D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags=0;
	depthStencilDesc.MiscFlags=0;

	ID3D11Texture2D* pDepthStencilBuffer;
	
	dev->CreateTexture2D(&depthStencilDesc,0,&pDepthStencilBuffer);
	dev->CreateDepthStencilView(pDepthStencilBuffer,0,&mDepthStencilView);
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
    VERTEX OurVertices[] =
    {
        {0.0f, 0.5f, 0.0f, PackedVector::XMCOLOR(1.0f, 0.0f, 0.0f, 1.0f)},
        {0.45f, -0.5, 0.0f, PackedVector::XMCOLOR(0.0f, 1.0f, 0.0f, 1.0f)},
        {-0.45f, -0.5f, 0.0f, PackedVector::XMCOLOR(0.0f, 0.0f, 1.0f, 1.0f)}
    };


	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	int numberVertex=3;
	bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
	bd.ByteWidth = sizeof(VERTEX) * numberVertex;             // size is the VERTEX struct * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

	dev->CreateBuffer(&bd, NULL, &pVBuffer);       // create the buffer

	// copy the vertices into the buffer
    D3D11_MAPPED_SUBRESOURCE ms;
    devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
    memcpy(ms.pData, OurVertices, sizeof(OurVertices));                 // copy the data
    devcon->Unmap(pVBuffer, NULL); 

	//temp world matrix
	XMMATRIX scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, 2.0f);

	XMMATRIX world = XMMatrixMultiply(scale,translation);
	//constant buffer
	VS_CONSTANT_BUFFER VsConstData;
	XMMATRIX wvp = world *camera.GetViewMatrix()*camera.GetProjectionMatrix();
	//XMMatrixTranspose(wvp);
	XMStoreFloat4x4(&VsConstData.worldViewProjection,wvp);
	

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CONSTANT_BUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &VsConstData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	
	XMVECTOR test = XMVector4Transform(XMVectorSet(0.0f, 0.5f, 0.0f, 1.0f), wvp);
	XMVECTOR test1 = XMVector4Transform(XMVectorSet(0.45f, 0.5f, 0.0f, 1.0f),wvp);
	XMVECTOR test2 = XMVector4Transform(XMVectorSet(-0.45f, -0.5f, 0.0f, 1.0f),wvp);

	XMFLOAT4 t1, t2, t3;
	XMStoreFloat4(&t1,test);
	XMStoreFloat4(&t2, test1);
	XMStoreFloat4(&t3, test2);

	dev->CreateBuffer(&cbDesc, &InitData,
		&pConstantBuffer11);

	


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

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = width;
    viewport.Height = height;

    devcon->RSSetViewports(1, &viewport);
}

//render the stufff!!!
void Graphics::Update(float dt)
{
	// clear the back buffer to a deep blue
	const float c[4]= { 0.0f, 0.2f, 0.4f, 1.0f };
    devcon->ClearRenderTargetView(backbuffer, c);

    // select which vertex buffer to display
    UINT stride = sizeof(VERTEX);
    UINT offset = 0;
    devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
	
    // select which primtive type we are using
    devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devcon->VSSetConstantBuffers(0, 1, &pConstantBuffer11);
    // draw the vertex buffer to the back buffer
    devcon->Draw(3, 0);

    // switch the back buffer and the front buffer
    swapchain->Present(0, 0);
}
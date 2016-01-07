#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

struct VERTEX
{	float x, y, z;
	D3DXCOLOR Color;
};

class Graphics
{
	public:
		Graphics();
		~Graphics();

		void Initialize(HWND hWnd, int width, int height);
		void Update(float dt);
		void Close();
	private:

		IDXGISwapChain *swapchain;             // the pointer to the swap chain interface
		ID3D11Device *dev;                     // the pointer to our Direct3D device interface
		ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context
		ID3D11RenderTargetView *backbuffer;    // back buffer
		ID3D11VertexShader *pVS;    // the vertex shader
		ID3D11PixelShader *pPS;     // the pixel shader
		ID3D11Buffer *pVBuffer;    // vertex buffer
		ID3D11InputLayout *pLayout;    // global
		ID3D11DepthStencilView* mDepthStencilView;

		void CreateViewPort(int width, int height);

		void CreateShaders();

		void CreateBuffer();
};

#endif
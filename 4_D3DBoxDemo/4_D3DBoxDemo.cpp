/**********************************************************************
 @FILE		4_D3DBoxDemo.cpp
 @BRIEF		Box demo application class.
 @AUTHOR	Ray1024
 @DATE		2016.11.24
 *********************************************************************/
#include "../Common/D3D11App.h"
#include "../Common/MathHelper.h"
#include "d3dx11Effect.h"

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

class D3D11BoxDemoApp : public D3D11App
{
public:
	D3D11BoxDemoApp(HINSTANCE hInstance);
	~D3D11BoxDemoApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene(); 

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void BuildGeometryBuffers();
	void BuildFX();
	void BuildVertexLayout();

private:
	ID3D11Buffer* m_pBoxVB;	// ���㻺����
	ID3D11Buffer* m_pBoxIB; // ����������

	ID3DX11Effect* m_pFX;	// effect
	ID3DX11EffectTechnique* m_pTech;	// technique
	ID3DX11EffectMatrixVariable* m_pFXWorldViewProj;	// �洢effect�еı���

	ID3D11InputLayout* m_pInputLayout;	// �������벼��

	ID3D11RasterizerState* m_pWireframeRS;	// �߿�ģʽ
	ID3D11RasterizerState* m_pSolidRS;	// ʵ��ģʽ

	XMFLOAT4X4 m_world;
	XMFLOAT4X4 m_view;	
	XMFLOAT4X4 m_proj;	

	float m_theta;	//
	float m_phi;	//
	float m_radius;	//�뾶

	POINT m_lastMousePos;
};

// �������
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	D3D11BoxDemoApp theApp(hInstance);

	if( !theApp.Init() )
		return 0;

	return theApp.Run();
}

//
// D3D11BoxDemoApp Implement
//

D3D11BoxDemoApp::D3D11BoxDemoApp(HINSTANCE hInstance)
	: D3D11App(hInstance)
	, m_pBoxVB(0)
	, m_pBoxIB(0)
	, m_pFX(0)
	, m_pTech(0)
	, m_pFXWorldViewProj(0)
	, m_pInputLayout(0)
	, m_theta(1.5f*MathHelper::Pi)
	, m_phi(0.25f*MathHelper::Pi)
	, m_radius(5.0f)
	, m_pWireframeRS(NULL)
	, m_pSolidRS(NULL)
{
	m_mainWndCaption = L"4_D3DBoxDemo";

	m_lastMousePos.x = 0;
	m_lastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&m_world, I);
	XMStoreFloat4x4(&m_view, I);
	XMStoreFloat4x4(&m_proj, I);
}

D3D11BoxDemoApp::~D3D11BoxDemoApp()
{
	ReleaseCOM(m_pBoxVB);
	ReleaseCOM(m_pBoxIB);
	ReleaseCOM(m_pFX);
	ReleaseCOM(m_pInputLayout);
	ReleaseCOM(m_pWireframeRS);
	ReleaseCOM(m_pSolidRS);
}

bool D3D11BoxDemoApp::Init()
{
	if(!D3D11App::Init())
		return false;

	BuildGeometryBuffers();
	BuildFX();
	BuildVertexLayout();

	// �ڳ�ʼ��ʱ�����������Ⱦ״̬
	D3D11_RASTERIZER_DESC rsDesc; 
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC)); 
	rsDesc.FillMode = D3D11_FILL_WIREFRAME; 
	rsDesc.CullMode = D3D11_CULL_NONE; 
	rsDesc.FrontCounterClockwise = false; 
	rsDesc.DepthClipEnable = true;

	HR(m_pD3DDevice->CreateRasterizerState(&rsDesc,&m_pWireframeRS));

	return true;
}

void D3D11BoxDemoApp::OnResize()
{
	D3D11App::OnResize();

	// �����ڴ�С�ı�ʱ����Ҫ���º��ݱȣ������¼���ͶӰ����
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_proj, P);
}

void D3D11BoxDemoApp::UpdateScene(float dt)
{
	/************************************************************************/
	/* 6.����ÿ֡�ľ���任                                                 */
	/************************************************************************/

	// �ӽǱ任����

	// ����������ת��Ϊ�ѿ�������
	float x = m_radius*sinf(m_phi)*cosf(m_theta);
	float z = m_radius*sinf(m_phi)*sinf(m_theta);
	float y = m_radius*cosf(m_phi);

	XMVECTOR pos    = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up     = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&m_view, V);

	// �������任��ˣ��ϳ�һ��
	XMMATRIX world = XMLoadFloat4x4(&m_world);
	XMMATRIX view  = XMLoadFloat4x4(&m_view);
	XMMATRIX proj  = XMLoadFloat4x4(&m_proj);
	XMMATRIX worldViewProj = world*view*proj;

	// ͨ��C++�������Shader��Ӧ�ı���
	m_pFXWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
}

void D3D11BoxDemoApp::DrawScene()
{
	/************************************************************************/
	/* 7.��������                                                           */
	/************************************************************************/

	// ����
	m_pD3DImmediateContext->ClearRenderTargetView(m_pRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	m_pD3DImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	// ָ�����벼�֡�ͼԪ�������͡����㻺�塢�������塢��Ⱦ״̬
	m_pD3DImmediateContext->IASetInputLayout(m_pInputLayout);
	m_pD3DImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	m_pD3DImmediateContext->IASetVertexBuffers(0, 1, &m_pBoxVB, &stride, &offset);
	m_pD3DImmediateContext->IASetIndexBuffer(m_pBoxIB, DXGI_FORMAT_R32_UINT, 0);
	// �Ƿ�ʹ���߿�ģʽ
	//m_pD3DImmediateContext->RSSetState(m_pWireframeRS);

	// ��technique��ȡpass�������Ⱦ
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pTech->GetPassByIndex(p)->Apply(0, m_pD3DImmediateContext);
		m_pD3DImmediateContext->DrawIndexed(36, 0, 0);// ��������36������
	}

	// ��ʾ
	HR(m_pSwapChain->Present(0, 0));
}

void D3D11BoxDemoApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_lastMousePos.x = x;
	m_lastMousePos.y = y;

	// һ�����ڲ�������꣬����������붼��Ըô��ڣ����۹���Ƿ��ڴ��ڵı߽���
	SetCapture(m_hMainWnd);
}

void D3D11BoxDemoApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	// ����Ҫ������������Ϣ���ͷŵ�
	ReleaseCapture();
}

void D3D11BoxDemoApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	// �����������ӽǽǶ�
	if( (btnState & MK_LBUTTON) != 0 )
	{
		// �������Pos��lastPos��x/y�����ϵı仯��dx/dy�ó��Ƕȣ�ÿ�����صľ����൱��1�ȣ��ٽ��Ƕ�תΪ����
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - m_lastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - m_lastMousePos.y));

		// ����dx��dy���½Ƕ�
		m_theta += dx;
		m_phi   += dy;

		// ���ƽǶ�m_phi
		m_phi = MathHelper::Clamp(m_phi, 0.1f, MathHelper::Pi-0.1f);
	}
	// �����������ӽǾ���
	else if( (btnState & MK_RBUTTON) != 0 )
	{
		// ʹÿ�����ض�Ӧ�ڳ����е�0.005����Ԫ
		float dx = 0.005f*static_cast<float>(x - m_lastMousePos.x);
		float dy = 0.005f*static_cast<float>(y - m_lastMousePos.y);

		// ��������������������뾶
		m_radius += dx - dy;

		// ����m_radius
		m_radius = MathHelper::Clamp(m_radius, 3.0f, 15.0f);
	}

	m_lastMousePos.x = x;
	m_lastMousePos.y = y;
}

void D3D11BoxDemoApp::BuildGeometryBuffers()
{
	/************************************************************************/
	/* 1.�������㻺��                                                       */
	/************************************************************************/
	Vertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), (const float*)&Colors::White   },
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f), (const float*)&Colors::Black   },
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f), (const float*)&Colors::Red     },
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f), (const float*)&Colors::Green   },
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), (const float*)&Colors::Blue    },
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f), (const float*)&Colors::Yellow  },
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), (const float*)&Colors::Cyan    },
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f), (const float*)&Colors::Magenta }
	};
	// ׼���ṹ�壬����������
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * 8;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	// ׼���ṹ�壬Ϊ������ָ����ʼ������
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	// ����������
	HR(m_pD3DDevice->CreateBuffer(&vbd, &vinitData, &m_pBoxVB));

	/************************************************************************/
	/* 2.������������                                                       */
	/************************************************************************/
	UINT indices[] = {
		// ǰ����
		0, 1, 2,
		0, 2, 3,

		// �����
		4, 6, 5,
		4, 7, 6,

		// �����
		4, 5, 1,
		4, 1, 0,

		// �ұ���
		3, 2, 6,
		3, 6, 7,

		// �ϱ���
		1, 5, 6,
		1, 6, 2,

		// �±���
		4, 0, 3, 
		4, 3, 7
	};
	// ׼���ṹ�壬����������
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 36;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	// ׼���ṹ�壬Ϊ������ָ����ʼ������
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	// ����������
	HR(m_pD3DDevice->CreateBuffer(&ibd, &iinitData, &m_pBoxIB));
}

void D3D11BoxDemoApp::BuildFX()
{
	/************************************************************************/
	/* 4.������ɫ��������Effect                                             */
	/************************************************************************/
	// ������ɫ������
	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;
	HRESULT hr = D3DX11CompileFromFile(L"FX/color.fx", 0, 0, 0, "fx_5_0", shaderFlags, 
		0, 0, &compiledShader, &compilationMsgs, 0);

	// compilationMsgs�а�������򾯸���Ϣ
	if( compilationMsgs != 0 )
	{
		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		ReleaseCOM(compilationMsgs);
	}

	// ����û��compilationMsgs��Ҳ��Ҫȷ��û����������
	if(FAILED(hr))
	{
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
	}

	// ����Effect
	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 
		0, m_pD3DDevice, &m_pFX));

	// ��������ͷ���Դ
	ReleaseCOM(compiledShader);

	// ��Effect�л�ȡtechnique����
	m_pTech    = m_pFX->GetTechniqueByName("ColorTech");
	// ��Effect�л�ȡ��������
	m_pFXWorldViewProj = m_pFX->GetVariableByName("gWorldViewProj")->AsMatrix();
}

void D3D11BoxDemoApp::BuildVertexLayout()
{
/************************************************************************/
/* 5.�������벼��                                                       */
/************************************************************************/
// �������벼������
D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

// ��technique�����л�ȡpass��Ϣ
D3DX11_PASS_DESC passDesc;
m_pTech->GetPassByIndex(0)->GetDesc(&passDesc);

// �����������벼��
HR(m_pD3DDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, 
	passDesc.IAInputSignatureSize, &m_pInputLayout));
}
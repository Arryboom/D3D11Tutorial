/**********************************************************************
 @FILE		D3D11App.h
 @BRIEF		Simple Direct3D demo application class.
 @AUTHOR	Ray1024
 @DATE		2016.11.16
 *********************************************************************/

#ifndef D3D11App_H
#define D3D11App_H

#include "D3D11Util.h"
#include "GameTimer.h"
#include <string>

class D3D11App
{
public:
	D3D11App(HINSTANCE hInstance);
	virtual ~D3D11App();
	
	// ��ȡӦ�ó���ʵ�����
	HINSTANCE AppInst()const;
	// ��ȡ�����ھ��
	HWND MainWnd()const;
	// ��̨�������ĳ����
	float AspectRatio()const;
	// Ӧ�ó�����Ϣѭ��
	int Run();
 
	// ��ܷ���
	// ��������Ҫ������Щ����ʵ������Ĺ���

	virtual bool Init();
	virtual void OnResize(); 
	virtual void UpdateScene(float dt)=0;
	virtual void DrawScene()=0; 
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// ������������¼��ı�����غ���
	virtual void OnMouseDown(WPARAM btnState, int x, int y){ }
	virtual void OnMouseUp(WPARAM btnState, int x, int y)  { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y){ }

protected:

	// ��������
	bool InitMainWindow();

	// ��ʼ��D3D
	bool InitDirect3D();

	// ����֡��
	void CalculateFrameStats();

protected:

	HINSTANCE m_hAppInst;		// Ӧ�ó���ʵ�����
	HWND      m_hMainWnd;		// �����ھ��
	bool      m_appPaused;		// �����Ƿ�����ͣ״̬
	bool      m_minimized;		// �����Ƿ���С��
	bool      m_maximized;		// �����Ƿ����
	bool      m_Resizing;		// �����Ƿ��ڸı��С��״̬
	UINT      m_4xMsaaQuality;	// 4X MSAA�����ȼ�

	GameTimer m_timer;			// ���ڼ�¼deltatime����Ϸʱ��

	ID3D11Device*			m_pD3DDevice;			// D3D11�豸
	ID3D11DeviceContext*	m_pD3DImmediateContext;	// ������
	IDXGISwapChain*			m_pSwapChain;			// ������
	ID3D11Texture2D*		m_pDepthStencilBuffer;	// ��Ȼ�����
	ID3D11RenderTargetView* m_pRenderTargetView;	// ��ȾĿ����ͼ
	ID3D11DepthStencilView* m_pDepthStencilView;	// ��Ȼ�����ͼ	
	D3D11_VIEWPORT			m_screenViewport;		// �ӿ�


	std::wstring	m_mainWndCaption;		// ���ڱ���
	D3D_DRIVER_TYPE m_D3DDriverType;		// �Ƿ�ʹ��Ӳ������
	int				m_clientWidth;			// ���ڴ�С
	int				m_clientHeight;			// ���ڴ�С
	bool			m_enable4xMsaa;			// �Ƿ�ʹ��4XMSAA
};

#endif // D3D11App_H
#pragma once
#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_


//#include <windows.h>

#include "inputclass.h"
#include "graphicsclass.h"

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();
	HWND& GetHWND();

	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

	// todo ���� �ʿ�, ���� ���� �ƴ�.
	// SystemClass ��ü �ϳ��� ��������� ������ �޽����ڽ� �뵵�� �����.
	static SystemClass* ApplicationHandle;

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();


private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input = nullptr;
	GraphicsClass* m_Graphics = nullptr;
	
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//static SystemClass* ApplicationHandle = nullptr;




#endif

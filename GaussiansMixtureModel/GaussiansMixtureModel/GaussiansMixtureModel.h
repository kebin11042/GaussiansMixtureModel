
// GaussiansMixtureModel.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CGaussiansMixtureModelApp:
// �� Ŭ������ ������ ���ؼ��� GaussiansMixtureModel.cpp�� �����Ͻʽÿ�.
//

class CGaussiansMixtureModelApp : public CWinApp
{
public:
	CGaussiansMixtureModelApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CGaussiansMixtureModelApp theApp;
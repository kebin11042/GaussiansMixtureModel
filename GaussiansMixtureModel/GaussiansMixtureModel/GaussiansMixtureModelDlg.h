
// GaussiansMixtureModelDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "atltypes.h"
#include "gmm_process.h"

// CGaussiansMixtureModelDlg 대화 상자
class CGaussiansMixtureModelDlg : public CDialogEx
{
// 생성입니다.
public:
	CGaussiansMixtureModelDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_GAUSSIANSMIXTUREMODEL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLoadimages();
	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	
	void GetImage();	
	bool IMGProTransBMPToBuffer(char *Image_Path);
	void SetStatic(void);
	void DrawOriginalImage(void);
	void DrawBackgroundImage(void);
	void DrawForegroundImage(void);
	void DrawFirstFrame(void);
	void InItLabelingImage(void);
	void DrawLabelingImage(void);

	static UINT ThreadDrawImages(LPVOID pParam);

	BITMAPINFOHEADER m_bmpih;
	RGBQUAD RGB[256];

	CStatic m_static_origin;
	CStatic m_static_background;
	CStatic m_static_foreground;
	CRect m_static_rect;
	HDC m_hdc;
	CStatic m_static_origin_text;
	CStatic m_static_back_text;
	CStatic m_static_fore_text;
	CStatic m_static_label;
	CStatic m_static_label_text;
	CStatic m_static_1;

	TCHAR m_szFolderPath[256];
	bool THREADSTOP_FLAG;
	bool FIRSTDRAW_FLAG;
	int Frame_Cnt;
	int m_Image_Width;
	int m_Image_Height;
	int m_Label_Cnt;
	int **m_label_ImageInt;

	BYTE *m_original_init;
	BYTE *m_Background_Image;
	BYTE *m_Foreground_Image;
	BYTE *m_label_Image;
	BYTE **m_label_Image_2d;

	CWinThread *pThread;
	bool MEDAINFILTER_FLAG;
	int m_labelingSize;
};

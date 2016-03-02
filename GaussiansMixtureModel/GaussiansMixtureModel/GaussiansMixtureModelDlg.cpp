
// GaussiansMixtureModelDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "GaussiansMixtureModel.h"
#include "GaussiansMixtureModelDlg.h"
#include "afxdialogex.h"
#include "GMM_Process.h"
#include "Median_Filter.h"
#include "Image_Labeling.h"
#include "DilationBinary.h"
//#include "Histogram_Normalization.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WIDTHBYTE(bits)  ((bits)+31 / 32 *4 )
#define DIB_HEADER_MAKER ((WORD)('M' << 8) | 'B')

GMM_Process *Gmm_Process;
Median_Filter MF;
Image_Labeling IL;
DilationBinary DB;
//Histogram_Normalization HN;


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CGaussiansMixtureModelDlg 대화 상자



CGaussiansMixtureModelDlg::CGaussiansMixtureModelDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGaussiansMixtureModelDlg::IDD, pParent)
	, Frame_Cnt(0)
	, m_Image_Width(0)
	, m_Image_Height(0)
	, m_original_init(NULL)
	, THREADSTOP_FLAG(false)
	, FIRSTDRAW_FLAG(false)
	, pThread(NULL)
	, m_Background_Image(NULL)
	, m_Foreground_Image(NULL)
	, m_Label_Cnt(0)
	, m_label_ImageInt(NULL)
	, m_label_Image_2d(NULL)
	, m_label_Image(NULL)
	, MEDAINFILTER_FLAG(false)
	, m_labelingSize(50)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGaussiansMixtureModelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ORIGIN, m_static_origin);
	DDX_Control(pDX, IDC_STATIC_ORIGIN_TEXT, m_static_origin_text);
	DDX_Control(pDX, IDC_STATIC_BACK_TEXT, m_static_back_text);
	DDX_Control(pDX, IDC_STATIC_FORE_TEXT, m_static_fore_text);
	DDX_Control(pDX, IDC_STATIC_BACKGROUND, m_static_background);
	DDX_Control(pDX, IDC_STATIC_FOREGROUND, m_static_foreground);
	DDX_Control(pDX, IDC_STATIC_LABEL, m_static_label);
	DDX_Control(pDX, IDC_STATIC_LABEL_TEXT, m_static_label_text);
	DDX_Control(pDX, IDC_STATIC_1, m_static_1);
}

BEGIN_MESSAGE_MAP(CGaussiansMixtureModelDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LOADIMAGES, &CGaussiansMixtureModelDlg::OnBnClickedButtonLoadimages)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CGaussiansMixtureModelDlg::OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_START, &CGaussiansMixtureModelDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CGaussiansMixtureModelDlg::OnBnClickedButtonStop)
END_MESSAGE_MAP()


// CGaussiansMixtureModelDlg 메시지 처리기
CRITICAL_SECTION cs;

BOOL CGaussiansMixtureModelDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	Frame_Cnt = 0;	//프레임 수 0 초기화
	m_Image_Width = 0;
	m_Image_Height = 0;
	THREADSTOP_FLAG = false;

	::InitializeCriticalSection(&cs);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CGaussiansMixtureModelDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CGaussiansMixtureModelDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CGaussiansMixtureModelDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CGaussiansMixtureModelDlg::OnBnClickedButtonLoadimages()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	Gmm_Process = new GMM_Process;

	BROWSEINFO Browse_Info;	//폴더 다이얼로그

	Browse_Info.hwndOwner=GetSafeHwnd();
	Browse_Info.pidlRoot=NULL;	//NULL로 초기화할 경우 네임스페이스루트(바탕 화면 폴더) 사용
	Browse_Info.ulFlags = BIF_RETURNONLYFSDIRS;	//시스템 디렉토리만 리턴, 다른것을 선택하면 OK버튼 비활성화
	memset(&Browse_Info, 0, sizeof(Browse_Info));

	LPITEMIDLIST Path;
	Path = SHBrowseForFolder(&Browse_Info);
	SHGetPathFromIDList(Path, m_szFolderPath);	//m_szFolderPath에 선택한 경로가 들어간다

	Frame_Cnt = 0;
	THREADSTOP_FLAG = false;
	CGaussiansMixtureModelDlg::DrawFirstFrame();

	//AfxMessageBox((m_szFolderPath), MB_ICONWARNING);	//경로확인
}


void CGaussiansMixtureModelDlg::OnBnClickedButtonExit()	//종료버튼
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if((AfxMessageBox(_T("프로그램을 종료하시겠습니까?"), MB_YESNO| MB_ICONQUESTION)==IDYES))
	{
		delete [] m_original_init;
		delete [] m_Background_Image;
		delete [] m_Foreground_Image;
		delete [] m_label_Image;
		delete [] m_label_Image_2d;
		delete [] m_label_ImageInt;

		THREADSTOP_FLAG = true;
		exit(0);	
		//CDialog::OnCancel();
	}
}


void CGaussiansMixtureModelDlg::GetImage()
{
	char Image_Path[256];	//이미지 경로
	char Image_Name[30];	//이미지 파일 이름
	bool IMG_SUCCESS_FLAG = false;

	WideCharToMultiByte(CP_ACP, 0, m_szFolderPath, 256, Image_Path, 256, NULL, NULL); //파일 경로(유니코드 -> 멀티바이트) 변환
	ZeroMemory(Image_Name, sizeof(char)*30);

	sprintf_s(Image_Name,"\\%04d.BMP", Frame_Cnt);

	strcat_s(Image_Path, Image_Name);	// 디렉토리 + 파일이름

	IMG_SUCCESS_FLAG = IMGProTransBMPToBuffer(Image_Path);

	if(IMG_SUCCESS_FLAG)
	{
		if(FIRSTDRAW_FLAG)
		{
			FIRSTDRAW_FLAG = false;
			GetDlgItem(IDC_EDIT_LABELING)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_MEDIAN)->EnableWindow(TRUE);
		}
		else
		{
			Frame_Cnt++;
		}
	}
	else
	{
		THREADSTOP_FLAG = true;
		if(Frame_Cnt >= 2)
		{
			AfxMessageBox(_T("영상이 끝났습니다"), MB_ICONWARNING);
			GetDlgItem(IDC_EDIT_LABELING)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_MEDIAN)->EnableWindow(TRUE);

			GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
		}
	}
}


bool CGaussiansMixtureModelDlg::IMGProTransBMPToBuffer(char *Img_Path)
{
	FILE *File_In;
	BITMAPFILEHEADER Bmp_fh;			// 파일 정보 헤드
	BITMAPINFOHEADER Bmp_ih;	        // 영상 정보 헤드

	fopen_s(&File_In, Img_Path, "rb");	//읽어 들여올 파일 열기
	if(File_In == NULL)	//비어있으면 종료
	{
		return false;
	}

	fread(&Bmp_fh, sizeof(BITMAPFILEHEADER), 1, File_In);	//파일 정보 헤드 읽어오기
	fread(&Bmp_ih, sizeof(BITMAPINFOHEADER), 1, File_In);	//영상 정보 헤드 읽어오기
	if(Bmp_ih.biBitCount != 8)
	{
		AfxMessageBox(_T("흑백이미지가 아닙니다"), MB_ICONWARNING);
		return false;	// 흑백영상이 아니면 종료
	}

	m_Image_Width = Bmp_ih.biWidth;
	m_Image_Height = Bmp_ih.biHeight;

	SetStatic();

	//if(FIRSTDRAW_FLAG)
	//{
		m_original_init = new BYTE [m_Image_Width * m_Image_Height];
		m_Background_Image = new BYTE [m_Image_Width * m_Image_Height];
		m_Foreground_Image = new BYTE [m_Image_Width * m_Image_Height];
		m_label_Image = new BYTE[m_Image_Width * m_Image_Height];
		m_label_Image_2d = new BYTE*[m_Image_Height];
		m_label_ImageInt = new int*[m_Image_Height];

		for(int i=0;i<m_Image_Height;i++)
		{
			m_label_Image_2d[i] = new BYTE[m_Image_Width];
			m_label_ImageInt[i] = new int[m_Image_Width];
		}
	//}

	fread(RGB, sizeof(RGBQUAD), 256, File_In);	//팔레트 읽어오기
	fread(m_original_init, sizeof(char), Bmp_ih.biSizeImage, File_In);
	fclose(File_In);

	return true;
}


void CGaussiansMixtureModelDlg::DrawFirstFrame(void)
{
	FIRSTDRAW_FLAG = true;

	GetImage();
	const int W = m_Image_Width;
	const int H = m_Image_Height;

	BYTE *Out_original = new BYTE[W*H*3];

	m_bmpih.biSize = sizeof(BITMAPINFOHEADER);
	m_bmpih.biWidth = m_Image_Width;
	m_bmpih.biHeight = m_Image_Height;
	m_bmpih.biPlanes = 1;
	m_bmpih.biBitCount = 24 ;
	m_bmpih.biCompression = BI_RGB;
	m_bmpih.biSizeImage = WIDTHBYTE(m_Image_Width*3)*m_Image_Height;
	m_bmpih.biXPelsPerMeter = 0;
	m_bmpih.biYPelsPerMeter = 0;
	m_bmpih.biClrUsed = 0;
	m_bmpih.biClrImportant = 0;

	::GetWindowRect(m_static_origin.m_hWnd, m_static_rect);
	m_hdc = ::GetWindowDC(m_static_origin.m_hWnd);

	for(int j = 0 ; j < m_Image_Height ; j++)
	{
		for(int i = 0 ; i < m_Image_Width ; i++)
		{
			Out_original[j * m_Image_Width * 3 + i * 3 + 2] = m_original_init[j * m_Image_Width + i];
			Out_original[j * m_Image_Width * 3 + i * 3 + 1] = m_original_init[j * m_Image_Width + i];
			Out_original[j * m_Image_Width * 3 + i * 3    ] = m_original_init[j * m_Image_Width + i];
		}
	}
	
	SetStretchBltMode(m_hdc,COLORONCOLOR);

	StretchDIBits(m_hdc, 0, 0, m_Image_Width, m_Image_Height, 0, 0, m_bmpih.biWidth, m_bmpih.biHeight,
		Out_original, (LPBITMAPINFO)&m_bmpih, DIB_RGB_COLORS,SRCCOPY);

	GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);	//Start 버튼 활성화

	delete [] Out_original;
}

UINT CGaussiansMixtureModelDlg::ThreadDrawImages(LPVOID pParam)
{
	CGaussiansMixtureModelDlg *pDlg = (CGaussiansMixtureModelDlg *) pParam;	//ThreadDrawImages 함수가 static이기 때문에 받아와야 한다!!

	while(!(pDlg->THREADSTOP_FLAG))
	{
		::EnterCriticalSection(&cs);

		pDlg->GetImage();

		//HN.OnInitHistogram(pDlg->m_Image_Width, pDlg->m_Image_Height, pDlg->m_original_init);

		Gmm_Process->OnInitGMM(pDlg->m_Image_Width, pDlg->m_Image_Height);
		Gmm_Process->Gaussian_Initialize(pDlg->m_original_init, pDlg->m_Foreground_Image, pDlg->m_Background_Image);

		if(pDlg->MEDAINFILTER_FLAG)
		{
			MF.OnInitMedian_Filter(pDlg->m_Image_Width, pDlg->m_Image_Height, 3, pDlg->m_Foreground_Image);	//미디언 필터 전경에 적용
		}
		//DB.Init(pDlg->m_Foreground_Image, pDlg->m_Foreground_Image, pDlg->m_Image_Width, pDlg->m_Image_Height, 1, 5);
		//DB.Init(pDlg->m_Foreground_Image, pDlg->m_Foreground_Image, pDlg->m_Image_Width, pDlg->m_Image_Height, 2, 2);

		pDlg->DrawOriginalImage();
		pDlg->DrawForegroundImage();
		pDlg->DrawBackgroundImage();
		pDlg->InItLabelingImage();

		//Sleep(50);

		::LeaveCriticalSection(&cs);
	}

	return 0;
}


void CGaussiansMixtureModelDlg::SetStatic(void)
{
	CWnd *Idc_Static = NULL;

	Idc_Static = (CWnd *)GetDlgItem(IDC_STATIC_ORIGIN);	// 원본화면 출력 창 받아오기
	Idc_Static->MoveWindow(20, 20, m_Image_Width, m_Image_Height);	//화면출력 창 이동 및 크기조절

	m_static_origin_text.MoveWindow(20, m_Image_Height+25, 60*2, 8*2);	//text 이동 및 크기조절

	Idc_Static = (CWnd *)GetDlgItem(IDC_STATIC_BACKGROUND);	// Background 출력 창 받아오기
	Idc_Static->MoveWindow(20 + m_Image_Width + 20, 20, m_Image_Width, m_Image_Height);	//화면출력 창 이동 및 크기조절

	m_static_back_text.MoveWindow(20 + m_Image_Width + 20, m_Image_Height+25, 75*2, 8*2);

	Idc_Static = (CWnd *)GetDlgItem(IDC_STATIC_FOREGROUND);	// ForeGround 출력 창 받아오기
	Idc_Static->MoveWindow(20, 20 + m_Image_Height + 40, m_Image_Width, m_Image_Height);	//화면출력 창 이동 및 크기조절
	
	m_static_fore_text.MoveWindow(20, m_Image_Height*2 + 40 + 25, 74*2, 8*2);

	Idc_Static = (CWnd *)GetDlgItem(IDC_STATIC_LABEL);	// Background 출력 창 받아오기
	Idc_Static->MoveWindow(20 + m_Image_Width + 20, 20 + m_Image_Height + 40, m_Image_Width, m_Image_Height);	//화면출력 창 이동 및 크기조절

	m_static_label_text.MoveWindow(20 + m_Image_Width + 20, m_Image_Height*2 + 40 + 25, 63*2, 8*2);
}


void CGaussiansMixtureModelDlg::DrawOriginalImage(void)
{
	BYTE *Out_original = new BYTE[m_Image_Width * m_Image_Height * 3];

	m_bmpih.biSize = sizeof(BITMAPINFOHEADER);
	m_bmpih.biWidth = m_Image_Width;
	m_bmpih.biHeight = m_Image_Height;
	m_bmpih.biPlanes = 1;
	m_bmpih.biBitCount = 24 ;
	m_bmpih.biCompression = BI_RGB;
	m_bmpih.biSizeImage = WIDTHBYTE(m_Image_Width*3)*m_Image_Height;
	m_bmpih.biXPelsPerMeter = 0;
	m_bmpih.biYPelsPerMeter = 0;
	m_bmpih.biClrUsed = 0;
	m_bmpih.biClrImportant = 0;

	::GetWindowRect(m_static_origin.m_hWnd, m_static_rect);
	m_hdc = ::GetWindowDC(m_static_origin.m_hWnd);

	for(int j = 0 ; j < m_Image_Height ; j++)
	{
		for(int i = 0 ; i < m_Image_Width ; i++)
		{
			Out_original[j * m_Image_Width * 3 + i * 3 + 2] = m_original_init[j * m_Image_Width + i];
			Out_original[j * m_Image_Width * 3 + i * 3 + 1] = m_original_init[j * m_Image_Width + i];
			Out_original[j * m_Image_Width * 3 + i * 3    ] = m_original_init[j * m_Image_Width + i];
		}
	}
	
	SetStretchBltMode(m_hdc,COLORONCOLOR);

	StretchDIBits(m_hdc, 0, 0, m_Image_Width, m_Image_Height, 0, 0, m_bmpih.biWidth, m_bmpih.biHeight,
		Out_original, (LPBITMAPINFO)&m_bmpih, DIB_RGB_COLORS,SRCCOPY);

	delete [] Out_original;
}


void CGaussiansMixtureModelDlg::OnBnClickedButtonStart()
{
	THREADSTOP_FLAG = false;
	
	m_labelingSize = GetDlgItemInt(IDC_EDIT_LABELING);
	if(m_labelingSize < 50)
	{
		AfxMessageBox(_T("Labeling Size는 50px 이상으로 설정해 주세요"), MB_ICONWARNING);
		return;
	}

	MEDAINFILTER_FLAG = IsDlgButtonChecked(IDC_CHECK_MEDIAN);

	pThread = AfxBeginThread((AFX_THREADPROC)ThreadDrawImages, (LPVOID)this);

	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_LABELING)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_MEDIAN)->EnableWindow(FALSE);
}


void CGaussiansMixtureModelDlg::DrawBackgroundImage(void)
{
	BYTE *Out_background = new BYTE[m_Image_Width * m_Image_Height * 3];

	m_bmpih.biSize = sizeof(BITMAPINFOHEADER);
	m_bmpih.biWidth = m_Image_Width;
	m_bmpih.biHeight = m_Image_Height;
	m_bmpih.biPlanes = 1;
	m_bmpih.biBitCount = 24 ;
	m_bmpih.biCompression = BI_RGB;
	m_bmpih.biSizeImage = WIDTHBYTE(m_Image_Width*3)*m_Image_Height;
	m_bmpih.biXPelsPerMeter = 0;
	m_bmpih.biYPelsPerMeter = 0;
	m_bmpih.biClrUsed = 0;
	m_bmpih.biClrImportant = 0;

	::GetWindowRect(m_static_background.m_hWnd, m_static_rect);
	m_hdc = ::GetWindowDC(m_static_background.m_hWnd);

	for(int j = 0 ; j < m_Image_Height ; j++)
	{
		for(int i = 0 ; i < m_Image_Width ; i++)
		{
			Out_background[j * m_Image_Width * 3 + i * 3 + 2] = m_Background_Image[j * m_Image_Width + i];
			Out_background[j * m_Image_Width * 3 + i * 3 + 1] = m_Background_Image[j * m_Image_Width + i];
			Out_background[j * m_Image_Width * 3 + i * 3    ] = m_Background_Image[j * m_Image_Width + i];
		}
	}
	
	SetStretchBltMode(m_hdc,COLORONCOLOR);

	StretchDIBits(m_hdc, 0, 0, m_Image_Width, m_Image_Height, 0, 0, m_bmpih.biWidth, m_bmpih.biHeight,
		Out_background, (LPBITMAPINFO)&m_bmpih, DIB_RGB_COLORS,SRCCOPY);

	delete [] Out_background;
}


void CGaussiansMixtureModelDlg::DrawForegroundImage(void)
{
	BYTE *Out_foreground = new BYTE[m_Image_Width * m_Image_Height * 3];

	m_bmpih.biSize = sizeof(BITMAPINFOHEADER);
	m_bmpih.biWidth = m_Image_Width;
	m_bmpih.biHeight = m_Image_Height;
	m_bmpih.biPlanes = 1;
	m_bmpih.biBitCount = 24 ;
	m_bmpih.biCompression = BI_RGB;
	m_bmpih.biSizeImage = WIDTHBYTE(m_Image_Width*3)*m_Image_Height;
	m_bmpih.biXPelsPerMeter = 0;
	m_bmpih.biYPelsPerMeter = 0;
	m_bmpih.biClrUsed = 0;
	m_bmpih.biClrImportant = 0;

	::GetWindowRect(m_static_foreground.m_hWnd, m_static_rect);
	m_hdc = ::GetWindowDC(m_static_foreground.m_hWnd);

	for(int j = 0 ; j < m_Image_Height ; j++)
	{
		for(int i = 0 ; i < m_Image_Width ; i++)
		{
			Out_foreground[j * m_Image_Width * 3 + i * 3 + 2] = m_Foreground_Image[j * m_Image_Width + i];
			Out_foreground[j * m_Image_Width * 3 + i * 3 + 1] = m_Foreground_Image[j * m_Image_Width + i];
			Out_foreground[j * m_Image_Width * 3 + i * 3    ] = m_Foreground_Image[j * m_Image_Width + i];
		}
	}
	
	SetStretchBltMode(m_hdc,COLORONCOLOR);

	StretchDIBits(m_hdc, 0, 0, m_Image_Width, m_Image_Height, 0, 0, m_bmpih.biWidth, m_bmpih.biHeight,
		Out_foreground, (LPBITMAPINFO)&m_bmpih, DIB_RGB_COLORS,SRCCOPY);
	
	delete [] Out_foreground;
}


void CGaussiansMixtureModelDlg::InItLabelingImage(void)
{
	int x,y;

	m_Label_Cnt = IL.OnInitLabeling(m_Foreground_Image, m_label_ImageInt, m_Image_Width, m_Image_Height, m_labelingSize);
	
	CString a;
	a.Format(_T("%d"), m_Label_Cnt);
	m_static_1.SetWindowTextW(a);
	
	if(m_Label_Cnt > 0)
	{
		CRect *pBoundBox;
		pBoundBox = new CRect[m_Label_Cnt];

		for(int i=0;i<m_Label_Cnt;i++)
		{
			pBoundBox[i] = CRect(m_Image_Width, m_Image_Height, 0, 0);

			for(y=0;y<m_Image_Height;y++)
			{
				for(x=0;x<m_Image_Width;x++)
				{
					m_label_Image_2d[y][x] = m_original_init[m_Image_Width*y + x];	//2차원 배열로 복사
				}
			}
		}

		for(y=0;y<m_Image_Height;y++)
		{
			for(x=0;x<m_Image_Width;x++)
			{
				if(m_label_ImageInt[y][x] < 0) continue;
				if(pBoundBox[m_label_ImageInt[y][x]].left > x)
					pBoundBox[m_label_ImageInt[y][x]].left = x;
				if(pBoundBox[m_label_ImageInt[y][x]].top > y)
					pBoundBox[m_label_ImageInt[y][x]].top = y;
				if(pBoundBox[m_label_ImageInt[y][x]].right < x)
					pBoundBox[m_label_ImageInt[y][x]].right = x;
				if(pBoundBox[m_label_ImageInt[y][x]].bottom < y)
					pBoundBox[m_label_ImageInt[y][x]].bottom = y;
			}
		}

		for(int i=0;i<m_Label_Cnt;i++)
		{
			for(int h = pBoundBox[i].top ; h <=pBoundBox[i].bottom ; h++)
			{
				m_label_Image_2d[h][pBoundBox[i].left] = 255;
				m_label_Image_2d[h][pBoundBox[i].left - 1] = 255;
				m_label_Image_2d[h][pBoundBox[i].right] = 255;
				m_label_Image_2d[h][pBoundBox[i].right - 1] = 255;
			}
			for(int w = pBoundBox[i].left ; w <= pBoundBox[i].right ; w++)
			{
				m_label_Image_2d[pBoundBox[i].top][w] = 255;
				m_label_Image_2d[pBoundBox[i].top - 1][w] = 255;
				m_label_Image_2d[pBoundBox[i].bottom][w] = 255;
				m_label_Image_2d[pBoundBox[i].bottom - 1][w] = 255;
			}
		}

		for(y=0;y<m_Image_Height;y++)
		{
			for(x=0;x<m_Image_Width;x++)
			{
				m_label_Image[m_Image_Width*y + x] = m_label_Image_2d[y][x];	//1차원 배열로 복사
			}
		}
		delete [] pBoundBox;
	}
	else
	{
		m_label_Image = m_original_init;
	}
	DrawLabelingImage();
}


void CGaussiansMixtureModelDlg::DrawLabelingImage(void)
{
	BYTE *Out_Labeling = new BYTE[m_Image_Width * m_Image_Height * 3];

	m_bmpih.biSize = sizeof(BITMAPINFOHEADER);
	m_bmpih.biWidth = m_Image_Width;
	m_bmpih.biHeight = m_Image_Height;
	m_bmpih.biPlanes = 1;
	m_bmpih.biBitCount = 24 ;
	m_bmpih.biCompression = BI_RGB;
	m_bmpih.biSizeImage = WIDTHBYTE(m_Image_Width*3)*m_Image_Height;
	m_bmpih.biXPelsPerMeter = 0;
	m_bmpih.biYPelsPerMeter = 0;
	m_bmpih.biClrUsed = 0;
	m_bmpih.biClrImportant = 0;

	::GetWindowRect(m_static_label.m_hWnd, m_static_rect);
	m_hdc = ::GetWindowDC(m_static_label.m_hWnd);

	for(int j = 0 ; j < m_Image_Height ; j++)
	{
		for(int i = 0 ; i < m_Image_Width ; i++)
		{
			Out_Labeling[j * m_Image_Width * 3 + i * 3 + 2] = m_label_Image[j * m_Image_Width + i	];	//빨간색을 뽑기위함
			Out_Labeling[j * m_Image_Width * 3 + i * 3 + 1] = m_original_init[j * m_Image_Width + i	];
			Out_Labeling[j * m_Image_Width * 3 + i * 3    ] = m_original_init[j * m_Image_Width + i ];
		}
	}
	
	SetStretchBltMode(m_hdc,COLORONCOLOR);

	StretchDIBits(m_hdc, 0, 0, m_Image_Width, m_Image_Height, 0, 0, m_bmpih.biWidth, m_bmpih.biHeight,
		Out_Labeling, (LPBITMAPINFO)&m_bmpih, DIB_RGB_COLORS,SRCCOPY);

	delete [] Out_Labeling;
}


void CGaussiansMixtureModelDlg::OnBnClickedButtonStop()
{
	THREADSTOP_FLAG = true;

	GetDlgItem(IDC_EDIT_LABELING)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_MEDIAN)->EnableWindow(TRUE);

	GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
}

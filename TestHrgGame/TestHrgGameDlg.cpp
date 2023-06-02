
// TestHrgGameDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "TestHrgGame.h"
#include "TestHrgGameDlg.h"
#include "CommPlugin.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTestHrgGameDlg dialog
CTestHrgGameDlg* instance = nullptr;


CTestHrgGameDlg::CTestHrgGameDlg(LogCallback* pLogger, Game* pGame, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TESTHRGGAME_DIALOG, pParent)
    , m_pLog(pLogger)
    , m_pGame(pGame)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	instance = this;
}

CTestHrgGameDlg* CTestHrgGameDlg::Instance()
{
	return instance;
}

void CTestHrgGameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestHrgGameDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CTestHrgGameDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTestHrgGameDlg::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMBO_BET, &CTestHrgGameDlg::OnCbnSelchangeComboBet)
	ON_BN_CLICKED(IDC_BUTTONSERVICE, &CTestHrgGameDlg::OnBnClickedButtonservice)
	ON_BN_CLICKED(IDC_BUTTONCASHOUT, &CTestHrgGameDlg::OnBnClickedButtoncashout)
END_MESSAGE_MAP()


// CTestHrgGameDlg message handlers

BOOL CTestHrgGameDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_pGame->StartGame();
	m_pLog->Log(LogDebug, "GameDialog", "initialized=>true");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestHrgGameDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestHrgGameDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestHrgGameDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTestHrgGameDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}


void CTestHrgGameDlg::OnBnClickedCancel()
{
	m_pGame->EndGame();
	CDialogEx::OnCancel();
}


void CTestHrgGameDlg::OnCbnSelchangeComboBet()
{
	// TODO: Add your control notification handler code here
}


void CTestHrgGameDlg::OnBnClickedButtonservice()
{
	// TODO: Add your control notification handler code here
}


void CTestHrgGameDlg::OnBnClickedButtoncashout()
{
	// TODO: Add your control notification handler code here
}

void CTestHrgGameDlg::UpdateParameters(std::map<std::string, std::string>& parameters)
{
	char buf[1000];

    auto pListParams = (CListBox*)GetDlgItem(IDC_LISTPARAMETERS);
	for (auto i = 0; i < pListParams->GetCount(); i++)
	{
		pListParams->DeleteString(0);
	}

    for (auto iter = parameters.begin(); iter != parameters.end(); ++iter)
	{
		sprintf(buf, "%s = %s", iter->first.c_str(), iter->second.c_str());
		m_pLog->Log(LogDebug, "GameDialog", buf);
		CString myString(buf);
		LPTSTR lpBuffer = myString.GetBuffer(myString.GetLength());
		pListParams->AddString(lpBuffer);
	}
}

void CTestHrgGameDlg::UpdateDenomMeter(std::string denom)
{
	auto pEdit = (CEdit*)GetDlgItem(IDC_DENOM);
	CString myString(denom.c_str());
	LPTSTR lpBuffer = myString.GetBuffer(myString.GetLength());
	pEdit->SetWindowTextW(lpBuffer);
}

void CTestHrgGameDlg::UpdateCreditMeter(std::string credit)
{
	auto pEdit = (CEdit*)GetDlgItem(IDC_EDIT_CREDITS);
	CString myString(credit.c_str());
	LPTSTR lpBuffer = myString.GetBuffer(myString.GetLength());
	pEdit->SetWindowTextW(lpBuffer);
}

void CTestHrgGameDlg::UpdateWinMeter(std::string win)
{
	auto pEdit = (CEdit*)GetDlgItem(IDC_EDIT_WIN);
	CString myString(win.c_str());
	LPTSTR lpBuffer = myString.GetBuffer(myString.GetLength());
	pEdit->SetWindowTextW(lpBuffer);
}

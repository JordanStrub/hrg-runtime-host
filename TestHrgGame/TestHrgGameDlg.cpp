
// TestHrgGameDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "TestHrgGame.h"
#include "TestHrgGameDlg.h"
#include "CommPlugin.h"
#include "afxdialogex.h"


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
	ON_BN_CLICKED(IDC_BUTTONPLAY, &CTestHrgGameDlg::OnBnClickedButtonplay)
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
	m_pGame->Start();
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
	m_pGame->Stop();
	CDialogEx::OnCancel();
}


void CTestHrgGameDlg::OnCbnSelchangeComboBet()
{
	auto pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_BET);
	auto sel = pCombo->GetCurSel();
	CString result;
	pCombo->GetLBText(sel, result);
	std::wstring betStrw(result);
	std::string betStr;
	betStr.assign(betStrw.begin(), betStrw.end());
	m_pLog->Log(LogDebug, "betComboSelection", betStr.c_str());
	if (m_pGame->TrySetBet(betStr))
	{
		m_currentBetIndex = sel;
	}
	else
	{
		pCombo->SetCurSel(m_currentBetIndex);
		PlayMessageBeepAndWait(MB_ICONWARNING, 250);
	}
}

void CTestHrgGameDlg::OnBnClickedButtonplay()
{
	m_pGame->PlayGame();

	UpdateStatusLine("Game playing ...");

	// simulate game with variety of consecutive noises
	PlayMessageBeepAndWait(MB_ICONQUESTION, 250);
	PlayMessageBeepAndWait(MB_ICONHAND, 250);
	PlayMessageBeepAndWait(MB_OK, 1000);
	PlayMessageBeepAndWait(MB_ICONQUESTION, 250);
	PlayMessageBeepAndWait(MB_ICONHAND, 250);
	PlayMessageBeepAndWait(MB_OK, 1000);

	m_pGame->EndGame();

	UpdateStatusLine("Present wins");

	// presentation
	PlayMessageBeepAndWait(MB_ICONHAND, 250);
	PlayMessageBeepAndWait(MB_OK, 1000);

	UpdateStatusLine("Game over");

	m_pGame->Idle();

	UpdateStatusLine("Game over");
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

    for (auto parameter : parameters)
	{
		sprintf(buf, "%s = %s", parameter.first.c_str(), parameter.second.c_str());
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

void CTestHrgGameDlg::UpdatePlatformMessage(std::vector<std::string> messages)
{
	auto pList = (CListBox*)GetDlgItem(IDC_LISTPLATMSGS);
	for (auto i = 0; i < pList->GetCount(); i++)
	{
		pList->DeleteString(0);
	}

	for (auto message : messages)
	{
		m_pLog->Log(LogDebug, "GameDialog", message.c_str());
		CString myString(message.c_str());
		LPTSTR lpBuffer = myString.GetBuffer(myString.GetLength());
		pList->AddString(lpBuffer);
	}
}

void CTestHrgGameDlg::UpdateBetChoices(std::vector<std::string> betCredits)
{
	auto pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_BET);
	pCombo->Clear();
	for (auto betCredit : betCredits)
	{
		CString myString(betCredit.c_str());
		LPTSTR lpBuffer = myString.GetBuffer(myString.GetLength());
		pCombo->AddString(lpBuffer);
	}

	m_currentBetIndex = 0;
	pCombo->SetCurSel(0);
}

void CTestHrgGameDlg::UpdateStatusLine(std::string status)
{
	auto pLine = (CEdit*)GetDlgItem(IDC_EDIT_STATUS);
	CString myString(status.c_str());
	LPTSTR lpBuffer = myString.GetBuffer(myString.GetLength());
	pLine->SetWindowTextW(lpBuffer);
}

void CTestHrgGameDlg::PlayMessageBeepAndWait(unsigned beepType, unsigned delayMs)
{
	MessageBeep(beepType);
	Sleep(delayMs);
}

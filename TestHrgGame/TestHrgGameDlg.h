
// TestHrgGameDlg.h : header file
//

#pragma once
#include <map>
#include <string>

#include "Game.h"
#include "LogCallback.h"

// CTestHrgGameDlg dialog
class CTestHrgGameDlg : public CDialogEx
{
// Construction
public:
	CTestHrgGameDlg(LogCallback* pLogger, Game* pGame, CWnd* pParent = nullptr);	// standard constructor
	static CTestHrgGameDlg* Instance();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTHRGGAME_DIALOG };
#endif

	void UpdateParameters(std::map<std::string, std::string>& parameters);
	void UpdateDenomMeter(std::string denom);
	void UpdateCreditMeter(std::string credits);
	void UpdateWinMeter(std::string win);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
private:

protected:
	HICON m_hIcon;
	LogCallback* m_pLog;
	Game* m_pGame;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnSelchangeComboBet();
	afx_msg void OnBnClickedButtonservice();
	afx_msg void OnBnClickedButtoncashout();
};

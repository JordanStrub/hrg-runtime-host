
// TestHrgGameDlg.h : header file
//

#pragma once
#include <map>
#include <string>
#include "LogCallback.h"

class CommPlugin;

// CTestHrgGameDlg dialog
class CTestHrgGameDlg : public CDialogEx
{
// Construction
public:
	CTestHrgGameDlg(LogCallback* pLogger, CWnd* pParent = nullptr);	// standard constructor
	void SetCommPlugin(CommPlugin* pCommPlugin) { m_pCommPlugin = pCommPlugin; }

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTHRGGAME_DIALOG };
#endif

	void UpdateParameters(std::map<std::string, std::string>& parameters);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
private:
	void TryUpdateParameterList();

protected:
	HICON m_hIcon;
	LogCallback* m_pLog;
	CommPlugin* m_pCommPlugin;
	bool m_bInitialized = false;
	std::map<std::string, std::string> m_mParameters;

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

#pragma once
#include "afxdialogex.h"

// Cui_popMsg 대화 상자

class Cui_popMsg : public CDialogEx
{

public:

  enum class Type_e
  {
    information = 0x01,
    question = 0x02,
    stop = 0x04,
    error = 0x08,
    ok = 0x10,
    ok_cancel = 0x20,
    yes_no = 0x40,
		retry_cancel
  };

  struct Set_t
	{
		CString			title;
		CString			message;
		UINT			form_type{};
	};
	CString			m_title;
	CString			m_message;
	UINT			m_form_type;


	CWnd* m_pParent;
	CBrush m_brBckColor;
	int m_cx;
	int m_cy;
	CRect m_imageRect; // picture Control의 위치를 기억할 변수
	CImage m_image; //사용자가 선택한 이미지 객체를 구성할 변수

	DECLARE_DYNAMIC(Cui_popMsg)

public:
	Cui_popMsg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	Cui_popMsg(Set_t& set_data, CWnd* pParent =nullptr);
	virtual ~Cui_popMsg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MSG_BOX1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

protected:
	//HICON m_hIcon;
	CFont m_fontStyle;
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedMsgBoxOk();
	afx_msg void OnBnClickedMsgBoxCancel();
private:
	void viewDefault();
	// 프레임에 이미지를 그린다
	void drawImage();
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
private:
	// 내용 출력
	CEdit m_editView;

public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
private:
	// 내용이 나타나는 창의 스타일 셋팅
	void setViewWindow();
	CMFCButton m_btnYes;
	CMFCButton m_btnOK;
	CMFCButton m_btnCancel;
	CMFCButton m_btnRetry;
	CMFCButton m_btnNO;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
private:
	void clickOk();
	void moveButton(UINT type);
public:
	afx_msg void OnBnClickedMsgBoxNo();
	afx_msg void OnBnClickedMsgBoxRetry();
	afx_msg void OnBnClickedMsgBoxYes();
};

#include "stdafx.h"
#include "ChatCppDlg.h"
#include ".\chatcppdlg.h"
#include "CAboutDlg.h"
#include ".\caboutdlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CChatCppDlg::CChatCppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChatCppDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatCppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_SERVER, btnStartServer);
	DDX_Control(pDX, IDC_RADIO_CLIENT, btnStartClient);
	DDX_Control(pDX, IDC_EDIT_NAME, inputUserName);
	DDX_Control(pDX, IDC_EDIT_IP, inputServerIPAdress);
	DDX_Control(pDX, IDC_EDIT_PORT, inputPort);
	DDX_Control(pDX, IDC_EDIT_SEND, wndSend);
	DDX_Control(pDX, IDC_BUTTON_SEND, btnSendMes);
	DDX_Control(pDX, IDC_BUTTON_STOPCHAT, btnStopChat);
	DDX_Control(pDX, IDC_EDIT_CHAT, wndChat);
	DDX_Control(pDX, IDC_STATIC_COUNTPEOPLE, wndCountUsers);
}

BEGIN_MESSAGE_MAP(CChatCppDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO_SERVER, OnBtnClickedRadioServer)
	ON_BN_CLICKED(IDC_RADIO_CLIENT, OnBtnClickedRadioClient)
	ON_BN_CLICKED(IDC_BUTTON_STOPCHAT, OnBtnClickedButtonStopchat)
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnBtnClickedButtonSend)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CChatCppDlg message handlers

BOOL CChatCppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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
	
	// ������������� �� ���������
	SetWindowText("��� ����!");

	// ����������� ���������� �������� ��������
	inputUserName.SetLimitText(12); 
	inputUserName.SetWindowText(mess_EmptyName);

	// ����������� ���������� �������� � ���������
	wndSend.SetLimitText(50); 

	// ����� �� ���������
	inputServerIPAdress.SetWindowText("127.0.0.1"); 

	// ���� �� ���������
	inputPort.SetWindowText("500");

	btnStopChat.SetWindowText(mess_ExitFromChat);
	m_mainSocket.wndParent = this;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CChatCppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CChatCppDlg::OnPaint() 
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
		CDialog::OnPaint();
	}
}


// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CChatCppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ��������� ������
void CChatCppDlg::OnBtnClickedRadioServer()
{
	//������������ ������
	if (btnStartServer.GetCheck() != BST_CHECKED) { //�� ������ ������ �������� �������
		return;
	}
	else if (m_mainSocket.m_hSocket != INVALID_SOCKET) { //������ ������
		return;
	} 
	else if(QueryName() == false) {
		AfxMessageBox("������� ���� ��� ��� ����!");
		StopChat();
		return;
	}

	CString strPort;
	inputPort.GetWindowText(strPort);


	// ����������� ������
	if(m_mainSocket.Create(atoi(strPort)) == FALSE)
	{
		AfxMessageBox("������ �������� �������!\n�������� ������ ���� ��� ������������!");
		StopChat();
	}
	else
	{
		// ������ ����� � ����� ������������� �������� �� �����������
		if(m_mainSocket.Listen() == FALSE)
		{
			StopChat();
		}
		else
		{
			// ���� ��� � �������
			DisabledControl(true);
			SetWindowText("������ ���� �����������!");
		}
	}
}

// ��������� �������
void CChatCppDlg::OnBtnClickedRadioClient()
{
	// ������������ ������ �������
	if (btnStartClient.GetCheck() != BST_CHECKED) { //�� ������ ������ ������
		return;
	}
	else if (m_mainSocket.m_hSocket != INVALID_SOCKET) { //������ ������
		return;
	}
	else if(CChatCppDlg::QueryName() == false) { //��� ��� ����������
		AfxMessageBox("������� ���� ���!");
		StopChat();
		return;
	}


	// �������� ������ �� ������� ����� � ������
	if(m_mainSocket.Create() == TRUE)
	{
		CString strAddress;
		inputServerIPAdress.GetWindowText(strAddress);
		CString strPort;
		inputPort.GetWindowText(strPort);
		if(m_mainSocket.Connect(strAddress, atoi(strPort)) == FALSE)
		{
			// �������� ����������� (������ ��������� �������, �� ��� ��������)
			if(GetLastError() == WSAEWOULDBLOCK)
			{
				DisabledControl(false);
			}
			else
			{
				// ��������� ���� � ������ ������
				StopChat();
			}
		}
	}

}


// ����� �� ����
void CChatCppDlg::OnBtnClickedButtonStopchat()
{
	StopChat();
}

// ��������� ������ � ����������� ��� ������ ���������� � ������ ������� ��� �������.
void CChatCppDlg::DisabledControl(bool server)
{
	// �������.
	inputServerIPAdress.EnableWindow(FALSE);
	inputPort.EnableWindow(FALSE);
	btnSendMes.EnableWindow(FALSE);

	if(server == true)
	{
		btnStopChat.SetWindowText(mess_StopChat);
		btnStartClient.EnableWindow(FALSE);
	}
	else
	{
		btnStopChat.SetWindowText(mess_ExitFromChat);
		btnStartServer.EnableWindow(FALSE);
	}

	// ��������� ����������� ������ �� ����
	btnStopChat.EnableWindow(TRUE);
}

// ��������� ������ � ����������� ����� �������� �������
// ���� ������� - �������� ���������� �� 
// ���������� ������� "������������" �����
void CChatCppDlg::EnabledControl(void)
{
	// ����������
	inputServerIPAdress.EnableWindow(TRUE);
	inputPort.EnableWindow(TRUE);
	btnStartClient.EnableWindow(TRUE);
	btnStartServer.EnableWindow(TRUE);
	

	// �������
	btnStopChat.EnableWindow(FALSE);
	btnSendMes.EnableWindow(FALSE);
}

// ��������� ������� �� �����������
void CChatCppDlg::OnAccept(void)
{
	CSock* pSock = new CSock;
	pSock->wndParent = this;

	// ���� ��� � ������� ������� ������� ����� � ������ 
	// ������������ ������� �������.
	if(m_mainSocket.Accept(*pSock) == TRUE)
	{
		m_vecSockets.push_back(pSock);
		btnSendMes.EnableWindow(TRUE);
		SendCountPeople();

		SetWindowText("���� ��������!");
	}
	else 
	{
		delete pSock;
	}
}


// ����� �� ����
void CChatCppDlg::StopChat(void)
{
	// �������� ������ �� ���������� �� ����.
	SendDisconnect();

	m_mainSocket.Close();
	for(int i = 0; i < (int)m_vecSockets.size(); i++)
	{
		m_vecSockets[i]->Close();
		delete m_vecSockets[i];
	}
	m_vecSockets.clear();

	btnStartServer.SetCheck(BST_UNCHECKED);
	btnStartClient.SetCheck(BST_UNCHECKED);

	EnabledControl();

	// � ���� ��� ������.
	wndCountUsers.SetWindowText("� ���� 0 �������������");
	SetWindowText("��� ����!");
}


// �������� ��������������� ���������.
void CChatCppDlg::OnBtnClickedButtonSend()
{
	CString strChat;
	wndSend.GetWindowText(strChat);
	SendToChat(strChat);

}

// ���������� ��������� �� ���� ����.
void CChatCppDlg::OnReceive(void)
{
	SENDBUFFER buff;
	if(btnStartServer.GetCheck() == BST_CHECKED)
	{
		for(int index = 0; index < (int)m_vecSockets.size(); index++)
		{
			m_vecSockets[index]->Receive(&buff, sizeof(SENDBUFFER));
			// ���� ���-�� ����������, ������� ����� ������� �� ������ ��������� ��������
			if(buff.typemessage == m_TypeMessage::tmDisconnect)
			{
				m_vecSockets[index]->Close();
				delete m_vecSockets[index];
				m_vecSockets.erase(m_vecSockets.begin() + index);
				
				SendCountPeople();
				
				// ������� ��������� ������� ��������� � ���� ������ ��������
				SendBuffer(buff, false);

				break;
			}
			// ��������� ��������� ������� �� ���� ����.
			if(buff.typemessage == m_TypeMessage::tmChat)
			{
				SendBuffer(buff, false);
				break;
			}
		}
		
	}
	else if(btnStartClient.GetCheck() == BST_CHECKED)
	{
		m_mainSocket.Receive(&buff, sizeof(SENDBUFFER));
	}

	// ��������� ��������� ��������� �� ������ ��� ����
	switch(buff.typemessage)
	{
	case m_TypeMessage::tmCountPeople:
		{
			wndCountUsers.SetWindowText("� ���� " + IntToStr(buff.countpeople) + "������������");
		}
		break;
	case m_TypeMessage::tmChat:
		{
			CString strChat;
			wndChat.GetWindowText(strChat);
			strChat += CString(buff.name) + ": " + CString(buff.buffer) + "\r\n";
			wndChat.SetWindowText(strChat);
			int number_line = wndChat.GetLineCount();
			wndChat.LineScroll(number_line);
		}
		break;
	case m_TypeMessage::tmDisconnect:
		{
			CString strChat;
			wndChat.GetWindowText(strChat);

			// ���� ������� ��������� �� ��������� ����(���������� �������),
			// ������� ���������� ���������.
			if(buff.stopchat == true)
			{
				StopChat();
				strChat += CString(buff.name) + ": ��� ����������!" + "\r\n";
			}
			else
			{
				strChat += CString(buff.name) + " - �������(�) ���!" + "\r\n";
			}
			wndChat.SetWindowText(strChat);
			int number_line = wndChat.GetLineCount();
			wndChat.LineScroll(number_line);
			
			
		}
		break;
	default:
		AfxMessageBox("����������� ���������!");
		break;
	}
}

// ��� �������� ���������� �������� � ������������ � �� ���������� ������������
void CChatCppDlg::OnClose()
{
	StopChat();

	CDialog::OnClose();
}

// ������� ������-��������� � ���
void CChatCppDlg::SendToChat(CString strMessage)
{
	SENDBUFFER sb;
	int len = strMessage.GetLength();
	memcpy(sb.buffer, strMessage.GetBuffer(), sizeof(TCHAR)*len);
	inputUserName.GetWindowText(strMessage);
	len = strMessage.GetLength();
	memcpy(sb.name, strMessage.GetBuffer(), sizeof(TCHAR)*len);
	sb.typemessage = m_TypeMessage::tmChat;

	SendBuffer(sb, true);
}

// ������� ����� ��������������� ��������� � ����
void CChatCppDlg::SendBuffer(SENDBUFFER sb, bool toserver)
{
	// ���� ����� �� ������, �� ������� ������ �� ������
	if(m_mainSocket.m_hSocket == INVALID_SOCKET) return;

	if(btnStartServer.GetCheck() == BST_CHECKED)
	{
		for(int i = 0; i < (int)m_vecSockets.size(); i++)
		{
			int send = m_vecSockets[i]->Send(&sb, sizeof(SENDBUFFER));
			if(send == sizeof(SENDBUFFER))
			{
				wndSend.SetWindowText("");
			}
		}
		
		/*
		���� ��������������� �������� ������������ ������,��������� ��� ��������� � ��� �� ���� ��������,
		���� toserver ��������� ��������� ������ ������������ �������� � ������ ������� ��. CChatCppDlg::OnReceive(void)
		*/
		if(toserver == true )
		{
			if(sb.typemessage == m_TypeMessage::tmChat)
			{
				CString strChat;
				wndChat.GetWindowText(strChat);
				strChat += "    " + CString(sb.name) + ": " + CString(sb.buffer)  + "\r\n";
				wndChat.SetWindowText(strChat);
				int number_line = wndChat.GetLineCount();
				wndChat.LineScroll(number_line);
			}
			if(sb.typemessage == m_TypeMessage::tmDisconnect)
			{
				CString strChat;
				wndChat.GetWindowText(strChat);
				strChat += "    " + CString(sb.name) + ": " + "��� ����������!"  + "\r\n";
				wndChat.SetWindowText(strChat);
				int number_line = wndChat.GetLineCount();
				wndChat.LineScroll(number_line);
			}
		}

	}
	else if(btnStartClient.GetCheck() == BST_CHECKED)
	{
		int send = m_mainSocket.Send(&sb, sizeof(SENDBUFFER));
		if (send == sizeof(SENDBUFFER)) {
			wndSend.SetWindowText("");
		}
	}
}


// ��������� �� ���������� �� ����
void CChatCppDlg::SendDisconnect(void)
{
	SENDBUFFER buff;
	CString str;
	inputUserName.GetWindowText(str);
	int len = str.GetLength();
	memcpy(buff.name, str.GetBuffer(), sizeof(TCHAR)*len);
	buff.typemessage = m_TypeMessage::tmDisconnect;
	
	// ������������ ������� ������������� ���.
	if (btnStartServer.GetCheck() == BST_CHECKED) {
		buff.stopchat = true;
	}
	
	SendBuffer(buff, true);
	
}

// ������� ����������� �� ������� �������.
void CChatCppDlg::OnConnect(BOOL Error)
{
	if(Error == TRUE)
	{
		AfxMessageBox("������� ����������� ���� ����������!\n�������� ������ ��� �� ������!");
		StopChat();
	}
	else
	{
		btnSendMes.EnableWindow(TRUE);
		SetWindowText("���� ��������!");
	}
}

// ������ ���������� �������� ���������� ����� � ����.
void CChatCppDlg::SendCountPeople(void)
{
	int countpeople = (int)m_vecSockets.size(); //����������� ��������

	wndCountUsers.SetWindowText("� ���� " +  IntToStr(countpeople) + " �������������");

	SENDBUFFER sb;
	sb.typemessage = m_TypeMessage::tmCountPeople;
	sb.countpeople = countpeople;

	for(int i = 0; i < (int)m_vecSockets.size(); i++)
	{
		// ���������� ����� � ���� �� ������ �������
		m_vecSockets[i]->Send(&sb, sizeof(SENDBUFFER));
	}
}

// ������ ����� ������������ ����� ��������� ������.
bool CChatCppDlg::QueryName(void)
{
	CString strName;
	inputUserName.GetWindowText(strName);
	if (strName == mess_EmptyName || strName.IsEmpty() == true) {
		return false;
	}
	else {
		return true;
	}

}


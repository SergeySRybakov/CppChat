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
	
	// Инициализация по умолчанию
	SetWindowText("Нет сети!");

	// Ограничение количества вводимых символов
	inputUserName.SetLimitText(12); 
	inputUserName.SetWindowText(mess_EmptyName);

	// Ограничение количества символов в сообщении
	wndSend.SetLimitText(50); 

	// Адрес по умолчанию
	inputServerIPAdress.SetWindowText("127.0.0.1"); 

	// Порт по умолчанию
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

// Запускаем сервер
void CChatCppDlg::OnBtnClickedRadioServer()
{
	//Отлавливание ошибок
	if (btnStartServer.GetCheck() != BST_CHECKED) { //не нажата кнопка создания сервера
		return;
	}
	else if (m_mainSocket.m_hSocket != INVALID_SOCKET) { //ошибка сокета
		return;
	} 
	else if(QueryName() == false) {
		AfxMessageBox("Введите свое имя для чата!");
		StopChat();
		return;
	}

	CString strPort;
	inputPort.GetWindowText(strPort);


	// Подключение сокета
	if(m_mainSocket.Create(atoi(strPort)) == FALSE)
	{
		AfxMessageBox("Ошибка создания сервера!\nВозможно данный порт уже используется!");
		StopChat();
	}
	else
	{
		// Вводим сокет в режим прослушивания запросов на подключения
		if(m_mainSocket.Listen() == FALSE)
		{
			StopChat();
		}
		else
		{
			// Если все в порядке
			DisabledControl(true);
			SetWindowText("Сервер ждет подключения!");
		}
	}
}

// Запускаем клиента
void CChatCppDlg::OnBtnClickedRadioClient()
{
	// Отслеживание ошибки запуска
	if (btnStartClient.GetCheck() != BST_CHECKED) { //не нажата кнопка старта
		return;
	}
	else if (m_mainSocket.m_hSocket != INVALID_SOCKET) { //ошибка сокета
		return;
	}
	else if(CChatCppDlg::QueryName() == false) { //имя уже существует
		AfxMessageBox("Введите свое имя!");
		StopChat();
		return;
	}


	// Создание сокета со связкой порта и адреса
	if(m_mainSocket.Create() == TRUE)
	{
		CString strAddress;
		inputServerIPAdress.GetWindowText(strAddress);
		CString strPort;
		inputPort.GetWindowText(strPort);
		if(m_mainSocket.Connect(strAddress, atoi(strPort)) == FALSE)
		{
			// Ожидание подключения (статус возвращен ошибкой, но как ожидание)
			if(GetLastError() == WSAEWOULDBLOCK)
			{
				DisabledControl(false);
			}
			else
			{
				// Остановка чата в случае ошибки
				StopChat();
			}
		}
	}

}


// Выход из чата
void CChatCppDlg::OnBtnClickedButtonStopchat()
{
	StopChat();
}

// Запрещает доступ к управлениям при работе приложения в режиме сервера или клиента.
void CChatCppDlg::DisabledControl(bool server)
{
	// Запреты.
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

	// Разрешить возможность выхода из чата
	btnStopChat.EnableWindow(TRUE);
}

// Разрешить доступ к управлениям после закрытия сокетов
// Цель запрета - избежать исключения от 
// случайного нажатия "неправильных" кнопо
void CChatCppDlg::EnabledControl(void)
{
	// Разрешения
	inputServerIPAdress.EnableWindow(TRUE);
	inputPort.EnableWindow(TRUE);
	btnStartClient.EnableWindow(TRUE);
	btnStartServer.EnableWindow(TRUE);
	

	// Запреты
	btnStopChat.EnableWindow(FALSE);
	btnSendMes.EnableWindow(FALSE);
}

// Принимаем запросы на подключения
void CChatCppDlg::OnAccept(void)
{
	CSock* pSock = new CSock;
	pSock->wndParent = this;

	// Если все в порядке добавим рабочий сокет в список 
	// подключенных рабочих сокетов.
	if(m_mainSocket.Accept(*pSock) == TRUE)
	{
		m_vecSockets.push_back(pSock);
		btnSendMes.EnableWindow(TRUE);
		SendCountPeople();

		SetWindowText("Сеть работает!");
	}
	else 
	{
		delete pSock;
	}
}


// Выход из чата
void CChatCppDlg::StopChat(void)
{
	// Отсылаем сигнал об отключении от чата.
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

	// В чате нет никого.
	wndCountUsers.SetWindowText("В чате 0 пользователей");
	SetWindowText("Нет сети!");
}


// Отправка подготовленного сообщения.
void CChatCppDlg::OnBtnClickedButtonSend()
{
	CString strChat;
	wndSend.GetWindowText(strChat);
	SendToChat(strChat);

}

// Извлечение сообщений из сети чата.
void CChatCppDlg::OnReceive(void)
{
	SENDBUFFER buff;
	if(btnStartServer.GetCheck() == BST_CHECKED)
	{
		for(int index = 0; index < (int)m_vecSockets.size(); index++)
		{
			m_vecSockets[index]->Receive(&buff, sizeof(SENDBUFFER));
			// Если кто-то отключился, удаляем этого клиента из списка доступных клиентов
			if(buff.typemessage == m_TypeMessage::tmDisconnect)
			{
				m_vecSockets[index]->Close();
				delete m_vecSockets[index];
				m_vecSockets.erase(m_vecSockets.begin() + index);
				
				SendCountPeople();
				
				// Отсылка принятого севером сообщения в сеть другим клиентам
				SendBuffer(buff, false);

				break;
			}
			// Рассылаем сообщения клиента по сети чата.
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

	// Обработка принятого сообщения на основе его типа
	switch(buff.typemessage)
	{
	case m_TypeMessage::tmCountPeople:
		{
			wndCountUsers.SetWindowText("В чате " + IntToStr(buff.countpeople) + "ползователей");
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

			// Если принято сообщение об остановки чата(отключении сервера),
			// изменим содержимое сообщения.
			if(buff.stopchat == true)
			{
				StopChat();
				strChat += CString(buff.name) + ": Чат остановлен!" + "\r\n";
			}
			else
			{
				strChat += CString(buff.name) + " - покинул(а) чат!" + "\r\n";
			}
			wndChat.SetWindowText(strChat);
			int number_line = wndChat.GetLineCount();
			wndChat.LineScroll(number_line);
			
			
		}
		break;
	default:
		AfxMessageBox("Неизвестное сообщение!");
		break;
	}
}

// При закрытии приложения отправим в чатинформаци ю об отключении пользователя
void CChatCppDlg::OnClose()
{
	StopChat();

	CDialog::OnClose();
}

// Послать строку-сообщение в чат
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

// Послать буфер подготовленного сообщения в сеть
void CChatCppDlg::SendBuffer(SENDBUFFER sb, bool toserver)
{
	// Если сокет не создан, то функция ничего не делает
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
		Если непосредственно отправку осуществляет сервер,отобразим его сообщение в его же окне отправки,
		флаг toserver необходим поскольку данная функцияможет работать в режиме зеркала см. CChatCppDlg::OnReceive(void)
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
				strChat += "    " + CString(sb.name) + ": " + "Чат остановлен!"  + "\r\n";
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


// Сообщение об отключении от сети
void CChatCppDlg::SendDisconnect(void)
{
	SENDBUFFER buff;
	CString str;
	inputUserName.GetWindowText(str);
	int len = str.GetLength();
	memcpy(buff.name, str.GetBuffer(), sizeof(TCHAR)*len);
	buff.typemessage = m_TypeMessage::tmDisconnect;
	
	// Отсоединение сервера останавливает чат.
	if (btnStartServer.GetCheck() == BST_CHECKED) {
		buff.stopchat = true;
	}
	
	SendBuffer(buff, true);
	
}

// Событие подключения на стороне клиента.
void CChatCppDlg::OnConnect(BOOL Error)
{
	if(Error == TRUE)
	{
		AfxMessageBox("Попытка подключения была отвергнута!\nВозможно сервер еще не создан!");
		StopChat();
	}
	else
	{
		btnSendMes.EnableWindow(TRUE);
		SetWindowText("Сеть работает!");
	}
}

// Сервер отправляет клиентам количество людей в чате.
void CChatCppDlg::SendCountPeople(void)
{
	int countpeople = (int)m_vecSockets.size(); //количесвтво клиентов

	wndCountUsers.SetWindowText("В чате " +  IntToStr(countpeople) + " пользователей");

	SENDBUFFER sb;
	sb.typemessage = m_TypeMessage::tmCountPeople;
	sb.countpeople = countpeople;

	for(int i = 0; i < (int)m_vecSockets.size(); i++)
	{
		// Количество людей в сети на данном сервере
		m_vecSockets[i]->Send(&sb, sizeof(SENDBUFFER));
	}
}

// Запрос имени пользователя перед созданием сокета.
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


#include "stdafx.h"
#include "ChatCppDlg.h"
#include "Sock.h"

CSock::CSock()
: wndParent(NULL)
{
	
}


CSock::~CSock()
{

}

// Событие подключения на стороне клиента
void CSock::OnConnect(int nErrorCode)
{
	// Данные в родительское окно для индикации процесса соединения
	CChatCppDlg* pDlg = (CChatCppDlg*)wndParent;
	nErrorCode == 0 ? pDlg->OnConnect(FALSE) : pDlg->OnConnect(TRUE);
	
	
	CAsyncSocket::OnConnect(nErrorCode);
}

// Событие отключения от сети
void CSock::OnClose(int nErrorCode)
{
	Beep(2000, 300);
	
	CAsyncSocket::OnClose(nErrorCode);
}

// Событие получения сообщений.
void CSock::OnReceive(int nErrorCode)
{
	// Данные в родительское окно для визуальности приема сообщений.
	CChatCppDlg* pDlg = (CChatCppDlg*)wndParent;
	pDlg->OnReceive();
	
	CAsyncSocket::OnReceive(nErrorCode);
}

// Запрос на подключение, направляемый клиентом серверу.
// Происходит на стороне серверного приложения.
void CSock::OnAccept(int nErrorCode)
{
	// Данные в родительское окно для индикации процесса соединения.
	CChatCppDlg* pDlg = (CChatCppDlg*)wndParent;
	pDlg->OnAccept();


	CAsyncSocket::OnAccept(nErrorCode);
}


#include "EventSelect.h"

CEventSelect::CEventSelect()
{
	m_StartupEvent = 0;
	m_DestroyEvent = 0;
	m_hSelectEvent = 0;
	m_hSelectThread = 0;
	m_hSocket = 0;
}

CEventSelect::~CEventSelect()
{
}

bool CEventSelect::Begin(SOCKET hSocket)
{
	if (!hSocket)
		return false;

	else if (m_hSocket)
		return false;

	m_hSocket = hSocket;

	m_hSelectEvent = WSACreateEvent();
	if (m_hSelectEvent == WSA_INVALID_EVENT)
	{
		End();
		return false;
	}

	m_DestroyEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!m_DestroyEvent)
	{
		End();
		return false;
	}

	m_StartupEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!m_StartupEvent)
	{
		End();
		return false;
	}

	int iResult = WSAEventSelect(hSocket, m_hSelectEvent, FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE);

	if (iResult == SOCKET_ERROR)
	{
		End();
		return false;
	}

	m_hSelectThread = (HANDLE)_beginthreadex(nullptr, 0, CEventSelect::ThreadCall, this, 0, 0);
	if (!m_hSelectThread)
	{
		End();
		return false;
	}

	WaitForSingleObject(m_StartupEvent, INFINITE);

	return true;
}

bool CEventSelect::End()
{
	if (!m_hSocket)
		return false;

	if (m_hSelectThread)
	{
		SetEvent(m_DestroyEvent);
		WaitForSingleObject(m_hSelectThread, INFINITE);
		CloseHandle(m_hSelectThread);
	}

	if (m_hSelectEvent)
		CloseHandle(m_hSelectEvent);

	if (m_StartupEvent)
		CloseHandle(m_StartupEvent);

	if (m_DestroyEvent)
		CloseHandle(m_DestroyEvent);

	return true;
}

void CEventSelect::ThreadCallback()
{
	WSANETWORKEVENTS	netEvent;
	unsigned int	iEventID = 0;
	unsigned int	iResult = 0;
	HANDLE		hThreadEvent[2] = { m_DestroyEvent, m_hSelectEvent };

	while (true)
	{
		SetEvent(m_StartupEvent);

		iEventID = WaitForMultipleObjects(2, hThreadEvent, FALSE, INFINITE);

		switch (iEventID)
		{
		case WAIT_OBJECT_0:
			return;
		case WAIT_OBJECT_0 + 1:
			iEventID = WSAEnumNetworkEvents(m_hSocket, m_hSelectEvent, &netEvent);

			if (iEventID == SOCKET_ERROR)
				return;

			else
			{
				if (netEvent.lNetworkEvents & FD_CONNECT)
				{
					OnIoConnected();

					if (netEvent.iErrorCode[FD_CONNECT_BIT])
						return;
				}

				else if (netEvent.lNetworkEvents & FD_WRITE)
				{
				}

				else if (netEvent.lNetworkEvents & FD_READ)
				{
					OnIoRead();
				}

				else if (netEvent.lNetworkEvents & FD_CLOSE)
				{
					OnIoDisconnected();

					return;
				}
			}
			break;
		default:
			return;
		}
	}
}

unsigned int __stdcall CEventSelect::ThreadCall(void* pArg)
{
	CEventSelect* pEventSelect = (CEventSelect*)pArg;

	pEventSelect->ThreadCallback();

	return 0;
}

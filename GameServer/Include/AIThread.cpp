#include "AIThread.h"

AIThread::AIThread()
{
	m_hSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	//m_hSocket = WSASocket(PF_INET, SOCK_DGRAM, IPPROTO_UDP, nullptr, 0, WSA_FLAG_OVERLAPPED);
	m_taddr.sin_family = AF_INET;
	m_taddr.sin_port = htons(AISERVER_PORT);
	m_taddr.sin_addr.s_addr = inet_addr(AISERVER_IP);
	m_iAddrSize = sizeof(m_taddr);
}

AIThread::~AIThread()
{
	if (!PacketSession->End())
		
	SAFE_DELETE(PacketSession);
	closesocket(m_hSocket);
}

bool AIThread::Init()
{
	PacketSession = new CPacketSession;


	return true;
}

void AIThread::Run()
{
	if (!Init())
		return;
	CStream stream;
	char strPacket[MAX_BUFFER_LENGTH] = {};
	WSABUF buf;
	buf.buf = strPacket;
	buf.len = MAX_BUFFER_LENGTH;
	DWORD dwNumberOfTransfer = 0;
	DWORD dwFlag = 0;
	OVERLAPPED Overlapped;

	while (1)
	{
		stream.SetBuffer(strPacket);

		if (sendto(m_hSocket, strPacket, stream.GetLength(), 0, (SOCKADDR*)&m_taddr, m_iAddrSize) == SOCKET_ERROR)
			return;

		//WSASendTo(m_hSocket, &buf, 1, &dwNumberOfTransfer, dwFlag, (SOCKADDR*)&m_taddr, m_iAddrSize, &Overlapped, nullptr);

		ZeroMemory(strPacket, stream.GetLength());
		memset(&Overlapped, 0, sizeof(Overlapped));
	}
}

#include "MonitorThread.h"
#include "Stream.h"


CMonitorThread::CMonitorThread()
{
	//	m_hSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	m_hSocket = WSASocket(PF_INET, SOCK_DGRAM, IPPROTO_UDP, nullptr, 0, WSA_FLAG_OVERLAPPED);
	m_taddr.sin_family = AF_INET;
	m_taddr.sin_port = htons(MONITORINGSERVER_PORT);
	m_taddr.sin_addr.s_addr = inet_addr(MONITORINGSERVER_IP);
	m_iAddrSize = sizeof(m_taddr);
}

CMonitorThread::~CMonitorThread()
{
	closesocket(m_hSocket);
}

bool CMonitorThread::Init()
{
	return true;
}

void CMonitorThread::Run()
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
		int MonitorList = MonitoringList::LoginServer;
		double ProcessCpu = m_Monitor.processCpuUsage();
		SIZE_T ProcessMemory = m_Monitor.processMemUsage();
		DWORD  ProcessPageFaulut = m_Monitor.PagedFalut();

		stream.SetBuffer(strPacket);
		stream.Write<int>(&MonitorList, sizeof(int));
		stream.Write<double>(&ProcessCpu, sizeof(double));
		stream.Write<SIZE_T>(&ProcessMemory, sizeof(SIZE_T));
		stream.Write<DWORD>(&ProcessPageFaulut, sizeof(DWORD));

		if (sendto(m_hSocket, strPacket, stream.GetLength(), 0, (SOCKADDR*)&m_taddr, m_iAddrSize) == SOCKET_ERROR)
			return;
		//WSASendTo(m_hSocket, &buf, 1, &dwNumberOfTransfer, dwFlag, (SOCKADDR*)&m_taddr, m_iAddrSize, &Overlapped, nullptr);

		ZeroMemory(strPacket, stream.GetLength());
		memset(&Overlapped, 0, sizeof(Overlapped));
		m_Monitor.Time();
	}
}

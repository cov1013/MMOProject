#pragma once
#include <vector>
#include <time.h>
#include "Protocol.h"
#include "../netlib/CMMOServer.h"
#include "../netlib/CLanClient.h"

using namespace std;

namespace cov1013
{
	class CGameServer : public CMMOServer
	{
		//==================================================================
		// 모니터링 서버 접속을 위한 LanClient 상속 클래스
		//==================================================================
		class CMonitorClient : public CLanClient
		{
		public:
			CMonitorClient();
			~CMonitorClient();
			void SendPacket_UpdateData(const int DataValues[], const int DataCount);

		protected:
			virtual void OnEnterJoinServer(void);					// 서버와의 연결 성공 후
			virtual void OnLeaveServer(void);						// 서버와의 연결이 끊어졌을 때
			virtual void OnRecv(CPacket* pRecvPacket);				// 하나의 패킷 수신 완료 후
			virtual void OnSend(int sendsize);						// 패킷 송신 완료 후
			virtual void OnWorkerThreadBegin(void);
			virtual void OnWorkerThreadEnd(void);
			virtual void OnError(int errorcode, wchar_t* szMsg);

		private:
			void MakePacket_Login(CPacket* pSendPacket, const int ServerNo);
			void MakePacket_DataUpdate(CPacket* pSendPacket, const BYTE DataType, const int DataValue, int TimeStamp);

		public:
			int		m_MonitorNo;
			bool	m_bConnectedFlag;
		};

	public:
		CGameServer();
		~CGameServer();
		void Run(void);
		void Quit(void);
		void Controler(void);

	private:
		static unsigned int __stdcall	MonitorThread(void* lpParam);
		int								MonitorThread_Procedure(void);

		static unsigned int __stdcall	HeartBeatThread(void* lpParam);
		int								HeartBeatThread_Procedure(void);

		static unsigned int __stdcall	MonitorServerConnectThread(void* lpParam);
		int								MonitorServerConnectThread_Procedure(void);

		void							LockPlayers(void);
		void							UnlockPlayers(void);

	public:
		int								m_TimeOutMax;

	private:
		bool							m_bLoop;
		bool							m_bRunFlag;

		CParser							m_Parser;
		CMonitorClient					m_MonitorClient;

		vector<CPlayer*>				m_Players;			
		SRWLOCK							m_Players_srw;

		HANDLE							m_hMonitorThread;
		HANDLE							m_hHeartBeatThread;
		HANDLE							m_hHeartBeatEvent;

		HANDLE							m_hMonitorServerConnectThread;
		HANDLE							m_hMonitorServerConnectThreadEvent;
	};
}
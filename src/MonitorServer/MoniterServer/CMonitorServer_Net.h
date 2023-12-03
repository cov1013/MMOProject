#pragma once
#include <unordered_map>
#include "Protocol.h"
#include "Monitor_Common.h"
#include "CMonitorServer_Lan.h"
#include "../../netlib/NetServer/CNetServer.h"

namespace cov1013
{
	class CMonitorServer_Net : public CNetServer
	{
	public:
		//==================================================================
		// 모니터링 서버에서 관리할 클라이언트 구조체 (관리 단위)
		//==================================================================
		struct st_CLIENT
		{
			SESSION_ID	SessionID;
			DWORD		LastRecvTime;
		};

	public:
		CMonitorServer_Net();
		~CMonitorServer_Net();
		void Run(void);
		void Quit(void);
		void Controler(void);

	protected:
		virtual bool OnConnectionRequest(const WCHAR* ConnectIP, const WORD ConnectPORT);
		virtual void OnClientJoin(const SESSION_ID SessionID);
		virtual void OnClientLeave(const SESSION_ID SessionID);
		virtual void OnRecv(const SESSION_ID SessionID, CPacket* pRecvPacket);
		virtual void OnSend(const SESSION_ID SessionID, const DWORD dwSendLength);
		virtual void OnWorkerThreadBegin(void);
		virtual void OnWorkerThreadEnd(void);
		virtual void OnError(const en_ERROR_CODE eErrCode, const SESSION_ID SessionID = df_INVALID_SESSION_ID);

	private:
		void							PacketProcedure(SESSION_ID SessionID, CPacket* pRecvPacket);
		void							PacketProcedure_Login(SESSION_ID SessionID, CPacket* pRecvPacket);
										
		void							MakePacket_ResLogin(CPacket* pSendPacket, const BYTE Status);
		void							MakePacket_UpdateData(CPacket* pSendPacket, const BYTE ServerNo, const BYTE DataType, const int DataValue, const int TimeStamp);

		st_CLIENT*						CreateClient(const SESSION_ID SessionID);
		st_CLIENT*						FindClient(const SESSION_ID SessionID);
		void							DeleteClient(const SESSION_ID SessionID, st_CLIENT* pServer);
		void							ClearClients(void);
		void							LockClients(void);
		void							UnlockClients(void);

		static unsigned int __stdcall	MonitorThread(void* lpParam);
		int								MonitorThread_Procedure(void);

		static unsigned int __stdcall	HeartBeatThread(void* lpParam);
		int								HeartBeatThread_Procedure(void);

		static unsigned int __stdcall	SendThread(void* lpParam);
		int								SendThread_Procedure(void);

	private:
		bool									m_bLoop;
		bool									m_bRunFlag;
		int										m_TimeOutMax;
		char									m_LoginKey[32];

		CMonitorServer_Lan						m_MonitorServer_Lan;

		unordered_map<SESSION_ID, st_CLIENT*>	m_Clients;
		CRITICAL_SECTION						m_Clients_cs;
		CMemoryPool<st_CLIENT>					m_ClientPool = CMemoryPool<st_CLIENT>(0);

		HANDLE									m_hMonitorThread;
		HANDLE									m_hSendThread;
		HANDLE									m_hHeartBeatThread;
		HANDLE									m_hHeartBeatEvent;
	};
}
#include "PacketBuffer.h"
#include "Profiler.h"
#include "CrashDumper.h"

namespace cov1013
{
	MemoryPool_TLS<PacketBuffer> PacketBuffer::sm_MemoryPool = MemoryPool_TLS<PacketBuffer>(0);
	//MemoryPool<PacketBuffer> PacketBuffer::sm_MemoryPool = MemoryPool<PacketBuffer>(0);
	unsigned char PacketBuffer::sm_byPacketKey = -1;
	unsigned char PacketBuffer::sm_byPacketCode = -1;

#ifdef __MULTI_THREAD_DEBUG_MODE__
	PacketBuffer*	PacketBuffer::sm_AlloPacketBuffer[USHRT_MAX] = { NULL };
	short		PacketBuffer::sm_AlloPacketBufferIndex = -1;
#endif

#ifdef __MULTI_THREAD_DEBUG_MODE__
	PacketBuffer* PacketBuffer::Alloc(int Logic, SESSION_ID SessionID)
	{
		PacketBuffer* pPacket = sm_MemoryPool.Alloc();
		pPacket->Initial();
		
		WORD Index = InterlockedIncrement16(&pPacket->m_LogIndex);
		Index %= st_LOG::en_LOG_MAX;
		pPacket->m_Logs[Index].ID = GetCurrentThreadId();
		pPacket->m_Logs[Index].Logic = Logic;
		pPacket->m_Logs[Index].Time = timeGetTime();
		pPacket->m_Logs[Index].RefCount = pPacket->m_lRefCount;
		pPacket->m_Logs[Index].SessionID = SessionID;

		return pPacket;
	}

	void PacketBuffer::AddRef(int Logic, SESSION_ID SessionID)
	{
		InterlockedIncrement(&m_lRefCount);

		WORD Index = InterlockedIncrement16(&m_LogIndex);
		Index %= st_LOG::en_LOG_MAX;
		m_Logs[Index].ID = GetCurrentThreadId();
		m_Logs[Index].Logic = Logic;
		m_Logs[Index].Time = timeGetTime();
		m_Logs[Index].RefCount = m_lRefCount;
		m_Logs[Index].SessionID = SessionID;
	}

	void PacketBuffer::SubRef(int Logic, SESSION_ID SessionID)
	{
		//-----------------------------------------------------
		// 래퍼런스 카운터 차감
		//-----------------------------------------------------
		long lRefCount = InterlockedDecrement(&m_lRefCount);

		WORD Index = InterlockedIncrement16(&m_LogIndex);
		Index %= st_LOG::en_LOG_MAX;
		m_Logs[Index].ID = GetCurrentThreadId();
		m_Logs[Index].Logic = Logic;
		m_Logs[Index].Time = timeGetTime();
		m_Logs[Index].RefCount = m_lRefCount;
		m_Logs[Index].SessionID = SessionID;

		//-----------------------------------------------------
		// 해당 패킷을 사용중인 곳이 없다면 정리 후 청크에 반환한다.
		//-----------------------------------------------------
		if (lRefCount == 0)
		{
			this->Release();
			sm_MemoryPool.Free(this);
		}
	}
#endif

	//////////////////////////////////////////////////////////////////
	// 패킷 메모리 할당 받기
	//////////////////////////////////////////////////////////////////
	PacketBuffer* PacketBuffer::Alloc(void)
	{
		//PRO_BEGIN(L"PacketBuffer::Alloc()");

		PacketBuffer* pPacket = sm_MemoryPool.Alloc();
		pPacket->Initial();

		//PRO_END(L"PacketBuffer::Alloc()");

		return pPacket;
	}

	//////////////////////////////////////////////////////////////////
	// 참조 카운트 상승
	//////////////////////////////////////////////////////////////////
	void PacketBuffer::AddRef(void)
	{
		//PRO_BEGIN(L"PacketBuffer::AddRef()");

		InterlockedIncrement(&m_lRefCount);

		//PRO_END(L"PacketBuffer::AddRef()");
	}

	//////////////////////////////////////////////////////////////////
	// 참조 카운트 차감
	//////////////////////////////////////////////////////////////////
	void PacketBuffer::SubRef(void)
	{
		//PRO_BEGIN(L"PacketBuffer::SubRef()");

		//-----------------------------------------------------
		// 래퍼런스 카운터 차감
		//-----------------------------------------------------
		long lRefCount = InterlockedDecrement(&m_lRefCount);

		//-----------------------------------------------------
		// 해당 패킷을 사용중인 곳이 없다면 정리 후 청크에 반환한다.
		//-----------------------------------------------------
		if (lRefCount == 0)
		{
			this->Release();

			//PRO_BEGIN(L"PacketBuffer::SubRef::Free()");
			sm_MemoryPool.Free(this);
			//PRO_END(L"PacketBuffer::SubRef::Free()");
		}

		//PRO_END(L"PacketBuffer::SubRef()");
	}

	//////////////////////////////////////////////////////////////////
	// 데이터 얻기
	//////////////////////////////////////////////////////////////////
	const int PacketBuffer::Get(char* bypDest, const int iLength)
	{
		//----------------------------------------------------
		// 예외 처리
		//----------------------------------------------------
		if (iLength <= 0)
		{
			CrashDumper::Crash();
		}
		if (m_iUseSize < iLength)
		{
			CrashDumper::Crash();
		}

		//PRO_BEGIN(L"PacketBuffer::Get()");

		//----------------------------------------------------
		// 목적지에 사이즈만큼 데이터 복사
		//----------------------------------------------------
		memcpy_s(bypDest, iLength, m_bypReadPos, iLength);

		//----------------------------------------------------
		// 데이터를 읽어들인 만큼 읽기 포인터 이동
		//----------------------------------------------------
		m_bypReadPos = m_bypReadPos + iLength;

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기를 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::Get()");

		return iLength;
	}

	const int PacketBuffer::Get_UNI16(wchar_t* bypDest, int iLength)
	{
		//----------------------------------------------------
		// 유니코드 크기에 맞게끔 사이즈 * 2
		//----------------------------------------------------
		iLength = sizeof(wchar_t) * iLength;

		//----------------------------------------------------
		// 예외 처리
		//----------------------------------------------------
		if (iLength <= 0)
		{
			CrashDumper::Crash();
		}
		if (m_iUseSize < iLength)
		{
			CrashDumper::Crash();
		}

		//PRO_BEGIN(L"PacketBuffer::Get_UNI16()");

		//----------------------------------------------------
		// 목적지에 사이즈만큼 데이터 복사
		//----------------------------------------------------
		memcpy_s(bypDest, iLength, m_bypReadPos, iLength);

		//----------------------------------------------------
		// 데이터를 읽어들인 만큼 읽기 포인터 이동
		//----------------------------------------------------
		m_bypReadPos = m_bypReadPos + iLength;

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기를 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::Get_UNI16()");

		return iLength;
	}

	//////////////////////////////////////////////////////////////////
	// 데이터 넣기
	//////////////////////////////////////////////////////////////////
	const int PacketBuffer::Put(char* bypSrc, const int iLength)
	{
		//----------------------------------------------------
		// 예외 처리
		//----------------------------------------------------
		if (iLength <= 0)
		{
			CrashDumper::Crash();
		}
		if (m_iFreeSize < iLength)
		{
			CrashDumper::Crash();
		}

		//PRO_BEGIN(L"PacketBuffer::Put()");

		//----------------------------------------------------
		// 직렬화버퍼로 사이즈만큼 데이터 복사
		//----------------------------------------------------
		memcpy_s(m_bypWritePos, iLength, bypSrc, iLength);

		//----------------------------------------------------
		// 데이터를 복사한 만큼 쓰기 포인터 이동
		//----------------------------------------------------
		m_bypWritePos = m_bypWritePos + iLength;

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::Put()");

		return iLength;
	}

	const int PacketBuffer::Put_UNI16(const wchar_t* bypSrc, int iLength)
	{
		//----------------------------------------------------
		// 유니코드 크기에 맞게끔 사이즈 * 2
		//----------------------------------------------------
		iLength = sizeof(wchar_t) * iLength;

		//----------------------------------------------------
		// 예외 처리
		//----------------------------------------------------
		if (iLength <= 0)
		{
			CrashDumper::Crash();
		}
		if (m_iFreeSize < iLength)
		{
			CrashDumper::Crash();
		}

		//PRO_BEGIN(L"PacketBuffer::Put_UNI16()");

		//----------------------------------------------------
		// 직렬화버퍼로 사이즈만큼 데이터 복사
		//----------------------------------------------------
		memcpy_s(m_bypWritePos, iLength, bypSrc, iLength);

		//----------------------------------------------------
		// 데이터를 복사한 만큼 쓰기 포인터 이동
		//----------------------------------------------------
		m_bypWritePos = m_bypWritePos + iLength;

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::Put_UNI16()");

		return iLength;
	}

	//////////////////////////////////////////////////////////////////
	// 데이터 엿보기
	//////////////////////////////////////////////////////////////////
	const int PacketBuffer::Peek(char* bypDest, const int iLength)
	{
		//----------------------------------------------------
		// 예외 처리
		//----------------------------------------------------
		if (iLength <= 0)
		{
			CrashDumper::Crash();
		}
		if (m_iUseSize < iLength)
		{
			CrashDumper::Crash();
		}

		//PRO_BEGIN(L"PacketBuffer::Peek()");

		//----------------------------------------------------
		// 목적지에 사이즈만큼 데이터 복사
		//----------------------------------------------------
		memcpy_s(bypDest, iLength, m_bypReadPos, iLength);

		//PRO_END(L"PacketBuffer::Peek()");

		return iLength;
	}

	const int PacketBuffer::Peek_UNI16(wchar_t* bypDest, int iLength)
	{
		//----------------------------------------------------
		// 유니코드 크기에 맞게끔 사이즈 * 2
		//----------------------------------------------------
		iLength = sizeof(wchar_t) * iLength;

		//----------------------------------------------------
		// 예외 처리
		//----------------------------------------------------
		if (iLength <= 0)
		{
			CrashDumper::Crash();
		}
		if (m_iUseSize < iLength)
		{
			CrashDumper::Crash();
		}

		//PRO_BEGIN(L"PacketBuffer::Peek_UNI16()");

		//----------------------------------------------------
		// 목적지에 사이즈만큼 데이터 복사
		//----------------------------------------------------
		memcpy_s(bypDest, iLength, m_bypReadPos, iLength);

		//PRO_END(L"PacketBuffer::Peek_UNI16()");

		return iLength;
	}

	//////////////////////////////////////////////////////////////////
	// 쓰기 포인터 이동
	//////////////////////////////////////////////////////////////////
	const int PacketBuffer::MoveWritePos(const int iLength)
	{
		//----------------------------------------------------
		// 예외 처리
		//----------------------------------------------------
		if (iLength <= 0)
		{
			CrashDumper::Crash();
		}
		if (iLength >= m_iBufferSize)
		{
			CrashDumper::Crash();
		}

		//PRO_BEGIN(L"PacketBuffer::MoveWritePos()");

		//----------------------------------------------------
		// 쓰기 포인터 이동
		//----------------------------------------------------
		m_bypWritePos = m_bypWritePos + iLength;

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::MoveWritePos()");

		return iLength;
	}

	//////////////////////////////////////////////////////////////////
	// 읽기 포인터 이동
	//////////////////////////////////////////////////////////////////
	const int PacketBuffer::MoveReadPos(const int iLength)
	{
		//----------------------------------------------------
		// 예외 처리
		//----------------------------------------------------
		if (iLength <= 0)
		{
			CrashDumper::Crash();
		}
		if (iLength >= m_iBufferSize)
		{
			CrashDumper::Crash();
		}

		//PRO_BEGIN(L"PacketBuffer::MoveReadPos()");

		//----------------------------------------------------
		// 읽기 포인터 이동
		//----------------------------------------------------
		m_bypReadPos = m_bypReadPos + iLength;

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::MoveReadPos()");

		return iLength;
	}

	//////////////////////////////////////////////////////////////////
	// 버퍼 시작 포인터 얻기
	//////////////////////////////////////////////////////////////////
	char* PacketBuffer::GetBufferPtr(void) const
	{
		return (CHAR*)m_bypBuffer;
	}

	//////////////////////////////////////////////////////////////////
	// 읽기 위치 얻기
	//////////////////////////////////////////////////////////////////
	char* PacketBuffer::GetReadPos(void) const
	{
		return m_bypReadPos;
	}

	//////////////////////////////////////////////////////////////////
	// 쓰기 위치 얻기
	//////////////////////////////////////////////////////////////////
	char* PacketBuffer::GetWritePos(void) const
	{
		return m_bypWritePos;
	}

	//////////////////////////////////////////////////////////////////
	// 사용 가능 사이즈 얻기
	//////////////////////////////////////////////////////////////////
	const int PacketBuffer::GetFreeSize(void) const
	{
		return m_iFreeSize;
	}

	//////////////////////////////////////////////////////////////////
	// 사용중인 사이즈 얻기
	//////////////////////////////////////////////////////////////////
	const int PacketBuffer::GetUseSize(void) const
	{
		return m_iUseSize;
	}

	//////////////////////////////////////////////////////////////////
	// 헤더 크기 포함한 전체 패킷 사이즈 얻기
	//////////////////////////////////////////////////////////////////
	const int PacketBuffer::GetPacketSize(void) const
	{
		return eHEADER_LEN + m_iUseSize;
	}

	//////////////////////////////////////////////////////////////////
	// 메모리풀 용량 얻기
	//////////////////////////////////////////////////////////////////
	const long PacketBuffer::GetPoolCapacity(void)
	{
		return sm_MemoryPool.GetCapacity();
	}

	//////////////////////////////////////////////////////////////////
	// 메모리풀 사용중인 크기 얻기
	//////////////////////////////////////////////////////////////////
	const long PacketBuffer::GetPoolUseSize(void)
	{
		return sm_MemoryPool.GetUseCount();
	}

	//////////////////////////////////////////////////////////////////
	// 암호화
	//////////////////////////////////////////////////////////////////
	void PacketBuffer::Encode(void)
	{
		//----------------------------------------------------
		// 이미 인코딩이 되어있다면 리턴
		//----------------------------------------------------
		if (true == (bool)InterlockedExchange8((CHAR*)&m_bEncode, true))
		{
			return;
		}

		//PRO_BEGIN(L"PacketBuffer::Encode()");

		//----------------------------------------------------
		// 1. 헤더 세팅 및 버퍼에 넣기
		//----------------------------------------------------
		st_PACKET_HEADER Header;
		Header.Code = sm_byPacketCode;
		Header.Len = GetUseSize();
		Header.RandKey = (rand() % 65536);
		Header.CheckSum = MakeChecksum();
		memcpy_s(m_bypBuffer, eHEADER_LEN, (void*)&Header, eHEADER_LEN);

		//----------------------------------------------------
		// 2. 인코딩 시작
		//----------------------------------------------------
		unsigned char P = 0;
		unsigned char E = 0;
		int iLen = 1 + GetUseSize(); // Checksum + Payload

		for (int i = 0; i < iLen; i++)
		{
			//----------------------------------------------------
			// 처음 P와 E만 0, 0 이고 한 번 인코딩 한 다음부터는
			// 이전의 P와 E를 가지고 인코딩한다.
			//----------------------------------------------------
			P = (m_bypReadPos - 1)[i] ^ (P + Header.RandKey + (i + 1));
			E = P ^ (E + sm_byPacketKey + (i + 1));

			//----------------------------------------------------
			// 인코딩한 결과를 버퍼에 재삽입
			//----------------------------------------------------
			(m_bypReadPos - 1)[i] = E;
		}

		//PRO_END(L"PacketBuffer::Encode()");

	}

	//////////////////////////////////////////////////////////////////
	// 복호화
	//////////////////////////////////////////////////////////////////
	bool PacketBuffer::Decode(void)
	{
		//----------------------------------------------------
		// 1. 버퍼의 시작점을 헤더로 해석
		//----------------------------------------------------
		st_PACKET_HEADER* pHeader = (st_PACKET_HEADER*)m_bypBuffer;

		//----------------------------------------------------
		// 2. 패킷 코드 확인
		//----------------------------------------------------
		if (pHeader->Code != sm_byPacketCode)
		{
			return false;
		}

		//PRO_BEGIN(L"PacketBuffer::Decode()");

		//----------------------------------------------------
		// 3. 디코딩
		//----------------------------------------------------
		unsigned char D = 0;
		unsigned char P = 0;
		unsigned char E = 0;
		unsigned char _E = 0;			// 이전값 저장용
		unsigned char _P = 0;			// 이전값 저장용
		int iLen = GetUseSize() + 1;	// Checksum + Payload

		for (int i = 0; i < iLen; i++)
		{
			E = (m_bypReadPos - 1)[i];

			P = E ^ (_E + sm_byPacketKey + (i + 1));	// P = E ^ FK
			D = P ^ (_P + pHeader->RandKey + (i + 1));	// D = P ^ RK

			_E = (m_bypReadPos - 1)[i];					// 이전 E 저장		
			_P = P;										// 이전 P 저장

			(m_bypReadPos - 1)[i] = D;
		}

		//----------------------------------------------------
		// 4. 체크섬 생성 후 비교
		//----------------------------------------------------
		unsigned char byCheckSum = MakeChecksum();
		if (byCheckSum != pHeader->CheckSum)
		{
			return false;
		}

		//----------------------------------------------------
		// 5. 인코딩 여부 갱신
		//----------------------------------------------------
		m_bEncode = false;

		//PRO_END(L"PacketBuffer::Decode()");

		return true;
	}

	//////////////////////////////////////////////////////////////////
	// 체크섬 제작
	//////////////////////////////////////////////////////////////////
	unsigned char PacketBuffer::MakeChecksum(void)
	{
		//PRO_BEGIN(L"PacketBuffer::MakeChecksum()");

		int iSum = 0;
		int iLen = GetUseSize();

		//----------------------------------------------------
		// 페이로드를 전부 더한 후 % 256
		//----------------------------------------------------
		for (int i = 0; i < iLen; i++)
		{
			iSum += m_bypReadPos[i];
		}

		//PRO_END(L"PacketBuffer::MakeChecksum()");

		return (unsigned char)(iSum % 256);
	}

	//////////////////////////////////////////////////////////////////
	// 연산자 오버로딩
	//////////////////////////////////////////////////////////////////
	PacketBuffer& PacketBuffer::operator<<(const BYTE src)
	{
		//PRO_BEGIN(L"PacketBuffer::<< BYTE");

		if (m_iFreeSize < sizeof(BYTE))
		{
			return *this;
		}

		//----------------------------------------------------
		// 파라미터 사이즈만큼 직렬화 버퍼에 데이터 삽입
		//----------------------------------------------------
		*m_bypWritePos = src;
		m_bypWritePos = m_bypWritePos + sizeof(BYTE);

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::<< BYTE");

		return *this;
	}

	PacketBuffer& PacketBuffer::operator<<(const char src)
	{
		//PRO_BEGIN(L"PacketBuffer::<<char");

		if (m_iFreeSize < sizeof(char))
		{
			return *this;
		}

		//----------------------------------------------------
		// 파라미터 사이즈만큼 직렬화 버퍼에 데이터 삽입
		//----------------------------------------------------
		*m_bypWritePos = src;
		m_bypWritePos = m_bypWritePos + sizeof(char);

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::<<char");

		return *this;
	}

	PacketBuffer& PacketBuffer::operator<<(const WORD src)
	{
		//PRO_BEGIN(L"PacketBuffer::<<WORD");

		if (m_iFreeSize < sizeof(WORD))
		{
			return *this;
		}

		//----------------------------------------------------
		// 파라미터 사이즈만큼 직렬화 버퍼에 데이터 삽입
		//----------------------------------------------------
		*((WORD*)m_bypWritePos) = src;
		m_bypWritePos = m_bypWritePos + sizeof(WORD);

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::<<WORD");

		return *this;
	}

	PacketBuffer& PacketBuffer::operator<<(const short src)
	{
		//PRO_BEGIN(L"PacketBuffer::<<short");

		if (m_iFreeSize < sizeof(short))
		{
			return *this;
		}

		//----------------------------------------------------
		// 파라미터 사이즈만큼 직렬화 버퍼에 데이터 삽입
		//----------------------------------------------------
		*((short*)m_bypWritePos) = src;
		m_bypWritePos = m_bypWritePos + sizeof(short);

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::<<short");

		return *this;
	}

	PacketBuffer& PacketBuffer::operator<<(const DWORD src)
	{
		//PRO_BEGIN(L"PacketBuffer::<<DWORD");

		if (m_iFreeSize < sizeof(DWORD))
		{
			return *this;
		}

		//----------------------------------------------------
		// 파라미터 사이즈만큼 직렬화 버퍼에 데이터 삽입
		//----------------------------------------------------
		*((DWORD*)m_bypWritePos) = src;
		m_bypWritePos = m_bypWritePos + sizeof(DWORD);

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::<<DWORD");

		return *this;
	}

	PacketBuffer& PacketBuffer::operator<<(const UINT src)
	{
		//PRO_BEGIN(L"PacketBuffer::<<UINT");

		if (m_iFreeSize < sizeof(UINT))
		{
			return *this;
		}

		//----------------------------------------------------
		// 파라미터 사이즈만큼 직렬화 버퍼에 데이터 삽입
		//----------------------------------------------------
		*((UINT*)m_bypWritePos) = src;
		m_bypWritePos = m_bypWritePos + sizeof(UINT);

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::<<UINT");

		return *this;
	}

	PacketBuffer& PacketBuffer::operator<<(const int src)
	{
		//PRO_BEGIN(L"PacketBuffer::<<int");

		if (m_iFreeSize < sizeof(int))
		{
			return *this;
		}

		//----------------------------------------------------
		// 파라미터 사이즈만큼 직렬화 버퍼에 데이터 삽입
		//----------------------------------------------------
		*((int*)m_bypWritePos) = src;
		m_bypWritePos = m_bypWritePos + sizeof(int);

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::<<int");

		return *this;
	}

	PacketBuffer& PacketBuffer::operator<<(const long src)
	{
		//PRO_BEGIN(L"PacketBuffer::<<long");

		if (m_iFreeSize < sizeof(long))
		{
			return *this;
		}

		//----------------------------------------------------
		// 파라미터 사이즈만큼 직렬화 버퍼에 데이터 삽입
		//----------------------------------------------------
		*((long*)m_bypWritePos) = src;
		m_bypWritePos = m_bypWritePos + sizeof(long);

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::<<long");

		return *this;
	}

	PacketBuffer& PacketBuffer::operator<<(const float src)
	{
		//PRO_BEGIN(L"PacketBuffer::<<float");

		if (m_iFreeSize < sizeof(float))
		{
			return *this;
		}

		//----------------------------------------------------
		// 파라미터 사이즈만큼 직렬화 버퍼에 데이터 삽입
		//----------------------------------------------------
		*((float*)m_bypWritePos) = src;
		m_bypWritePos = m_bypWritePos + sizeof(float);

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::<<float");

		return *this;
	}

	PacketBuffer& PacketBuffer::operator<<(const __int64 src)
	{
		//PRO_BEGIN(L"PacketBuffer::<<__int64");

		if (m_iFreeSize < sizeof(__int64))
		{
			return *this;
		}

		//----------------------------------------------------
		// 파라미터 사이즈만큼 직렬화 버퍼에 데이터 삽입
		//----------------------------------------------------
		*((__int64*)m_bypWritePos) = src;
		m_bypWritePos = m_bypWritePos + sizeof(__int64);

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::<<__int64");

		return *this;
	}

	PacketBuffer& PacketBuffer::operator<<(const double src)
	{
		//PRO_BEGIN(L"PacketBuffer::<<double");

		if (m_iFreeSize < sizeof(double))
		{
			return *this;
		}

		//----------------------------------------------------
		// 파라미터 사이즈만큼 직렬화 버퍼에 데이터 삽입
		//----------------------------------------------------
		*((double*)m_bypWritePos) = src;
		m_bypWritePos = m_bypWritePos + sizeof(double);

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_BEGIN(L"PacketBuffer::<<double");

		return *this;
	}

	PacketBuffer& PacketBuffer::operator>>(BYTE& dest)
	{
		//PRO_BEGIN(L"PacketBuffer::>>BYTE");

		if (m_iUseSize < sizeof(BYTE))
		{
			return *this;
		}

		//----------------------------------------------------
		// 파라미터 사이즈만큼 직렬화 버퍼 데이터를 외부로 복사
		//----------------------------------------------------
		dest = *m_bypReadPos;
		m_bypReadPos = m_bypReadPos + sizeof(BYTE);

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::>>BYTE");

		return *this;
	}

	PacketBuffer& PacketBuffer::operator>>(char& dest)
	{
		//PRO_BEGIN(L"PacketBuffer::>>char");

		if (m_iUseSize < sizeof(char))
		{
			return *this;
		}

		//----------------------------------------------------
		// 파라미터 사이즈만큼 직렬화 버퍼 데이터를 외부로 복사
		//----------------------------------------------------
		dest = *m_bypReadPos;
		m_bypReadPos = m_bypReadPos + sizeof(char);

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::>>char");

		return *this;
	}

	PacketBuffer& PacketBuffer::operator>>(short& dest)
	{
		//PRO_BEGIN(L"PacketBuffer::>>short");

		if (m_iUseSize < sizeof(short))
		{
			return *this;
		}

		//----------------------------------------------------
		// 파라미터 사이즈만큼 직렬화 버퍼 데이터를 외부로 복사
		//----------------------------------------------------
		dest = *((short*)m_bypReadPos);
		m_bypReadPos = m_bypReadPos + sizeof(short);

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::>>short");

		return *this;
	}

	PacketBuffer& PacketBuffer::operator>>(WORD& dest)
	{
		//PRO_BEGIN(L"PacketBuffer::>>WORD");

		if (m_iUseSize < sizeof(WORD))
		{
			return *this;
		}

		//----------------------------------------------------
		// 파라미터 사이즈만큼 직렬화 버퍼 데이터를 외부로 복사
		//----------------------------------------------------
		dest = *((WORD*)m_bypReadPos);
		m_bypReadPos = m_bypReadPos + sizeof(WORD);

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::>>WORD");

		return *this;
	}

	PacketBuffer& PacketBuffer::operator>>(UINT& dest)
	{
		//PRO_BEGIN(L"PacketBuffer::>>UINT");

		if (m_iUseSize < sizeof(UINT))
		{
			return *this;
		}

		//----------------------------------------------------
		// 파라미터 사이즈만큼 직렬화 버퍼 데이터를 외부로 복사
		//----------------------------------------------------
		dest = *((UINT*)m_bypReadPos);
		m_bypReadPos = m_bypReadPos + sizeof(UINT);

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::>>UINT");

		return *this;
	}

	PacketBuffer& PacketBuffer::operator>>(int& dest)
	{
		//PRO_BEGIN(L"PacketBuffer::>>int");

		if (m_iUseSize < sizeof(int))
		{
			return *this;
		}

		//----------------------------------------------------
		// 파라미터 사이즈만큼 직렬화 버퍼 데이터를 외부로 복사
		//----------------------------------------------------
		dest = *((int*)m_bypReadPos);
		m_bypReadPos = m_bypReadPos + sizeof(int);

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::>>int");

		return *this;
	}

	PacketBuffer& PacketBuffer::operator>>(DWORD& dest)
	{
		//PRO_BEGIN(L"PacketBuffer::>>DWORD");

		if (m_iUseSize < sizeof(DWORD))
		{
			return *this;
		}

		//----------------------------------------------------
		// 파라미터 사이즈만큼 직렬화 버퍼 데이터를 외부로 복사
		//----------------------------------------------------
		dest = *((DWORD*)m_bypReadPos);
		m_bypReadPos = m_bypReadPos + sizeof(DWORD);

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::>>DWORD");

		return *this;
	}

	PacketBuffer& PacketBuffer::operator>>(long& dest)
	{
		//PRO_BEGIN(L"PacketBuffer::>>long");

		if (m_iUseSize < sizeof(long))
		{
			return *this;
		}

		//----------------------------------------------------
		// 파라미터 사이즈만큼 직렬화 버퍼 데이터를 외부로 복사
		//----------------------------------------------------
		dest = *((long*)m_bypReadPos);
		m_bypReadPos = m_bypReadPos + sizeof(long);

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::>>long");

		return *this;
	}

	PacketBuffer& PacketBuffer::operator>>(float& dest)
	{
		//PRO_BEGIN(L"PacketBuffer::>>float");

		if (m_iUseSize < sizeof(float))
		{
			return *this;
		}

		//----------------------------------------------------
		// 파라미터 사이즈만큼 직렬화 버퍼 데이터를 외부로 복사
		//----------------------------------------------------
		dest = *((float*)m_bypReadPos);
		m_bypReadPos = m_bypReadPos + sizeof(float);

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::>>float");

		return *this;
	}

	PacketBuffer& PacketBuffer::operator>>(double& dest)
	{
		//PRO_BEGIN(L"PacketBuffer::>>double");

		if (m_iUseSize < sizeof(double))
		{
			return *this;
		}

		//----------------------------------------------------
		// 파라미터 사이즈만큼 직렬화 버퍼 데이터를 외부로 복사
		//----------------------------------------------------
		dest = *((double*)m_bypReadPos);
		m_bypReadPos = m_bypReadPos + sizeof(double);

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::>>double");

		return *this;
	}

	PacketBuffer& PacketBuffer::operator>>(__int64& dest)
	{
		//PRO_BEGIN(L"PacketBuffer::>>__in64");

		if (m_iUseSize < sizeof(__int64))
		{
			return *this;
		}

		//----------------------------------------------------
		// 파라미터 사이즈만큼 직렬화 버퍼 데이터를 외부로 복사
		//----------------------------------------------------
		dest = *((__int64*)m_bypReadPos);
		m_bypReadPos = m_bypReadPos + sizeof(__int64);

		//----------------------------------------------------
		// 사용중인 크기와 사용 가능 크기 갱신
		//----------------------------------------------------
		m_iUseSize = (int)(m_bypWritePos - m_bypReadPos);
		m_iFreeSize = ePAYLOAD_LEN - m_iUseSize;

		//PRO_END(L"PacketBuffer::>>__in64");

		return *this;
	}

	////////////////////////////////////////////////////////////////////
	// LanServer 전용
	////////////////////////////////////////////////////////////////////
	char* PacketBuffer::GetLanHeaderPtr(void) const
	{
		return (CHAR*)m_bypBuffer + 3;
	}

	const int PacketBuffer::GetLanPacketSize(void) const
	{
		return 2 + m_iUseSize;
	}

	void PacketBuffer::SetLanHeader(void)
	{
		short shPayloadLen = GetUseSize();
		*((short*)&m_bypBuffer[3]) = shPayloadLen;

		//memcpy_s(m_bypBuffer + 3, sizeof(short), &shPayloadLen, sizeof(short));
	}

	//////////////////////////////////////////////////////////////////
	// 생성자
	//////////////////////////////////////////////////////////////////
	PacketBuffer::PacketBuffer()
	{
		if (m_bInitialize == false)
		{
			m_bInitialize = true;
		}
		else
		{
			CrashDumper::Crash();
		}
#ifdef __MULTI_THREAD_DEBUG_MODE__
		m_iIndex = -1;
		m_LogIndex = -1;
#endif
	}

	//////////////////////////////////////////////////////////////////
	// 소멸자
	//////////////////////////////////////////////////////////////////
	PacketBuffer::~PacketBuffer()
	{

	}

	//////////////////////////////////////////////////////////////////
	// 패킷 초기화
	//////////////////////////////////////////////////////////////////
	void PacketBuffer::Initial(void)
	{
		//PRO_BEGIN(L"PacketBuffer::Initial()");

		m_bEncode		= false;
		//memset(m_bypBuffer, 0, eHEADER_LEN + ePAYLOAD_LEN);
		m_bypWritePos	= m_bypBuffer + eHEADER_LEN;	// 헤더 영역 제외
		m_bypReadPos	= m_bypBuffer + eHEADER_LEN;	// 헤더 영역 제외
		m_iBufferSize	= eHEADER_LEN + ePAYLOAD_LEN;	// 헤더 + 페이로드
		m_iFreeSize		= ePAYLOAD_LEN;					// 페이로드 사이즈
		m_iUseSize		= 0;
		m_lRefCount		= 1;

		//PRO_END(L"PacketBuffer::Initial()");

#ifdef __MULTI_THREAD_DEBUG_MODE__
		if (m_iIndex == -1)
		{
			m_iIndex = (WORD)InterlockedIncrement16(&sm_AlloPacketBufferIndex);
			sm_AlloPacketBuffer[m_iIndex] = this;
		}
#endif
	}

	//////////////////////////////////////////////////////////////////
	// 패킷 정리
	//////////////////////////////////////////////////////////////////
	void PacketBuffer::Release(void)
	{
		//PRO_BEGIN(L"PacketBuffer::Release()");

		m_bEncode = false;
		memset(m_bypBuffer, 0, eHEADER_LEN + ePAYLOAD_LEN);
		m_bypWritePos = m_bypBuffer + eHEADER_LEN;
		m_bypReadPos = m_bypBuffer + eHEADER_LEN;
		m_iFreeSize = ePAYLOAD_LEN;
		m_iUseSize = 0;

		//PRO_END(L"PacketBuffer::Release()");
	}

#ifdef __MULTI_THREAD_DEBUG_MODE__

	SESSION_ID PacketBuffer::OutputDebugData(void)
	{
		for (int i = 0; i < USHRT_MAX; i++)
		{
			if (sm_AlloPacketBuffer[i] == nullptr)
			{
				break;
			}

			if (sm_AlloPacketBuffer[i]->m_lRefCount != 0)
			{
				//CrashDumper::Crash();
				return sm_AlloPacketBuffer[i]->m_Logs[sm_AlloPacketBuffer[i]->m_LogIndex].SessionID;
			}
		}
	}

#endif

	//////////////////////////////////////////////////////////////////
	// 패킷 고정키 세팅
	//////////////////////////////////////////////////////////////////
	void PacketBuffer::SetPacketKey(unsigned char byKey)
	{
		sm_byPacketKey = byKey;
	}

	//////////////////////////////////////////////////////////////////
	// 패킷 코드 세팅
	//////////////////////////////////////////////////////////////////
	void PacketBuffer::SetPacketCode(unsigned char byCode)
	{
		sm_byPacketCode = byCode;
	}
}
#pragma once	

namespace cov1013
{
	class RingBuffer
	{
	public:
		/*---------------------------------------------------------------------*/
		// 클래스 기본 구성
		//
		// eBUFFER_SIZE  : 버퍼 크기
		// eBUFFER_BLANK : 버퍼 크기 계산을 위한 더미 공간
		/*---------------------------------------------------------------------*/
		enum en_CONFIG
		{
			eBUFFER_SIZE = 1024,
			eBUFFER_BLANK = 1
		};

	public:
		//////////////////////////////////////////////////////////////////
		// 생성자
		//////////////////////////////////////////////////////////////////
		RingBuffer();

		//////////////////////////////////////////////////////////////////
		// 소멸자
		//////////////////////////////////////////////////////////////////
		virtual ~RingBuffer();

		//////////////////////////////////////////////////////////////////
		// 버퍼 사이즈 얻기
		//////////////////////////////////////////////////////////////////
		const int GetBufferSize(void) const;

		//////////////////////////////////////////////////////////////////
		// 프론트 버퍼 포인터 얻기
		//////////////////////////////////////////////////////////////////
		char* GetReadPos(void) const;

		//////////////////////////////////////////////////////////////////
		// 니어 버퍼 포인터 얻기
		//////////////////////////////////////////////////////////////////
		char* GetWritePos(void) const;

		//////////////////////////////////////////////////////////////////
		// 버퍼 시작 포인터 얻기
		//////////////////////////////////////////////////////////////////
		char* GetBufferPtr(void) const;

		//////////////////////////////////////////////////////////////////
		// 버퍼가 비어있는지 확인
		//////////////////////////////////////////////////////////////////
		const bool IsEmpty(void) const;

		//////////////////////////////////////////////////////////////////
		// 버퍼가 꽉 차있는지 확인
		//////////////////////////////////////////////////////////////////
		const bool IsFull(void);

		//////////////////////////////////////////////////////////////////
		// 사용중인 사이즈 얻기
		//////////////////////////////////////////////////////////////////
		const int GetUseSize(void);

		//////////////////////////////////////////////////////////////////
		// 사용 가능 사이즈 얻기
		//////////////////////////////////////////////////////////////////
		const int GetFreeSize(void);

		//////////////////////////////////////////////////////////////////
		// 끊기지않고 넣을 수 있는 사이즈 얻기
		//////////////////////////////////////////////////////////////////
		const int GetNonBrokenPutSize(void);

		//////////////////////////////////////////////////////////////////
		// 끊기지않고 뺄 수 있는 사이즈 얻기
		//////////////////////////////////////////////////////////////////
		const int GetNonBrokenGetSize(void);

		//////////////////////////////////////////////////////////////////
		// 큐에 데이터 넣기
		//////////////////////////////////////////////////////////////////
		int Put(const char* bypSource, int iSize);

		//////////////////////////////////////////////////////////////////
		// 큐에서 데이터 뽑기
		//////////////////////////////////////////////////////////////////
		int Get(char* bypDest, int iSize);

		//////////////////////////////////////////////////////////////////
		// 큐에서 데이터 뽑기 (읽기 포인터 이동 X)
		//////////////////////////////////////////////////////////////////
		int Peek(char* bypDest, int iSize);

		//////////////////////////////////////////////////////////////////
		// 쓰기 포인터 이동
		//////////////////////////////////////////////////////////////////
		int MoveWritePos(const int iSize);

		//////////////////////////////////////////////////////////////////
		// 읽기 포인터 이동
		//////////////////////////////////////////////////////////////////
		int MoveReadPos(const int iSize);

		//////////////////////////////////////////////////////////////////
		// 버퍼 초기화
		//////////////////////////////////////////////////////////////////
		void Clear(void);

	private:
		int		m_iBufferSize;
		int		m_iWritePos;
		int		m_iReadPos;
		char	m_bypBuffer[eBUFFER_SIZE];
	};
}



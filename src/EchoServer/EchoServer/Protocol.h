#pragma once

enum en_PACKET_TYPE
{
	//------------------------------------------------------------
	// 테스트용 에코 요청
	//
	//	{
	//	}
	//
	//------------------------------------------------------------	
	en_PACKET_CS_ECHO_REQ_ECHO = 5000,

	//------------------------------------------------------------
	// 테스트용 에코 응답 (REQ 를 그대로 돌려줌)
	//
	//	{
	//	}
	//
	//------------------------------------------------------------
	en_PACKET_CS_ECHO_RES_ECHO,
};
#pragma once

#include "PacketID.h"
#include "ErrorCode.h"

#include <Poco/Types.h>


struct RecvPacketInfo
{
	int SessionIndex = 0;
	short PacketID = 0;
	short PacketBodySize = 0;
	char* pRefData = 0;
};


#pragma pack(push, 1)
struct PACKET_HEADER
{
	UINT16 PacketLength;
	UINT16 PacketId;
	UINT8 Type; //압축여부 암호화여부 등 속성을 알아내는 값
};

//- 로그인 요청
const int MAX_USER_ID_LEN = 20;
const int MAX_USER_PW_LEN = 20;

struct LOGIN_REQUEST_PACKET : public PACKET_HEADER
{
	char UserID[MAX_USER_ID_LEN + 1];
	char UserPW[MAX_USER_PW_LEN + 1];
};
const size_t LOGIN_REQUEST_PACKET_SZIE = sizeof(LOGIN_REQUEST_PACKET);


struct LOGIN_RESPONSE_PACKET : public PACKET_HEADER
{
	UINT16 Result;
};



//- 룸에 들어가기 요청
const int MAX_ROOM_TITLE_SIZE = 16;
struct ROOM_ENTER_REQUEST_PACKET : public PACKET_HEADER
{
	INT32 RoomNumber;
};

struct ROOM_ENTER_RESPONSE_PACKET : public PACKET_HEADER
{
	INT16 Result;
	char RivalUserID[MAX_USER_ID_LEN + 1] = { 0, };
};


//- 룸 나가기 요청
struct ROOM_LEAVE_REQUEST_PACKET : public PACKET_HEADER
{

};

struct ROOM_LEAVE_RESPONSE_PACKET : public PACKET_HEADER
{
	INT16 Result;
};



// 룸 채팅
const int MAX_CHAT_MSG_SIZE = 256;
struct ROOM_CHAT_REQUEST_PACKET : public PACKET_HEADER
{
	char Message[MAX_CHAT_MSG_SIZE + 1] = { 0, };
};

struct ROOM_CHAT_RESPONSE_PACKET : public PACKET_HEADER
{
	INT16 Result;
};

struct ROOM_CHAT_NOTIFY_PACKET : public PACKET_HEADER
{
	char UserID[MAX_USER_ID_LEN + 1] = { 0, };
	char Msg[MAX_CHAT_MSG_SIZE + 1] = { 0, };
};

#pragma pack(pop)
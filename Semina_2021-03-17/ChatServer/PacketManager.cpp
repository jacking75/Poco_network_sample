#include <utility>
#include <cstring>

#include "Packet.h"
#include "PacketManager.h"



	void PacketManager::Init(const int maxSessionCount)
	{
		mUserManager.Init(maxSessionCount);

		mRoomManager.SendPacketFunc = SendPacketFunc;
		mRoomManager.Init(0, 100, 4);


		m_RecvFuntionDictionary = std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION>();

		m_RecvFuntionDictionary[(int)PACKET_ID::DEV_ECHO] = &PacketManager::ProcessDevEcho;

		m_RecvFuntionDictionary[(int)PACKET_ID::INTERNAL_CLOSE] = &PacketManager::ProcessDisConnected;

		m_RecvFuntionDictionary[(int)PACKET_ID::LOGIN_REQUEST] = &PacketManager::ProcessLogin;		
		m_RecvFuntionDictionary[(int)PACKET_ID::ROOM_ENTER_REQUEST] = &PacketManager::ProcessEnterRoom;
		m_RecvFuntionDictionary[(int)PACKET_ID::ROOM_LEAVE_REQUEST] = &PacketManager::ProcessLeaveRoom;
		m_RecvFuntionDictionary[(int)PACKET_ID::ROOM_CHAT_REQUEST] = &PacketManager::ProcessRoomChatMessage;				
	}



	void PacketManager::Process(const INT32 connectionIndex, const UINT16 packetID, char* pBodyData, INT16 bodySize) 
	{
		auto iter = m_RecvFuntionDictionary.find(packetID);
		if (iter != m_RecvFuntionDictionary.end())
		{
			(this->*(iter->second))(connectionIndex, pBodyData, bodySize);
		}

	}
		
	void PacketManager::ProcessDevEcho(INT32 connIndex, char* pBodyData, INT16 bodySize)
	{
		auto packetID = (UINT16)PACKET_ID::DEV_ECHO;
		auto packetSize = (UINT16)(bodySize + sizeof(PACKET_HEADER));
		char echoData[1024] = { 0, };

		CopyMemory(&echoData, (char*)&packetSize, 2);
		CopyMemory(&echoData[2], (char*)&packetID, 2);
		CopyMemory(&echoData[5], pBodyData, bodySize);

		SendPacketFunc(connIndex, echoData, packetSize);
	}

	void PacketManager::ProcessDisConnected(INT32 connIndex, char* pBodyData, INT16 bodySize)
	{
		auto pUser = mUserManager.GetUserByConnIdx(connIndex);
		if (pUser == nullptr)
		{
			return;
		}

		if (pUser->IsCurInRoom())
		{
			mRoomManager.LeaveUser(pUser->GetCurrentRoomIndex(), pUser);
		}

		mUserManager.RemoveUserInfo(pUser);
	}

	void PacketManager::ProcessLogin(const INT32 connIndex, char* pBodyData, INT16 bodySize)
	{ 		
		auto pLoginReqPacket = reinterpret_cast<LOGIN_REQUEST_PACKET*>(pBodyData);

		auto pUserID = pLoginReqPacket->UserID;
		printf("requested user id = %s\n", pUserID);

		LOGIN_RESPONSE_PACKET loginResPacket;
		loginResPacket.PacketID = (UINT16)PACKET_ID::LOGIN_RESPONSE;
		loginResPacket.PacketSize = sizeof(LOGIN_RESPONSE_PACKET);

		if (mUserManager.GetCurrentUserCnt() >= mUserManager.GetMaxUserCnt())
		{ 
			//접속자수가 최대수를 차지해서 접속불가
			loginResPacket.Result = (UINT16)ERROR_CODE::LOGIN_USER_USED_ALL_OBJ;
			SendPacketFunc(connIndex, (char*)&loginResPacket, sizeof(LOGIN_RESPONSE_PACKET));
			return;
		}

		//여기에서 이미 접속된 유저인지 확인하고, 접속된 유저라면 실패한다.
		if (mUserManager.FindUserIndexByID(pUserID) == -1)
		{ 
			//접속중이 아닌 유저라면
			//유저를 관리하는 객체에 넣는다.
			mUserManager.AddUser(pUserID, connIndex);
			loginResPacket.Result = (UINT16)ERROR_CODE::NONE;
		}
		else 
		{
			//접속중인 유저여서 실패를 반환한다.
			loginResPacket.Result = (UINT16)ERROR_CODE::LOGIN_USER_ALREADY;
			SendPacketFunc(connIndex, (char*)&loginResPacket, sizeof(LOGIN_RESPONSE_PACKET));
			return;
		}

		SendPacketFunc(connIndex, (char*)&loginResPacket, sizeof(LOGIN_RESPONSE_PACKET));
	}



	void PacketManager::ProcessEnterRoom(INT32 connIndex, char* pBodyData, INT16 bodySize)
	{
		UNREFERENCED_PARAMETER(bodySize);

		auto pRoomEnterReqPacket = reinterpret_cast<ROOM_ENTER_REQUEST_PACKET*>(pBodyData);
		auto pReqUser = mUserManager.GetUserByConnIdx(connIndex);

		if (!pReqUser || pReqUser == nullptr) 
		{
			return;
		}
		
		//TODO 방에 있는 유저들이 있으면 이 유저들의 정보를 새로운 들어온 유저에게 알린다

		//TODO 방에 유저가 있었다면 이 유저에게 새로 들어온 유저 정보를 알린다

		ROOM_ENTER_RESPONSE_PACKET roomEnterResPacket;
		roomEnterResPacket.PacketID = (UINT16)PACKET_ID::ROOM_ENTER_RESPONSE;
		roomEnterResPacket.PacketSize = sizeof(ROOM_ENTER_RESPONSE_PACKET);
				
		roomEnterResPacket.Result = mRoomManager.EnterUser(pRoomEnterReqPacket->RoomNumber, pReqUser);

		SendPacketFunc(connIndex, (char*)&roomEnterResPacket, sizeof(ROOM_ENTER_RESPONSE_PACKET));
	}


	void PacketManager::ProcessLeaveRoom(INT32 connIndex, char* pBodyData, INT16 copySize) 
	{
		UNREFERENCED_PARAMETER(pBodyData);
		UNREFERENCED_PARAMETER(copySize);

		ROOM_LEAVE_RESPONSE_PACKET roomLeaveResPacket;
		roomLeaveResPacket.PacketID = (UINT16)PACKET_ID::ROOM_LEAVE_RESPONSE;
		roomLeaveResPacket.PacketSize = sizeof(ROOM_LEAVE_RESPONSE_PACKET);

		auto reqUser = mUserManager.GetUserByConnIdx(connIndex);
		auto roomNum = reqUser->GetCurrentRoomIndex();
				
		roomLeaveResPacket.Result = mRoomManager.LeaveUser(roomNum, reqUser);
		
		SendPacketFunc(connIndex, (char*)&roomLeaveResPacket, sizeof(ROOM_LEAVE_RESPONSE_PACKET));
	}


	void PacketManager::ProcessRoomChatMessage(INT32 connIndex, char* pBodyData, INT16 bodySize)
	{
		UNREFERENCED_PARAMETER(bodySize);

		auto pRoomChatReqPacketet = reinterpret_cast<ROOM_CHAT_REQUEST_PACKET*>(pBodyData);
		
		ROOM_CHAT_RESPONSE_PACKET roomChatResPacket;
		roomChatResPacket.PacketID = (UINT16)PACKET_ID::ROOM_CHAT_RESPONSE;
		roomChatResPacket.PacketSize = sizeof(ROOM_CHAT_RESPONSE_PACKET);
		roomChatResPacket.Result = (INT16)ERROR_CODE::NONE;

		auto reqUser = mUserManager.GetUserByConnIdx(connIndex);
		auto roomNum = reqUser->GetCurrentRoomIndex();
				
		auto pRoom = mRoomManager.GetRoomByNumber(roomNum);
		if (pRoom == nullptr)
		{
			roomChatResPacket.Result = (INT16)ERROR_CODE::CHAT_ROOM_INVALID_ROOM_NUMBER;
			SendPacketFunc(connIndex, (char*)&roomChatResPacket, sizeof(ROOM_CHAT_RESPONSE_PACKET));
			return;
		}
				
		pRoom->NotifyChat(connIndex, reqUser->GetUserId().c_str(), pRoomChatReqPacketet->Message);		
	}		   





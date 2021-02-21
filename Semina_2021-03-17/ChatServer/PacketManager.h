#pragma once
#include <unordered_map>
#include <functional>

#include "UserManager.h"
#include "RoomManager.h"



	class PacketManager {
	public:
		PacketManager() = default;
		~PacketManager() = default;

		void Init(const int maxSessionCount);
		void ProcessRecvPacket(INT32 connectionIndex, char* pBuf, INT16 copySize);
		void ProcessLogin(INT32 connIndex, char* pBuf, INT16 copySize);
		void ProcessEnterRoom(INT32 connIndex, char* pBuf, INT16 copySize);
		void ProcessLeaveRoom(INT32 connIndex, char* pBuf, INT16 copySize);
		void ProcessRoomChatMessage(INT32 connIndex, char* pBuf, INT16 copySize);
		

		void ClearConnectionInfo(INT32 connIndex);
		

		std::function<void(const int, const int, char*)> SendPacketFunc;


	private:
		typedef void(PacketManager::* PROCESS_RECV_PACKET_FUNCTION)(INT32, char*, INT16);
		std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION> m_RecvFuntionDictionary;

		UserManager mUserManager;
		RoomManager mRoomManager;
		
		
	};


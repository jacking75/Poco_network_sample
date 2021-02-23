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
		
		void Process(INT32 connectionIndex, const UINT16 packetID, char* pBuf, INT16 bodySize);
		
		void ProcessDevEcho(INT32 connIndex, char* pBodyData, INT16 bodySize);

		void ProcessDisConnected(INT32 connIndex, char* pBodyData, INT16 bodySize);

		void ProcessLogin(INT32 connIndex, char* pBodyData, INT16 bodySize);
		void ProcessEnterRoom(INT32 connIndex, char* pBodyData, INT16 bodySize);
		void ProcessLeaveRoom(INT32 connIndex, char* pBodyData, INT16 bodySize);
		void ProcessRoomChatMessage(INT32 connIndex, char* pBodyData, INT16 bodySize);
		

	
		std::function<void(const int, const char*, const int)> SendPacketFunc;


	private:
		typedef void(PacketManager::* PROCESS_RECV_PACKET_FUNCTION)(INT32, char*, INT16);
		std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION> m_RecvFuntionDictionary;

		UserManager mUserManager;
		RoomManager mRoomManager;
		
		
	};


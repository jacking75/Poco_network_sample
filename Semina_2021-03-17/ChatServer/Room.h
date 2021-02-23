#pragma once

#include <functional>
#include "UserManager.h"


	class Room 
	{
	public:
		Room() = default;
		~Room() = default;

		void Init(const Poco::Int32 roomNum, const Poco::Int32 maxUserCount);

		INT32 GetMaxUserCount() { return m_MaxUserCount; }
		
		INT32 GetCurrentUserCount() { return m_CurrentUserCount; }
		
		INT32 GetRoomNumber() { return m_RoomNum; }

		UINT16 EnterUser(User* pUser);
		
		void LeaveUser(User* pUser);
						
		void NotifyChat(int connIndex, const char* UserID, const char* Msg);
		
		
		std::function<void(const int, const char*, const int)> SendPacketFunc;

	private:
		void SendToAllUser(const Poco::UInt16 dataSize, const char* pData, const Poco::Int32 passUserindex, bool exceptMe);


		Poco::Int32 m_RoomNum = -1;

		std::list<User*> m_UserList;
		
		Poco::Int32 m_MaxUserCount = 0;

		Poco::UInt16 m_CurrentUserCount = 0;
	};

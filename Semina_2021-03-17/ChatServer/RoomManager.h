#pragma once
#include "Room.h"

	class RoomManager
	{
	public:
		RoomManager() = default;
		~RoomManager() = default;

		void Init(const Poco::Int32 beginRoomNumber, const Poco::Int32 maxRoomCount, const Poco::Int32 maxRoomUserCount)
		{
			m_BeginRoomNumber = beginRoomNumber;
			m_MaxRoomCount = maxRoomCount;
			m_EndRoomNumber = beginRoomNumber + maxRoomCount;

			m_RoomList = std::vector<Room*>(maxRoomCount);

			for (auto i = 0; i < maxRoomCount; i++)
			{
				m_RoomList[i] = new Room();
				m_RoomList[i]->SendPacketFunc = SendPacketFunc;
				m_RoomList[i]->Init((i+ beginRoomNumber), maxRoomUserCount);
			}
		}

		Poco::UInt32 GetMaxRoomCount() { return m_MaxRoomCount; }
		
		Poco::UInt16 EnterUser(Poco::Int32 roomNumber, User* pUser)
		{
			auto pRoom = GetRoomByNumber(roomNumber);
			if (pRoom == nullptr)
			{
				return (Poco::UInt16)ERROR_CODE::ROOM_INVALID_INDEX;
			}


			return pRoom->EnterUser(pUser);
		}
		
		INT16 LeaveUser(Poco::Int32 roomNumber, User* pUser)
		{
			auto pRoom = GetRoomByNumber(roomNumber);
			if (pRoom == nullptr)
			{
				return (Poco::Int16)ERROR_CODE::ROOM_INVALID_INDEX;
			}
			
			pUser->SetDomainState(User::DOMAIN_STATE::LOGIN);
			pRoom->LeaveUser(pUser);
			return (Poco::Int16)ERROR_CODE::NONE;
		}

		Room* GetRoomByNumber(Poco::Int32 number)
		{ 
			if (number < m_BeginRoomNumber || number >= m_EndRoomNumber)
			{
				return nullptr;
			}

			auto index = (number - m_BeginRoomNumber);
			return m_RoomList[index]; 
		} 

		
		std::function<void(const int, const int, char*)> SendPacketFunc;
		

	private:
		std::vector<Room*> m_RoomList;
		Poco::Int32 m_BeginRoomNumber = 0;
		Poco::Int32 m_EndRoomNumber = 0;
		Poco::Int32 m_MaxRoomCount = 0;
	};

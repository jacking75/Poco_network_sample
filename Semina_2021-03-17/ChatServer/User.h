#pragma once

//#define POCO_STATIC
#include <Poco/Types.h>

#include <string>

#include "Packet.h"




	class User
	{
	public:		
		User() = default;
		~User() = default;

		void Init(const Poco::Int32 index)
		{
			m_Index = index;
		}

		void Clear()
		{
			m_RoomIndex = -1;
			m_UserId = "";
			m_IsConfirm = false;
		}

		int SetLogin(char* login_id)
		{
			m_IsConfirm = true;
			m_UserId = login_id;

			return 0;
		}
		
		void EnterRoom(Poco::Int32 roomIndex) {	m_RoomIndex = roomIndex; }
		void LeaveRoom() { m_RoomIndex = -1; }
				
		Poco::Int32 GetCurrentRoomIndex()
		{
			return m_RoomIndex;
		}

		Poco::Int32 GetNetConnIdx()
		{
			return m_Index;
		}

		std::string GetUserId() const
		{
			return  m_UserId;
		}
				
		bool IsCurInRoom() { return m_RoomIndex > -1 ? true : false; }
						

	private:		
		Poco::Int32 m_RoomIndex = -1;
		bool m_IsConfirm = false;

		std::string m_UserId;
		
		std::string AuthToken;

		Poco::Int32 m_Index = -1;
	};
#include "Room.h"
#include "Packet.h"
#define MAX_USER_ID_LEN  20


	void Room::Init(const Poco::Int32 roomNum, const Poco::Int32 maxUserCount)
	{
		m_RoomNum = roomNum;
		m_MaxUserCount = maxUserCount;
	}

	Poco::UInt16 Room::EnterUser(User* pUser)
	{
		if (m_CurrentUserCount >= m_MaxUserCount)
		{
			return (Poco::UInt16)ERROR_CODE::ENTER_ROOM_FULL_USER;
		}
				
		m_UserList.push_back(pUser);
		++m_CurrentUserCount;

		pUser->EnterRoom(m_RoomNum);
		return (Poco::UInt16)ERROR_CODE::NONE;
	}


	void Room::LeaveUser(User* pLeaveUser) 
	{
		m_UserList.remove_if([leaveUserId = pLeaveUser->GetUserId()](User *pUser) {
									return leaveUserId == pUser->GetUserId();
								});

		pLeaveUser->LeaveRoom();
	}
			
	void Room::NotifyChat(Poco::Int32 connIndex, const char* UserID, const char* Msg)
	{
		ROOM_CHAT_NOTIFY_PACKET roomChatNtfyPkt;
		roomChatNtfyPkt.PacketID = (UINT16)PACKET_ID::ROOM_CHAT_NOTIFY;
		roomChatNtfyPkt.PacketSize = sizeof(roomChatNtfyPkt);

		CopyMemory(roomChatNtfyPkt.Msg, Msg, sizeof(roomChatNtfyPkt.Msg));
		CopyMemory(roomChatNtfyPkt.UserID, UserID, sizeof(roomChatNtfyPkt.UserID));
		SendToAllUser(sizeof(roomChatNtfyPkt), (char*)&roomChatNtfyPkt, -1, false);
	}


	void Room::SendToAllUser(const Poco::UInt16 dataSize, const char* pData, const Poco::Int32 passUserindex, bool exceptMe)
	{

		for (auto pUser : m_UserList)
		{
			if (pUser == nullptr) {
				continue;
			}

			if (exceptMe && pUser->GetNetConnIdx() == passUserindex) {
				continue;
			}
						
			SendPacketFunc(pUser->GetNetConnIdx(), pData, dataSize);
		}
	}

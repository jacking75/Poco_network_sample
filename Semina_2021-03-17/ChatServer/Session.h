#pragma once

#include <iostream>
#include <functional>

#include "RecvRingBuffer.h"


class Session 
{
public:
	Session(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor) :
		m_socket(socket),
		m_reactor(reactor)
	{
		mRecvBuffer.Create(1024*8);

		m_peerAddress = socket.peerAddress().toString();
		std::cout << "connection from " << m_peerAddress << " ..." << std::endl;

		m_reactor.addEventHandler(m_socket,
			Poco::Observer<Session, Poco::Net::ReadableNotification>(*this, &Session::onReadable));

		OnConnection(this);
	}

	~Session()
	{
		std::cout << m_peerAddress << " disconnected ..." << std::endl;
	}


	static std::function<void(Session*)> OnConnection;
	static std::function<void(Session*)> OnClose;
	static std::function<void(const bool, const int, const short, const short, char*)> AddPacketFunc;


	void SetIndex(const int index) { mIndex = index; }
	int GetIndex() { return mIndex; }


	void onReadable(Poco::Net::ReadableNotification* pNf)
	{
		pNf->release();

		try
		{
			auto pBuffer = mRecvBuffer.GetWriteBuffer(1024);
			
			int recvSize = m_socket.receiveBytes(pBuffer, 1024);
			if (recvSize > 0)
			{
				//pBuffer[recvSize] = '\0';
				//std::cout << "클라이언트에서 받은 메시지: " << pBuffer << std::endl;				
				auto [remainSize, pRemainData ] = mRecvBuffer.GetReceiveData(recvSize);
								
				auto deliveredSize = ToPacketProcess(remainSize, pRemainData);

				mRecvBuffer.ForwardReadPos(deliveredSize);
			}
			else
			{
				Close();
			}
		}
		catch (Poco::Exception ex)
		{
			Close();
		}		
	}

	void SendPacket(const char* pData, const int size)
	{
		if (mIsConnected == false)
		{
			return;
		}

		m_socket.sendBytes(pData, size);
	}


private:
	int ToPacketProcess(int remainByte, char* pBuffer)
	{
		const int MAX_PACKET_SIZE = 1024;
		const int PACKET_HEADER_LENGTH = 5;
		const int PACKET_SIZE_LENGTH = 2;
		const int PACKET_TYPE_LENGTH = 2;

		int totalReadSize = 0;

		while (true)
		{
			if (remainByte < PACKET_HEADER_LENGTH)
			{
				break;
			}

			auto pHeader = (PACKET_HEADER*)pBuffer;

			if (pHeader->PacketSize > MAX_PACKET_SIZE)
			{
				break;
			}

			if (remainByte < pHeader->PacketSize)
			{
				break;
			}

			auto bodySize = pHeader->PacketSize - PACKET_HEADER_LENGTH;
			auto packetID = pBuffer[PACKET_SIZE_LENGTH];
			AddPacketFunc(false, mIndex, pHeader->PacketID, bodySize, &pBuffer[PACKET_HEADER_LENGTH]);
						
			remainByte -= pHeader->PacketSize;
			totalReadSize += pHeader->PacketSize;
			pBuffer += pHeader->PacketSize;
			
		}

		return totalReadSize;
	}

	void Close()
	{
		mIsConnected = false;

		m_socket.close();

		m_reactor.removeEventHandler(m_socket,
			Poco::Observer<Session, Poco::Net::ReadableNotification>(*this, &Session::onReadable)
		);

		OnClose(this);
	}

	Poco::Net::StreamSocket   m_socket;
	std::string m_peerAddress;
	Poco::Net::SocketReactor& m_reactor;

	int mIndex;
	bool mIsConnected = true;
	RecvRingBuffer mRecvBuffer;
};

inline std::function<void(Session*)> Session::OnConnection;
inline std::function<void(Session*)> Session::OnClose;
inline std::function<void(const bool, const int, const short, const short, char*)> Session::AddPacketFunc;

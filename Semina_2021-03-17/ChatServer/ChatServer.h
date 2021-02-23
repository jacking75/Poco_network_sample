#pragma comment(lib, "IPHLPAPI.lib")  

#include <deque>
#include <thread>
#include <mutex>
#include <optional>


#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/ParallelSocketAcceptor.h>

#include "PacketManager.h"
#include "Session.h"
#include "Packet.h"


class ChatServer
{
public:
	ChatServer() = default;
	~ChatServer() = default;

	void Init(const int maxSessionCount)
	{
		SetFunction();
		
		for (int i = 0; i < maxSessionCount; ++i)
		{
			mSessionIndexPool.push_back(i);
			mSessions.push_back(nullptr);
		}

		mPacketManager.Init(maxSessionCount);	
	}

	void Start(const Poco::UInt16 port)
	{		
		std::thread logicThread([&]() {
			Run(); }
		);
		
		mServerSocket.bind(port, true);
		mServerSocket.listen();

		mpAcceptor = std::make_unique<Poco::Net::ParallelSocketAcceptor<Session, Poco::Net::SocketReactor>>(mServerSocket, mReactor);
		
		mReactor.run();		
	}

	void Run()
	{
		std::cout << "Start Packet Thread >>>" << std::endl;

		m_IsRun = true;

		while (m_IsRun)
		{
			while (true)
			{
				auto packetInfo = GetPacketInfo();

				if (packetInfo.PacketID == 0)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
					break;
				}
				else
				{
					if (packetInfo.PacketID > (short)PACKET_ID::INTERNAL_END)
					{
						mPacketManager.Process(packetInfo.SessionIndex,
							packetInfo.PacketID,
							packetInfo.pBodyData, 
												packetInfo.PacketBodySize);
					}
					
					if (packetInfo.PacketID == (short)PACKET_ID::INTERNAL_CLOSE)
					{
						UnRegisterSesssion(packetInfo.SessionIndex);
					}					
				}
			}
		}
	}


private:
	void SetFunction()
	{
		auto onConnectFunc = [&](Session* sock)
		{
			this->OnConnect(sock);
		};
		Session::OnConnection = onConnectFunc;

		auto onCloseFunc = [&](Session* pSession)
		{
			this->OnClose(pSession);
		};
		Session::OnClose = onCloseFunc;

		auto addPacketFunc = [&](const bool bInternal,  const int sessionIndex, const short pktID, const short bodySize, char* pDataPos)
		{
			this->AddPacketQueue(bInternal, sessionIndex, pktID, bodySize, pDataPos);
		};
		Session::AddPacketFunc = addPacketFunc;

		auto sendPacketFunc = [&](const int sessionIndex, const char* pData, const int dataSize)
		{
			this->SendPacket(sessionIndex, pData, dataSize);
		};
		mPacketManager.SendPacketFunc = sendPacketFunc;
	}

	void OnConnect(Session* pSession)
	{
		RegisterSesssion(pSession);
	}

	void OnClose(Session* pSession)
	{
		AddPacketQueue(true, pSession->GetIndex(), (short)PACKET_ID::INTERNAL_CLOSE, 0, nullptr);		
	}


	void AddPacketQueue(const bool bInternal, const Poco::Int32 sessionIndex, const short pktID, const short bodySize, char* pDataPos)
	{
		if (bInternal == false && pktID <= (short)PACKET_ID::INTERNAL_END)
		{
			return;
		}

		std::lock_guard<std::mutex> lock(mMutexPacketQueue);

		RecvPacketInfo packetInfo;
		packetInfo.SessionIndex = sessionIndex;
		packetInfo.PacketID = pktID;
		packetInfo.PacketBodySize = bodySize;
		packetInfo.pBodyData = pDataPos;

		m_PacketQueue.push_back(packetInfo);
	}
		
	RecvPacketInfo GetPacketInfo()
	{
		std::lock_guard<std::mutex> lock(mMutexPacketQueue);

		RecvPacketInfo packetInfo;

		if (m_PacketQueue.empty() == false)
		{
			packetInfo = m_PacketQueue.front();
			m_PacketQueue.pop_front();
		}

		return packetInfo;
	}

	void RegisterSesssion(Session* pSession)
	{
		auto newSessionIndex = AllocSesionIndex();
		if (newSessionIndex)
		{
			pSession->SetIndex(newSessionIndex.value());
			mSessions[newSessionIndex.value()] = pSession;
		}
	}

	void UnRegisterSesssion(const int index)
	{
		delete mSessions[index];
		mSessions[index] = nullptr;
		FreeSessionIndex(index);
	}

	void SendPacket(const int sessionIndex, const char* pData, const int size)
	{
		mSessions[sessionIndex]->SendPacket(pData, size);
	}

	std::optional<int> AllocSesionIndex()
	{
		std::lock_guard<std::mutex> lock(mMutexSessionPool);
		if (mSessionIndexPool.empty())
		{
			return std::nullopt;
		}

		auto index = mSessionIndexPool.front();
		mSessionIndexPool.pop_front();

		return index;
	}

	void FreeSessionIndex(const int index)
	{
		std::lock_guard<std::mutex> lock(mMutexSessionPool);
		mSessionIndexPool.push_back(index);
	}



	Poco::Net::ServerSocket mServerSocket;
	Poco::Net::SocketReactor mReactor;
	std::unique_ptr<Poco::Net::ParallelSocketAcceptor<Session, Poco::Net::SocketReactor>> mpAcceptor = nullptr;


	bool m_IsRun = false;

	std::mutex mMutexPacketQueue;
	std::deque<RecvPacketInfo> m_PacketQueue;

	std::mutex mMutexSessionPool;
	std::deque<int> mSessionIndexPool;

	std::vector<Session*> mSessions;

	PacketManager mPacketManager;
};


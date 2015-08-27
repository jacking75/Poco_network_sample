#pragma once 

#include <Poco/Timestamp.h>

class StopWatch
{
public:
	StopWatch() {}
	~StopWatch() {}

	void Start()
	{
		m_startTimestamp.update();
		m_startTime = m_startTimestamp.epochTime(); 
	}

	__int64 End()
	{		
		m_endTimestamp.update();
		Poco::Timestamp::TimeDiff diff = m_endTimestamp - m_startTimestamp; 
		return diff / 1000000;
	}

private:
	Poco::Timestamp m_startTimestamp;
	Poco::Timestamp m_endTimestamp;
	std::time_t m_startTime;
	std::time_t m_endTime;
};
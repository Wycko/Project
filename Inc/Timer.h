#ifndef _TIMER_H_
#define _TIMER_H_

#include "Common.h"


class Timer
{
public:
	Timer();
	~Timer();

	void Start();
	void Stop();
	void Reset();
	double GetAbsoluteTime() const;
	double GetTime() const;
	double GetElapsedTime();

private:
	double ConvertToMicroSeconds( LONGLONG& Ticks ) const;

	LONGLONG m_LastStop;
	LONGLONG m_LastElapsed;
	LONGLONG m_BaseTime;
	LONGLONG m_Frequency;

	bool m_Running;
};


#endif // !_TIMER_H_

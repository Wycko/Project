#ifndef _PERFORMANCELOGGER_H_
#define _PERFORMANCELOGGER_H_

#include "Common.h"
#include <algorithm>



class PerformanceLogger
{
public:
	PerformanceLogger();
	~PerformanceLogger();

	void StartLog();
	void StopLog();
	void ResetLog();

	double GetElapsed() const;
	double GetAverage() const;
	void GetMinMax( double& Min, double& Max ) const;

private:
	double ConvertToMicroSeconds( LONGLONG Ticks ) const;

	LONGLONG m_Current;
	LONGLONG m_LastElapsed;
	LONGLONG m_Minimum;
	LONGLONG m_Maximum;
	LONGLONG m_Frequency;

	LONGLONG m_Counter;
	LONGLONG m_Average;
};


#endif // !_PERFORMANCELOGGER_H_

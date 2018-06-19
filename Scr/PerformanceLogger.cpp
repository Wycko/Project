#include "PerformanceLogger.h"


PerformanceLogger::PerformanceLogger()
{
	LARGE_INTEGER TicksPerSec = { 0 };
	QueryPerformanceFrequency( &TicksPerSec );

	m_Frequency	= TicksPerSec.QuadPart;

	ResetLog();
}


PerformanceLogger::~PerformanceLogger()
{
}


void PerformanceLogger::StartLog()
{
	LARGE_INTEGER Time = { 0 };
	QueryPerformanceCounter( &Time );

	m_Current = Time.QuadPart;
}


void PerformanceLogger::StopLog()
{
	LARGE_INTEGER Time = { 0 };
	QueryPerformanceCounter( &Time );

	m_LastElapsed = Time.QuadPart - m_Current;

	m_Minimum = std::min( m_LastElapsed, m_Minimum );
	m_Maximum = std::max( m_LastElapsed, m_Maximum );

	m_Average += m_LastElapsed;
	m_Counter++;
}


void PerformanceLogger::ResetLog()
{
	m_Current		= 0;
	m_LastElapsed	= 0;
	m_Maximum		= INT_MIN;
	m_Minimum		= INT_MAX;

	m_Average		= 0;
	m_Counter		= 0;
}


double PerformanceLogger::GetElapsed() const
{
	return ConvertToMicroSeconds( m_LastElapsed );
}


double PerformanceLogger::GetAverage() const
{
	// Prevent division by zero.
	if( m_Counter == 0 )
		return ConvertToMicroSeconds( m_Average );

	else
		return ConvertToMicroSeconds( m_Average / m_Counter );
}


void PerformanceLogger::GetMinMax( double& Min, double& Max ) const
{
	Min = ConvertToMicroSeconds( m_Minimum );
	Max = ConvertToMicroSeconds( m_Maximum );
}


double PerformanceLogger::ConvertToMicroSeconds( LONGLONG Ticks ) const
{
	double T = ( double )Ticks;

	T *= 1000000;
	T /= ( double )m_Frequency;

	if( T < 0 )
		T = 0;

	return T;
}

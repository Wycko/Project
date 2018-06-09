#include "Timer.h"


Timer::Timer()
{
	LARGE_INTEGER TicksPerSec = { 0 };
	QueryPerformanceFrequency( &TicksPerSec );
	m_Frequency = TicksPerSec.QuadPart;

	m_Running		= false;
	m_BaseTime		= 0;
	m_LastElapsed	= 0;
	m_LastStop		= 0;
}


Timer::~Timer()
{
}


void Timer::Start()
{
	// Get the current time
	LARGE_INTEGER Time = { 0 };
	QueryPerformanceCounter( &Time );

	if( !m_Running )
		m_BaseTime += Time.QuadPart - m_LastStop;

	m_LastStop		= 0;
	m_LastElapsed	= Time.QuadPart;
	m_Running		= true;
}


void Timer::Stop()
{
	if( m_Running )
	{
		LARGE_INTEGER Time = { 0 };
		QueryPerformanceCounter( &Time );

		m_LastStop		= Time.QuadPart;
		m_LastElapsed	= Time.QuadPart;
		m_Running		= false;
	}
}


void Timer::Reset()
{
	LARGE_INTEGER Time;
	if( m_LastStop != 0 )
		Time.QuadPart = m_LastStop;
	else
		QueryPerformanceCounter( &Time );

	m_BaseTime		= Time.QuadPart;
	m_LastElapsed	= Time.QuadPart;
	m_LastStop		= 0;
	m_Running		= true;
}


double Timer::GetAbsoluteTime() const
{
	LARGE_INTEGER Time = { 0 };
	QueryPerformanceCounter( &Time );

	return ConvertToMicroSeconds( Time.QuadPart );
}


double Timer::GetTime() const
{
	LARGE_INTEGER Time = { 0 };
	if( m_LastStop != 0 )
		Time.QuadPart = m_LastStop;
	else
		QueryPerformanceCounter( &Time );

	Time.QuadPart =	Time.QuadPart - m_BaseTime;

	return ConvertToMicroSeconds( Time.QuadPart );
}


double Timer::GetElapsedTime()
{
	LARGE_INTEGER Time = { 0 };
	if( m_LastStop != 0 )
		Time.QuadPart = m_LastStop;
	else
		QueryPerformanceCounter( &Time );

	LONGLONG Elapsed = Time.QuadPart - m_LastElapsed;
	m_LastElapsed	 = Time.QuadPart;

	return ConvertToMicroSeconds( Elapsed );
}


double Timer::ConvertToMicroSeconds( LONGLONG& Ticks ) const
{
	Ticks *= 1000000;
	Ticks /= m_Frequency;

	if( Ticks < 0 )
		Ticks = 0;

	return ( double )Ticks;
}

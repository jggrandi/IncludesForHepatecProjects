#ifndef TIMER_INCLUDED
#define TIMER_INCLUDED

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else // !_WIN32
#include <unistd.h>
#include <sys/times.h>
#endif // _WIN32

#include <ErrorManager.h>

class CTimer
{
public:
    CTimer(void)
    {
#ifdef _WIN32
        __int64 status;
        
        if (QueryPerformanceCounter((LARGE_INTEGER*)&status) == 0)
			MarkError();
		else
			QueryPerformanceCounter((LARGE_INTEGER*)&m_start);

#else // !_WIN32
        m_clockTicks = sysconf(_SC_CLK_TCK);
        
        if (m_clockTicks == -1)
        {
            std::cout << "unable to initialize timer" << std::endl;
            std::cout << "exiting now" << std::endl;
            
            exit(EXIT_FAILURE);
        }
        
        m_start = times(&m_tmsStart);
#endif // _WIN32
    }
    
    inline void Restart(void)
    {
#ifdef _WIN32
        QueryPerformanceCounter((LARGE_INTEGER*)&m_start);

#else // !_WIN32
        m_start = times(&m_tmsStart);
#endif // _WIN32
    }
    
    inline double GetElapsed(void)
    {
        double time;

#ifdef _WIN32
        __int64 clocksPerSecond;
        
        QueryPerformanceCounter((LARGE_INTEGER*)&m_end);
        
        QueryPerformanceFrequency((LARGE_INTEGER*)&clocksPerSecond);
        
        time = (double)(m_end - m_start)/(double)clocksPerSecond;

#else // !_WIN32
        m_end = times(&m_tmsEnd);
        
        time = (double)(m_end - m_start)/(double)m_clockTicks;
#endif // _WIN32
        
        return 1000.0*time; 
    }
protected:
#ifdef _WIN32
    __int64 m_start;
    __int64 m_end;
#else // !_WIN32
    tms m_tmsStart;
    tms m_tmsEnd;
    clock_t m_start;
    clock_t m_end;
    long m_clockTicks;
#endif // _WIN32
};

#endif // TIMER_INCLUDED


#include "Time.h"

Time::Time() : 
	_secondsPerCount(0), _deltaTime(-1.), _baseTime(0), _stopTime(0),
	_pausedTime(0), _prevTime(0), _currentTime(0), _isStoped(false)
{
	uint64_t countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	_secondsPerCount = 1. / countsPerSec;
}

void Time::tick()
{
	if (_isStoped)
	{
		_deltaTime = 0.;
		return;
	}

	uint64_t currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	_currentTime = currentTime;

	_deltaTime = (currentTime - _prevTime) * _secondsPerCount;
	_prevTime = currentTime;

	if (_deltaTime < 0.)
		_deltaTime = 0.;
}

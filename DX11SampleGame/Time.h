#pragma once
#include "framework.h"

class Time
{
public:
	Time();

	float deltaTime() const { return _deltaTime; }
	float gameTime() const { return (float)(((_currentTime - _pausedTime) - _baseTime) * _secondsPerCount); }

	void reset() {}
	void start() {}
	void stop() {}
	void tick();
private:
	double _secondsPerCount;
	double _deltaTime;

	uint64_t _baseTime;
	uint64_t _pausedTime;
	uint64_t _stopTime;
	uint64_t _prevTime;
	uint64_t _currentTime;

	bool _isStoped;
};


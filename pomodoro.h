#pragma once
#include "globals.h"


class Pomodoro
{
public:

	Pomodoro();
	~Pomodoro() = default;


	void RenderPomodoroTimer();

private:
	bool		m_isPomodoroRunning;
	int			m_pomodoroMinutes;
	float		m_pomodoroTimeLeft;
	float		m_pomodoroMaxTime;
	std::chrono::steady_clock::time_point m_lastUpdateTime;
	std::chrono::duration<float> m_elapsed;

	int minutes;
	int seconds;
};


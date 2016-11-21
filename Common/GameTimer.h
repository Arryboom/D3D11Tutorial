/**********************************************************************
 @FILE		GameTimer.h
 @BRIEF		This file provide a game timer.
			The game timer is used to calculate the amount of time 
			that elapses between frames of animation.
 @AUTHOR	Ray1024
 @DATE		2016.11.16
 *********************************************************************/

#ifndef GAMETIMER_H
#define GAMETIMER_H

class GameTimer
{
public:
	GameTimer();

	float TotalTime()const; // ��λΪ��
	float DeltaTime()const;	// ��λΪ��

	void Reset();	// ��Ϣѭ��ǰ����
	void Start();	// ȡ����ͣʱ����
	void Stop();	// ��ͣʱ����
	void Tick();	// ÿ֡����

private:
	double m_secondsPerCount;
	double m_deltaTime;

	__int64 m_baseTime;
	__int64 m_pausedTime;
	__int64 m_stopTime;
	__int64 m_prevTime;
	__int64 m_currTime;

	bool m_stopped;
};

#endif // GAMETIMER_H
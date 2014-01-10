#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

using namespace std;

namespace util {

class PollableThread
{
private:
	queue<char> _queue;
	mutex _mutex;
	condition_variable _queue_cond_var;
	long _auto_poll_timeout;

	thread* _thread;

	int run();

protected:
	virtual void onInit();
	virtual void onPoll(bool timeout) = 0;
	void sendToThread(char c);

public:
	PollableThread(long auto_poll_timeout);
	virtual ~PollableThread();

	void start();
	void poll();
	void stop();
};

}

#pragma once

#include <sys/resource.h>
#include <sys/epoll.h>
#include <map>
#include <thread>

using namespace std;

namespace util {

class PollableThread
{
private:
	int _pipe_to_thread[2];
	long _auto_poll_timeout;

	int _epoll_fd;
	struct epoll_event _input_events;
	struct epoll_event _output_events;

	std::thread* _thread;

	int run();

protected:
	virtual void onPoll(bool timeout) = 0;
	void sendFromThread();

public:
	PollableThread(long auto_poll_timeout);
	virtual ~PollableThread();

	void start();
	void poll();
	void stop();
};

}

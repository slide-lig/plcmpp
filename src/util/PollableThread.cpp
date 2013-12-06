
#include <iostream>
#include <unistd.h>
#include <cstring>

#include "util/PollableThread.hpp"

using namespace std;

static char POLL_REQUEST = 'P';
static char INTERRUPT_REQUEST = 'I';

namespace util {

PollableThread::PollableThread(
		long auto_poll_timeout) {
	_thread = 0;
	_auto_poll_timeout = auto_poll_timeout;
	_epoll_fd = epoll_create(1);
}

PollableThread::~PollableThread() {
	if (_thread != 0)
		delete _thread;
}

int PollableThread::run() {
	int number_of_fds;

	// Close other end of pipe
	close(_pipe_to_thread[1]);

	memset(&_input_events, 0, sizeof(struct epoll_event));
	_input_events.events = EPOLLIN | EPOLLPRI | EPOLLERR ;
	_input_events.data.fd = _pipe_to_thread[0];
	epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _pipe_to_thread[0], &_input_events);

	while (1)
	{
		number_of_fds = epoll_wait(
				_epoll_fd, &_output_events, 1, _auto_poll_timeout);

		if (number_of_fds == 0)
		{	// no event, epoll_wait returned because
			// of the timeout.
			onPoll(true);
			continue;
		}

		if (number_of_fds < 0) {
			cerr << "epoll_wait error: " << strerror(errno) <<
					". Aborting." << endl;
			abort();
		}

		// 1 event
		if (_output_events.events & (EPOLLERR | EPOLLHUP))
		{
			cerr << "error on thread input pipe: " << strerror(errno) <<
								". Aborting." << endl;
			abort();
		}
		else
		{	// EPOLLIN or EPOLLPRI: OK
			// read the pipe
			char request;
			read(_pipe_to_thread[0], &request, 1);
			if (request == POLL_REQUEST)
			{
				onPoll(false);
			}
			else
			{ 	// INTERRUPT_REQUEST
				cout << "Thread got interruption request." << endl;
				break;
			}
		}
	}

	return 0;
}

void PollableThread::start() {
	if (pipe(_pipe_to_thread))
	{
		cerr << "Could not open the pipe for communicating with a PollableThread. " <<
				"Aborting." << endl;
		abort();
	}
	// start a thread
	_thread = new std::thread(&PollableThread::run, this);

	// Close other end of pipe
	close(_pipe_to_thread[0]);
}

void PollableThread::stop() {
	write(_pipe_to_thread[1], &INTERRUPT_REQUEST, 1);
	_thread->join();
	delete _thread;
	_thread = 0;
}

void PollableThread::poll() {
	write(_pipe_to_thread[1], &POLL_REQUEST, 1);
}

}


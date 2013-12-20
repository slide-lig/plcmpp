
#include <chrono>
#include <iostream>
#include <unistd.h>
#include <cstring>

#include "util/PollableThread.hpp"

using namespace std;
using std::chrono::system_clock;
using std::chrono::milliseconds;

static char POLL_REQUEST = 'P';
static char INTERRUPT_REQUEST = 'I';

namespace util {

PollableThread::PollableThread(
		long auto_poll_timeout) {
	_thread = 0;
	_auto_poll_timeout = auto_poll_timeout;
}

PollableThread::~PollableThread() {
	if (_thread != 0){
		cout << "Deleting thread." << endl;
		cout.flush();
		delete _thread;
	}
}

int PollableThread::run() {
	while (true)
	{
		char request;

		auto timeout_date = system_clock::now() + milliseconds(_auto_poll_timeout);

		{
			unique_lock<mutex> ul(_mutex);
			_queue_cond_var.wait_until(ul, timeout_date,
					[=] { return !_queue.empty();} );
			request = _queue.front();
			_queue.pop();
		}

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

	return 0;
}

void PollableThread::start() {
	// start a thread
	_thread = new thread(&PollableThread::run, this);
}

void PollableThread::sendToThread(char c) {
	{
		lock_guard<mutex> lg(_mutex);
		_queue.push(c);
	}
	_queue_cond_var.notify_one();
}

void PollableThread::stop() {
	sendToThread(INTERRUPT_REQUEST);
	_thread->join();
	delete _thread;
	_thread = 0;
}

void PollableThread::poll() {
	sendToThread(POLL_REQUEST);
}

}


#include "qctrlsignalhandler_unix.h"
#include <unistd.h>
#include <sys/socket.h>

//TODO error handling

int QCtrlSignalHandlerUnix::sockpair[2];

QCtrlSignalHandlerPrivate *QCtrlSignalHandlerPrivate::createInstance(QCtrlSignalHandler *q_ptr)
{
	return new QCtrlSignalHandlerUnix(q_ptr);
}

QCtrlSignalHandlerUnix::QCtrlSignalHandlerUnix(QCtrlSignalHandler *q_ptr) :
	QObject(q_ptr),
	QCtrlSignalHandlerPrivate(q_ptr),
	socketNotifier(nullptr)
{
	if(::socketpair(AF_UNIX, SOCK_STREAM, 0, sockpair) == 0) {
		socketNotifier = new QSocketNotifier(sockpair[1], QSocketNotifier::Read, this);
		QObject::connect(socketNotifier, &QSocketNotifier::activated,
						 this, &QCtrlSignalHandlerUnix::socketNotifyTriggerd);
		socketNotifier->setEnabled(true);
	} else
		qWarning("Failed to create socket pair");
}

bool QCtrlSignalHandlerUnix::setSignalHandlerEnabled(bool enabled)
{
	auto ok = true;
	foreach(auto signal, activeSignals) {
		if(!updateSignalHandler(signal, enabled))
			ok = false;
	}
	return ok;
}

bool QCtrlSignalHandlerUnix::registerSignal(int signal)
{
	if(enabled)
		return updateSignalHandler(signal, true);
	else
		return true;
}

bool QCtrlSignalHandlerUnix::unregisterSignal(int signal)
{
	if(enabled)
		return updateSignalHandler(signal, false);
	else
		return true;
}

void QCtrlSignalHandlerUnix::changeAutoShutMode(bool enabled)
{
}

QReadWriteLock *QCtrlSignalHandlerUnix::lock() const
{
	return nullptr;//no locks needed on unix
}

void QCtrlSignalHandlerUnix::socketNotifyTriggerd(int socket)
{
	int signal;
	::read(socket, &signal, sizeof(signal));

	if(!autoShut/* || !handleAutoShut(signal)*/)
		reportSignalTriggered(signal);
}

bool QCtrlSignalHandlerUnix::updateSignalHandler(int signal, bool active)
{
	struct sigaction action;
	action.sa_handler = active ? QCtrlSignalHandlerUnix::unixSignalHandler : SIG_DFL;
	::sigemptyset(&action.sa_mask);
	action.sa_flags |= SA_RESTART;
	if(::sigaction(signal, &action, NULL))
		return false;
	else
		return true;
}

void QCtrlSignalHandlerUnix::unixSignalHandler(int signal)
{
	::write(sockpair[0], &signal, sizeof(int));
}

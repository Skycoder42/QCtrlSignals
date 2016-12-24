#include "qctrlsignalhandler_unix.h"
#include <QCoreApplication>
#include <unistd.h>
#include <sys/socket.h>

int QCtrlSignalHandlerUnix::sockpair[2];
QVector<int> QCtrlSignalHandlerUnix::shutSignals = {SIGINT, SIGTERM, SIGQUIT, SIGHUP};

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
		qCWarning(logQCtrlSignalHandler) << "Failed to create socket pair with error:" << ::strerror(errno);
}

bool QCtrlSignalHandlerUnix::registerSignal(int signal)
{
	if(isAutoShutRegistered(signal))
		return true;
	else
		return updateSignalHandler(signal, true);
}

bool QCtrlSignalHandlerUnix::unregisterSignal(int signal)
{
	if(isAutoShutRegistered(signal))
		return true;
	else
		return updateSignalHandler(signal, false);
}

void QCtrlSignalHandlerUnix::changeAutoShutMode(bool enabled)
{
	foreach(auto sig, shutSignals) {
		if(!activeSignals.contains(sig))
			updateSignalHandler(sig, enabled);
	}
}

QReadWriteLock *QCtrlSignalHandlerUnix::lock() const
{
	return nullptr;//no locks needed on unix
}

void QCtrlSignalHandlerUnix::socketNotifyTriggerd(int socket)
{
	int signal;
	if(::read(socket, &signal, sizeof(int)) == sizeof(int)) {
		if(!reportSignalTriggered(signal) &&
		   isAutoShutRegistered(signal))
			qApp->quit();
	} else
		qCWarning(logQCtrlSignalHandler) << "Failed to read signal from socket pair";
}

bool QCtrlSignalHandlerUnix::isAutoShutRegistered(int signal) const
{
	if(autoShut)
		return shutSignals.contains(signal);
	else
		return false;
}

bool QCtrlSignalHandlerUnix::updateSignalHandler(int signal, bool active)
{
	struct sigaction action;
	action.sa_handler = active ? QCtrlSignalHandlerUnix::unixSignalHandler : SIG_DFL;
	::sigemptyset(&action.sa_mask);
	action.sa_flags |= SA_RESTART;
	if(::sigaction(signal, &action, NULL) == 0)
		return true;
	else {
		qCWarning(logQCtrlSignalHandler) << "Failed to"
										 << (active ? "register" : "unregister")
										 << "signal with error:"
										 << ::strerror(errno);
		return false;
	}
}

void QCtrlSignalHandlerUnix::unixSignalHandler(int signal)
{
	auto wr = ::write(sockpair[0], &signal, sizeof(int));
	Q_UNUSED(wr);
}

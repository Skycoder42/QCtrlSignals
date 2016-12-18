#include "qctrlsignalhandler_win.h"

#include <QCoreApplication>
#include <QSemaphore>
#include <QThread>

//TODO error handling

namespace {
static QSemaphore shutdownLock;
}

QCtrlSignalHandlerPrivate *QCtrlSignalHandlerPrivate::createInstance(QCtrlSignalHandler *q_ptr)
{
	return new QCtrlSignalHandlerWin(q_ptr);
}

QCtrlSignalHandlerWin::QCtrlSignalHandlerWin(QCtrlSignalHandler *q_ptr) :
	QCtrlSignalHandlerPrivate(q_ptr),
	rwLock(QReadWriteLock::Recursive)
{}

bool QCtrlSignalHandlerWin::setSignalHandlerEnabled(bool enabled)
{
	return ::SetConsoleCtrlHandler(HandlerRoutine, enabled);
}

bool QCtrlSignalHandlerWin::registerSignal(int)
{
	return true;
}

bool QCtrlSignalHandlerWin::unregisterSignal(int)
{
	return true;
}

void QCtrlSignalHandlerWin::changeAutoShutMode(bool) {}

QReadWriteLock *QCtrlSignalHandlerWin::lock() const
{
	return &rwLock;
}

bool QCtrlSignalHandlerWin::handleAutoShut(DWORD signal)
{
	switch (signal) {
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
		QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
		return true;
	case CTRL_CLOSE_EVENT:
		QObject::connect(qApp, &QCoreApplication::destroyed, qApp, [&](){//will be executed in the main thread
			shutdownLock.release();
			QThread::msleep(250);//give the killer thread time to exit the process
		}, Qt::DirectConnection);
		QMetaObject::invokeMethod(qApp, "quit", Qt::BlockingQueuedConnection);
		shutdownLock.acquire();
		::ExitProcess(EXIT_SUCCESS);
		return true;
	default:
		return false;
	}
}

BOOL QCtrlSignalHandlerWin::HandlerRoutine(DWORD dwCtrlType)
{
	auto self = p_instance<QCtrlSignalHandlerWin>();
	QReadLocker lock(self->lock());
	if(self->autoShut && self->handleAutoShut(dwCtrlType))
		return TRUE;
	else if(self->activeSignals.contains((int)dwCtrlType) &&
			self->reportSignalTriggered((int)dwCtrlType))
		return TRUE;
	else
		return FALSE;
}

#include "qctrlsignalhandler.h"
#include "qctrlsignalhandler_p.h"

#include <QCoreApplication>
#include <QSemaphore>
#include <QThread>

namespace {
static QSemaphore shutdownLock;
}

bool QCtrlSignalHandlerPrivate::registerHandler()
{
	return ::SetConsoleCtrlHandler(HandlerRoutine, TRUE);
}

bool QCtrlSignalHandlerPrivate::unregisterHandler()
{
	return ::SetConsoleCtrlHandler(HandlerRoutine, FALSE);
}

bool QCtrlSignalHandlerPrivate::handleAutoShut(int signal)
{
	switch (signal) {
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
		QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
		return true;
	case CTRL_CLOSE_EVENT:
		QObject::connect(qApp, &QCoreApplication::destroyed, qApp, [&](){
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

BOOL QCtrlSignalHandlerPrivate::HandlerRoutine(DWORD dwCtrlType)
{
	auto self = p_instance();
	if(self->reportSignalTriggered(dwCtrlType))
		return TRUE;
	else
		return FALSE;
}

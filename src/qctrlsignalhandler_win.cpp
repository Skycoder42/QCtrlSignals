#include "qctrlsignalhandler_win.h"

#include <QCoreApplication>
#include <QSemaphore>
#include <QThread>
#include <QDebug>

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
{
	if(!::SetConsoleCtrlHandler(HandlerRoutine, true)) {
		qCWarning(logQCtrlSignals).noquote()
				<< "Failed to create signal handler with error:"
				<< lastErrorMessage();
	}
}

QCtrlSignalHandlerWin::~QCtrlSignalHandlerWin()
{
	if(!::SetConsoleCtrlHandler(HandlerRoutine, false)) {
		qCWarning(logQCtrlSignals).noquote()
				<< "Failed to remove signal handler with error:"
				<< lastErrorMessage();
	}
}

bool QCtrlSignalHandlerWin::registerSignal(int signal)
{
	switch (signal) {
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
		return true;
	default://TODO logoff and shutdown events --> are they possible?
		return false;
	}
}

bool QCtrlSignalHandlerWin::unregisterSignal(int)
{
	return true;
}

void QCtrlSignalHandlerWin::changeAutoQuittMode(bool) {}

QReadWriteLock *QCtrlSignalHandlerWin::lock() const
{
	return &rwLock;
}

bool QCtrlSignalHandlerWin::handleAutoQuit(DWORD signal)
{
	if(!autoQuit)
		return false;

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
	if(self->reportSignalTriggered((int)dwCtrlType))
		return TRUE;
	else
		return self->handleAutoQuit(dwCtrlType);
}

QString QCtrlSignalHandlerWin::lastErrorMessage()
{
	LPWSTR bufPtr = NULL;
	auto err = GetLastError();
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				   FORMAT_MESSAGE_FROM_SYSTEM |
				   FORMAT_MESSAGE_IGNORE_INSERTS,
				   NULL, err, 0, (LPWSTR)&bufPtr, 0, NULL);
	const QString result = bufPtr ?
							   QString::fromUtf16((const ushort*)bufPtr).trimmed() :
							   QString("Unknown Error %1").arg(err);
	LocalFree(bufPtr);
	return result;
}

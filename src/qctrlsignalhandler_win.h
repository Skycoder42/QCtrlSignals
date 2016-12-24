#ifndef QCTRLSIGNALHANDLER_WIN_H
#define QCTRLSIGNALHANDLER_WIN_H

#include "qctrlsignalhandler_p.h"

#include <qt_windows.h>

class QCtrlSignalHandlerWin : public QCtrlSignalHandlerPrivate
{
public:
	QCtrlSignalHandlerWin(QCtrlSignalHandler *q_ptr);
	~QCtrlSignalHandlerWin();

	bool registerSignal(int signal) override;
	bool unregisterSignal(int signal) override;
	void changeAutoQuittMode(bool) override;

	QReadWriteLock *lock() const override;

private:
	mutable QReadWriteLock rwLock;

	bool handleAutoQuit(DWORD signal);

	static BOOL WINAPI HandlerRoutine(_In_ DWORD dwCtrlType);
	static QString lastErrorMessage();
};

#endif // QCTRLSIGNALHANDLER_WIN_H

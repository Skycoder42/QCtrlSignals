#ifndef QCTRLSIGNALHANDLERPRIVATE_H
#define QCTRLSIGNALHANDLERPRIVATE_H

#include "qctrlsignalhandler.h"
#include <QHash>
#include <QSet>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

class QCtrlSignalHandlerPrivate
{
public:
	QCtrlSignalHandlerPrivate(QCtrlSignalHandler *q_ptr);

	QSet<int> activeSignals;
	QHash<int, std::function<bool(int)>> callbacks;

	bool registerHandler();
	bool unregisterHandler();

private:
	static QCtrlSignalHandlerPrivate *p_instance();

	QCtrlSignalHandler *q_ptr;

	bool reportSignalTriggered(int signal);

#ifdef Q_OS_WIN
	static BOOL WINAPI HandlerRoutine(_In_ DWORD dwCtrlType);
#endif
};

#endif // QCTRLSIGNALHANDLERPRIVATE_H

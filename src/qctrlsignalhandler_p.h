#ifndef QCTRLSIGNALHANDLERPRIVATE_H
#define QCTRLSIGNALHANDLERPRIVATE_H

#include "qctrlsignalhandler.h"
#include <QHash>
#include <QReadWriteLock>
#include <QSet>

class QCtrlSignalHandlerPrivate
{
public:
	static QCtrlSignalHandlerPrivate *createInstance(QCtrlSignalHandler *q_ptr);

	virtual ~QCtrlSignalHandlerPrivate();

	virtual bool setSignalHandlerEnabled(bool enabled) = 0;
	virtual bool registerSignal(int signal) = 0;
	virtual bool unregisterSignal(int signal) = 0;

	virtual void changeAutoShutMode(bool enabled) = 0;

	bool enabled;
	QSet<int> activeSignals;
	bool autoShut;

	virtual QReadWriteLock *lock() const = 0;

protected:
	QCtrlSignalHandlerPrivate(QCtrlSignalHandler *q_ptr);

	template <typename T>
	static T *p_instance() {
		return static_cast<T*>(QCtrlSignalHandler::instance()->d_ptr.data());
	}

	bool reportSignalTriggered(int signal);

	QCtrlSignalHandler *q_ptr;
};

#endif // QCTRLSIGNALHANDLERPRIVATE_H

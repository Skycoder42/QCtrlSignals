#include "qctrlsignalhandler.h"
#include "qctrlsignalhandler_p.h"
#include <QGlobalStatic>
#include <qdebug.h>

Q_LOGGING_CATEGORY(logQCtrlSignalHandler, "QCtrlSignalHandler")

class QCtrlSignalHandlerInstance : public QCtrlSignalHandler {
public:
	inline QCtrlSignalHandlerInstance() :
		QCtrlSignalHandler()
	{}
};

Q_GLOBAL_STATIC(QCtrlSignalHandlerInstance, handler)

QCtrlSignalHandler::QCtrlSignalHandler() :
	QObject(),
	d_ptr(QCtrlSignalHandlerPrivate::createInstance(this))
{}

QCtrlSignalHandler::~QCtrlSignalHandler() {}

QCtrlSignalHandler *QCtrlSignalHandler::instance()
{
	return handler;
}

bool QCtrlSignalHandler::registerForSignal(int signal)
{
	QWriteLocker lock(d_ptr->lock());
	if(!d_ptr->activeSignals.contains(signal)) {
		if(d_ptr->registerSignal(signal)) {
			d_ptr->activeSignals.insert(signal);
			return true;
		} else
			return false;
	} else
		return true;
}

bool QCtrlSignalHandler::unregisterFromSignal(int signal)
{
	QWriteLocker lock(d_ptr->lock());
	if(d_ptr->activeSignals.contains(signal)) {
		if(d_ptr->unregisterSignal(signal)) {
			d_ptr->activeSignals.remove(signal);
			return true;
		} else
			return false;
	} else
		return true;
}

bool QCtrlSignalHandler::isAutoShutActive() const
{
	QReadLocker lock(d_ptr->lock());
	return d_ptr->autoShut;
}

void QCtrlSignalHandler::setAutoShutActive(bool autoShutActive)
{
	QWriteLocker lock(d_ptr->lock());
	if (d_ptr->autoShut == autoShutActive)
		return;

	d_ptr->autoShut = autoShutActive;
	emit autoShutActiveChanged(autoShutActive);
}



QCtrlSignalHandlerPrivate::QCtrlSignalHandlerPrivate(QCtrlSignalHandler *q_ptr) :
	activeSignals(),
	autoShut(false),
	q_ptr(q_ptr)
{}

QCtrlSignalHandlerPrivate::~QCtrlSignalHandlerPrivate() {}

bool QCtrlSignalHandlerPrivate::reportSignalTriggered(int signal)
{
	//Not locked, because this method is called by the signal handler, which should do the lock, if required
	if(signal == QCtrlSignalHandler::SigInt)
		return QMetaObject::invokeMethod(q_ptr, "sigInt", Qt::QueuedConnection);
	else if(signal == QCtrlSignalHandler::SigTerm)
		return QMetaObject::invokeMethod(q_ptr, "sigTerm", Qt::QueuedConnection);
	else {
		return QMetaObject::invokeMethod(q_ptr, "ctrlSignal", Qt::QueuedConnection,
										 Q_ARG(int, signal));
	}
}

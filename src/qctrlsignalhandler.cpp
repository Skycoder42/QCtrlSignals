#include "qctrlsignalhandler.h"
#include "qctrlsignalhandler_p.h"
#include <QGlobalStatic>

class QCtrlSignalHandlerInstance : public QCtrlSignalHandler {
public:
	inline QCtrlSignalHandlerInstance() :
		QCtrlSignalHandler()
	{}
};

Q_GLOBAL_STATIC(QCtrlSignalHandlerInstance, handler)

QCtrlSignalHandler::QCtrlSignalHandler() :
	QObject(),
	d_ptr(new QCtrlSignalHandlerPrivate(this))
{}

QCtrlSignalHandler *QCtrlSignalHandler::instance()
{
	return handler;
}

void QCtrlSignalHandler::registerSynchronousSignalHandler(int signal, std::function<bool ()> handler)
{
	d_ptr->callbacks.insert(signal, [=](int){return handler();});
}

void QCtrlSignalHandler::registerSynchronousSignalHandler(int signal, std::function<bool (int)> handler)
{
	d_ptr->callbacks.insert(signal, handler);
}

void QCtrlSignalHandler::unregisterSynchronousSignalHandler(int signal)
{
	d_ptr->callbacks.remove(signal);
}

void QCtrlSignalHandler::enableAsyncSignal(int signal)
{
	d_ptr->activeSignals.insert(signal);
}

void QCtrlSignalHandler::disableAsyncSignal(int signal)
{
	d_ptr->activeSignals.remove(signal);
}

bool QCtrlSignalHandler::isEnabled() const
{
	return enabled;
}

bool QCtrlSignalHandler::setEnabled(bool enabled)
{
	if (this->enabled == enabled)
		return true;

	if(enabled) {
		if(!d_ptr->registerHandler())
			return false;
	} else {
		if(!d_ptr->unregisterHandler())
			return false;
	}
	this->enabled = enabled;
	emit enabledChanged(enabled);
	return true;
}



QCtrlSignalHandlerPrivate::QCtrlSignalHandlerPrivate(QCtrlSignalHandler *q_ptr) :
	q_ptr(q_ptr)
{}

QCtrlSignalHandlerPrivate *QCtrlSignalHandlerPrivate::p_instance()
{
	return handler->d_ptr.data();
}

bool QCtrlSignalHandlerPrivate::reportSignalTriggered(int signal)
{
	auto handler = callbacks.value(signal);
	if(!handler || !handler(signal)) {
		if(activeSignals.contains(signal)) {
			if(signal == QCtrlSignalHandler::SigInt)
				return QMetaObject::invokeMethod(q_ptr, "sigInt", Qt::QueuedConnection);
			else if(signal == QCtrlSignalHandler::SigTerm)
				return QMetaObject::invokeMethod(q_ptr, "sigTerm", Qt::QueuedConnection);
			else {
				return QMetaObject::invokeMethod(q_ptr, "ctrlSignal", Qt::QueuedConnection,
												 Q_ARG(int, signal));
			}
		} else
			return false;
	} else//handler handelt it
		return true;
}

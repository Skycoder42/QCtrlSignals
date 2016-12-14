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
	d_ptr(QCtrlSignalHandlerPrivate::createInstance(this))
{}

QCtrlSignalHandler *QCtrlSignalHandler::instance()
{
	return handler;
}

bool QCtrlSignalHandler::registerSynchronousSignalHandler(int signal, std::function<bool ()> handler, bool registerSignal)
{
	return registerSynchronousSignalHandler(signal, [=](int){return handler();}, registerSignal);
}

bool QCtrlSignalHandler::registerSynchronousSignalHandler(int signal, std::function<bool (int)> handler, bool registerSignal)
{
	d_ptr->callbacks.insert(signal, handler);
	if(registerSignal)
		return registerForSignal(signal);
	else
		return true;
}

bool QCtrlSignalHandler::unregisterSynchronousSignalHandler(int signal, bool unregisterSignal)
{
	d_ptr->callbacks.remove(signal);
	if(unregisterSignal)
		return unregisterFromSignal(signal);
	else
		return true;
}

bool QCtrlSignalHandler::registerForSignal(int signal)
{
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
	if(d_ptr->activeSignals.contains(signal)) {
		if(d_ptr->unregisterSignal(signal)) {
			d_ptr->activeSignals.remove(signal);
			return true;
		} else
			return false;
	} else
		return true;
}

bool QCtrlSignalHandler::isEnabled() const
{
	return d_ptr->enabled;
}

bool QCtrlSignalHandler::isAutoShutActive() const
{
	return d_ptr->autoShut;
}

bool QCtrlSignalHandler::setEnabled(bool enabled)
{
	if (d_ptr->enabled == enabled)
		return true;

	if(!d_ptr->setSignalHandlerEnabled(enabled))
		return false;
	d_ptr->enabled = enabled;
	emit enabledChanged(enabled);
	return true;
}

void QCtrlSignalHandler::setAutoShutActive(bool autoShutActive)
{
	if (d_ptr->autoShut == autoShutActive)
		return;

	d_ptr->autoShut = autoShutActive;
	emit autoShutActiveChanged(autoShutActive);
}



QCtrlSignalHandlerPrivate::QCtrlSignalHandlerPrivate(QCtrlSignalHandler *q_ptr) :
	q_ptr(q_ptr),
	enabled(false),
	activeSignals(),
	callbacks(),
	autoShut(false)
{}

bool QCtrlSignalHandlerPrivate::reportSignalTriggered(int signal)
{
	auto handler = callbacks.value(signal);
	if(!handler || !handler(signal)) {
		if(signal == QCtrlSignalHandler::SigInt)
			return QMetaObject::invokeMethod(q_ptr, "sigInt", Qt::QueuedConnection);
		else if(signal == QCtrlSignalHandler::SigTerm)
			return QMetaObject::invokeMethod(q_ptr, "sigTerm", Qt::QueuedConnection);
		else {
			return QMetaObject::invokeMethod(q_ptr, "ctrlSignal", Qt::QueuedConnection,
											 Q_ARG(int, signal));
		}
	} else//handler handelt it
		return true;
}

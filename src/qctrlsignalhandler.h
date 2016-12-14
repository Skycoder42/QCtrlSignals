#ifndef QCTRLSIGNALHANDLER_H
#define QCTRLSIGNALHANDLER_H

#include <QObject>
#include <QScopedPointer>

#include <functional>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

class QCtrlSignalHandlerPrivate;
class QCtrlSignalHandler : public QObject
{
	Q_OBJECT
	friend class QCtrlSignalHandlerPrivate;
	friend class QCtrlSignalHandlerInstance;

	Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)

public:
	enum CommonSignals {
#ifdef Q_OS_WIN
		SigInt = CTRL_C_EVENT,
		SigTerm = CTRL_BREAK_EVENT
#endif
	};

	static QCtrlSignalHandler *instance();

	void registerSynchronousSignalHandler(int signal, std::function<bool()> handler);
	void registerSynchronousSignalHandler(int signal, std::function<bool(int)> handler);
	void unregisterSynchronousSignalHandler(int signal);

	void enableAsyncSignal(int signal);
	void disableAsyncSignal(int signal);

	bool isEnabled() const;

public slots:
	bool setEnabled(bool enabled);

signals:
	//common, asynchrounus signals
	void sigInt();
	void sigTerm();

	//other signals
	void ctrlSignal(int signal);

	void enabledChanged(bool enabled);

private:
	QScopedPointer<QCtrlSignalHandlerPrivate> d_ptr;

	explicit QCtrlSignalHandler();
	bool enabled;
};

#endif // QCTRLSIGNALHANDLER_H

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
	Q_PROPERTY(bool autoShutActive READ isAutoShutActive WRITE setAutoShutActive NOTIFY autoShutActiveChanged)

public:
	enum CommonSignals {
#ifdef Q_OS_WIN
		SigInt = CTRL_C_EVENT,
		SigTerm = CTRL_BREAK_EVENT
#endif
	};

	static QCtrlSignalHandler *instance();

	bool registerSynchronousSignalHandler(int signal, std::function<bool()> handler, bool registerSignal = true);
	bool registerSynchronousSignalHandler(int signal, std::function<bool(int)> handler, bool registerSignal = true);
	bool unregisterSynchronousSignalHandler(int signal, bool unregisterSignal = true);

	bool registerForSignal(int signal);
	bool unregisterFromSignal(int signal);

	bool isEnabled() const;	
	bool isAutoShutActive() const;

public slots:
	bool setEnabled(bool enabled);	
	void setAutoShutActive(bool autoShutActive);

signals:
	//common, asynchrounus signals
	void sigInt();
	void sigTerm();

	//other signals
	void ctrlSignal(int signal);

	void enabledChanged(bool enabled);	
	void autoShutActiveChanged(bool autoShutActive);

private:
	QScopedPointer<QCtrlSignalHandlerPrivate> d_ptr;

	explicit QCtrlSignalHandler();
};

#endif // QCTRLSIGNALHANDLER_H

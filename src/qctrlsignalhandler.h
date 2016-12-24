#ifndef QCTRLSIGNALHANDLER_H
#define QCTRLSIGNALHANDLER_H

#include <QObject>
#include <QScopedPointer>
#include <QLoggingCategory>

#include <functional>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#else
#include <signal.h>
#endif

class QCtrlSignalHandlerPrivate;
class QCtrlSignalHandler : public QObject
{
	Q_OBJECT
	friend class QCtrlSignalHandlerPrivate;
	friend class QCtrlSignalHandlerInstance;

	Q_PROPERTY(bool autoShutActive READ isAutoShutActive WRITE setAutoShutActive NOTIFY autoShutActiveChanged)

public:
	enum CommonSignals {
#ifdef Q_OS_WIN
		SigInt = CTRL_C_EVENT,
		SigTerm = CTRL_BREAK_EVENT
#else
		SigInt = SIGINT,
		SigTerm = SIGTERM
#endif
	};

	static QCtrlSignalHandler *instance();

	bool registerForSignal(int signal);
	bool unregisterFromSignal(int signal);

	bool isAutoShutActive() const;

public slots:
	void setAutoShutActive(bool autoShutActive);

signals:
	//common, asynchrounus signals
	void sigInt();
	void sigTerm();

	//other signals
	void ctrlSignal(int signal);

	void autoShutActiveChanged(bool autoShutActive);

private:
	QScopedPointer<QCtrlSignalHandlerPrivate> d_ptr;

	explicit QCtrlSignalHandler();
	~QCtrlSignalHandler();
};

Q_DECLARE_LOGGING_CATEGORY(logQCtrlSignalHandler)

#endif // QCTRLSIGNALHANDLER_H

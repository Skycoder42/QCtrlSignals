#ifndef QCTRLSIGNALHANDLER_H
#define QCTRLSIGNALHANDLER_H

#include <QObject>

#include <functional>

class QCtrlSignalHandler : public QObject
{
	Q_OBJECT

public:
	explicit QCtrlSignalHandler(QObject *parent = nullptr);

	void registerSynchronousSignalHandler(int signal, std::function<void()> handler);
	void unregisterSynchronousSignalHandler(int signal);

signals:
	//common, asynchrounus signals
	void sigInt();
	void sigTerm();

	//other signals
	void asyncCtrlSignal(int signal);
};

#endif // QCTRLSIGNALHANDLER_H

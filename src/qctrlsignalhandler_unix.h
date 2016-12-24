#ifndef QCTRLSIGNALHANDLER_UNIX_H
#define QCTRLSIGNALHANDLER_UNIX_H

#include "qctrlsignalhandler_p.h"

#include <QSocketNotifier>
#include <QObject>

class QCtrlSignalHandlerUnix : public QObject, public QCtrlSignalHandlerPrivate
{
	Q_OBJECT

public:
	QCtrlSignalHandlerUnix(QCtrlSignalHandler *q_ptr);

	bool registerSignal(int signal) override;
	bool unregisterSignal(int signal) override;
	void changeAutoQuittMode(bool enabled) override;
	QReadWriteLock *lock() const override;

private slots:
	void socketNotifyTriggerd(int socket);

private:
	QSocketNotifier *socketNotifier;

	bool isAutoQuitRegistered(int signal) const;
	bool updateSignalHandler(int signal, bool active);

	static int sockpair[2];
	static QVector<int> shutSignals;

	static void unixSignalHandler(int signal);
};

#endif // QCTRLSIGNALHANDLER_UNIX_H

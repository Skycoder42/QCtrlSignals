#include <QCoreApplication>
#include <QCtrlSignals>
#include <QDebug>
#include <QThread>

#define CUSTOM_HANDLERS 0
#define AUTO_SHUTDOWN 1
#define AUTO_SHUTDOWN_OVERWRITE 2

#define MODE AUTO_SHUTDOWN_OVERWRITE

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	auto handler = QCtrlSignalHandler::instance();
#if MODE == CUSTOM_HANDLERS
	qDebug() << "SigInt reg" << handler->registerForSignal(QCtrlSignalHandler::SigInt);
	qDebug() << "SigTerm reg" << handler->registerForSignal(QCtrlSignalHandler::SigTerm);

	QObject::connect(handler, &QCtrlSignalHandler::sigInt, qApp, [](){
		qDebug() << "SigInt  triggered";
	});
	QObject::connect(handler, &QCtrlSignalHandler::sigTerm, qApp, [](){
		qDebug() << "SigTerm triggered";
	});

#ifdef Q_OS_UNIX
	qDebug() << "SIGQUIT(3) reg" << handler->registerForSignal(SIGQUIT);
	QObject::connect(handler, &QCtrlSignalHandler::ctrlSignal, qApp, [](int signal){
		qDebug() << "SIGNAL" << signal << "triggered";
	});
#endif
#elif MODE >= AUTO_SHUTDOWN
	QObject::connect(qApp, &QCoreApplication::aboutToQuit, qApp, [](){
		qDebug() << "App about to quit!";
		QThread::sleep(1);
	}, Qt::DirectConnection);
	handler->setAutoShutActive(true);

#if MODE == AUTO_SHUTDOWN_OVERWRITE
	qDebug() << "SigInt reg" << handler->registerForSignal(QCtrlSignalHandler::SigInt);
	QObject::connect(handler, &QCtrlSignalHandler::sigInt, qApp, [](){
		qDebug() << "SigInt  triggered, no auto shutdown anymore!";
	});
#endif
#endif

	return a.exec();
}

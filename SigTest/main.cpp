#include <QCoreApplication>
#include <QCtrlSignals>
#include <QDebug>
#include <QThread>

#define CUSTOM_HANDLERS 0
#define AUTO_SHUTDOWN 1
#define MODE CUSTOM_HANDLERS

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	auto handler = QCtrlSignalHandler::instance();
#if MODE == CUSTOM_HANDLERS
	qDebug() << "sigint reg" << handler->registerForSignal(QCtrlSignalHandler::SigInt);
	qDebug() << "sigterm reg" << handler->registerForSignal(QCtrlSignalHandler::SigTerm);

	QObject::connect(handler, &QCtrlSignalHandler::sigInt, qApp, [](){
		qDebug() << "SIGINT";
		qDebug() << qApp->thread() << QThread::currentThread();
	});
	QObject::connect(handler, &QCtrlSignalHandler::sigTerm, qApp, [](){
		qDebug() << "SIGTERM";
		qDebug() << qApp->thread() << QThread::currentThread();
	});

#ifdef Q_OS_WIN
	//cannot handle CTRL_CLOSE_EVENT asynchronously
#else
	qDebug() << "SIGQUIT reg" << handler->registerForSignal(SIGQUIT);
	QObject::connect(handler, &QCtrlSignalHandler::ctrlSignal, qApp, [](int signal){
		qDebug() << "SIGNAL" << signal << "(3 -> SIGQUIT)";
	});

	//cannot handle SIGHUP asynchronously
#endif
#elif MODE == AUTO_SHUTDOWN
	QObject::connect(qApp, &QCoreApplication::aboutToQuit, qApp, [](){
		qDebug() << "App about to quit!";
		QThread::sleep(1);
	}, Qt::DirectConnection);
	handler->setAutoShutActive(true);
#endif

	qDebug() << "enable" << handler->setEnabled(true);

	return a.exec();
}

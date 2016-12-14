#include <QCoreApplication>
#include <QCtrlSignals>
#include <QDebug>
#include <QThread>

#define CUSTOM_HANDLERS 0
#define AUTO_SHUTDOWN 1
#define MODE AUTO_SHUTDOWN

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	auto handler = QCtrlSignalHandler::instance();
#if MODE == CUSTOM_HANDLERS
	handler->registerSynchronousSignalHandler(CTRL_CLOSE_EVENT, [](){
		qDebug() << "CTRL_CLOSE_EVENT";
		qDebug() << qApp->thread() << QThread::currentThread();
		QThread::sleep(2);
		return true;
	});
	handler->registerForSignal(QCtrlSignalHandler::SigInt);
	handler->registerForSignal(QCtrlSignalHandler::SigTerm);

	QObject::connect(handler, &QCtrlSignalHandler::sigInt, qApp, [](){
		qDebug() << "SIGINT";
		qDebug() << qApp->thread() << QThread::currentThread();
	});
	QObject::connect(handler, &QCtrlSignalHandler::sigTerm, qApp, [](){
		qDebug() << "SIGTERM";
		qDebug() << qApp->thread() << QThread::currentThread();
	});
#elif MODE == AUTO_SHUTDOWN
	QObject::connect(qApp, &QCoreApplication::aboutToQuit, qApp, [](){
		qDebug() << "App about to quit!";
		QThread::sleep(1);
	}, Qt::DirectConnection);
	handler->setAutoShutActive(true);
#endif

	handler->setEnabled(true);

	return a.exec();
}

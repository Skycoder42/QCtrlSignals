#include <QCoreApplication>
#include <QCtrlSignals>
#include <QDebug>
#include <QThread>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	auto handler = QCtrlSignalHandler::instance();
	handler->registerSynchronousSignalHandler(CTRL_CLOSE_EVENT, [](){
		qDebug() << "CTRL_CLOSE_EVENT";
		qDebug() << qApp->thread() << QThread::currentThread();
		QThread::sleep(2);
		return true;
	});
	handler->enableAsyncSignal(QCtrlSignalHandler::SigInt);
	handler->enableAsyncSignal(QCtrlSignalHandler::SigTerm);

	QObject::connect(handler, &QCtrlSignalHandler::sigInt, qApp, [](){
		qDebug() << "SIGINT";
		qDebug() << qApp->thread() << QThread::currentThread();
	});
	QObject::connect(handler, &QCtrlSignalHandler::sigTerm, qApp, [](){
		qDebug() << "SIGTERM";
		qDebug() << qApp->thread() << QThread::currentThread();
	});

	handler->setEnabled(true);

	return a.exec();
}

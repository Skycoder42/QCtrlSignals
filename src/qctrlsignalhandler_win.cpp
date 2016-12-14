#include "qctrlsignalhandler.h"
#include "qctrlsignalhandler_p.h"

bool QCtrlSignalHandlerPrivate::registerHandler()
{
	return ::SetConsoleCtrlHandler(HandlerRoutine, TRUE);
}

bool QCtrlSignalHandlerPrivate::unregisterHandler()
{
	return ::SetConsoleCtrlHandler(HandlerRoutine, FALSE);
}

BOOL QCtrlSignalHandlerPrivate::HandlerRoutine(DWORD dwCtrlType)
{
	auto self = p_instance();
	if(self->reportSignalTriggered(dwCtrlType))
		return TRUE;
	else
		return FALSE;
}

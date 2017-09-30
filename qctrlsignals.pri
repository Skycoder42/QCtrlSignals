PUBLIC_HEADERS += $$PWD/src/QCtrlSignals \
	$$PWD/src/qctrlsignalhandler.h

HEADERS += $$PUBLIC_HEADERS \
	$$PWD/src/qctrlsignalhandler_p.h

SOURCES += \
	$$PWD/src/qctrlsignalhandler.cpp

win32 {
	HEADERS += $$PWD/src/qctrlsignalhandler_win.h
	SOURCES += $$PWD/src/qctrlsignalhandler_win.cpp
}

unix {
	HEADERS += $$PWD/src/qctrlsignalhandler_unix.h
	SOURCES += $$PWD/src/qctrlsignalhandler_unix.cpp
}

INCLUDEPATH += $$PWD/src

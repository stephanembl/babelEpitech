#-------------------------------------------------
#
# Project created by QtCreator 2013-12-03T14:40:22
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

Release:DESTDIR = bin
Debug:DESTDIR = bin
TARGET = Babibel
TEMPLATE = app

OBJECTS_DIR = obj
MOC_DIR = moc
UI_DIR = ui_make
RCC_DIR = rcc_make

win32:LIBS += -L./lib -lportaudio -L../lib -lopus -lcelt -lsilk_common -lsilk_float
unix:LIBS += -lportaudio -L./lib -lopus

SOURCES += src/main.cpp\
src/view/babibelwindow.cpp \
src/view/conversation.cpp \
src/view/login.cpp \
src/model/contact.cpp \
src/controller/controller.cpp \
src/model/core.cpp \
src/controller/AClientTCP.cpp \
src/controller/QClientTCP.cpp \
src/controller/Network.cpp \
../sources-shared/shared/Packet.cpp \
../sources-shared/shared/Exception.cpp \
../sources-shared/shared/PacketFactory.cpp \
../sources-shared/shared/CircularBuffer.cpp \
src/controller/AClientUDP.cpp \
src/controller/QClientUDP.cpp \
../sources-shared/shared/LibC.cpp \
src/audio/AudioManager.cpp \
src/audio/EncodeManager.cpp \
../sources-shared/shared/Mutex.cpp \
../sources-shared/shared/MutexPool.cpp

win32: SOURCES += ../sources-shared/windows/CWMutex.cpp
unix: SOURCES += ../sources-shared/linux/CUMutex.cpp

HEADERS  += include/view/babibelwindow.h \
include/view/ConcactListItem.h \
include/view/conversation.h \
include/view/IMainWindow.h \
include/Enum.h \
include/view/login.h \
include/view/MyApplication.h \
include/model/contact.h \
include/controller/controller.h \
include/model/core.h \
include/controller/AClientTCP.h \
include/controller/QClientTCP.h \
include/controller/Network.hpp \
include/audio/audiosettings.h \
include/audio/opus.h \
include/audio/opus_custom.h \
include/audio/opus_defines.h \
include/audio/opus_multistream.h \
include/audio/opus_types.h \
include/audio/portaudio.h \
include/audio/AbsAudio.hpp \
include/audio/AbsEncode.hpp \
include/audio/AudioManager.hpp \
include/audio/EncodeManager.hpp \
../headers-shared/shared/Packet.h \
../headers-shared/shared/PacketFactory.h \
../headers-shared/shared/Exception.h \
../headers-shared/shared/CircularBuffer.hpp \
include/controller/AClientUDP.h \
include/controller/QClientUDP.h \
../headers-shared/shared/LibC.hh \
../headers-shared/shared/IMutex.h \
../headers-shared/shared/Mutex.h \
../headers-shared/shared/MutexPool.h

win32: HEADERS += ../headers-shared/windows/CWMutex.h
unix: HEADERS += ../headers-shared/linux/CUMutex.h


FORMS    += ui/babibelwindow.ui \
ui/conversation.ui \
ui/login.ui

RESOURCES += \
ressources/images.qrc

re.commands = $(MAKE) distclean && qmake &&  $(MAKE) all
QMAKE_EXTRA_TARGETS += re

fclean.commands = $(MAKE) distclean qmake
QMAKE_EXTRA_TARGETS += fclean

INCLUDEPATH += .
INCLUDEPATH += ..

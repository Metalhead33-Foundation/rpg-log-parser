QT += gui widgets

CONFIG += c++20 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        Agnaistic.cpp \
        HtmlHeader.cpp \
        HtmlToMarkdown.cpp \
        QFileException.cpp \
        RpgLog.cpp \
        RpgPostTemplate.cpp \
        RpgPostWidget.cpp \
        RpgSection.cpp \
        RpgSession.cpp \
        TavernAI.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
	Agnaistic.hpp \
	HtmlHeader.hpp \
	HtmlToMarkdown.hpp \
	QFileException.hpp \
	RpgLog.hpp \
	RpgPostTemplate.hpp \
	RpgPostWidget.hpp \
	RpgSection.hpp \
	RpgSession.hpp \
	TavernAI.hpp

FORMS += \
	RpgPostWidget.ui

DISTFILES += \
	LICENSE \
	README.md \
	process.sh

TEMPLATE = app
CONFIG += console
CONFIG += qtestlib
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11 -pedantic -Wall -Wextra

SOURCES += main.cpp \
    job.cpp \
    user.cpp \
    resource_plan.cpp \
    resource_gap.cpp \
    schedule.cpp \
    config.cpp

HEADERS += \
    job.h \
    user.h \
    resource_plan.h \
    resource_gap.h \
    resources.h \
    schedule.h \
    config.h


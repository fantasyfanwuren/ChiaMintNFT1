QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cmdcommand.cpp \
    dialogmetahelp.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    cmdcommand.h \
    dialogmetahelp.h \
    mainwindow.h

FORMS += \
    dialogmetahelp.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

#应用图标
RC_ICONS += ChiaMintNFT1.ico

#版本号
VERSION = 1.0.0
#中文
RC_LANG = 0x0004
# 公司名
QMAKE_TARGET_COMPANY = 中国数字党
# 产品名称
QMAKE_TARGET_PRODUCT = ChiaMintNFT1
# 详细描述
QMAKE_TARGET_DESCRIPTION = 批量化铸造NFT脚本
# 版权
QMAKE_TARGET_COPYRIGHT = Copyright(C) 2022 the China Digital party



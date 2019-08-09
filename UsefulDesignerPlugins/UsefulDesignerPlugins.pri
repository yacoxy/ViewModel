INCLUDEPATH += $$PWD/include

CONFIG(release, debug|release): LIBS += $$PWD/lib/release/UsefulDesignerPlugins.lib
CONFIG(debug, debug|release): LIBS += $$PWD/lib/debug/UsefulDesignerPlugins.lib

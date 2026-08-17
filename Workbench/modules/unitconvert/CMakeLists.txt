wb_add_module(NAME unitconvert
    SOURCES module/UnitConvertModule.cpp
    QT_SOURCES  platforms/qt/UnitConvertView.cpp
                platforms/qt/UnitConvertViewEntry.cpp
    IOS_SOURCES platforms/ios/UnitConvertView.mm platforms/ios/UnitConvertViewEntry.mm
)

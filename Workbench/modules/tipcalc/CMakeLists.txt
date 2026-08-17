wb_add_module(NAME tipcalc
    SOURCES viewmodels/TipCalcVm.cpp module/TipcalcModule.cpp
    QT_SOURCES  platforms/qt/TipCalcView.cpp
                platforms/qt/TipCalcViewEntry.cpp
    IOS_SOURCES platforms/ios/TipCalcView.mm platforms/ios/TipCalcViewEntry.mm
)

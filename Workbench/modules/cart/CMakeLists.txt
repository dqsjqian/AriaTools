wb_add_module(NAME cart
    SOURCES viewmodels/CartVm.cpp models/CartItem.cpp module/CartModule.cpp
    QT_SOURCES  platforms/qt/CartView.cpp
    IOS_SOURCES platforms/ios/CartView.mm
)

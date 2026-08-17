wb_add_module(NAME cart
    SOURCES viewmodels/CartVm.cpp models/CartItem.cpp module/CartModule.cpp
    QT_SOURCES  platforms/qt/CartView.cpp
                platforms/qt/CartViewEntry.cpp
    IOS_SOURCES platforms/ios/CartView.mm platforms/ios/CartViewEntry.mm
)

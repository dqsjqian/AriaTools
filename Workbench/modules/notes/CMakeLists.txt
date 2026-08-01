wb_add_module(NAME notes
    SOURCES     models/NotesModel.cpp
                services/MarkdownNotesService.cpp
                viewmodels/NotesVm.cpp
                module/NotesModule.cpp
    QT_SOURCES  platforms/qt/NotesView.cpp
    IOS_SOURCES platforms/ios/NotesView.mm
)

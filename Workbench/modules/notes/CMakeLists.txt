wb_add_module(NAME notes
    SOURCES     models/NotesModel.cpp
                services/MarkdownNotesService.cpp
                viewmodels/NotesVm.cpp
                module/NotesModule.cpp
    QT_SOURCES  platforms/qt/NotesView.cpp
                platforms/qt/NotesViewEntry.cpp
    IOS_SOURCES platforms/ios/NotesView.mm platforms/ios/NotesViewEntry.mm
)

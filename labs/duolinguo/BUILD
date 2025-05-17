load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

# Создаем пустые файлы для других БД (если нужны)
genrule(
    name = "create_empty_dbs",
    srcs = [],
    outs = [
        "data/translation.db",
    ],
    cmd = """
        mkdir -p $(RULEDIR)/data && \
        touch $(location data/translation.db)
    """,
)

qt_cc_library(
    name = "main_window",
    srcs = [
        "src/controller.cpp",
        "src/mainwindow.cpp",
        "src/grammarwidget.cpp",
        "src/statistics.cpp",
        "src/translationwidget.cpp",
        "src/progressbar.cpp"
    ],
    hdrs = [
        "src/controller.h",
        "src/mainwindow.h",
        "src/grammarwidget.h",
        "src/statistics.h",
        "src/translationwidget.h",
        "src/progressbar.h"
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_sql",
    ],
    data = [
        "src/db/grammartesteasy.db",
        "src/db/grammartesthard.db",
        "src/db/grammarquestioneasy.db",
        "src/db/grammarquestionhard.db",
        "src/db/translationentorueasy.db",
        "src/db/translationentoruhard.db",
        "src/db/translationrutoeneasy.db",
        "src/db/translationrutoenhard.db",
    ],
)

qt_cc_binary(
    name = "duolinguo",
    srcs = ["src/main.cpp"],
    deps = [
        ":main_window",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_sql",
    ],
    data = [":create_empty_dbs"],
)
load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "main_window",
    srcs = [
        "src/controller.cpp",
        "src/mainwindow.cpp",
        "src/grammarwidget.cpp",
        "src/statistics.cpp",
        "src/translationwidget.cpp"
    ],
    hdrs = ["src/controller.h",
            "src/mainwindow.h",
            "src/grammarwidget.h",
            "src/statistics.h",
            "src/translationwidget.h"
            ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "duolinguo",
    srcs = ["src/main.cpp"],
    deps = [
        ":main_window",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)

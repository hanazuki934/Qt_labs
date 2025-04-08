load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "raycaster_lib",
    srcs = [
        "mainwindow.cpp",
        "ray.cpp",
        "polygon.cpp",
        "controller.cpp",
        "canva.cpp"
    ],
    hdrs = [
        "mainwindow.h",
        "ray.h",
        "polygon.h",
        "controller.h",
        "canva.h"
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "raycaster",
    srcs = ["main.cpp"],
    deps = [
        ":raycaster_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)

load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "raycaster_lib",
    srcs = [
        "src/mainwindow.cpp",
        "src/ray.cpp",
        "src/polygon.cpp",
        "src/controller.cpp",
        "src/canva.cpp"
    ],
    hdrs = [
        "src/mainwindow.h",
        "src/ray.h",
        "src/polygon.h",
        "src/controller.h",
        "src/canva.h"
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "raycaster",
    srcs = ["src/main.cpp"],
    deps = [
        ":raycaster_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)

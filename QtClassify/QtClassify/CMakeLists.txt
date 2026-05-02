cmake_minimum_required(VERSION 3.16)

project(QtClassify VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- OpenCV ----
find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs)

# ---- Qt 6 ----
find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2)

qt_standard_project_setup(REQUIRES 6.10)

qt_add_executable(appQtClassify
    main.cpp
    classifier.cpp
    classifierbackend.cpp
)

qt_add_qml_module(appQtClassify
    URI QtClassify
    QML_FILES
        Main.qml
)

set_target_properties(appQtClassify PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING
        ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_include_directories(appQtClassify PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(appQtClassify
    PRIVATE
        Qt6::Quick
        Qt6::QuickControls2
        ${OpenCV_LIBS}
)

# ---- macOS: explicit two-level namespace (defence-in-depth) ---------
# Homebrew OpenCV links against Homebrew libpng16.16.dylib.  Apple's
# ImageIO framework (used by CoreText when rendering emoji glyphs) contains
# its own internal libpng.  If Homebrew libpng symbols are visible in the
# flat namespace, ImageIO's PNG reader can resolve to the wrong symbols,
# corrupting function pointers and crashing with EXC_BAD_ACCESS (SIGBUS).
#
# -twolevel_namespace (the macOS default) ensures each library resolves
# symbols only against its explicit dependencies.  Setting it explicitly
# prevents accidental flat-namespace behaviour.
#
# Primary fix: all emoji characters removed from Main.qml to avoid
# triggering CoreText's CopyEmojiImage → ImageIO → PNG path entirely.
if(APPLE)
    target_link_options(appQtClassify PRIVATE
        -Wl,-twolevel_namespace
    )
endif()

install(TARGETS appQtClassify
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

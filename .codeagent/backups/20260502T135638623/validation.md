# validation.md — Build, Test & Validation Commands

This file defines the commands used to build, test, and validate the project.
The agent may run these commands after making changes.

## Build (QtClassify C++/QML application)
```
cmake -S QtClassify/QtClassify -B QtClassify/QtClassify/build/Debug \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6 2>/dev/null || echo /opt/homebrew/opt/qt@6) \
    -DOpenCV_DIR=$(brew --prefix opencv 2>/dev/null || echo /opt/homebrew/opt/opencv) \
    -G Ninja

cmake --build QtClassify/QtClassify/build/Debug
```

## Run
```
QtClassify/QtClassify/build/Debug/appQtClassify
```

## Python classifier (for reference)
```
python3 classify.py --p80p150 Dataset/P80 Dataset/P150
```

## Lint / Format
```
# TODO: add lint/format command
```

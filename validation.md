# validation.md — Build, Test & Validation Commands

This file defines the commands used to build, test, and validate the project.
The agent may run these commands after making changes.

## Build (QtClassify C++/QML application)
```
export PATH="/opt/homebrew/bin:$HOME/Qt/Tools/Ninja:$PATH"
cmake -S QtClassify/QtClassify -B QtClassify/QtClassify/build/Debug \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@6 \
    -DOpenCV_DIR=/opt/homebrew/opt/opencv \
    -G Ninja
cmake --build QtClassify/QtClassify/build/Debug
```

## Run
```
QtClassify/QtClassify/build/Debug/appQtClassify.app/Contents/MacOS/appQtClassify
```

## Python classifier (for reference)
```
python3 classify.py --p80p150 Dataset/P80 Dataset/P150
```

## Lint / Format
```
# TODO: add lint/format command
```

export PATH="/opt/homebrew/bin:$HOME/Qt/Tools/Ninja:$PATH"
cmake -S QtClassify/QtClassify -B QtClassify/QtClassify/build/Debug \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@6 \
    -DOpenCV_DIR=/opt/homebrew/opt/opencv \
    -G Ninja
cmake --build QtClassify/QtClassify/build/Debug

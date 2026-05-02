import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Window {
    id: mainWindow
    width: 900
    height: 700
    minimumWidth: 700
    minimumHeight: 500
    visible: true
    title: qsTr("QtClassify — Image Classifier")
    color: "#1e1e2e"

    // ── Colour palette ──────────────────────────────────────────────
    readonly property color bgDark:        "#1e1e2e"
    readonly property color bgCard:        "#2a2a3c"
    readonly property color bgInput:       "#363650"
    readonly property color accent:        "#7c8aff"
    readonly property color textPrimary:   "#e0e0f0"
    readonly property color textSecondary: "#a0a0c0"
    readonly property color p50Color:      "#50fa7b"
    readonly property color p80Color:      "#8be9fd"
    readonly property color p150Color:     "#ffb86c"
    readonly property color errorColor:    "#ff5555"
    readonly property color successColor:  "#50fa7b"

    function labelColor(label) {
        switch (label) {
            case "P50":  return p50Color;
            case "P80":  return p80Color;
            case "P150": return p150Color;
            default:     return errorColor;
        }
    }

    // ── Top bar ─────────────────────────────────────────────────────
    Rectangle {
        id: topBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: 52
        color: "#16162a"

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 16
            anchors.rightMargin: 16
            spacing: 12

            Text {
                text: "QtClassify"
                color: textPrimary
                font.pixelSize: 20
                font.bold: true
            }

            Item { Layout.fillWidth: true }

            Text {
                text: "ROI:"
                color: textSecondary
                font.pixelSize: 13
            }

            SpinBox {
                id: roiSpin
                from: 10
                to: 100
                value: 80
                stepSize: 5
                editable: true
                Layout.preferredWidth: 80
                onValueChanged: backend.roiFraction = value / 100.0

                background: Rectangle {
                    color: bgInput
                    radius: 6
                    border.color: "#444466"
                }
                contentItem: TextInput {
                    color: textPrimary
                    font.pixelSize: 13
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Text {
                text: "%"
                color: textSecondary
                font.pixelSize: 13
            }
        }
    }

    // ── Tab bar ─────────────────────────────────────────────────────
    Rectangle {
        id: tabBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: topBar.bottom
        height: 44
        color: bgDark

        Row {
            anchors.fill: parent
            anchors.leftMargin: 16
            spacing: 4

            TabButton {
                id: singleTab
                text: "Single Image"
                width: 160
                height: 40
                checked: true
                onClicked: swipeView.currentIndex = 0

                background: Rectangle {
                    color: singleTab.checked ? bgCard : "transparent"
                    radius: 8
                }
                contentItem: Text {
                    text: singleTab.text
                    color: singleTab.checked ? textPrimary : textSecondary
                    font.pixelSize: 14
                    font.bold: singleTab.checked
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            TabButton {
                id: batchTab
                text: "Batch Directory"
                width: 180
                height: 40
                onClicked: swipeView.currentIndex = 1

                background: Rectangle {
                    color: batchTab.checked ? bgCard : "transparent"
                    radius: 8
                }
                contentItem: Text {
                    text: batchTab.text
                    color: batchTab.checked ? textPrimary : textSecondary
                    font.pixelSize: 14
                    font.bold: batchTab.checked
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }

    // ── SwipeView (Single / Batch pages) ────────────────────────────
    SwipeView {
        id: swipeView
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: tabBar.bottom
        anchors.bottom: parent.bottom
        anchors.margins: 16
        currentIndex: 0
        interactive: false

        // ════════════════════════════════════════════════════════════
        // PAGE 0 — Single Image
        // ════════════════════════════════════════════════════════════
        Item {
            ColumnLayout {
                anchors.fill: parent
                spacing: 12

                // ---- Controls row ----
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    Button {
                        text: "Select Image..."
                        onClicked: fileDialog.open()

                        background: Rectangle {
                            color: accent
                            radius: 8
                        }
                        contentItem: Text {
                            text: parent.text
                            color: "#ffffff"
                            font.pixelSize: 13
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 36
                        color: bgInput
                        radius: 8
                        border.color: "#444466"

                        Text {
                            anchors.fill: parent
                            anchors.leftMargin: 12
                            anchors.rightMargin: 12
                            text: backend.imagePath || "No image selected"
                            color: backend.imagePath ? textPrimary : textSecondary
                            font.pixelSize: 12
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideMiddle
                        }
                    }

                    Button {
                        text: "Re-classify"
                        enabled: backend.imagePath !== ""
                        onClicked: backend.classifyCurrentImage()

                        background: Rectangle {
                            color: parent.enabled ? "#5a5a80" : "#333350"
                            radius: 8
                        }
                        contentItem: Text {
                            text: parent.text
                            color: parent.enabled ? textPrimary : textSecondary
                            font.pixelSize: 13
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }

                // ---- Image preview + result card ----
                RowLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 16

                    // Image preview
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredWidth: 60
                        color: bgCard
                        radius: 12
                        border.color: "#333355"
                        clip: true

                        Image {
                            id: previewImage
                            anchors.fill: parent
                            anchors.margins: 8
                            fillMode: Image.PreserveAspectFit
                            source: backend.imagePath ? "file://" + backend.imagePath : ""
                            visible: backend.imagePath !== ""

                            Rectangle {
                                anchors.centerIn: parent
                                width: roiSpin.value / 100.0 * Math.min(parent.width, parent.height)
                                height: roiSpin.value / 100.0 * Math.min(parent.width, parent.height)
                                color: "transparent"
                                border.color: "#7c8aff40"
                                border.width: 2
                                visible: backend.imagePath !== ""
                            }
                        }

                        Text {
                            anchors.centerIn: parent
                            text: "Select an image to preview"
                            color: textSecondary
                            font.pixelSize: 14
                            horizontalAlignment: Text.AlignHCenter
                            visible: backend.imagePath === ""
                        }
                    }

                    // Result card
                    Rectangle {
                        Layout.preferredWidth: 300
                        Layout.fillHeight: true
                        color: bgCard
                        radius: 12
                        border.color: "#333355"

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 20
                            spacing: 16

                            Text {
                                text: "Classification Result"
                                color: textSecondary
                                font.pixelSize: 13
                                font.bold: true
                                visible: backend.imagePath !== ""
                            }

                            // Big label badge
                            Rectangle {
                                Layout.alignment: Qt.AlignHCenter
                                implicitWidth: labelText.implicitWidth + 40
                                implicitHeight: 56
                                radius: 28
                                color: labelColor(backend.classificationLabel)
                                opacity: 0.2
                                visible: backend.imagePath !== ""
                            }
                            Text {
                                id: labelText
                                Layout.alignment: Qt.AlignHCenter
                                text: backend.classificationLabel || "—"
                                color: labelColor(backend.classificationLabel)
                                font.pixelSize: 32
                                font.bold: true
                                visible: backend.imagePath !== ""
                            }

                            // Tiebreaker badge
                            Rectangle {
                                Layout.alignment: Qt.AlignHCenter
                                implicitWidth: tieText.implicitWidth + 16
                                implicitHeight: 24
                                radius: 12
                                color: "#ffb86c30"
                                visible: backend.usedTiebreaker
                                Text {
                                    id: tieText
                                    anchors.centerIn: parent
                                    text: "Canny tiebreaker used"
                                    color: p150Color
                                    font.pixelSize: 11
                                }
                            }

                            // Detail rows
                            GridLayout {
                                columns: 2
                                rowSpacing: 8
                                columnSpacing: 20
                                visible: backend.imagePath !== ""

                                Text { text: "Brightness:";     color: textSecondary; font.pixelSize: 12 }
                                Text { text: backend.brightness.toFixed(1);
                                       color: textPrimary; font.pixelSize: 13; font.bold: true }

                                Text { text: "Entropy:";        color: textSecondary; font.pixelSize: 12 }
                                Text { text: backend.entropy.toFixed(4);
                                       color: textPrimary; font.pixelSize: 13; font.bold: true }

                                Text { text: "Canny Edge %:";   color: textSecondary; font.pixelSize: 12 }
                                Text { text: backend.cannyEdgeDensity.toFixed(2) + "%";
                                       color: textPrimary; font.pixelSize: 13; font.bold: true }

                                Text { text: "ROI:";            color: textSecondary; font.pixelSize: 12 }
                                Text { text: (backend.roiFraction * 100).toFixed(0) + "%";
                                       color: textPrimary; font.pixelSize: 13; font.bold: true }
                            }

                            // Stage info
                            Text {
                                text: {
                                    if (backend.classificationLabel === "P50")
                                        return "→ Classified at Stage 1 (brightness)"
                                    if (backend.classificationLabel === "ERROR" || !backend.imagePath)
                                        return ""
                                    if (backend.usedTiebreaker)
                                        return "→ Stage 2b: Canny tiebreaker resolved ambiguity"
                                    return "→ Stage 2a: entropy alone determined class"
                                }
                                color: textSecondary
                                font.pixelSize: 11
                                font.italic: true
                                wrapMode: Text.WordWrap
                                Layout.fillWidth: true
                            }

                            Item { Layout.fillHeight: true }

                            // Placeholder when no image
                            Text {
                                anchors.centerIn: parent
                                text: "Select an image\nto classify"
                                color: textSecondary
                                font.pixelSize: 14
                                horizontalAlignment: Text.AlignHCenter
                                visible: backend.imagePath === ""
                            }
                        }
                    }
                }
            }
        }

        // ════════════════════════════════════════════════════════════
        // PAGE 1 — Batch Directory
        // ════════════════════════════════════════════════════════════
        Item {
            ColumnLayout {
                anchors.fill: parent
                spacing: 12

                // ---- Controls row ----
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    Button {
                        text: "Select Directory..."
                        onClicked: batchDirDialog.open()

                        background: Rectangle {
                            color: accent
                            radius: 8
                        }
                        contentItem: Text {
                            text: parent.text
                            color: "#ffffff"
                            font.pixelSize: 13
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 36
                        color: bgInput
                        radius: 8
                        border.color: "#444466"

                        Text {
                            anchors.fill: parent
                            anchors.leftMargin: 12
                            anchors.rightMargin: 12
                            text: batchDirText.text || "No directory selected"
                            color: batchDirText.text ? textPrimary : textSecondary
                            font.pixelSize: 12
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideMiddle
                        }
                    }

                    Text {
                        id: batchDirText
                        visible: false
                    }

                    Button {
                        text: batchProgress.running ? "Classifying..." : "Classify All"
                        enabled: batchDirText.text !== "" && !batchProgress.running
                        onClicked: {
                            backend.clearBatchResults()
                            backend.classifyDirectory(batchDirText.text);
                        }

                        background: Rectangle {
                            color: parent.enabled ? "#50fa7b" : "#333350"
                            opacity: parent.enabled ? 0.9 : 0.5
                            radius: 8
                        }
                        contentItem: Text {
                            text: parent.text
                            color: parent.enabled ? "#1e1e2e" : textSecondary
                            font.pixelSize: 13
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }

                    Button {
                        text: "Clear"
                        enabled: backend.batchTotal > 0 && !batchProgress.running
                        onClicked: backend.clearBatchResults()

                        background: Rectangle {
                            color: parent.enabled ? "#5a5a80" : "#333350"
                            radius: 8
                        }
                        contentItem: Text {
                            text: parent.text
                            color: parent.enabled ? textPrimary : textSecondary
                            font.pixelSize: 13
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }

                // ---- Progress bar ----
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 6
                    color: bgInput
                    radius: 3
                    visible: batchProgress.running || batchProgress.value > 0

                    Rectangle {
                        width: parent.width * (batchProgress.value / Math.max(batchProgress.to, 1))
                        height: parent.height
                        color: successColor
                        radius: 3
                    }
                }

                Text {
                    text: batchProgress.running
                          ? "Processing " + batchProgress.value + " / " + batchProgress.to
                          : (backend.batchTotal > 0
                             ? "Done — " + backend.batchTotal + " images classified"
                             : "")
                    color: textSecondary
                    font.pixelSize: 12
                    visible: text !== ""
                }

                // ---- Results table ----
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: bgCard
                    radius: 12
                    border.color: "#333355"

                    // Column headers
                    Row {
                        id: headerRow
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.leftMargin: 16
                        anchors.rightMargin: 16
                        anchors.topMargin: 10
                        height: 28
                        spacing: 0

                        Text { text: "File";               color: textSecondary;
                               font.pixelSize: 11; font.bold: true;
                               width: parent.width * 0.40; elide: Text.ElideRight }
                        Text { text: "Result";             color: textSecondary;
                               font.pixelSize: 11; font.bold: true;
                               width: parent.width * 0.12 }
                        Text { text: "Brightness";         color: textSecondary;
                               font.pixelSize: 11; font.bold: true;
                               width: parent.width * 0.12 }
                        Text { text: "Entropy";            color: textSecondary;
                               font.pixelSize: 11; font.bold: true;
                               width: parent.width * 0.14 }
                        Text { text: "Canny%";             color: textSecondary;
                               font.pixelSize: 11; font.bold: true;
                               width: parent.width * 0.12 }
                        Text { text: "Note";               color: textSecondary;
                               font.pixelSize: 11; font.bold: true;
                               width: parent.width * 0.10 }
                    }

                    Rectangle {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: headerRow.bottom
                        anchors.topMargin: 4
                        height: 1
                        color: "#333355"
                    }

                    ListView {
                        id: batchList
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: headerRow.bottom
                        anchors.topMargin: 8
                        anchors.bottom: summaryRow.top
                        anchors.bottomMargin: 8
                        anchors.leftMargin: 8
                        anchors.rightMargin: 8
                        clip: true
                        spacing: 2
                        model: backend.batchModel

                        delegate: Rectangle {
                            width: batchList.width
                            height: 28
                            color: index % 2 === 0 ? "transparent" : "#ffffff04"
                            radius: 4

                            Row {
                                anchors.fill: parent
                                anchors.leftMargin: 8
                                anchors.rightMargin: 8
                                spacing: 0

                                Text {
                                    text: filename ? filename.split('/').pop().split('\\').pop() : ""
                                    color: isError ? errorColor : textPrimary
                                    font.pixelSize: 11
                                    width: parent.width * 0.40
                                    elide: Text.ElideRight
                                    verticalAlignment: Text.AlignVCenter
                                }
                                Text {
                                    text: label
                                    color: mainWindow.labelColor(label)
                                    font.pixelSize: 11
                                    font.bold: true
                                    width: parent.width * 0.12
                                    verticalAlignment: Text.AlignVCenter
                                }
                                Text {
                                    text: brightness.toFixed(1)
                                    color: textPrimary
                                    font.pixelSize: 11
                                    width: parent.width * 0.12
                                    verticalAlignment: Text.AlignVCenter
                                }
                                Text {
                                    text: entropy.toFixed(4)
                                    color: textPrimary
                                    font.pixelSize: 11
                                    width: parent.width * 0.14
                                    verticalAlignment: Text.AlignVCenter
                                }
                                Text {
                                    text: canny.toFixed(2)
                                    color: textPrimary
                                    font.pixelSize: 11
                                    width: parent.width * 0.12
                                    verticalAlignment: Text.AlignVCenter
                                }
                                Text {
                                    text: usedTiebreaker ? "tie" : ""
                                    color: p150Color
                                    font.pixelSize: 10
                                    width: parent.width * 0.10
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                        }
                    }

                    // Summary bar at bottom
                    Rectangle {
                        id: summaryRow
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        height: 36
                        color: "#1a1a30"
                        radius: 12

                        Rectangle {
                            // anti-radius top corners to sit flush
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            height: parent.height / 2
                            color: parent.color
                        }

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 20
                            anchors.rightMargin: 20
                            spacing: 24

                            Text {
                                text: "Total: " + backend.batchTotal
                                color: textPrimary
                                font.pixelSize: 12
                                font.bold: true
                            }
                            Text {
                                text: "P50: " + backend.batchCountP50
                                color: p50Color
                                font.pixelSize: 12
                                font.bold: true
                            }
                            Text {
                                text: "P80: " + backend.batchCountP80
                                color: p80Color
                                font.pixelSize: 12
                                font.bold: true
                            }
                            Text {
                                text: "P150: " + backend.batchCountP150
                                color: p150Color
                                font.pixelSize: 12
                                font.bold: true
                            }
                            Text {
                                text: "Errors: " + backend.batchCountErrors
                                color: errorColor
                                font.pixelSize: 12
                                font.bold: true
                                visible: backend.batchCountErrors > 0
                            }
                        }
                    }

                    // Placeholder when empty
                    Text {
                        anchors.centerIn: parent
                        text: "Select a directory and click\n\"Classify All\" to begin"
                        color: textSecondary
                        font.pixelSize: 14
                        horizontalAlignment: Text.AlignHCenter
                        visible: backend.batchTotal === 0 && !batchProgress.running
                    }
                }
            }
        }
    }

    // ── Progress indicator (internal) ───────────────────────────────
    QtObject {
        id: batchProgress
        property int value: 0
        property int to: 1
        property bool running: false
    }

    // ── File dialogs ────────────────────────────────────────────────
    FileDialog {
        id: fileDialog
        title: "Select an image"
        nameFilters: ["Image files (*.png *.jpg *.jpeg *.bmp *.tif *.tiff)"]
        onAccepted: {
            if (selectedFile) {
                backend.imagePath = selectedFile.toString();
            }
        }
    }

    FileDialog {
        id: batchDirDialog
        title: "Select a directory"
        fileMode: FileDialog.OpenFolder
        onAccepted: {
            if (selectedFolder) {
                batchDirText.text = selectedFolder.toString();
            }
        }
    }

    // ── Backend signal connections ──────────────────────────────────
    Connections {
        target: backend
        function onBatchClassifyStarted(total) {
            batchProgress.value = 0;
            batchProgress.to = total;
            batchProgress.running = true;
        }
        function onBatchClassifyProgress(current, total, filename) {
            batchProgress.value = current;
            batchProgress.to = total;
        }
        function onBatchClassifyFinished() {
            batchProgress.running = false;
        }
    }
}

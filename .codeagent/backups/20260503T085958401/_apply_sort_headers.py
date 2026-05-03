#!/usr/bin/env python3
"""Apply sortable column headers to Main.qml."""
import sys

with open('QtClassify/QtClassify/Main.qml', 'r') as f:
    content = f.read()

# 1. Add sort state properties before '// ---- Results table ----'
old_props = '                // ---- Results table ----'
new_props = '''                // ---- Sort state ----
                property int sortColumn: -1
                property bool sortAscending: true

                // ---- Results table ----'''

if old_props in content:
    content = content.replace(old_props, new_props)
    print('Step 1 OK: Added sort state properties')
else:
    print('ERROR step 1: marker not found')

# 2. Replace header Row
old_header_marker = '                    // Column headers\n                    Row {'
if old_header_marker in content:
    # Find the full block from "// Column headers" to the matching closing "}"
    start_marker = '                    // Column headers\n                    Row {\n                        id: headerRow'
    end_marker = '                    }\n\n                    Rectangle {\n                        anchors.left: parent.left'
    
    start_idx = content.find(start_marker)
    end_idx = content.find(end_marker, start_idx)
    
    if start_idx >= 0 and end_idx >= 0:
        # end_idx points to the "}" before the separator Rectangle, include that "}"
        # Actually end_marker starts with "                    }" which is the closing
        # of headerRow. Let me find the exact closing brace.
        close_brace = content.find('\n                    }\n\n                    Rectangle {\n                        anchors.left: parent.left', start_idx)
        if close_brace >= 0:
            close_brace += len('\n                    }')
            old_block = content[start_idx:close_brace]
            
            new_block = '''                    // Column headers (click to sort)
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

                        // ---- File header ----
                        Item {
                            width: parent.width * 0.40; height: parent.height
                            Rectangle {
                                anchors.fill: parent
                                color: sortColumn === 0 ? "#7c8aff20" : "transparent"
                                radius: 4
                            }
                            Row {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                spacing: 2
                                Text {
                                    text: "File"
                                    color: sortColumn === 0 ? accent : textSecondary
                                    font.pixelSize: 11; font.bold: true
                                }
                                Text {
                                    text: sortColumn === 0 ? (sortAscending ? " \u25B2" : " \u25BC") : ""
                                    color: accent
                                    font.pixelSize: 9
                                }
                            }
                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                hoverEnabled: true
                                onClicked: {
                                    if (sortColumn === 0) sortAscending = !sortAscending;
                                    else { sortColumn = 0; sortAscending = true; }
                                    backend.sortBatchModel(0);
                                }
                            }
                        }

                        // ---- Result header ----
                        Item {
                            width: parent.width * 0.12; height: parent.height
                            Rectangle {
                                anchors.fill: parent
                                color: sortColumn === 1 ? "#7c8aff20" : "transparent"
                                radius: 4
                            }
                            Row {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                spacing: 2
                                Text {
                                    text: "Result"
                                    color: sortColumn === 1 ? accent : textSecondary
                                    font.pixelSize: 11; font.bold: true
                                }
                                Text {
                                    text: sortColumn === 1 ? (sortAscending ? " \u25B2" : " \u25BC") : ""
                                    color: accent
                                    font.pixelSize: 9
                                }
                            }
                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    if (sortColumn === 1) sortAscending = !sortAscending;
                                    else { sortColumn = 1; sortAscending = true; }
                                    backend.sortBatchModel(1);
                                }
                            }
                        }

                        // ---- Brightness header ----
                        Item {
                            width: parent.width * 0.12; height: parent.height
                            Rectangle {
                                anchors.fill: parent
                                color: sortColumn === 2 ? "#7c8aff20" : "transparent"
                                radius: 4
                            }
                            Row {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                spacing: 2
                                Text {
                                    text: "Brightness"
                                    color: sortColumn === 2 ? accent : textSecondary
                                    font.pixelSize: 11; font.bold: true
                                }
                                Text {
                                    text: sortColumn === 2 ? (sortAscending ? " \u25B2" : " \u25BC") : ""
                                    color: accent
                                    font.pixelSize: 9
                                }
                            }
                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    if (sortColumn === 2) sortAscending = !sortAscending;
                                    else { sortColumn = 2; sortAscending = true; }
                                    backend.sortBatchModel(2);
                                }
                            }
                        }

                        // ---- Entropy header ----
                        Item {
                            width: parent.width * 0.14; height: parent.height
                            Rectangle {
                                anchors.fill: parent
                                color: sortColumn === 3 ? "#7c8aff20" : "transparent"
                                radius: 4
                            }
                            Row {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                spacing: 2
                                Text {
                                    text: "Entropy"
                                    color: sortColumn === 3 ? accent : textSecondary
                                    font.pixelSize: 11; font.bold: true
                                }
                                Text {
                                    text: sortColumn === 3 ? (sortAscending ? " \u25B2" : " \u25BC") : ""
                                    color: accent
                                    font.pixelSize: 9
                                }
                            }
                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    if (sortColumn === 3) sortAscending = !sortAscending;
                                    else { sortColumn = 3; sortAscending = true; }
                                    backend.sortBatchModel(3);
                                }
                            }
                        }

                        // ---- Canny% header ----
                        Item {
                            width: parent.width * 0.12; height: parent.height
                            Rectangle {
                                anchors.fill: parent
                                color: sortColumn === 4 ? "#7c8aff20" : "transparent"
                                radius: 4
                            }
                            Row {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                spacing: 2
                                Text {
                                    text: "Canny%"
                                    color: sortColumn === 4 ? accent : textSecondary
                                    font.pixelSize: 11; font.bold: true
                                }
                                Text {
                                    text: sortColumn === 4 ? (sortAscending ? " \u25B2" : " \u25BC") : ""
                                    color: accent
                                    font.pixelSize: 9
                                }
                            }
                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    if (sortColumn === 4) sortAscending = !sortAscending;
                                    else { sortColumn = 4; sortAscending = true; }
                                    backend.sortBatchModel(4);
                                }
                            }
                        }

                        // ---- Note header ----
                        Item {
                            width: parent.width * 0.10; height: parent.height
                            Rectangle {
                                anchors.fill: parent
                                color: sortColumn === 5 ? "#7c8aff20" : "transparent"
                                radius: 4
                            }
                            Row {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                spacing: 2
                                Text {
                                    text: "Note"
                                    color: sortColumn === 5 ? accent : textSecondary
                                    font.pixelSize: 11; font.bold: true
                                }
                                Text {
                                    text: sortColumn === 5 ? (sortAscending ? " \u25B2" : " \u25BC") : ""
                                    color: accent
                                    font.pixelSize: 9
                                }
                            }
                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    if (sortColumn === 5) sortAscending = !sortAscending;
                                    else { sortColumn = 5; sortAscending = true; }
                                    backend.sortBatchModel(5);
                                }
                            }
                        }
                    }'''
            
            content = content[:start_idx] + new_block + content[close_brace:]
            print('Step 2 OK: Replaced header row')
        else:
            print('ERROR step 2: could not find closing brace')
    else:
        print('ERROR step 2: could not find header block boundaries')
else:
    print('ERROR step 2: header marker not found')

with open('QtClassify/QtClassify/Main.qml', 'w') as f:
    f.write(content)
print('Main.qml written')

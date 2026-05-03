Button {
    text: batchProgress.running ? "Classifying..." : "Classify All"
    enabled: batchDirText.text !== "" && !batchProgress.running
    onClicked: {
        backend.clearBatchResults()
        sortColumn = -1
        sortAscending = true
        backend.classifyDirectory(batchDirText.text);
    }
}

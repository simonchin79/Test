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

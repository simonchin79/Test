FileDialog {
    id: batchDirDialog
    title: "Select a directory"
    fileMode: FileDialog.OpenFolder
    onAccepted: {
        // On macOS, FileDialog.OpenFolder may not reliably populate
        // selectedFolder when the user navigates INTO a directory and
        // clicks Open.  In that case the dialog can default to selecting
        // a file (image) inside the directory instead of the directory
        // itself.  Fall back to currentFolder (the directory currently
        // displayed) when selectedFolder is empty so we always get a
        // valid directory path regardless of interaction style.
        var folder = selectedFolder ? selectedFolder : currentFolder;
        if (folder) {
            batchDirText.text = folder.toString();
        }
    }
}

import QtQuick 2.0

import eventsequencer 1.0 as ES

QtObject {
    id: root

    // Input
    property string fileResourceDirectory
    property string corralFileBase
    property string corralFileSuffix

    // Output
    property bool takeable: false
    property string corralFileName: corralFileBase + corralFileSuffix
    property string corralUrl: {
        // TODO This is a hack around a bug where corralUrl doesn't get updated
        //      when root.fileResourceDirectory gets updated.
        managedResources.fileResourceDirectory = Qt.binding(function () { return root.fileResourceDirectory })
        var ret = managedResources.urlForFileName(corralFileName)
        takeable = managedResources.existsUrl(ret) // If something's there from last time.
        return ret
    }

    function begin() {
        takeable = false
        if (managedResources.fileResourceDirectory === "") {
            return {
                success: false,
                errorMsg: "Resource directory doesn't exist. Save the file first."
            }
        } else {
            return {
                success: true,
                url: root.corralUrl
            }
        }
    }

    function done() {
        takeable = true
    }

    function take() {
        takeable = false
        var newUrl = managedResources.renameUrlToGeneratedFileName(corralUrl, corralFileSuffix)
        if (newUrl == "") {
            return {
                success: false,
                errorMsg: "Unable to rename file"
            }
        } else {
            return {
                success: true,
                newUrl: newUrl
            }
        }
    }

    function cancel() {
        takeable = false
        if (!managedResources.deleteUrl(root.corralUrl)) {
            console.warn("Deletion failed")
        }
    }

    /// BEGIN PRIVATE IMPLEMENTAITON

    property ES.ManagedResources managedResources : ES.ManagedResources {
        id: managedResources
        fileResourceDirectory: root.fileResourceDirectory
    }
}

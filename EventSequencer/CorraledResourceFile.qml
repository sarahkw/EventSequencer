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
        var result = managedResources.renameUrlToGeneratedFileName(corralUrl, corralFileSuffix)
        var success = result[0]
        if (success) {
            takeable = false // Not takeable only if successful!
            return {
                success: true,
                newUrl: result[1]
            }
        } else {
            return {
                success: false,
                errorMsg: result[1]
            }
        }
    }

    function recorral(url) {
        // Let's just let the rename fail.
//        if (takeable) {
//            return false
//        }

        var result = managedResources.renameUrlToFileName(url, corralFileName)
        var success = result[0]
        if (success) {
            takeable = true
            return {
                success: true
            }
        } else {
            return {
                success: false,
                errorMsg: result[1]
            }
        }
    }

    function cancel() {
        var result = managedResources.deleteUrl(root.corralUrl)
        var success = result[0]
        if (success) {
            takeable = false
            return {
                success: true
            }
        } else {
            return {
                success: false,
                errorMsg: result[1]
            }
        }
    }

    /// BEGIN PRIVATE IMPLEMENTAITON

    property ES.ManagedResources managedResources : ES.ManagedResources {
        id: managedResources
        fileResourceDirectory: root.fileResourceDirectory
    }
}

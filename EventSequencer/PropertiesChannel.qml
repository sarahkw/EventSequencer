import QtQuick 2.0

Loader {
    property var sourceCppChannel

    property var cppChannel
    onSourceCppChannelChanged: {
        var needBlank = true
        sourceComponent = null
        cppChannel = null
        if (sourceCppChannel !== null) {
            var control = controlResolver.resolve(sourceCppChannel.channelType)
            if (control !== null) {
                if (control.channelPropertiesComponent !== undefined) {
                    cppChannel = sourceCppChannel
                    sourceComponent = control.channelPropertiesComponent
                    needBlank = false
                }
            }
        }
        if (needBlank) {
            sourceComponent = blankComponent
        }
    }
}

import QtQuick 2.0

QtObject {
    property color defaultDarkerColor: "#121212"
    property color defaultDarkColor: "#1e1e1e"
    property color defaultControlColor: "#292929"
    property color inactiveControlColor: "#404040"
    property color panelColor: "#242424"
    property color panelSelectedColor: "#5d5d5d"
    property color whiteColor: "#ffffff"
    property color goldColor: "#f49c12"

    property color inputBackgroundColor: panelSelectedColor
    property color inputInactiveBackground: panelColor
    property color inputForegroundColor: whiteColor
    property color inputInactiveForeground: labelActiveForeground
    property color labelActiveForeground: "#c9c9c9"
    property color labelInactiveForeground: "#939393"
    property color inputHintForeground: inactiveControlColor

    property color artworkBackground: "#181818"
    property color artworkModifiedColor: goldColor
    property color artworkSavedColor: "#435151"
    property color artworkActiveColor: "#12b9bc"

    property color buttonDefaultBackground: "#12b9bc"
    property color buttonDefaultForeground: labelActiveForeground
    property color buttonHoverBackground: "#283c3f"
    property color buttonHoverForeground: whiteColor
    property color buttonPressedBackground: "#283c3f"
    property color buttonPressedForeground: whiteColor
    property color buttonDisabledForeground: inputBackgroundColor

    property color inactiveKeywordBackground: inactiveControlColor
    property color inactiveKeywordForeground: labelActiveForeground
    property color closeIconActiveColor: inactiveControlColor
    property color closeIconInactiveColor: labelActiveForeground
    property color closeIconDisabledColor: inactiveControlColor

    property color listSeparatorColor: labelActiveForeground

    property color linkClickedColor: whiteColor

    property color selectedArtworkBackground: inactiveControlColor
    property color checkboxCheckedColor: "#ffffff"

    property color itemsSourceBackground: inactiveControlColor
    property color itemsSourceSelected: inputBackgroundColor
    property color itemsSourceForeground: inputInactiveForeground

    property color destructiveColor: "#e81313"
    property color greenColor: "#2daf02"

    property color statusBarColor: defaultDarkerColor
    property color leftSliderColor: artworkBackground
    property color popupBackgroundColor: inactiveControlColor
    property color popupDarkInputBackground: inputInactiveBackground
}

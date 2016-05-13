import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import WGControls 1.0

Rectangle {
  color: palette.MainWindowColor
  property var title: "Property Panel"
  property var layoutHints: { 'dockProperties': 1.0 }

  Label {
    id: searchBoxLabel
    anchors.left: reflectedTreeView.left
    anchors.verticalCenter: searchBox.verticalCenter
    text: "Search:"
  }

  WGTextBox {
    id: searchBox
    anchors.top : parent.top
    anchors.left: searchBoxLabel.right
    anchors.right: parent.right
  }

  WGTreeModel
  {
    id: reflectedModel
    source: PropertyTree

    ValueExtension {}
    ColumnExtension {}
    ComponentExtension {}
    TreeExtension {}
    ButtonsDefinitionExtension {}
    ThumbnailExtension {}
    SelectionExtension {
        id: treeModelSelection
        multiSelect: false
    }
  }

  WGTreeView {
    id: reflectedTreeView
    anchors.top: searchBox.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    model: reflectedModel
    columnDelegates: [defaultColumnDelegate, propertyDelegate]
    selectionExtension: treeModelSelection
    indentation: 4
    spacing: 1

    property Component propertyDelegate: Loader {
      clip: true
      source: "ComponentWithButtons.qml"
    }
  }
}

# Code convention

## Cpp
We follow QGIS code style https://docs.qgis.org/3.28/en/docs/developers_guide/codingstandards.html but not that strict about documentation.

There is a script that automatically formats your cpp code, see `scripts/format_cpp.bash` and `scripts/astyle.bash`. 
Code convention for cpp is required and CI fails in case the style is violated.

## QML
For QML we follow code style defined here: https://github.com/Furkanzmc/QML-Coding-Guide.

One difference is that we use 2 spaces instead of 4 for indentation.
To set up correct indentation, navigate to `Preferences -> Qt Quick` in QtCreator and it set up accordingly:

![image](https://github.com/MerginMaps/mobile/assets/22449698/3e59ae3d-6ea6-4887-ade9-c680386b47f2)

**Unfortunatelly, there is no script that would check this convention automatically.**

There can be only one inline `if-else` `cond ? if true : if false` in line. If you need to combine multiple of them, use normal JS `if-else` instead in multiple lines. Example:
```qml
// wrong
width: hasFocus ? 100 : isVisible ? 40 : 10

// good
width: {
  if (hasFocus) {
    return 100
  }
  else if (isVisible) {
    return 40
  }
  return 10
}

height: root.visible ? 100 : 0 // one inline if-else is ok though
```

See full example of QML file (adjusted from https://github.com/Furkanzmc/QML-Coding-Guide?tab=readme-ov-file#full-example):
```qml
// First Qt imports
import QtQuick 2.15
import QtQuick.Controls 2.15
// Then custom imports
import my.library 1.0

Item {
  id: root
  
  // ----- Property Declarations
  
  // Required properties should be at the top.
  required property int radius: 0
  
  property int radius: 0
  property color borderColor: "blue"
  
  // ----- Signal declarations
  
  signal clicked()
  signal doubleClicked()
  
  // ----- In this section, we group the size and position information together.
  
  x: 0
  y: 0
  z: 0
  width: 100
  height: 100
  anchors.top: parent.top // If a single assignment, dot notation can be used.
  // If the item is an image, sourceSize is also set here.
  // sourceSize: Qt.size(12, 12)
  
  // ----- Then comes the other properties. There's no predefined order to these.
  
  // Do not use empty lines to separate the assignments. Empty lines are reserved
  // for separating type declarations.
  enabled: true
  layer.enabled: true
  
  // ----- Then attached properties and attached signal handlers.
  
  Layout.fillWidth: true
  Drag.active: false
  Drag.onActiveChanged: {
    
  }
  
  // ----- States and transitions.
  
  states: [
    State {
      
    }
  ]
  transitions: [
    Transitions {
      
    }
  ]
  
  // ----- Signal handlers

  onHeightChanged: // single-line or mutliline with brackets

  onWidthChanged: { 
  
  }
  // onCompleted and onDestruction signal handlers are always the last in
  // the order.
  Component.onCompleted: {
    
  }
  Component.onDestruction: {
    
  }

  // ----- Visual children.

  Rectangle {
    height: 50
    anchors: { // For multiple assignments, use group notation.
      top: parent.top
      left: parent.left
      right: parent.right
    }
    color: "red"
    layer: {
      enabled: true
      samples: 4
    }
  }

  Rectangle {
    width: parent.width
    height: 1
    color: "green"
  }

// ----- Qt provided non-visual children

  Timer {
    
  }

  // ----- Custom non-visual children

  MyCustomNonVisualType {
    
  }

  QtObject {
    id: privates
    
    property int diameter: 0
  }

  // ----- JavaScript functions

  function collapse() {
    
  }

  function setCollapsed(value: bool) {
    if (value === true) {
    }
    else {
    }
  }
}
```

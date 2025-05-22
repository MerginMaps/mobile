/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtCore
import QtQuick.Controls

import mm 1.0 as MM

//
// MapCanvas QML object is a utility class for rendering map layers and handling user interaction
//

Item {
  id: mapRoot

  // Clicked signal is sent 350ms after click (to avoid mixing it with double click)
  signal clicked( point p )

  signal longPressed( point p )

  signal doubleClicked( point p )

  signal wheelTurned( point p, double angle )

  signal dragged( point oldPoint, point newPoint )

  // userInteractedWithMap signal is sent each time user pans/zooms the map
  signal userInteractedWithMap()

  property alias mapSettings: mapRenderer.mapSettings
  property alias isRendering: mapRenderer.isRendering

  // Requests map redraw
  function refresh() {
    mapRenderer.clearCache()
    mapRenderer.refresh()
  }

  // Animates movement of map canvas from the current center to newPos.
  function jumpTo( newPos )
  {
    rendererPrivate.freeze('jumpTo')

    let newPosMapCRS = mapRenderer.mapSettings.screenToCoordinate( newPos )
    let oldPosMapCRS = mapRenderer.mapSettings.center

    // Disable animation until initial position is set
    jumpAnimator.enabled = false
    jumpAnimator.startX = oldPosMapCRS.x
    jumpAnimator.startY = oldPosMapCRS.y
    jumpAnimator.endX = newPosMapCRS.x
    jumpAnimator.endY = newPosMapCRS.y

    jumpAnimator.percentage = 0
    jumpAnimator.enabled = true
    jumpAnimator.percentage = 100
    rendererPrivate.unfreeze('jumpTo')
  }

  function zoom( center, scale )
  {
    mapRenderer.zoom( center, scale )
  }

  function pan( oldPos, newPos )
  {
    mapRenderer.pan( oldPos, newPos )
  }

  Item {
    id: jumpAnimator

    property double startX
    property double startY
    property double endX
    property double endY
    property double percentage: 0

    readonly property double azimuth: Math.atan2( startX - endX, startY - endY )
    readonly property double distance: Math.sqrt( ( startX - endX ) * ( startX - endX ) + ( startY - endY ) * ( startY - endY ) )

    Behavior on percentage {
      NumberAnimation {
        easing.type: Easing.OutQuart
        duration: 500
      }
      enabled: jumpAnimator.enabled
    }

    onPercentageChanged: {
      if ( enabled ) {
        let tmpX = startX - percentage * 0.01 * distance * Math.sin( azimuth )
        let tmpY = startY - percentage * 0.01 * distance * Math.cos( azimuth )
        mapRenderer.mapSettings.center = mapRenderer.mapSettings.toQgsPoint( Qt.point( tmpX, tmpY ) )
      }
    }
  }

  MM.MapCanvasMap {
    id: mapRenderer

    width: mapRoot.width
    height: mapRoot.height

    freeze: false

    QtObject {
      id: rendererPrivate

      property var _freezeMap: ({})

      function freeze( id ) {
        _freezeMap[ id ] = true
        mapRenderer.freeze = true
      }

      function unfreeze( id ) {
        delete _freezeMap[ id ]
        mapRenderer.freeze = Object.keys( _freezeMap ).length !== 0
      }

      function vectorDistance( a, b ) {
        return Math.sqrt( Math.pow( b.x - a.x, 2 ) + Math.pow( b.y - a.y, 2 ) )
      }
    }
  }

  PinchArea {
    id: pinchArea

    property string freezeId: 'pinch'

    anchors.fill: parent

    onPinchStarted: {
      mapRoot.userInteractedWithMap()
      rendererPrivate.freeze( freezeId )
    }

    onPinchFinished: {
      rendererPrivate.unfreeze( freezeId )
    }

    onPinchUpdated: function ( pinch ) {
      mapRenderer.pan( pinch.center, pinch.previousCenter )
      mapRenderer.zoom( pinch.center, pinch.previousScale / pinch.scale )
    }

    MouseArea {
      id: mouseArea

      property var initialPosition
      property var previousPosition

      property bool isDragging: false

      property string freezeId: 'drag'

      anchors.fill: parent
      enabled: !pinchArea.pinch.active

      // ignore touchpad scroll, listen only to the mouse wheel
      scrollGestureEnabled: false

      onPressed: function ( mouse ) {
        initialPosition = Qt.point( mouse.x, mouse.y )
        rendererPrivate.freeze( mouseArea.freezeId )

        dragDifferentiatorTimer.start()
      }

      onReleased: function ( mouse ) {
        let clickPosition = Qt.point( mouse.x, mouse.y )

        if ( clickDifferentiatorTimer.running ) {
          //
          // n-th click in a row (second, third,..)
          // this can be double click if it is in a reasonable
          // distance from the previous click
          //

          let isDoubleClick = false
          let previousTapPosition = clickDifferentiatorTimer.clickedPoint

          if ( previousTapPosition ) {
            let tapDistance = rendererPrivate.vectorDistance( clickPosition, previousTapPosition )
            isDoubleClick = tapDistance < mouseArea.drag.threshold
          }

          if ( isDoubleClick ) {
            // do not emit clicked signal when zooming
            clickDifferentiatorTimer.ignoreNextTrigger = true

            mapRoot.doubleClicked( Qt.point( mouse.x, mouse.y ) )
          }

          clickDifferentiatorTimer.restart()
        }
        else if ( !isDragging && !clickDifferentiatorTimer.ignoreNextTrigger )
        {
          // this is a simple click

          clickDifferentiatorTimer.clickedPoint = clickPosition
          clickDifferentiatorTimer.ignoreNextTrigger = false // just in case
          clickDifferentiatorTimer.start()
        }
        else
        {
          // this was a pressAndHold or a drag release

          clickDifferentiatorTimer.ignoreNextTrigger = false
        }

        previousPosition = null
        initialPosition = null
        isDragging = false

        rendererPrivate.unfreeze( mouseArea.freezeId )

        dragDifferentiatorTimer.stop()
      }

      onPressAndHold: function ( mouse ) {
        if ( !isDragging ) {
          mapRoot.longPressed( Qt.point( mouse.x, mouse.y ) )
        }

        clickDifferentiatorTimer.ignoreNextTrigger = true
      }

      onWheel: function ( wheel ) {
        mapRoot.wheelTurned( Qt.point( wheel.x, wheel.y), wheel.angleDelta.y )
      }

      // drag map canvas
      onPositionChanged: function ( mouse ) {
        let target = Qt.point( mouse.x, mouse.y )

        if ( !previousPosition ) {
          previousPosition = target
          initialPosition = target
          return
        }

        // we need to simulate natural scroll ~> revert movement on x and y
        let reverted_x = previousPosition.x - ( mouse.x - previousPosition.x )
        let reverted_y = previousPosition.y - ( mouse.y - previousPosition.y )

        mapRoot.dragged( previousPosition, Qt.point( reverted_x, reverted_y ) )

        previousPosition = target

        if ( !isDragging ) {
          // do not compute if we are already dragging

          let dragDistance = rendererPrivate.vectorDistance( initialPosition, target )

          if ( dragDistance > Application.styleHints.startDragDistance ) {
            // Drag distance threshold is hit, this is dragging!
            isDragging = true
          }

          if ( !dragDifferentiatorTimer.running ) {
            // User is panning the map for too long (timer already elapsed), this is dragging now!
            isDragging = true
          }

          if ( isDragging ) {
            // do not emit click after drag

            clickDifferentiatorTimer.stop()
            clickDifferentiatorTimer.ignoreNextTrigger = false
          }
        }
      }

      onIsDraggingChanged: {
        if ( isDragging ) {
          mapRoot.userInteractedWithMap()
        }
      }

      onCanceled: {
        // pinch took over
        previousPosition = null
        initialPosition = null
        isDragging = false

        rendererPrivate.unfreeze( mouseArea.freezeId )
        dragDifferentiatorTimer.stop()
      }

      Timer {
        id: clickDifferentiatorTimer

        property var clickedPoint
        property bool ignoreNextTrigger

        interval: Application.styleHints.mouseDoubleClickInterval

        onTriggered: {
          if ( !ignoreNextTrigger ) {
            mapRoot.clicked( clickedPoint )
          }
          ignoreNextTrigger = false
          clickedPoint = null
        }
      }

      Timer {
        id: dragDifferentiatorTimer

        interval: Application.styleHints.startDragTime
      }
    }
  }
}

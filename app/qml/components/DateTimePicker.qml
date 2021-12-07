import QtQuick 2.14
import QtQuick.Window 2.2
import QtQuick.Controls 2.14
import Qt.labs.calendar 1.0

Item {
    id: root

    property bool hasTimePicker: true
    property bool hasDatePicker: true

    property date dateToSelect: new Date()

    property int fontPointSizeBig: 15
    property int fontPointSizeNormal: 12

    signal selected(date selectedDate)
    signal canceled()

    onSelected: console.log("DATE:", selectedDate)
    onCanceled: console.log("CANCEL")

    function getSelectedTime() {
        if (hasDatePicker && hasTimePicker) {
            let ddate = datepicker.date
            let time = timepicker.time
            var newDate = new Date(
                ddate.getFullYear(),
                ddate.getMonth(),
                ddate.getDate(),
                time.getHours(),
                time.getMinutes(),
                time.getSeconds(),
                time.getMilliseconds()
            )
            return newDate
        } else if (hasDatePicker) {
            return datepicker.date
        } else if (hasTimePicker) {
            return timepicker.time
        }
        return new Date()
    }

    function setDate(toDate) {
        datepicker.setDate(toDate)
        timepicker.set(toDate)
    }

    Component.onCompleted: {
        timepicker.set(dateToSelect)
    }

    height: 400
    width: 400

    Column {
        anchors.fill: parent
        spacing: 0

        Rectangle {
                id: titleOfDate

                width: parent.width
                height: 55 // HEADER HEIGHT!

                color: palette.primary_dark

                Rectangle {
                    id: selectedYear

                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                    }

                    visible: root.hasDatePicker
                    height: parent.height / 2
                    color: parent.color
                    Text {
                        id: yearTitle

                        anchors.fill: parent

                        leftPadding: 10
                        topPadding: 10

                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter

                        font.pointSize: root.fontPointSizeBig
                        opacity: yearsList.visible ? 1 : 0.7
                        color: "white"
                        text: calendar.currentYear
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            yearsList.show();
                        }
                    }
                }
                Text {
                    id: selectedWeekDayMonth

                    anchors {
                        left: parent.left
                        right: parent.right
                        top: selectedYear.bottom
                        bottom: parent.bottom
                    }

                    visible: root.hasDatePicker
                    leftPadding: root.fontPointSizeNormal
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: root.fontPointSizeNormal
                    text: calendar.weekNames[calendar.week].slice(0, 3) + ", " + calendar.currentDay + " " + calendar.months[calendar.currentMonth].slice(0, 3)
                    color: "white"
                    opacity: yearsList.visible ? 0.7 : 1
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            yearsList.hide();
                        }
                    }
                }

                Row {
                    layoutDirection: "RightToLeft"

                    anchors {
                        right: parent.right
                        rightMargin: 10
                    }

                    height: parent.height

                    Rectangle {
                        id: okBtn

                        height: parent.height
                        width: okBtnText.contentWidth + 15

                        color: palette.primary_dark

                        Text {
                            id: okBtnText

                            anchors.centerIn: parent

                            font.pointSize: root.fontPointSizeBig
                            font.bold: true

                            color: "#FD9626"
                            text: "Ok"
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                root.selected(getSelectedTime())
                            }
                        }
                    }

                    Rectangle {
                        id: cancelBtn

                        height: parent.height
                        width: cancelBtnText.contentWidth + 15
                        color: palette.primary_dark
                        Text {
                            id: cancelBtnText
                            anchors.centerIn: parent
                            font.pointSize: root.fontPointSizeBig
                            font.bold: true
                            color: "#FD9626"
                            text: "Cancel"
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                root.canceled();
                            }
                        }
                    }
                }
            }

        // TIMEPICKER
        Row {

            height: {
                if (!root.hasTimePicker) return 0
                else if (root.hasDatePicker) return parent.height * 1/5
                else return parent.height / 2
            }

            width: root.width
            visible: root.hasTimePicker

            spacing: 20

            leftPadding: 50

            Text {
                id: timetext

                text: qsTr("Time")
                font.pointSize: root.fontPointSizeNormal
                color: "black"
                anchors.verticalCenter: parent.verticalCenter
            }

            Item {
                id: timepicker

                property date time: new Date()
                property int rows: 3 // number of rows on the screen     (must be odd). Also change model ''
                property int repetitions: 5 // number of times data is repeated (must be odd)

                // public
                function set(toDate) {
                    // e.g. new Date(0, 0, 0,  0, 0)) // 12:00 AM

                    repeater.itemAt(0).positionViewAtIndex(24 * (repetitions - 1) / 2 + toDate.getHours(), ListView.Center) // hour
                    repeater.itemAt(1).positionViewAtIndex(60 / interval * (repetitions - 1) / 2 + toDate.getMinutes() / interval, ListView.Center) // minute
                    repeater.itemAt(2).positionViewAtIndex(60 / interval * (repetitions - 1) / 2 + toDate.getSeconds() / interval, ListView.Center) // am/pm

                    for (var column = 0; column < repeater.count; column++) select(repeater.itemAt(column))
                }

                function get() {
                    return new Date(0, 0, 0,
                        repeater.itemAt(0).get(repeater.itemAt(0).currentIndex), // hour
                        repeater.itemAt(1).get(repeater.itemAt(1).currentIndex), // minute
                        repeater.itemAt(2).get(repeater.itemAt(2).currentIndex)) // second
                }

                function select(view) {
                    view.currentIndex = view.indexAt(0, view.contentY + 0.5 * view.height)
                } // index at vertical center

                signal clicked(date d); //onClicked: print('onClicked', date.toTimeString())

                property int interval: 1 // 30 20 15 10 5 2 1 minutes

                width: parent.width - timetext.width - parent.spacing - parent.leftPadding
                height: parent.height // default size

                // Rectangle {anchors.fill: parent; color:"blue"; opacity:.1}

                clip: true

                // onHeightChanged: resizeTimer.start() // resize

                // Timer {
                //     // ensure same value is selected after resize
                //     id: resizeTimer;
                //     interval: 1000;
                //     onTriggered: timepicker.set(timepicker.get())
                // }

                Row {
                    id: numberspinner

                    anchors.right: parent.right
                    anchors.rightMargin: 50
                    spacing: 10

                    Repeater {
                        id: repeater

                        model: [24*5, 60*5, 60*5] // 1-12 hour, 0-59 minute, am/pm

                        delegate: ListView {
                            // hours minutes am/pm
                            id: view

                            property int column: index // outer index
                            width: timepicker.width / 10
                            height: timepicker.height

                            snapMode: ListView.SnapToItem

                            spacing: 0

                            model: modelData

                            delegate: Item {

                                property bool isSelected: view.currentIndex === index

                                width: 20
                                height: timepicker.height / 3

                                Text {
                                    text: view.get(index)

                                    font.bold: isSelected ? true : false
                                    font.pointSize: isSelected ? root.fontPointSizeBig : fontPointSizeNormal

                                    color: isSelected ? palette.primary_dark :"black"
                                    opacity: isSelected? 1: 0.3

                                    anchors {
                                        verticalCenter: parent.verticalCenter
                                        right: column == 0? parent.right: undefined
                                        horizontalCenter: column == 1? parent.horizontalCenter: undefined
                                        left: column == 2? parent.left: undefined
                                    }
                                }
                            }

                            onMovementEnded: {
                                timepicker.select(view)
                                timer.restart()
                            }
                            onFlickEnded: {
                                timepicker.select(view)
                                timer.restart()
                            }

                            Timer {
                                id: timer
                                interval: 1
                                onTriggered: {
                                    timepicker.time = timepicker.get()
                                    getSelectedTime()
                                }
                            } // emit only once

                            function get(index) {
                                // returns e.g. '00' given row
                                if (column == 0) return ('0' + (index * 1) % 24).slice(-2) // hour
                                else if (column == 1) return ('0' + (index * 1) % 60).slice(-2) // minute
                                else return ('0' + (index * 1) % 60).slice(-2) // seconds
                            }
                        }
                    }
                }

                Text {
                    // separator
                    text: ":"
                    anchors.verticalCenter: numberspinner.verticalCenter
                    anchors.left: numberspinner.left
                    anchors.leftMargin: 29

                    font.bold: true
                    font.pointSize: root.fontPointSizeNormal
                    color: "black"
                }
                Text {
                    // separator
                    text: ":"
                    anchors.verticalCenter: numberspinner.verticalCenter
                    anchors.left: numberspinner.left
                    anchors.leftMargin: 65

                    font.bold: true
                    font.pointSize: root.fontPointSizeNormal
                    color: "black"
                }
            }
        }

        Rectangle {
            id: separator

            color: palette.primary_dark
            width: parent.width
            height: 1
        }

        Rectangle {
            id: datepicker

            property double cellSize: root.height / 20
            property int fontSizePx
            property var date: new Date(calendar.currentYear, calendar.currentMonth, calendar.currentDay);

            function setCurrentDate() {
                datepicker.date = new Date(calendar.currentYear, calendar.currentMonth, calendar.currentDay);
                root.getSelectedTime()
            }

            function setDate(ddate) {
                calendar.currentDay = ddate.getDate();
                calendar.currentMonth = ddate.getMonth();
                calendar.week = ddate.getDay();
                calendar.currentYear = ddate.getFullYear();
                calendarModel.setYear(ddate.getFullYear())
            }

            width: parent.width
            height: {
                if (!root.hasDatePicker) return 0
                else if (root.hasTimePicker) return parent.height * 4/5
                else return parent.height
            }

            clip: true

            QtObject {
                id: palette
                property color primary: "#006146"
                property color primary_dark: "#006146"
                property color primary_light: "#B2EBF2"
                property color accent: "#FF5722"
                property color primary_text: "#212121"
                property color secondary_text: "#757575"
                property color icons: "#FFFFFF"
                property color divider: "#BDBDBD"
            }

            ListView {
                id: calendar

                property int currentDay: new Date().getDate()
                property int currentMonth: new Date().getMonth()
                property int currentYear: new Date().getFullYear()
                property int week: new Date().getDay()

                property var months: [
                    qsTr("January"), qsTr("February"), qsTr("March"), qsTr("April"),
                    qsTr("May"), qsTr("June"), qsTr("July"), qsTr("August"),
                    qsTr("September"), qsTr("October"), qsTr("November"), qsTr("December")
                ]

                property var weekNames: [
                    qsTr("Sunday"), qsTr("Monday"), qsTr("Tuesday"), qsTr("Wednesday"),
                    qsTr("Thursday"), qsTr("Friday"), qsTr("Saturday")
                ]

                anchors {
                    left: parent.left
                    right: parent.right
                }

                height: parent.height - titleOfDate.height
                visible: true

                snapMode: ListView.SnapOneItem
                orientation: ListView.Horizontal

                model: CalendarModel {
                    id: calendarModel

                    function setYear(newYear) {
                        if (calendarModel.from.getFullYear() > newYear) {
                            calendarModel.from = new Date(newYear, 0, 1);
                            calendarModel.to = new Date(newYear, 11, 31);
                        } else {
                            calendarModel.to = new Date(newYear, 11, 31);
                            calendarModel.from = new Date(newYear, 0, 1);
                        }
                        calendar.currentYear = newYear;
                        calendar.goToLastPickedDate();
                        datepicker.setCurrentDate();
                    }

                    from: new Date(new Date().getFullYear(), 0, 1);
                    to: new Date(new Date().getFullYear(), 11, 31);
                }

                delegate: Rectangle {
                    height: datepicker.cellSize * 8.5
                    width: datepicker.width

                    Rectangle {
                        id: monthYearTitle

                        anchors {
                            top: parent.top
                        }

                        height: datepicker.cellSize * 1.3
                        width: parent.width

                        Text {
                            anchors.centerIn: parent
                            font.pointSize: root.fontPointSizeNormal
                            text: calendar.months[model.month] + " " + model.year;
                        }
                    }

                    DayOfWeekRow {
                        id: weekTitles

                        locale: monthGrid.locale

                        anchors {
                            top: monthYearTitle.bottom
                        }

                        height: datepicker.cellSize
                        width: parent.width

                        delegate: Text {
                            text: model.shortName
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.pointSize: root.fontPointSizeNormal
                        }
                    }

                    MonthGrid {
                        id: monthGrid

                        month: model.month
                        year: model.year

                        spacing: 0

                        anchors {
                            top: weekTitles.bottom
                        }

                        width: parent.width
                        height: datepicker.height - titleOfDate.height

                        locale: Qt.locale()

                        delegate: Rectangle {
                            property bool highlighted: enabled && model.day === calendar.currentDay && model.month === calendar.currentMonth

                            height: datepicker.cellSize
                            width: datepicker.cellSize
                            radius: height * 0.5

                            enabled: model.month === monthGrid.month
                            color: enabled && highlighted ? palette.primary_dark : "white"

                            Text {
                                anchors.centerIn: parent
                                text: model.day
                                font.pointSize: root.fontPointSizeNormal
                                scale: highlighted ? 1.25 : 1
                                Behavior on scale {
                                    NumberAnimation {
                                        duration: 150
                                    }
                                }
                                visible: parent.enabled
                                color: parent.highlighted ? "white" : "black"
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    calendar.currentDay = model.date.getDate();
                                    calendar.currentMonth = model.date.getMonth();
                                    calendar.week = model.date.getDay();
                                    calendar.currentYear = model.date.getFullYear();
                                    datepicker.setCurrentDate();
                                }
                            }
                        }
                    }
                }

                Component.onCompleted: goToLastPickedDate()

                function goToLastPickedDate() {
                    positionViewAtIndex(calendar.currentMonth, ListView.SnapToItem)
                }
            }

            ListView {
                id: yearsList

                property int currentYear
                property int startYear: 1940
                property int endYear : new Date().getFullYear() + 50;

                anchors.fill: calendar

                orientation: ListView.Vertical
                visible: false
                clip: true

                model: ListModel {
                    id: yearsModel
                }

                delegate: Rectangle {
                    width: parent.width
                    height: datepicker.cellSize * 1.5
                    Text {
                        anchors.centerIn: parent
                        font.pointSize: root.fontPointSizeNormal
                        text: name
                        scale: index === (yearsList.currentYear - yearsList.startYear) ? 1.5 : 1
                        color: palette.primary_dark
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            calendarModel.setYear(yearsList.startYear + index);
                            yearsList.hide();
                        }
                    }
                }

                Component.onCompleted: {
                    for (var year = startYear; year <= endYear; year ++)
                        yearsModel.append({
                        name: year
                    });
                }
                function show() {
                    visible = true
                    calendar.visible = false
                    currentYear = calendar.currentYear
                    yearsList.positionViewAtIndex(currentYear - startYear, ListView.SnapToItem)
                }
                function hide() {
                    visible = false;
                    calendar.visible = true;
                }
            }

            Rectangle {
                height: datepicker.cellSize * 1.5
                anchors {
                    top: calendar.bottom
                    right: parent.right
                    rightMargin: datepicker.cellSize * 0.5
                }
                color: "black"
            }
        }
    }
}

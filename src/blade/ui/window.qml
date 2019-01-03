/*
 *   Copyright (C) 2018 Ivan Čukić <ivan.cukic(at)kde.org>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) version 3, or any
 *   later version accepted by the membership of KDE e.V. (or its
 *   successor approved by the membership of KDE e.V.), which shall
 *   act as a proxy defined in Section 6 of version 3 of the license.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library.
 *   If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.9

import QtQuick.Window 2.2
import QtQuick.Layouts 1.11
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

PlasmaCore.Dialog {
    id: dialog

    Component.onCompleted: {
        dialog.show();
    }

    ColumnLayout {
        id: root

        Layout.minimumWidth: 320
        Layout.minimumHeight: Screen.desktopAvailableHeight

        Connections {
            target: BladeUiBackend
            onSearchFinished: console.log("Connection valid: " + query)
        }

        PlasmaComponents.TextField {
            Layout.fillWidth: true
            onTextChanged: BladeUiBackend.search(text)
        }

        PlasmaExtras.ScrollArea {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                delegate: PlasmaComponents.ListItem {
                    height: 2 * units.smallSpacing + itemTitle.height

                    PlasmaComponents.Label {
                        id: itemTitle
                        anchors {
                            left: parent.left
                            top: parent.top
                        }
                        text: model.text
                    }
                }

                model: ListModel {
                    ListElement {
                        text: "Hello"
                    }
                    ListElement {
                        text: "World"
                    }
                }
            }
        }
    }
}

/*
    SPDX-FileCopyrightText: 2018 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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
            onSearchFinished: {
                title.text = query;
            }
            // onSearchRequested: console.log(" <---- " + query)
        }

        PlasmaComponents.TextField {
            Layout.fillWidth: true
            onTextChanged: BladeUiBackend.search(text)
        }

        PlasmaExtras.Heading {
            id: title
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

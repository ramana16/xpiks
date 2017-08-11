/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLIPBOARDHELPER
#define CLIPBOARDHELPER

#include <QApplication>
#include <QClipboard>
#include <QObject>
#include "../Common/defines.h"

namespace Helpers {
    class ClipboardHelper : public QObject
    {
        Q_OBJECT
    public:
        explicit ClipboardHelper(QObject *parent = 0) : QObject(parent) {
            clipboard = QApplication::clipboard();
        }

        Q_INVOKABLE QString getText() {
            QString clipboardText = clipboard->text(QClipboard::Clipboard);

            if (clipboardText.length() >= MAX_PASTE_SIZE) {
                clipboardText = clipboardText.left(MAX_PASTE_SIZE);
            }

            return clipboardText;
        }

        Q_INVOKABLE void setText(const QString &text) {
            clipboard->setText(text, QClipboard::Clipboard);
        }

    private:
        QClipboard *clipboard;
    };
}

#endif // CLIPBOARDHELPER


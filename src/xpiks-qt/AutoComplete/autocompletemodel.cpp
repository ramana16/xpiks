/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "autocompletemodel.h"
#include <QQmlEngine>
#include "../Common/defines.h"

namespace AutoComplete {
    AutoCompleteModel::AutoCompleteModel(QObject *parent) :
        QObject(parent),
        m_SelectedIndex(-1),
        m_IsActive(false)
    {
    }

    void AutoCompleteModel::setSelectedIndex(int value) {
        if (value != m_SelectedIndex) {
            LOG_INTEGR_TESTS_OR_DEBUG << value;
            m_SelectedIndex = value;
            emit selectedIndexChanged(value);
        }
    }

    void AutoCompleteModel::setIsActive(bool value) {
        if (value != m_IsActive) {
            m_IsActive = value;
            emit isActiveChanged(value);
        }
    }

    void AutoCompleteModel::initializeCompletions() {
        doInitializeCompletions();
        setSelectedIndex(-1);
        setIsActive(true);
    }

    bool AutoCompleteModel::moveSelectionUp() {
        const bool canMove = m_SelectedIndex > 0;
        LOG_INTEGR_TESTS_OR_DEBUG << "can move:" << canMove;
        if (canMove) {
            setSelectedIndex(m_SelectedIndex - 1);
        }
        return canMove;
    }

    bool AutoCompleteModel::moveSelectionDown() {
        const int size = getCompletionsCount();
        const bool canMove = m_SelectedIndex < size - 1;
        LOG_INTEGR_TESTS_OR_DEBUG << "can move:" << canMove;
        if (canMove) {
            setSelectedIndex(m_SelectedIndex + 1);
        }
        return canMove;
    }

    void AutoCompleteModel::cancelCompletion() {
        LOG_DEBUG << "#";
        clearCompletions();
        setIsActive(false);
        emit dismissPopupRequested();
    }

    void AutoCompleteModel::acceptSelected(bool withMetaAction) {
        LOG_DEBUG << "Selected index:" << m_SelectedIndex << ", meta action:" << withMetaAction;
        const int size = getCompletionsCount();

        if (0 <= m_SelectedIndex && m_SelectedIndex < size) {
            const int id = doAcceptCompletion(m_SelectedIndex, withMetaAction);
            emit completionAccepted(id);
        }

        emit dismissPopupRequested();

        setIsActive(false);
    }

    QObject *AutoCompleteModel::getCompletionsModel() {
        auto *listModel = doGetCompletionsModel();
        QQmlEngine::setObjectOwnership(listModel, QQmlEngine::CppOwnership);
        return listModel;
    }
}

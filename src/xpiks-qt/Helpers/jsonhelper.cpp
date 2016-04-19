/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2016 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "jsonhelper.h"

namespace Helpers {
    bool mergeJsonArrays (const QJsonArray &arrayFrom, QJsonArray &arrayTo, CompareValuesJson &comparer);
    bool mergeJsonObjects (const QJsonObject &objectMergeFrom, QJsonObject &objectMergeTo, CompareValuesJson &comparer);

    bool mergeArraysOfObjects(const QJsonArray &arrayFrom, QJsonArray &arrayTo, CompareValuesJson &comparer);
    bool containsObject(const QJsonArray &array, int minIndex, const QJsonObject &object, CompareValuesJson &comparer);
    bool mergeArraysOfStrings(const QJsonArray &arrayFrom, QJsonArray &arrayTo);

    bool mergeJsonArrays (const QJsonArray &arrayFrom, QJsonArray &arrayTo, CompareValuesJson &comparer) {
        LOG_DEBUG << "#";

        if (arrayTo.isEmpty()) {
            LOG_DEBUG << "ArrayTo is empty";
            arrayTo = arrayFrom;
            return true;
        }

        if (arrayFrom.isEmpty()) {
            LOG_DEBUG << "ArrayFrom is empty";
            return false;
        }

        if (arrayTo.first().type() != arrayFrom.first().type() ) {
            LOG_WARNING << "Type of arrays to be merged does not match!";
            return false;
        }

        bool mergeResult = false;
        int type = arrayTo.first().type();

        if (type == QJsonValue::Object) {
            mergeResult = mergeArraysOfObjects(arrayFrom, arrayTo, comparer);
        } else if (type == QJsonValue::String) {
            mergeResult = mergeArraysOfStrings(arrayFrom, arrayTo);
        } else {
            LOG_WARNING << "Unsupported type of QJsonArray:" << type;
        }

        return mergeResult;
    }

    bool mergeArraysOfObjects(const QJsonArray &arrayFrom, QJsonArray &arrayTo, CompareValuesJson &comparer) {
        LOG_DEBUG << "#";

        int minSize = qMin(arrayTo.size(), arrayFrom.size());

        int i = 0;
        // skip same elements
        while (i < minSize) {
            QJsonObject objectTo = arrayTo[i].toObject();
            QJsonObject objectFrom = arrayFrom[i].toObject();

            if (comparer(objectTo, objectFrom) == 0) {
                i++;
            } else {
                break;
            }
        }

        LOG_DEBUG << i << "same elements skipped";

        int maxIndexOfEqual = i;

        QJsonArray mergedArray = arrayTo;

        int sizeFrom = arrayFrom.size();
        QJsonArray elementsToAdd;

        for (; i < sizeFrom; i++) {
            Q_ASSERT(arrayFrom[i].isObject());
            QJsonObject objectToAdd = arrayFrom[i].toObject();

            bool alreadyExists = containsObject(arrayTo, maxIndexOfEqual, objectToAdd, comparer);
            if (!alreadyExists) {
                elementsToAdd.append(objectToAdd);
            }
        }

        LOG_DEBUG << elementsToAdd.size() << "new elements to be added";

        int sizeMergedAdd = elementsToAdd.size();
        for (int t = 0; t < sizeMergedAdd; t++) {
            mergedArray.append(elementsToAdd[t]);
        }

        arrayTo = mergedArray;

        return true;
    }

    bool containsObject(const QJsonArray &array, int minIndex, const QJsonObject &object, CompareValuesJson &comparer) {
        bool found = false;
        Q_ASSERT(minIndex < array.size());

        int size = array.size();

        for (int i = minIndex; i < size; ++i) {
            Q_ASSERT(array[i].type() == QJsonValue::Object);

            QJsonObject objectInArray = array[i].toObject();

            if (comparer(object, objectInArray) == 0) {
                found = true;
                break;
            }
        }

        return found;
    }

    bool mergeArraysOfStrings(const QJsonArray &arrayFrom, QJsonArray &arrayTo) {
        QJsonArray arrayMerged;

        QSet<QString> commonValues;
        commonValues.reserve(arrayTo.size() + arrayFrom.size());

        int i = 0;
        int sizeTo = arrayTo.size();
        for (i = 0; i < sizeTo; ++i) {
            Q_ASSERT(arrayTo[i].type() == QJsonValue::String);
            commonValues.insert(arrayTo[i].toString());
        }

        int sizeFrom = arrayFrom.size();
        for (i = 0; i < sizeFrom; ++i) {
            Q_ASSERT(arrayFrom[i].type() == QJsonValue::String);
            commonValues.insert(arrayFrom[i].toString());
        }

        QSet<QString>::iterator begin = commonValues.begin();
        QSet<QString>::iterator end = commonValues.end();

        for (QSet<QString>::iterator it = begin; it != end; ++it) {
            arrayMerged.append(*it);
        }

        arrayTo = arrayMerged;
        return true;
    }

    bool mergeJsonObjects (const QJsonObject &objectMergeFrom, QJsonObject &objectMergeTo, CompareValuesJson &comparer) {
        LOG_DEBUG << "#";

        QStringList keysMergeFrom = objectMergeFrom.keys();
        int keysSize = keysMergeFrom.size();

        bool anyError = false;

        for (int i = 0; i < keysSize; i++) {
            const QString &keyFrom = keysMergeFrom.at(i);

            if (objectMergeTo.contains(keyFrom)) {
                QJsonValue valueTo = objectMergeTo[keyFrom];
                QJsonValue valueFrom = objectMergeFrom[keyFrom];

                if (valueTo.type() != valueFrom.type()) {
                    LOG_WARNING << "Types of Json Values do not match at key:" << keyFrom;
                    continue;
                }

                if (valueTo.isObject()) {
                    QJsonObject objectTo = valueTo.toObject();
                    QJsonObject objectFrom = valueFrom.toObject();

                    if (mergeJsonObjects(objectFrom, objectTo, comparer)) {
                        valueTo = objectTo;
                    } else {
                        anyError = true;
                        break;
                    }
                } else if (valueTo.isArray()) {
                    QJsonArray arrayTo = valueTo.toArray();
                    QJsonArray arrayFrom = valueFrom.toArray();

                    if (mergeJsonArrays(arrayFrom, arrayTo, comparer)) {
                        valueTo = arrayTo;
                    } else {
                        anyError = true;
                        break;
                    }
                } else {
                    valueTo = valueFrom;
                }

                objectMergeTo[keyFrom] = valueTo;

            } else {
                objectMergeTo[keyFrom] = objectMergeFrom[keyFrom]; // insert if doesn't contain
            }
        }

        bool mergeResult = !anyError;
        return mergeResult;
    }

    void mergeJson(const QJsonDocument &mergeFrom, QJsonDocument &mergeTo, bool overwrite, CompareValuesJson &comparer) {
        if (mergeFrom.isNull()) {
            LOG_WARNING << "attempted to merge with Null json document";
            return;
        }

        if (overwrite || mergeTo.isNull()) {
            LOG_DEBUG << "Overwriting the document";
            mergeTo = mergeFrom;
        } else {
            if (mergeFrom.isArray()) {
                QJsonArray arrayFrom = mergeFrom.array();
                QJsonArray arrayTo = mergeTo.array();

                if (mergeJsonArrays(arrayFrom, arrayTo, comparer)) {
                    mergeTo.setArray(arrayTo);
                }
            } else {
                QJsonObject objectFrom = mergeFrom.object();
                QJsonObject objectTo = mergeTo.object();

                if (mergeJsonObjects(objectFrom, objectTo, comparer)) {
                    mergeTo.setObject(objectTo);
                }
            }
        }
    }
}

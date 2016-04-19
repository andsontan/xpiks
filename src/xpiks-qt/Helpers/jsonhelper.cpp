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
    bool mergeJsonArrays (QJsonArray &arrayTo, const QJsonArray &arrayFrom, CompareValuesJson &comparer);
    bool mergeJsonObjects (QJsonObject &jsonToObj, QJsonObject &jsonWhatObj, CompareValuesJson &comparer);

    bool mergeArraysOfObjects(QJsonArray &arrayTo, const QJsonArray &arrayFrom, CompareValuesJson &comparer);
    bool containsObject(const QJsonArray &array, int maxIndex, const QJsonObject &object, CompareValuesJson &comparer);
    bool mergeArraysOfStrings(QJsonArray &arrayTo, const QJsonArray &arrayFrom, CompareValuesJson &comparer);

    bool mergeJsonArrays (QJsonArray &arrayTo, const QJsonArray &arrayFrom, CompareValuesJson &comparer) {
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
            mergeResult = mergeArraysOfObjects(arrayTo, arrayFrom, comparer);
        } else if (type == QJsonValue::String) {
            mergeResult = mergeArraysOfStrings(arrayTo, arrayFrom, comparer);
        } else {
            LOG_WARNING << "Unsupported type of QJsonArray:" << type;
        }

        return mergeResult;
    }

    bool mergeArraysOfObjects(QJsonArray &arrayTo, const QJsonArray &arrayFrom, CompareValuesJson &comparer) {
        LOG_DEBUG << "#";

        int minSize = qMin(arrayTo.size(), arrayFrom.size());

        int i = 0;
        // skip same elements
        while (i < minSize) {
            QJsonObject objectTo = arrayTo[i].toObject();
            QJsonObject objectWhat = arrayFrom[i].toObject();

            if (comparer(objectTo, objectWhat) == 0) {
                i++;
            } else {
                break;
            }
        }

        LOG_DEBUG << i << "same elements skipped";

        int maxIndexOfEqual = i;

        QJsonArray arrayMerged = arrayFrom;

        int sizeTo = arrayTo.size();
        QJsonArray arrayMergedAdd;

        for (; i < sizeTo; i++) {
            QJsonObject objectTo = arrayTo[i].toObject();

            bool exists = containsObject(arrayFrom, maxIndexOfEqual, objectTo, comparer);
            if (!exists) {
                arrayMergedAdd.append(objectTo);
            }
        }

        LOG_DEBUG << arrayMergedAdd.size() << "new elements to be added";

        int sizeMergedAdd = arrayMergedAdd.size();
        for (int t = 0; t < sizeMergedAdd; t++) {
            arrayMerged.append(arrayMergedAdd[t]);
        }

        arrayTo = arrayMerged;

        return true;
    }

    bool containsObject(const QJsonArray &array, int maxIndex, const QJsonObject &object, CompareValuesJson &comparer) {
        bool found = false;
        Q_ASSERT(maxIndex <= array.size());

        for (int i = 0; i < maxIndex; ++i) {
            Q_ASSERT(array[i].type() == QJsonValue::Object);

            QJsonObject objectInArray = array[i].toObject();

            if (comparer(object, objectInArray) == 0) {
                found = true;
                break;
            }
        }

        return found;
    }

    bool mergeArraysOfStrings(QJsonArray &arrayTo, const QJsonArray &arrayFrom, CompareValuesJson &comparer) {
        QJsonArray arrayMerged;

        Q_UNUSED(comparer);

        int i = 0;
        QSet<QString> commonValues;

        int minSize = qMin(arrayFrom.size(), arrayTo.size());

        for (i = 0; i < minSize; i++ ) {
            Q_ASSERT(arrayTo[i].type() == arrayFrom[i].type());

            commonValues.insert(arrayTo[i].toString());
            commonValues.insert(arrayFrom[i].toString());
        }

        // allow reordering of first values
        Q_ASSERT(commonValues.size() == minSize);

        int fromSize = arrayFrom.size();
        int toSize = arrayTo.size();

        if (fromSize >= toSize) {
            for (; i < fromSize; ++i) {
                commonValues.insert(arrayFrom[i].toString());
            }
        } else {
            for (; i < toSize; i++ ) {
                commonValues.insert(arrayTo[i].toString());
            }
        }

        QSet<QString>::iterator begin = commonValues.begin();
        QSet<QString>::iterator end = commonValues.end();
        QSet<QString>::iterator it;

        for (it = begin; it != end; ++it) {
            arrayMerged.append(*it);
        }

        arrayTo = arrayMerged;
        return true;
    }

    bool mergeJsonObjects (QJsonObject &objectMergeTo, QJsonObject &objectMergeFrom, CompareValuesJson &comparer) {
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

                    if (mergeJsonObjects(objectTo, objectFrom, comparer)) {
                        valueTo = objectTo;
                    } else {
                        anyError = true;
                        break;
                    }
                } else if (valueTo.isArray()) {
                    QJsonArray arrayTo = valueTo.toArray();
                    QJsonArray arrayFrom = valueFrom.toArray();

                    if (mergeJsonArrays(arrayTo, arrayFrom, comparer)) {
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

    void mergeJson(QJsonDocument &mergeTo, const QJsonDocument &mergeWith, bool overwrite, CompareValuesJson &isEqual) {
        if (mergeWith.isNull()) {
            LOG_WARNING << "attempted to merge with Null json document";
            return;
        }

        if (overwrite || mergeTo.isNull()) {
            LOG_DEBUG << "Overwriting the document";
            mergeTo = mergeWith;
        } else {
            if (mergeWith.isArray()) {
                QJsonArray arrayWith = mergeWith.array();
                QJsonArray arrayTo = mergeTo.array();

                if (mergeJsonArrays(arrayTo, arrayWith, isEqual)) {
                    mergeTo.setArray(arrayTo);
                }
            } else {
                QJsonObject objWith = mergeWith.object();
                QJsonObject objTo = mergeTo.object();

                if (mergeJsonObjects(objTo, objWith, isEqual)) {
                    mergeTo.setObject(objTo);
                }
            }
        }
    }
}

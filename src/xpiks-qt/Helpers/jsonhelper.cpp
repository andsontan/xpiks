#include "jsonhelper.h"

namespace Helpers {

    bool mergeJsonArrays (QJsonArray &arrayTo, QJsonArray &arrayWhat,CompareValuesJson &compare);
    bool mergeJsonObjects (QJsonObject &jsonToObj, QJsonObject &jsonWhatObj,CompareValuesJson &compare);

    bool mergeJsonArrays (QJsonArray &arrayTo, QJsonArray &arrayWhat,CompareValuesJson &isEqual) {
        int minSize = std::min(arrayTo.size(), arrayWhat.size());
        int maxSize = std::max(arrayTo.size(), arrayWhat.size());
        if (minSize == 0) {
            if (maxSize == 0) {
                LOG_WARNING<< "both json arrays for merge are empty!";
                return true;
            } else {
                arrayTo = arrayWhat;
                return true;
            }
        } else if ( arrayTo[0].type() != arrayWhat[0].type() ) {
            LOG_WARNING << "types of arrays to be merged do not match!";
            return false;
        }
        QJsonArray arrayMerged;
        int  type = arrayTo[0].type();
        if (type == QJsonValue::Object) {
            int k = 0;
            while (k < minSize) {
                QJsonObject objectTo(arrayTo[k].toObject());
                QJsonObject objectWhat(arrayWhat[k].toObject());
                if (isEqual(objectTo, objectWhat)) {
                    k++;
                } else {
                    break;
                }
            }
            arrayMerged = arrayWhat;
            int sizeTo = arrayTo.size();
            QJsonArray arrayMergedAdd;
            int searchLimit = k;
            for (; k<sizeTo; k++) {
                QJsonObject objectTo(arrayTo[k].toObject());
                bool found=false;
                for (int t=0; t<searchLimit; t++) {
                    QJsonObject objectWhat(arrayMerged[t].toObject());
                    if  (isEqual(objectTo, objectWhat)) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    arrayMergedAdd.append(arrayTo[k].toObject());
                }
            }
            int sizeMergedAdd = arrayMergedAdd.size();
            for (int t=0; t < sizeMergedAdd; t++) {
                arrayMerged.append(arrayMergedAdd[t]);
            }
            arrayTo=arrayMerged;
        } else if (type == QJsonValue::String) {
            int i =0;
            QSet<QString> tempSet;
            for (i = 0; i < minSize; i++ ) {
                tempSet.insert(arrayTo[i].toString());
                tempSet.insert(arrayWhat[i].toString());
            }
            if ( arrayWhat.size() > arrayTo.size() ) {
                for ( ; i< maxSize; i++ ) {
                    tempSet.insert(arrayWhat[i].toString());
                }
            } else {
                for ( ; i< maxSize; i++ ) {
                    tempSet.insert(arrayTo[i].toString());
                }
            }
            QSet<QString>::iterator begin= tempSet.begin();
            QSet<QString>::iterator end = tempSet.end();
            QSet<QString>::iterator it;
            for (it = begin; it != end; ++it) {
                arrayMerged.append(*it);
            }
            arrayTo=arrayMerged;
        } else {
            LOG_WARNING << "Unsupported type of JsonArray!";
        }
        return true;
    }

    bool mergeJsonObjects (QJsonObject &jsonToObj, QJsonObject &jsonWhatObj,CompareValuesJson &isEqual) {
        QStringList keys = jsonWhatObj.keys();
        int keysSize = keys.size();
        for (int i = 0; i < keysSize; i++) {
            QString key = keys.at(i);
            if (jsonToObj.contains(key)) {
                QJsonValue valueTo = jsonToObj[key];
                QJsonValue valueWhat = jsonWhatObj[key];
                if (valueTo.type() != valueWhat.type()) {
                    LOG_WARNING << "types of Json Values do not match!";
                    continue;
                } else {
                    if (valueTo.isObject()) {
                        QJsonObject valTo(valueTo.toObject());
                        QJsonObject valWhat(valueWhat.toObject());
                        if (mergeJsonObjects(valTo,valWhat,isEqual)) {
                            valueTo=valTo;
                        } else {
                            return false;
                        }
                    } else if (valueTo.isArray()) {
                        QJsonArray arrayTo = valueTo.toArray();
                        QJsonArray arrayWhat = valueWhat.toArray();
                        if (mergeJsonArrays(arrayTo, arrayWhat,isEqual)) {
                            valueTo = arrayTo;
                        } else {
                            return false;
                        }
                    } else {
                        valueTo = valueWhat;
                    }
                }
                jsonToObj[key] = valueTo;
            } else {
                jsonToObj[key]=jsonWhatObj[key]; // insert if doesn't contain
            }
        }
        return true;
    }

    void mergeJson(QJsonDocument &mergeTo, const QJsonDocument &mergeWith, int overwrite,CompareValuesJson &isEqual) {
        if (mergeWith.isNull()) {
            LOG_WARNING << "attempted to merge with empty json Document";
            return;
        }
        if (overwrite || mergeTo.isNull()) {
            mergeTo = mergeWith;
        } else {
            if (mergeWith.isArray()) {
                QJsonArray arrayWith = mergeWith.array();
                QJsonArray arrayTo = mergeTo.array();
                if (mergeJsonArrays(arrayTo,arrayWith,isEqual)) {
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

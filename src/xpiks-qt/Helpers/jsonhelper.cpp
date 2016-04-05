#include "jsonhelper.h"

namespace Helpers {

    void mergeArray(QJsonArray & mergeTo, const QJsonArray & mergeFrom){
        int sizeFrom = mergeFrom.size();
        int sizeTo = mergeTo.size();
        QJsonValue val = mergeFrom.at(0);

        if ( val.isObject() ){
            QJsonObject obj = val.toObject();
            QString idKey  = obj.begin().key(); // the main id
            QStringList keys = obj.keys();
            keys.removeOne(idKey); // leave only the  data fields


            for (int i =0; i < sizeFrom; i++){
                QJsonValue valFrom = mergeFrom.at(i);
                QJsonObject objFrom = valFrom.toObject();
                QJsonValue idVal = objFrom[idKey];

                int found = false;
                for (int k = 0; k < sizeTo; k++){
                    QJsonValue valTo = mergeTo.at(k);
                    QJsonObject objTo = valTo.toObject();
                    if (objTo[idKey] == idVal){
                        int keysSize = keys.size();
                        for (int t =0; t < keysSize; t++){
                            objTo[keys.at(t)] = objFrom[keys.at(t)];
                        }
                        mergeTo.replace(k,objTo);
                        found = true;
                        break;
                    }
                }
                if (!found){
                    mergeTo.append(objFrom);
                }
        }
    }

 }



    void mergeJson(QJsonDocument &mergeTo, const QJsonDocument &mergeWith, int overwrite){
        if (mergeWith.isNull()) {
            LOG_WARNING << "attempted to merge with empty json Document";
            return;
        }
        if (overwrite || mergeTo.isNull()) {
            mergeTo = mergeWith;
        } else {
            if (mergeWith.isObject()){
                QJsonObject objWith = mergeWith.object();
                QJsonObject objTo = mergeTo.object();
                QStringList keys = objWith.keys();
                int keysSize = keys.size();
                for (int i = 0; i < keysSize; i++){
                    QString key = keys.at(i);
                    if (objTo[key].isArray()){
                        QJsonArray arTo = objTo[key].toArray();
                        QJsonArray arFrom = objWith[key].toArray();
                        mergeArray(arTo,arFrom);
                        objTo[key]=arTo;
                    }else{
                        objTo[key] = objWith[key];
                    }
                }
               mergeTo.setObject(objTo);
            }else{
                QJsonArray arTo =mergeTo.array();
                QJsonArray arFrom = mergeWith.array();
                mergeArray(arTo,arFrom);
                mergeTo.setArray(arTo);
            }

        }

    }
}

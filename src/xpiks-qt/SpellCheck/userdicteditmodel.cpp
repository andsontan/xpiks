#include "userdicteditmodel.h"
#include "../Helpers/keywordshelpers.h"
#include "../Commands/commandmanager.h"
#include "spellcheckerservice.h"

namespace SpellCheck {
    UserDictEditModel::UserDictEditModel(QObject *parent):
        QObject(parent),
        ArtworkProxyBase(),
        m_BasicModel(m_HoldPlaceholder, this)
    {
        m_BasicModel.setSpellCheckInfo(&m_SpellCheckInfo);
    }

    void UserDictEditModel::initializeModel() {
        LOG_DEBUG << "#";
        auto *spellCheckService = m_CommandManager->getSpellCheckerService();
        auto keywordsList = spellCheckService->getUserDictionary();
        m_BasicModel.setKeywords(keywordsList);
    }

    void UserDictEditModel::removeKeywordAt(int keywordIndex) {
        LOG_INFO << keywordIndex;
        QString keyword;
        doRemoveKeywordAt(keywordIndex, keyword);
    }

    void UserDictEditModel::removeLastKeyword() {
        LOG_DEBUG << "#";
        QString keyword;
        doRemoveLastKeyword(keyword);
    }

    void UserDictEditModel::appendKeyword(const QString &keyword) {
        LOG_INFO << keyword;
        doAppendKeyword(keyword);
    }

    void UserDictEditModel::clearKeywords() {
        LOG_DEBUG << "#";
        doClearKeywords();
    }

    void UserDictEditModel::resetModel() {
        LOG_DEBUG << "#";
        m_BasicModel.clearModel();
        auto *service = m_CommandManager->getSpellCheckerService();
        service->clearUserDictionary();
    }

    void UserDictEditModel::saveUserDict() {
        LOG_DEBUG << "#";

        auto keywords = m_BasicModel.getKeywords();
        auto *spellCheckService = m_CommandManager->getSpellCheckerService();
        spellCheckService->updateUserDictionary(keywords);
    }
}

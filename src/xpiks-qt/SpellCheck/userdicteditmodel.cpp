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
        auto *spellCheckService = m_CommandManager->getSpellCheckerService();
        auto keywordsSet = spellCheckService->getUserDictionary();
        m_BasicModel.setKeywords(keywordsSet.toList());
    }

    void UserDictEditModel::removeKeywordAt(int keywordIndex) {
        QString keyword;
        doRemoveKeywordAt(keywordIndex, keyword);
    }

    void UserDictEditModel::removeLastKeyword() {
        QString keyword;
        doRemoveLastKeyword(keyword);
    }

    void UserDictEditModel::appendKeyword(const QString &keyword) {
        doAppendKeyword(keyword);
    }

    void UserDictEditModel::clearKeywords() {
        doClearKeywords();
    }

    void UserDictEditModel::resetModel() {
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

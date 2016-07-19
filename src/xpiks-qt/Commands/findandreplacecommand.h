#ifndef FINDANDREPLACECOMMAND_H
#define FINDANDREPLACECOMMAND_H

#include "commandbase.h"
#include <QString>
#include <QVector>

namespace Models {
    class PreviewMetadataElement;
    class ArtworkMetadata;
}

namespace Commands {
    class FindAndReplaceCommand:
        public CommandBase
    {
    public:
        FindAndReplaceCommand(std::vector<Models::PreviewMetadataElement> &metadataElements,
                              const QString &replaceWhat, const QString &replaceTo, int flags):
            CommandBase(FindAndReplaceCommandType),
            m_MetadataElements(std::move(metadataElements)),
            m_ReplaceWhat(replaceWhat),
            m_ReplaceTo(replaceTo),
            m_Flags(flags)
        {}

        virtual ~FindAndReplaceCommand();

    public:
        std::shared_ptr<Commands::ICommandResult> execute(const ICommandManager *commandManagerInterface) const;

    private:
        std::vector<Models::PreviewMetadataElement> m_MetadataElements;
        QString m_ReplaceWhat;
        QString m_ReplaceTo;
        int m_Flags;
    };

    class FindAndReplaceCommandResult:
        public CommandResult
    {
    public:
        FindAndReplaceCommandResult(
        const QVector<Models::ArtworkMetadata *> &itemsToSave,
        const QVector<int> &indicesToUpdate):
            m_ItemsToSave(itemsToSave),
            m_IndicesToUpdate(indicesToUpdate)
        {}

    public:
        virtual void afterExecCallback(const ICommandManager *commandManagerInterface) const;

#ifndef CORE_TESTS
    private:
#else
    public:
#endif
        QVector<Models::ArtworkMetadata *> m_ItemsToSave;
        QVector<int> m_IndicesToUpdate;
    };
}

#endif // FINDANDREPLACE_H

#ifndef COMMANDMANAGERMOCK_H
#define COMMANDMANAGERMOCK_H

#include "../../xpiks-qt/Commands/commandmanager.h"
#include "../../xpiks-qt/Models/artitemsmodel.h"
#include "../../xpiks-qt/Models/artworksrepository.h"
#include "../../xpiks-qt/Models/artworkmetadata.h"
#include "../../xpiks-qt/Models/imageartwork.h"
#include "../../xpiks-qt/Helpers/filehelpers.h"

namespace Mocks {
    class CommandManagerMock : public Commands::CommandManager
    {
    public:
        CommandManagerMock():
            m_AnyCommandProcessed(false),
            m_CanExecuteCommands(true)
        {}

    public:
        bool anyCommandProcessed() const { return m_AnyCommandProcessed; }
        void resetAnyCommandProcessed() { m_AnyCommandProcessed = false; }
        void disableCommands() { m_CanExecuteCommands = false; }
        void enableCommands() { m_CanExecuteCommands = true; }
        void mockAcceptDeletion() { getDelegator()->removeUnavailableFiles();}

    public:
        void generateAndAddArtworks(size_t count, bool withVector=true) {
            generateAndAddArtworksEx(count, 2, withVector);
        }

        void generateAndAddArtworksEx(size_t count, int dirsCount, bool withVector) {
            Q_ASSERT(count >= 0);
            Q_ASSERT(dirsCount > 0);
            size_t i = 0;
            Models::ArtworksRepository *artworksRepository = getArtworksRepository();
            Models::ArtItemsModel *artItemsModel = getArtItemsModel();

            while (i < count) {
                QString filename = QString(ARTWORK_PATH).arg(i%dirsCount).arg(i);
                QString vectorname = QString(VECTOR_PATH).arg(i%dirsCount).arg(i);
                qint64 directoryID;

                if (artworksRepository->accountFile(filename, directoryID)) {
                    Models::ArtworkMetadata *artwork = artItemsModel->createMetadata(filename, directoryID);
                    artwork->initAsEmpty();

                    Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork*>(artwork);

                    this->connectArtworkSignals(artwork);

                    if (withVector) {
                        image->attachVector(vectorname);
                    }

                    artItemsModel->appendArtwork(artwork);
                } else {
                    Q_ASSERT(false);
                }

                i++;
            }
        }

        virtual std::shared_ptr<Commands::ICommandResult> processCommand(const std::shared_ptr<Commands::ICommandBase> &command) {
            m_AnyCommandProcessed = true;
            if (m_CanExecuteCommands) {
                return Commands::CommandManager::processCommand(command);
            } else {
                return std::shared_ptr<Commands::ICommandResult>();
            }
        }

        void mockDeletion(int count) {
            for (int i = 0; i < count; ++i) {
                CommandManager::getArtItemsModel()->getArtwork(i)->setUnavailable();
            }
        }

    private:
        bool m_AnyCommandProcessed;
        volatile bool m_CanExecuteCommands;
    };
}

#endif // COMMANDMANAGERMOCK_H

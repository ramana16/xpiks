#include "masterpasswordtest.h"
#include "../../xpiks-qt/Models/artworkuploader.h"
#include "../../xpiks-qt/Encryption/secretsmanager.h"
#include "../../xpiks-qt/Models/uploadinforepository.h"
#include "../../xpiks-qt/Models/uploadinfo.h"

QString MasterPasswordTest::testName() {
    return QLatin1String("MasterPasswordTest");
}

void MasterPasswordTest::setup() {
    Models::UploadInfoRepository *uploadRepo = m_CommandManager->getUploadInfoRepository();

    auto remote1 = uploadRepo->appendItem();
    const QString host1 = "ftp://random.host.com/";
    remote1->setHost(host1);
    remote1->setUsername("john");
    uploadRepo->setData(uploadRepo->index(0), "doe", Models::UploadInfoRepository::EditPasswordRole);

    auto remote2 = uploadRepo->appendItem();
    const QString host2 = "ftp://another.host.com/";
    remote2->setHost(host2);
    remote2->setUsername("doe");
    uploadRepo->setData(uploadRepo->index(1), "john", Models::UploadInfoRepository::EditPasswordRole);
}

int MasterPasswordTest::doTest() {
    Encryption::SecretsManager *secretsManager = m_CommandManager->getSecretsManager();
    const QString masterPassword = "brand new mp";

    Models::UploadInfoRepository *uploadRepo = m_CommandManager->getUploadInfoRepository();
    auto &uploadInfos = uploadRepo->getUploadInfos();

    QString pswd1 = uploadRepo->data(uploadRepo->index(0), Models::UploadInfoRepository::PasswordRole).toString();
    QString pswd2 = uploadRepo->data(uploadRepo->index(1), Models::UploadInfoRepository::PasswordRole).toString();

    bool changed = secretsManager->changeMasterPassword(true, "", masterPassword);
    VERIFY(changed, "Failed to change master password");
    secretsManager->purgeMasterPassword();

    VERIFY(uploadInfos[0]->getPassword() != pswd1, "Failed to do initial passwords encoding for first item");
    VERIFY(uploadInfos[1]->getPassword() != pswd2, "Failed to do initial passwords encoding for second item");

    VERIFY(!secretsManager->testMasterPassword("some other mp"), "Other master password works");
    VERIFY(secretsManager->testMasterPassword(masterPassword), "Original master password does not work");

    Models::ArtworkUploader *uploader = m_CommandManager->getArtworkUploader();

    uploader->resetModel();

    secretsManager->setMasterPassword(masterPassword);
    uploadRepo->initializeAccounts(true);

    QVariant decodedPswd1 = uploadRepo->data(uploadRepo->index(0), Models::UploadInfoRepository::PasswordRole);
    qDebug() << "decoded first password" << decodedPswd1;
    VERIFY(decodedPswd1.toString() == pswd1, "Decoded password 1 does not match");

    QVariant decodedPswd2 = uploadRepo->data(uploadRepo->index(1), Models::UploadInfoRepository::PasswordRole);
    qDebug() << "decoded second password" << decodedPswd2;
    VERIFY(decodedPswd2.toString() == pswd2, "Decoded password 2 does not match");

    return 0;
}


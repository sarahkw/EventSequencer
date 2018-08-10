#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QTemporaryDir>
#include <QDebug>

#include <memory>

#include "managedresources.h"

struct ManagedResourcesTest : public testing::Test {
    ManagedResources mres_;

    std::unique_ptr<QTemporaryDir> tempDir_;

    void setTempDirectory()
    {
        tempDir_.reset(new QTemporaryDir);
        mres_.setFileResourceDirectory(tempDir_->path());
        qInfo() << "fileResourceDirectory is" << mres_.fileResourceDirectory();
    }

};

TEST_F(ManagedResourcesTest, CreateFile)
{
    setTempDirectory();

    QUrl genUrl = mres_.generateResourceUrl(".txt");
    qInfo() << "genUrl" << genUrl;
    QFile f;
    ASSERT_TRUE(mres_.urlFile(genUrl, &f));
    ASSERT_TRUE(f.open(QFile::WriteOnly));
    f.close();

    QString filePath;
    ASSERT_TRUE(mres_.urlConvertToFilePath(genUrl, &filePath));
    qInfo() << "filePath" << filePath;
    EXPECT_TRUE(QFile::exists(filePath));
}

TEST_F(ManagedResourcesTest, RenameManagedResource)
{
    setTempDirectory();

    QUrl initialUrl = mres_.urlForBaseName("unassigned", ".au");
    EXPECT_EQ(initialUrl.toString(), "evseq://managed/unassigned.au");

    // Make the file so it can be renamed.
    {
        QFile f;
        ASSERT_TRUE(mres_.urlFile(initialUrl, &f));
        ASSERT_TRUE(f.open(QFile::WriteOnly));
        f.close();
    }

    QUrl doneUrl = mres_.renameUrlToFileName(initialUrl, "assigned.au");
    EXPECT_EQ(doneUrl.toString(), "evseq://managed/assigned.au");

    QString filePath;
    ASSERT_TRUE(mres_.urlConvertToFilePath(doneUrl, &filePath));
    qInfo() << "filePath" << filePath;
    EXPECT_TRUE(QFile::exists(filePath));
}

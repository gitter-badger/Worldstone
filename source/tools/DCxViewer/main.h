#pragma once
#include <DCxView.h>
#include <MpqArchive.h>
#include <QApplication>
#include <QListWidget>
#include <QMainWindow>
#include <QUrl>
#include <memory>

using WorldStone::MpqArchive;

class DCxViewerApp : public QApplication
{
    Q_OBJECT
private:
    QString mpqFileName;
    QString listFileName;
    QString                     paletteFile; //< The opened palette
    std::unique_ptr<MpqArchive> mpqArchive;
    QStringList                 mpqFiles;

public:
    DCxViewerApp(int& argc, char** argv);
    ~DCxViewerApp();
    static DCxViewerApp* instance()
    {
        return static_cast<DCxViewerApp*>(QCoreApplication::instance());
    }
    void readSettings();
    void writeSettings();

    const QStringList& getFileList()const { return mpqFiles; };
    const QString&     getPaletteFile() const { return paletteFile; };
    void setFileList(QStringList & newMpqFilesList);
    void updateMpqFileList();
    QString getMpqFileName() { return mpqFileName; }
    /// @warning Be careful not to read multiple files at the same time, it is not supported by
    /// StormLib
    WorldStone::StreamPtr getFilePtr(const QString& fileName);

public slots :
    void openMpq(const QUrl& mpqFileUrl);
    void addListFile(const QUrl& listFileUrl);
    void fileActivated(const QString& fileName);
    void setPaletteFile(const QUrl& folder);

signals:
    void fileListUpdated();
    void requestDisplayDC6(const QString& fileName);
    void paletteFileChanged(const QString& newFolder);
};

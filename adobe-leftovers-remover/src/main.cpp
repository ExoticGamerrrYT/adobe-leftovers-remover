#include <QApplication>
#include <QWidget>
#include <QTabWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QPushButton>
#include <QProgressBar>
#include <QTextEdit>
#include <QDateTime>
#include <QProcessEnvironment>
#include <QDir>
#include <QRegularExpression>
#include <QtConcurrent>
#include <QMetaObject>
#include <atomic>
#include <windows.h>
#include <shellapi.h>
#include "data.h"

// Expand environment variables in a path
QString expandEnv(const QString& raw) {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString result = raw;
    QRegularExpression re("%([^%]+)%");
    auto matches = re.globalMatch(raw);
    while (matches.hasNext()) {
        auto m = matches.next();
        QString var = m.captured(1);
        result.replace(m.captured(0), env.value(var));
    }
    return QDir::toNativeSeparators(result);
}

// Thread-safe deletion of file paths using shared counter
void deletePathsThread(const QList<QCheckBox*>& cbs,
    QTextEdit* log,
    QProgressBar* progress,
    std::atomic<int>* doneCounter)
{
    for (int i = 0; i < cbs.size(); ++i) {
        if (!cbs[i]->isChecked()) continue;
        QString raw = paths.at(i);
        QString path = expandEnv(raw);
        QDir dir(path);
        bool exists = dir.exists();
        bool success = exists && dir.removeRecursively();
        QString ts = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        QString msg;
        if (!exists) msg = QString("[%1] Path not found: %2").arg(ts).arg(path);
        else if (success) msg = QString("[%1] Deleted path: %2").arg(ts).arg(path);
        else msg = QString("[%1] Failed to delete path: %2").arg(ts).arg(path);
        QMetaObject::invokeMethod(log, "append", Qt::QueuedConnection, Q_ARG(QString, msg));
        int done = doneCounter->fetch_add(1) + 1;
        QMetaObject::invokeMethod(progress, "setValue", Qt::QueuedConnection, Q_ARG(int, done));
    }
}

// Thread-safe deletion of registry keys using shared counter
void deleteRegistryThread(const QList<QCheckBox*>& cbs,
    QTextEdit* log,
    QProgressBar* progress,
    std::atomic<int>* doneCounter)
{
    for (int i = 0; i < cbs.size(); ++i) {
        if (!cbs[i]->isChecked()) continue;
        QString raw = registryKeys.at(i);
        HKEY hive = nullptr;
        if (raw.startsWith("HKEY_LOCAL_MACHINE\\")) hive = HKEY_LOCAL_MACHINE;
        else if (raw.startsWith("HKEY_CURRENT_USER\\")) hive = HKEY_CURRENT_USER;
        else if (raw.startsWith("HKEY_CLASSES_ROOT\\")) hive = HKEY_CLASSES_ROOT;
        int pos = raw.indexOf('\\');
        QString sub = (pos >= 0 ? raw.mid(pos + 1) : raw);
        LONG res = RegDeleteTreeW(hive, reinterpret_cast<LPCWSTR>(sub.utf16()));
        QString ts = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        QString msg;
        if (res == ERROR_SUCCESS || res == ERROR_FILE_NOT_FOUND)
            msg = QString("[%1] Deleted registry key: %2").arg(ts).arg(raw);
        else
            msg = QString("[%1] Failed to delete registry key: %2 (Error %3)").arg(ts).arg(raw).arg(res);
        QMetaObject::invokeMethod(log, "append", Qt::QueuedConnection, Q_ARG(QString, msg));
        int done = doneCounter->fetch_add(1) + 1;
        QMetaObject::invokeMethod(progress, "setValue", Qt::QueuedConnection, Q_ARG(int, done));
    }
}

// Check if running with administrative privileges
bool isRunningAsAdmin() {
    BOOL isAdmin = FALSE;
    PSID adminGroup = NULL;
    SID_IDENTIFIER_AUTHORITY ntAuth = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&ntAuth, 2,
        SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0, &adminGroup)) {
        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }
    return isAdmin;
}

int main(int argc, char* argv[]) {
    // Relaunch as administrator if needed
    if (!isRunningAsAdmin()) {
        wchar_t path[MAX_PATH];
        GetModuleFileNameW(NULL, path, MAX_PATH);
        ShellExecuteW(NULL, L"runas", path, NULL, NULL, SW_SHOWNORMAL);
        return 0;
    }

    QApplication app(argc, argv);
    QWidget window;
    window.setWindowTitle("Adobe Leftovers Remover");
    window.resize(1000, 800);
    window.setFixedSize(window.size()); // Make the window non-resizable

    QVBoxLayout* mainLayout = new QVBoxLayout(&window);
    QTabWidget* tabs = new QTabWidget;

    // Cleanup Tab
    QWidget* cleanupTab = new QWidget;
    QVBoxLayout* cleanupLayout = new QVBoxLayout(cleanupTab);
    QHBoxLayout* groups = new QHBoxLayout;

    // Paths Group
    QGroupBox* pathsGroup = new QGroupBox("Paths");
    QVBoxLayout* pathsLayout = new QVBoxLayout;
    QPushButton* selectAllPaths = new QPushButton("Select All Paths");
    QPushButton* deselectAllPaths = new QPushButton("Deselect All Paths");
    pathsLayout->addWidget(selectAllPaths);
    pathsLayout->addWidget(deselectAllPaths);
    QList<QCheckBox*> pathCheckboxes;
    for (const QString& s : paths) {
        auto cb = new QCheckBox(s);
        pathCheckboxes.append(cb);
        pathsLayout->addWidget(cb);
    }
    pathsLayout->addStretch();
    pathsGroup->setLayout(pathsLayout);

    // Registry Keys Group
    QGroupBox* registryGroup = new QGroupBox("Registry Keys");
    QVBoxLayout* registryLayout = new QVBoxLayout;
    QPushButton* selectAllRegistry = new QPushButton("Select All Keys");
    QPushButton* deselectAllRegistry = new QPushButton("Deselect All Keys");
    registryLayout->addWidget(selectAllRegistry);
    registryLayout->addWidget(deselectAllRegistry);
    QList<QCheckBox*> registryCheckboxes;
    for (const QString& s : registryKeys) {
        auto cb = new QCheckBox(s);
        registryCheckboxes.append(cb);
        registryLayout->addWidget(cb);
    }
    registryLayout->addStretch();
    registryGroup->setLayout(registryLayout);

    groups->addWidget(pathsGroup);
    groups->addWidget(registryGroup);
    cleanupLayout->addLayout(groups);
    QPushButton* deleteButton = new QPushButton("Delete Selected");
    cleanupLayout->addWidget(deleteButton);

    // Logs Tab
    QWidget* logsTab = new QWidget;
    QVBoxLayout* logsLayout = new QVBoxLayout(logsTab);
    QTextEdit* logView = new QTextEdit;
    logView->setReadOnly(true);
    logsLayout->addWidget(logView);

    tabs->addTab(cleanupTab, "Cleanup");
    tabs->addTab(logsTab, "Logs");

    // Progress Bar
    QProgressBar* progress = new QProgressBar;
    progress->setValue(0);
    mainLayout->addWidget(tabs);
    mainLayout->addWidget(progress);

    // Allocate shared counter for progress
    auto sharedDone = new std::atomic<int>(0);

    // Connect button signals
    QObject::connect(selectAllPaths, &QPushButton::clicked, [&]() {
        for (auto cb : pathCheckboxes) cb->setChecked(true);
        });
    QObject::connect(deselectAllPaths, &QPushButton::clicked, [&]() {
        for (auto cb : pathCheckboxes) cb->setChecked(false);
        });
    QObject::connect(selectAllRegistry, &QPushButton::clicked, [&]() {
        for (auto cb : registryCheckboxes) cb->setChecked(true);
        });
    QObject::connect(deselectAllRegistry, &QPushButton::clicked, [&]() {
        for (auto cb : registryCheckboxes) cb->setChecked(false);
        });

    QObject::connect(deleteButton, &QPushButton::clicked, [&]() {
        logView->clear();
        int total = 0;
        for (auto cb : pathCheckboxes) if (cb->isChecked()) ++total;
        for (auto cb : registryCheckboxes) if (cb->isChecked()) ++total;
        // If nothing is selected, log and return
        if (total == 0) {
            QString ts = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
            logView->append(QString("[%1] No items selected for deletion.").arg(ts));
            return;
        }
        progress->setRange(0, total);
        progress->setValue(0);
        sharedDone->store(0);
        QString startTs = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        logView->append(QString("[%1] Starting cleanup...").arg(startTs));

        // Disable UI controls during deletion
        pathsGroup->setEnabled(false);
        registryGroup->setEnabled(false);
        deleteButton->setEnabled(false);

        // Run deletion in a separate thread and restore UI when done
        QtConcurrent::run([=]() {
            deletePathsThread(pathCheckboxes, logView, progress, sharedDone);
            deleteRegistryThread(registryCheckboxes, logView, progress, sharedDone);
            // Switch to Logs tab when finished
            QMetaObject::invokeMethod(tabs, "setCurrentIndex", Qt::QueuedConnection, Q_ARG(int, 1));
            // Re-enable UI controls
            QMetaObject::invokeMethod(pathsGroup, "setEnabled", Qt::QueuedConnection, Q_ARG(bool, true));
            QMetaObject::invokeMethod(registryGroup, "setEnabled", Qt::QueuedConnection, Q_ARG(bool, true));
            QMetaObject::invokeMethod(deleteButton, "setEnabled", Qt::QueuedConnection, Q_ARG(bool, true));
            });
        });

    window.show();
    return app.exec();
}

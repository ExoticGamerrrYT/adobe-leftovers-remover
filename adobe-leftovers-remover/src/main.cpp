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

// Thread-safe deletion of file paths
void deletePathsThread(const QList<QCheckBox*>& cbs, QTextEdit* log, QProgressBar* progress) {
    int done = 0;
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
        ++done;
        QMetaObject::invokeMethod(progress, "setValue", Qt::QueuedConnection, Q_ARG(int, done));
    }
}

// Thread-safe deletion of registry keys
void deleteRegistryThread(const QList<QCheckBox*>& cbs, QTextEdit* log, QProgressBar* progress) {
    int done = 0;
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
            msg = QString("[%1] Deleted registry: %2").arg(ts).arg(raw);
        else
            msg = QString("[%1] Failed registry delete: %2 (Error %3)").arg(ts).arg(raw).arg(res);
        QMetaObject::invokeMethod(log, "append", Qt::QueuedConnection, Q_ARG(QString, msg));
        ++done;
        QMetaObject::invokeMethod(progress, "setValue", Qt::QueuedConnection, Q_ARG(int, done));
    }
}

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
    // Relaunch as admin if needed
    if (!isRunningAsAdmin()) {
        wchar_t path[MAX_PATH];
        GetModuleFileNameW(NULL, path, MAX_PATH);
        ShellExecuteW(NULL, L"runas", path, NULL, NULL, SW_SHOWNORMAL);
        return 0;
    }

    QApplication app(argc, argv);
    QWidget window;
    window.setWindowTitle("Adobe Leftovers Remover");
    window.resize(800, 600);

    QVBoxLayout* mainLayout = new QVBoxLayout(&window);
    QTabWidget* tabs = new QTabWidget;

    // Selector Tab
    QWidget* selTab = new QWidget;
    QVBoxLayout* selLayout = new QVBoxLayout(selTab);
    QHBoxLayout* groups = new QHBoxLayout;

    // Paths Group
    QGroupBox* pGroup = new QGroupBox("Paths");
    QVBoxLayout* pLayout = new QVBoxLayout;
    QPushButton* selAllP = new QPushButton("Select All");
    QPushButton* deselAllP = new QPushButton("Deselect All");
    pLayout->addWidget(selAllP);
    pLayout->addWidget(deselAllP);
    QList<QCheckBox*> pCB;
    for (const QString& s : paths) { auto cb = new QCheckBox(s); pCB.append(cb); pLayout->addWidget(cb); }
    pLayout->addStretch(); pGroup->setLayout(pLayout);

    // Registry Group
    QGroupBox* rGroup = new QGroupBox("Registry Keys");
    QVBoxLayout* rLayout = new QVBoxLayout;
    QPushButton* selAllR = new QPushButton("Select All");
    QPushButton* deselAllR = new QPushButton("Deselect All");
    rLayout->addWidget(selAllR);
    rLayout->addWidget(deselAllR);
    QList<QCheckBox*> rCB;
    for (const QString& s : registryKeys) { auto cb = new QCheckBox(s); rCB.append(cb); rLayout->addWidget(cb); }
    rLayout->addStretch(); rGroup->setLayout(rLayout);

    groups->addWidget(pGroup);
    groups->addWidget(rGroup);
    selLayout->addLayout(groups);
    QPushButton* btnDel = new QPushButton("Eliminar");
    selLayout->addWidget(btnDel);

    // Logs Tab
    QWidget* logTab = new QWidget;
    QVBoxLayout* logLayout = new QVBoxLayout(logTab);
    QTextEdit* logView = new QTextEdit;
    logView->setReadOnly(true);
    logLayout->addWidget(logView);

    tabs->addTab(selTab, "Selector");
    tabs->addTab(logTab, "Logs");

    // Progress Bar
    QProgressBar* progress = new QProgressBar;
    progress->setValue(0);
    mainLayout->addWidget(tabs);
    mainLayout->addWidget(progress);

    // Connections
    QObject::connect(selAllP, &QPushButton::clicked, [&]() { for (auto cb : pCB) cb->setChecked(true); });
    QObject::connect(deselAllP, &QPushButton::clicked, [&]() { for (auto cb : pCB) cb->setChecked(false); });
    QObject::connect(selAllR, &QPushButton::clicked, [&]() { for (auto cb : rCB) cb->setChecked(true); });
    QObject::connect(deselAllR, &QPushButton::clicked, [&]() { for (auto cb : rCB) cb->setChecked(false); });

    QObject::connect(btnDel, &QPushButton::clicked, [&]() {
        logView->clear();
        int total = 0;
        for (auto cb : pCB) if (cb->isChecked()) ++total;
        for (auto cb : rCB) if (cb->isChecked()) ++total;
        progress->setRange(0, total);
        progress->setValue(0);
        QString startTs = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        logView->append(QString("[%1] Starting cleanup...").arg(startTs));
        // Run deletion in separate thread
        QtConcurrent::run([pCB, rCB, logView, progress]() {
            deletePathsThread(pCB, logView, progress);
            deleteRegistryThread(rCB, logView, progress);
            });
        });

    window.show();
    return app.exec();
}
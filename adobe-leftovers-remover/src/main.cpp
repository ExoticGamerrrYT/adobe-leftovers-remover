#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QPushButton>
#include <QStyle>
#include "data.h"

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);

	QWidget window;
	window.setWindowTitle("Adobe Leftovers Remover");
	window.setGeometry(100, 100, 600, 400);

	QHBoxLayout* mainLayout = new QHBoxLayout(&window);

	// --- Left Group (Paths) ---
	QGroupBox* leftGroup = new QGroupBox("Paths");
	QVBoxLayout* leftLayout = new QVBoxLayout;

	QPushButton* selectAllLeft = new QPushButton("Select All");
	QPushButton* deselectAllLeft = new QPushButton("Deselect All");
	leftLayout->addWidget(selectAllLeft);
	leftLayout->addWidget(deselectAllLeft);

	QList<QCheckBox*> leftCheckBoxes;
	for (const QString& path : paths) {
		QCheckBox* checkBox = new QCheckBox(path);
		leftCheckBoxes.append(checkBox);
		leftLayout->addWidget(checkBox);
	}

	leftLayout->addStretch();
	leftGroup->setLayout(leftLayout);

	// --- Right Group (Registry Keys) ---
	QGroupBox* rightGroup = new QGroupBox("Registry Keys");
	QVBoxLayout* rightLayout = new QVBoxLayout;

	QPushButton* selectAllRight = new QPushButton("Select All");
	QPushButton* deselectAllRight = new QPushButton("Deselect All");
	rightLayout->addWidget(selectAllRight);
	rightLayout->addWidget(deselectAllRight);

	QList<QCheckBox*> rightCheckBoxes;
	for (const QString& key : registryKeys) {
		QCheckBox* checkBox = new QCheckBox(key);
		rightCheckBoxes.append(checkBox);
		rightLayout->addWidget(checkBox);
	}

	rightLayout->addStretch();
	rightGroup->setLayout(rightLayout);

	mainLayout->addWidget(leftGroup);
	mainLayout->addWidget(rightGroup);

	// Connections
	QObject::connect(selectAllLeft, &QPushButton::clicked, [&]() {
		for (QCheckBox* cb : leftCheckBoxes) cb->setChecked(true);
		});
	QObject::connect(deselectAllLeft, &QPushButton::clicked, [&]() {
		for (QCheckBox* cb : leftCheckBoxes) cb->setChecked(false);
		});
	QObject::connect(selectAllRight, &QPushButton::clicked, [&]() {
		for (QCheckBox* cb : rightCheckBoxes) cb->setChecked(true);
		});
	QObject::connect(deselectAllRight, &QPushButton::clicked, [&]() {
		for (QCheckBox* cb : rightCheckBoxes) cb->setChecked(false);
		});

	window.show();
	return app.exec();
}
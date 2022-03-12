#include <QApplication>
#include <QFile>
#include <iostream>
#include <exception>
#include <QJsonDocument>
#include <QFileDialog>
#include <QMessageBox>
#include "RpgSession.hpp"


void text2json(QIODevice& in, QIODevice& out) {
	RpgSession sess;
	QTextStream strem(&in);
	sess.fromString(strem);
	sess.streamlineDates();
	QJsonDocument doc(sess.toJson());
	out.write(doc.toJson());
}
int main(int argc, char *argv[])
{
	if(argc >= 3) {
		QString in = QString::fromUtf8(argv[1]);
		QFile inF(in);
		if(!inF.open(QFile::ReadOnly)) {
			std::cout << "Error opening \"" << argv[1] << "\" for reading." << std::endl;
			return -1;
		}
		QString out = QString::fromUtf8(argv[2]);
		QFile outF(out);
		if(!outF.open(QFile::WriteOnly)) {
			std::cout << "Error opening \"" << argv[2] << "\" for writing." << std::endl;
			return -1;
		}
		try {
			text2json(inF, outF);
			inF.close();
			outF.flush();
			outF.close();
			return 0;
		}  catch (const std::exception& e) {
			std::cout << "Error encountered!\n" << e.what() << std::endl;
			return -1;
		}

	} else {
	QApplication a(argc, argv);
	QString in = QFileDialog::getOpenFileName(nullptr, QStringLiteral("Open File"),
											   QString(),
											   QStringLiteral("Text files (*.txt)"));
	if(in.isEmpty()) a.exit();
	QString out = QFileDialog::getSaveFileName(nullptr, QStringLiteral("Save File"),
											   QString(),
											   QStringLiteral("JSON files (*.json)"));
	if(out.isEmpty()) a.exit();
	QFile inF(in);
	if(!inF.open(QFile::ReadOnly)) {
		QMessageBox::critical(nullptr,QStringLiteral("Error!"),QStringLiteral("Error opening \"%1\" for reading!").arg(in));
		a.exit();
	}
	QFile outF(out);
	if(!outF.open(QFile::WriteOnly)) {
		QMessageBox::critical(nullptr,QStringLiteral("Error!"),QStringLiteral("Error opening \"%1\" for writing!").arg(out));
		a.exit();
	}
	try {
		text2json(inF, outF);
		inF.close();
		outF.flush();
		outF.close();
	}  catch (const std::exception& e) {
		QMessageBox::critical(nullptr,QStringLiteral("Exception encountered"),QString::fromUtf8(e.what()));
		a.exit();
	}
	a.exit();
	}
}

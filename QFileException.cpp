#include "QFileException.hpp"

QFileException::QFileException(const QFile& file)
{
	switch (file.openMode()) {
	case QFile::WriteOnly:
		str = QStringLiteral("Error opening \"%1\" for writing.").arg(file.fileName()).toStdString();
	break;
	default:
		str = QStringLiteral("Error opening \"%1\" for reading.").arg(file.fileName()).toStdString();
	break;
	}
}

QFileException::QFileException(const QString& filepath, const QIODevice::OpenMode& mode)
{
	switch (mode) {
	case QFile::WriteOnly:
		str = QStringLiteral("Error opening \"%1\" for writing.").arg(filepath).toStdString();
	break;
	default:
		str = QStringLiteral("Error opening \"%1\" for reading.").arg(filepath).toStdString();
	break;
	}
}

const char* QFileException::what() const noexcept
{
	return str.c_str();
}

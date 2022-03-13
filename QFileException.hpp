#ifndef QFILEEXCEPTION_HPP
#define QFILEEXCEPTION_HPP
#include <QFile>
#include <exception>
#include <string>

class QFileException : public std::exception
{
private:
	std::string str;
public:
	QFileException(const QFile& file);
	QFileException(const QString& filepath, const QFile::OpenMode& mode);
	virtual const char* what() const noexcept override;
};

#endif // QFILEEXCEPTION_HPP

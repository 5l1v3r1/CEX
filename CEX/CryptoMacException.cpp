#include "CryptoMacException.h"

NAMESPACE_EXCEPTION

CryptoMacException::CryptoMacException(const std::string &Location, const std::string &Origin, const std::string &Message, ErrorCodes ErrorCode)
	:
	CryptoException(Location, Origin, Message, ErrorCode)
{
}

const ExceptionTypes CryptoMacException::Enumeral()
{
	return ExceptionTypes::CryptoMacException;
}

const std::string CryptoMacException::Name()
{
	return Enumeration::ExceptionTypeConvert::ToName(Enumeral());
}

NAMESPACE_EXCEPTIONEND

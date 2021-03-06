#include "CipherModeFromName.h"
#include "BlockCipherFromName.h"
#include "CTR.h"
#include "CBC.h"
#include "CFB.h"
#include "CryptoCipherModeException.h"
#include "CryptoSymmetricException.h"
#include "ECB.h"
#include "ICM.h"
#include "OFB.h"

NAMESPACE_HELPER

using Enumeration::CipherModes;
using Exception::CryptoCipherModeException;
using Exception::CryptoSymmetricException;
using Enumeration::ErrorCodes;

const std::string CipherModeFromName::CLASS_NAME("CipherModeFromName");

ICipherMode* CipherModeFromName::GetInstance(IBlockCipher* Cipher, CipherModes CipherModeType)
{
	using namespace Cipher::Block::Mode;

	ICipherMode* mptr;

	mptr = nullptr;

	try
	{
		switch (CipherModeType)
		{
			case CipherModes::CTR:
			{
				mptr = new CTR(Cipher);
				break;
			}
			case CipherModes::CBC:
			{
				mptr = new CBC(Cipher);
				break;
			}
			case CipherModes::CFB:
			{
				mptr = new CFB(Cipher);
				break;
			}
			case CipherModes::ICM:
			{
				mptr = new ICM(Cipher);
				break;
			}
			case CipherModes::OFB:
			{
				mptr = new OFB(Cipher);
				break;
			}
			default:
			{
				// invalid option
				throw CryptoException(CLASS_NAME, std::string("GetInstance"), std::string("The cipher engine is not supported!"), ErrorCodes::InvalidParam);
			}
		}
	}
	catch (CryptoCipherModeException &ex)
	{
		throw CryptoException(CLASS_NAME, std::string("GetInstance"), ex.Message(), ex.ErrorCode());
	}
	catch (const std::exception &ex)
	{
		throw CryptoException(CLASS_NAME, std::string("GetInstance"), std::string(ex.what()), ErrorCodes::UnKnown);
	}

	return mptr;
}

ICipherMode* CipherModeFromName::GetInstance(BlockCiphers CipherType, CipherModes CipherModeType)
{
	using namespace Cipher::Block::Mode;

	ICipherMode* mptr;

	mptr = nullptr;

	try
	{
		switch (CipherModeType)
		{
			case CipherModes::CTR:
			{
				mptr = new CTR(CipherType);
				break;
			}
			case CipherModes::CBC:
			{
				mptr = new CBC(CipherType);
				break;
			}
			case CipherModes::CFB:
			{
				mptr = new CFB(CipherType);
				break;
			}
			case CipherModes::ECB:
			{
				mptr = new ECB(CipherType);
				break;
			}
			case CipherModes::ICM:
			{
				mptr = new ICM(CipherType);
				break;
			}
			case CipherModes::OFB:
			{
				mptr = new OFB(CipherType);
				break;
			}
			default:
			{
				// invalid option
				throw CryptoException(CLASS_NAME, std::string("GetInstance"), std::string("The cipher type is not supported!"), ErrorCodes::InvalidParam);
			}
		}
	}
	catch (CryptoSymmetricException &ex)
	{
		throw CryptoException(CLASS_NAME, std::string("GetInstance"), ex.Message(), ex.ErrorCode());
	}
	catch (CryptoCipherModeException &ex)
	{
		throw CryptoException(CLASS_NAME, std::string("GetInstance"), ex.Message(), ex.ErrorCode());
	}
	catch (CryptoException &ex)
	{
		throw CryptoException(CLASS_NAME, std::string("GetInstance"), ex.Message(), ex.ErrorCode());
	}
	catch (const std::exception &ex)
	{
		throw CryptoException(CLASS_NAME, std::string("GetInstance"), std::string(ex.what()), ErrorCodes::UnKnown);
	}

	return mptr;
}

NAMESPACE_HELPEREND

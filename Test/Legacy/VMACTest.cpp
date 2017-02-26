#include "VMACTest.h"
#include "../CEX/VMAC.h"

namespace Test
{
	std::string VMACTest::Run()
	{
		try
		{
			Initialize();
			CompareVector(m_key, m_iv, m_expected);
			OnProgress("Passed VMAC vector tests..");
			CompareAccess(m_key, m_iv);
			OnProgress("Passed DoFinal/ComputeHash methods output comparison..");

			return SUCCESS;
		}
		catch (std::string const& ex)
		{
			throw TestException(std::string(FAILURE + " : " + ex));
		}
		catch (...)
		{
			throw TestException(std::string(FAILURE + " : Internal Error"));
		}
	}

	void VMACTest::CompareAccess(std::vector<byte> &Key, std::vector<byte> &Iv)
	{
		std::vector<byte> hash1(20);
		CEX::Mac::VMAC mac;

		mac.Initialize(Key, Iv);
		mac.BlockUpdate(m_input, 0, m_input.size());
		mac.DoFinal(hash1, 0);

		std::vector<byte> hash2(20);
		mac.ComputeMac(m_input, hash2);

		if (hash1 != hash2)
			throw std::string("VMACTest: hash is not equal!");
	}

	void VMACTest::CompareVector(std::vector<byte> &Key, std::vector<byte> &Iv, std::vector<byte> &Expected)
	{
		std::vector<byte> hash(20);
		CEX::Mac::VMAC mac;

		mac.Initialize(Key, Iv);
		mac.BlockUpdate(m_input, 0, m_input.size());
		mac.DoFinal(hash, 0);

		if (Expected != hash)
			throw std::string("VMACTest: hash is not equal!");
	}

	void VMACTest::Initialize()
	{
		HexConverter::Decode("9BDA16E2AD0E284774A3ACBC8835A8326C11FAAD", m_expected);
		HexConverter::Decode("4B5C2F003E67F39557A8D26F3DA2B155", m_iv);
		HexConverter::Decode("9661410AB797D8A9EB767C21172DF6C7", m_key);

		for (unsigned int i = 0; i < m_input.size(); i++)
			m_input[i] = (byte)i;
	}

	void VMACTest::OnProgress(char* Data)
	{
		m_progressEvent(Data);
	}
}
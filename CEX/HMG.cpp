#include "HMG.h"
#include "ArrayUtils.h"
#include "DigestFromName.h"
#include "IntUtils.h"
#include "ProviderFromName.h"
#include "SymmetricKey.h"

NAMESPACE_DRBG

using Utility::IntUtils;

//~~~Constructor~~~//

HMG::HMG(Digests DigestType, Providers ProviderType)
	:
	m_hmacEngine(DigestType),
	m_destroyEngine(true),
	m_digestType(DigestType),
	m_distributionCode(0),
	m_distributionCodeMax(0),
	m_hmacKey(m_hmacEngine.BlockSize()),
	m_hmacState(m_hmacEngine.MacSize(), 0x01),
	m_isDestroyed(false),
	m_isInitialized(false),
	m_legalKeySizes(0),
	m_providerSource(Helper::ProviderFromName::GetInstance(ProviderType)),
	m_providerType(ProviderType),
	m_reseedCounter(0),
	m_reseedRequests(0),
	m_reseedThreshold(m_hmacEngine.MacSize() * 1000),
	m_secStrength((m_hmacEngine.MacSize() * 8) / 2),
	m_seedCtr(SEEDCTR_SIZE),
	m_stateCtr(STATECTR_SIZE)
{
	Scope();
}

HMG::HMG(IDigest* Digest, IProvider* Provider)
	:
	m_hmacEngine(Digest != 0 ? Digest : throw CryptoGeneratorException("HMG:Ctor", "Digest can not be null!")),
	m_destroyEngine(false),
	m_digestType(Digest->Enumeral()),
	m_distributionCode(0),
	m_distributionCodeMax(0),
	m_hmacKey(m_hmacEngine.BlockSize()),
	m_hmacState(m_hmacEngine.MacSize(), 0x01),
	m_isDestroyed(false),
	m_isInitialized(false),
	m_legalKeySizes(0),
	m_providerSource(Provider != 0 ? Provider : throw CryptoGeneratorException("HMG:Ctor", "Provider can not be null!")),
	m_providerType(Provider->Enumeral()),
	m_reseedCounter(0),
	m_reseedRequests(0),
	m_reseedThreshold(m_hmacEngine.MacSize() * 1000),
	m_secStrength((m_hmacEngine.MacSize() * 8) / 2),
	m_seedCtr(SEEDCTR_SIZE),
	m_stateCtr(STATECTR_SIZE)
{
	Scope();
}

HMG::~HMG()
{
	Destroy();
}

//~~~Public Functions~~~//

void HMG::Destroy()
{
	if (!m_isDestroyed)
	{
		m_isDestroyed = true;
		m_digestType = Digests::None;
		m_distributionCodeMax = 0;
		m_isInitialized = false;
		m_providerType = Providers::None;
		m_reseedCounter = 0;
		m_reseedRequests = 0;
		m_reseedThreshold = 0;
		m_secStrength = 0;

		try
		{
			Utility::ArrayUtils::ClearVector(m_distributionCode);
			Utility::ArrayUtils::ClearVector(m_hmacKey);
			Utility::ArrayUtils::ClearVector(m_hmacState);
			Utility::ArrayUtils::ClearVector(m_legalKeySizes);
			Utility::ArrayUtils::ClearVector(m_seedCtr);
			Utility::ArrayUtils::ClearVector(m_stateCtr);

			if (m_destroyEngine)
			{
				m_destroyEngine = false;

				if (m_hmacEngine.IsInitialized())
					m_hmacEngine.Destroy();
				if (m_providerSource != 0)
					delete m_providerSource;
			}
		}
		catch (std::exception& ex)
		{
			throw CryptoGeneratorException("HMG::Destroy", "Could not clear all variables!", std::string(ex.what()));
		}
	}
}

size_t HMG::Generate(std::vector<byte> &Output)
{
	return Generate(Output, 0, Output.size());
}

size_t HMG::Generate(std::vector<byte> &Output, size_t OutOffset, size_t Length)
{
	if (!m_isInitialized)
		throw CryptoGeneratorException("HMG:Generate", "The generator has not been initialized!");
	if ((Output.size() - Length) < OutOffset)
		throw CryptoGeneratorException("HMG:Generate", "Output buffer too small!");
	if (m_reseedRequests > MAX_RESEED)
		throw CryptoGeneratorException("HMG:Generate", "The maximum reseed requests have been exceeded!");
	if (Length > MAX_REQUEST)
		throw CryptoGeneratorException("HMG:Generate", "The maximum request size is 32768 bytes!");

	Generate(Output, OutOffset);
	m_reseedCounter += Length;

	if (m_reseedCounter >= m_reseedThreshold)
	{
		++m_reseedRequests;
		m_reseedCounter = 0;
		// use next block of state as seed material
		std::vector<byte> state(m_hmacEngine.BlockSize());
		Generate(state, 0);
		// combine with salt from provider, extract, and re-key
		Derive(state);
	}

	return Length;
}

void HMG::Initialize(ISymmetricKey &GenParam)
{
	if (GenParam.Nonce().size() != 0)
	{
		if (GenParam.Info().size() != 0)
			Initialize(GenParam.Key(), GenParam.Nonce(), GenParam.Info());
		else
			Initialize(GenParam.Key(), GenParam.Nonce());
	}
	else
	{
		Initialize(GenParam.Key());
	}
}

void HMG::Initialize(const std::vector<byte> &Seed)
{
	if (!SymmetricKeySize::Contains(LegalKeySizes(), Seed.size()))
		throw CryptoGeneratorException("HMG:Initialize", "Seed size is invalid! Check LegalKeySizes for accepted values.");

	// pre-initialize the HMAC
	m_hmacKey = Seed;
	Key::Symmetric::SymmetricKey kp(m_hmacKey);
	m_hmacEngine.Initialize(kp);
	// add entropy and re-mix before first output call
	Derive(m_hmacKey);

	size_t secLen = Seed.size();
	if (secLen < m_hmacEngine.MacSize())
		m_secStrength = (secLen * 8) / 2;

	m_isInitialized = true;
}

void HMG::Initialize(const std::vector<byte> &Seed, const std::vector<byte> &Nonce)
{
	if (Nonce.size() != NonceSize())
		throw CryptoGeneratorException("HMG:Initialize", "Nonce size is invalid! Check the NonceSize property for accepted value.");

	// added: nonce becomes the initial state counter value
	memcpy(&m_stateCtr[0], &Nonce[0], IntUtils::Min(Nonce.size(), m_stateCtr.size()));

	Initialize(Seed);
}

void HMG::Initialize(const std::vector<byte> &Seed, const std::vector<byte> &Nonce, const std::vector<byte> &Info)
{
	if (Nonce.size() != NonceSize())
		throw CryptoGeneratorException("HMG:Initialize", "Nonce size is invalid! Check the NonceSize property for accepted value.");

	// copy nonce to state counter
	memcpy(&m_stateCtr[0], &Nonce[0], IntUtils::Min(Nonce.size(), m_stateCtr.size()));

	// info can be a salt secret or domain limiter; added to derivation function input
	// for best security, info should be secret, random, and DistributionCodeMax size
	if (Info.size() <= m_distributionCodeMax)
	{
		m_distributionCode = Info;
	}
	else
	{
		// info is too large; size to optimal max, ignore remainder
		std::vector<byte> tmpInfo(m_distributionCodeMax);
		memcpy(&tmpInfo[0], &Info[0], tmpInfo.size());
		m_distributionCode = tmpInfo;
	}

	Initialize(Seed);
}

void HMG::Update(const std::vector<byte> &Seed)
{
	if (!SymmetricKeySize::Contains(LegalKeySizes(), Seed.size()))
		throw CryptoGeneratorException("HMG:Update", "Seed size is invalid! Check LegalKeySizes for accepted values.");

	Derive(Seed);
}

//~~~Private Functions~~~//

void HMG::Derive(const std::vector<byte> &Seed)
{
	// key expansion/strengthening function
	size_t blkOffset = m_seedCtr.size() + Seed.size();
	size_t keyLen = m_hmacEngine.BlockSize();
	size_t keyOffset = 0;
	std::vector<byte> macCode(m_hmacEngine.MacSize());
	std::vector<byte> tmpKey(keyLen);

	// preserve some initial entropy
	if (m_isInitialized)
	{
		m_hmacEngine.Update(m_hmacKey, 0, m_hmacKey.size());
		blkOffset += m_hmacKey.size();
	}

	do
	{
		size_t keyRmd = IntUtils::Min(macCode.size(), keyLen);
		// 1) increment seed counter by key-bytes copied
		Increase(m_seedCtr, keyRmd);
		// 2) process the seed counter
		m_hmacEngine.Update(m_seedCtr, 0, m_seedCtr.size());
		// 3) process the seed
		m_hmacEngine.Update(Seed, 0, Seed.size());
		// 4) pad with new entropy
		RandomPad(blkOffset);
		// 5) compress and add to HMAC key
		m_hmacEngine.Finalize(macCode, 0);
		memcpy(&tmpKey[keyOffset], &macCode[0], keyRmd);

		keyLen -= keyRmd;
		keyOffset += keyRmd;
	} 
	while (keyLen != 0);

	// store the new key
	m_hmacKey = tmpKey;
	// 6) rekey the HMAC
	Key::Symmetric::SymmetricKey kp(m_hmacKey);
	m_hmacEngine.Initialize(kp);
	// 7) generate the states initial entropy
	m_providerSource->GetBytes(m_hmacState);
}

void HMG::Generate(std::vector<byte> &Output, size_t OutOffset)
{
	size_t prcLen = Output.size();

	do
	{
		size_t rmdLen = IntUtils::Min(m_hmacState.size(), prcLen);
		// 1) increase state counter by output-bytes generated
		Increase(m_stateCtr, rmdLen);
		// 2) process the state counter
		m_hmacEngine.Update(m_stateCtr, 0, m_stateCtr.size());
		// 3) process the current state
		m_hmacEngine.Update(m_hmacState, 0, m_hmacState.size());
		// 4) optional personalization string
		if (m_distributionCode.size() != 0)
			m_hmacEngine.Update(m_distributionCode, 0, m_distributionCode.size());
		// 5) output the state
		m_hmacEngine.Finalize(m_hmacState, 0);
		memcpy(&Output[OutOffset], &m_hmacState[0], rmdLen);

		prcLen -= rmdLen;
		OutOffset += rmdLen;
	} 
	while (prcLen != 0);
}

void HMG::Increase(std::vector<byte> &Counter, const size_t Value)
{
	const size_t CTRSZE = Counter.size() - 1;
	std::vector<byte> ctrInc(sizeof(Value));
	memcpy(&ctrInc[0], &Value, ctrInc.size());
	byte carry = 0;

	for (size_t i = CTRSZE; i > 0; --i)
	{
		byte odst = Counter[i];
		byte osrc = CTRSZE - i < ctrInc.size() ? ctrInc[CTRSZE - i] : (byte)0;
		byte ndst = (byte)(odst + osrc + carry);
		carry = ndst < odst ? 1 : 0;
		Counter[i] = ndst;
	}
}

void HMG::Scope()
{
	m_distributionCodeMax = m_hmacEngine.BlockSize() + (m_hmacEngine.BlockSize() - (m_stateCtr.size() + m_hmacState.size() + Helper::DigestFromName::GetPaddingSize(m_digestType)));

	m_legalKeySizes.resize(3);
	// minimum seed size
	m_legalKeySizes[0] = SymmetricKeySize(m_hmacEngine.BlockSize() - Helper::DigestFromName::GetPaddingSize(m_digestType), 0, 0);
	// recommended size
	m_legalKeySizes[1] = SymmetricKeySize(m_legalKeySizes[0].KeySize() + m_hmacEngine.BlockSize(), STATECTR_SIZE, m_distributionCodeMax);
	// maximum security
	m_legalKeySizes[2] = SymmetricKeySize(m_legalKeySizes[1].KeySize() + m_hmacEngine.BlockSize(), STATECTR_SIZE, m_distributionCodeMax);
}

void HMG::RandomPad(size_t BlockOffset)
{
	size_t padLen = (BlockOffset > m_hmacEngine.BlockSize()) ? m_hmacEngine.BlockSize() - (BlockOffset % m_hmacEngine.BlockSize()) : m_hmacEngine.BlockSize() - BlockOffset;

	// if less than security size, add a full block
	if (padLen < m_hmacEngine.MacSize())
		padLen += m_hmacEngine.BlockSize();

	// adjust for finalizer code (Merkle�Damg�rd constructions)
	padLen -= Helper::DigestFromName::GetPaddingSize(m_digestType);
	std::vector<byte> tmpV(padLen);
	m_providerSource->GetBytes(tmpV);
	// digest processes full blocks by padding with entropy from provider
	m_hmacEngine.Update(tmpV, 0, tmpV.size());
}

NAMESPACE_DRBGEND

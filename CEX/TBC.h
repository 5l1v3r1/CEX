#ifndef _CEX_TBC_H
#define _CEX_TBC_H

#include "IPadding.h"

NAMESPACE_PADDING

/// <summary>
/// The Trailing Bit Compliment Padding Scheme.
/// </summary>
class TBC : public IPadding
{
private:

	TBC(const TBC&) = delete;
	TBC& operator=(const TBC&) = delete;
	TBC& operator=(TBC&&) = delete;

	const byte ZBCODE = (byte)0x00;
	const byte MKCODE = (byte)0xff;

public:
	//~~~Constructor~~~//

	/// <summary>
	/// CTor: Instantiate this class
	/// </summary>
	TBC() {}

	/// <summary>
	/// Destructor
	/// </summary>
	virtual ~TBC() {}

	//~~~Properties~~~//

	/// <summary>
	/// Get: The padding modes type name
	/// </summary>
	virtual const PaddingModes Enumeral() { return PaddingModes::TBC; }

	/// <summary>
	/// Get: The padding modes class name
	/// </summary>
	virtual const std::string Name() { return "TBC"; }

	//~~~Public Functions~~~//

	/// <summary>
	/// Add padding to input array
	/// </summary>
	///
	/// <param name="Input">Array to modify</param>
	/// <param name="Offset">Offset into array</param>
	///
	/// <returns>Length of padding</returns>
	///
	/// <exception cref="Exception::CryptoPaddingException">Thrown if the padding offset value is longer than the array length</exception>
	virtual size_t AddPadding(std::vector<byte> &Input, size_t Offset);

	/// <summary>
	/// Get the length of padding in an array
	/// </summary>
	///
	/// <param name="Input">Padded array of bytes</param>
	///
	/// <returns>Length of padding</returns>
	virtual size_t GetPaddingLength(const std::vector<byte> &Input);

	/// <summary>
	/// Get the length of padding in an array
	/// </summary>
	///
	/// <param name="Input">Padded array of bytes</param>
	/// <param name="Offset">Offset into array</param>
	///
	/// <returns>Length of padding</returns>
	virtual size_t GetPaddingLength(const std::vector<byte> &Input, size_t Offset);
};

NAMESPACE_PADDINGEND
#endif

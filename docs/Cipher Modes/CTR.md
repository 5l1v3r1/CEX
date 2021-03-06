# CTR: An implementation of a Big-Endian integer Counter Mode

## Description:
The CTR Counter mode generates a key-stream by encrypting successive values of an incrementing Big Endian ordered 8-bit integer counter array. 
The key-stream is then XOR'd with the input message block creating a type of stream cipher. 
In parallel mode, the CTR modes counter is increased by a number factored from the number of message blocks (ParallelBlockSize), allowing for counter pre-calculation and multi-threaded processing. 
The implementation is further parallelized by constructing a 'staggered' counter array, and processing large sub-blocks using AVX, AVX2, or AVX512 SIMD instructions.

The transformation function of the CTR mode is not limited by a dependency chain; this mode can be both SIMD pipelined and multi-threaded. Output from the parallelized functions aligns with the output from a standard sequential CTR implementation. 
This is achieved by pre-calculating the counters positional offset over multiple 'chunks' of key-stream, which are then generated independently across threads. 
The key stream generated by encrypting the counter array(s), is used as a source of random, and XOR'd with the message input to produce the cipher text.

## Implementation Notes
* In CTR mode, Encryption/Decryption can both be pipelined (AVX, AVX2, or AVX512), and multi-threaded. 
* A cipher mode constructor can either be initialized with a block-cipher instance, or using the block ciphers enumeration name. 
* A block-cipher instance created using the enumeration constructor, is automatically deleted when the class is destroyed. 
* The class functions are virtual, and can be accessed from an ICipherMode instance. 
* The EncryptBlock function can only be accessed through the class instance. 
* The transformation methods can not be called until the Initialize(bool, ISymmetricKey) function has been called. 
* If the system supports Parallel processing, and IsParallel() is set to true; passing an input block of ParallelBlockSize() to the transform will be auto parallelized. 
* The ParallelThreadsMax() property is used as the thread count in the parallel loop; this must be an even number no greater than the number of processer cores on the system. 
* ParallelBlockSize() is calculated automatically based on the processor(s) L1 data cache size, this property can be user defined, and must be evenly divisible by ParallelMinimumSize(). 
* The ParallelBlockSize(), IsParallel(), and ParallelThreadsMax() accessors, can be changed through the ParallelProfile() property; parallel processing can be disabled by setting IsParallel() to false in the ParallelProfile() accessor. 

## Example
```cpp
#include "CTR.h"

CTR cipher(new AES());
// initialize for encryption
cipher.Initialize(true, SymmetricKey(Key, Nonce));
// encrypt one block
cipher.EncryptBlock(Input, 0, Output, 0);
```
       
## Public Member Functions
```cpp
CTR(const CTR&)=delete
```
Copy constructor: copy is restricted, this function has been deleted.

```cpp
CTR &operator= (const CTR&)=delete
```
Copy operator: copy is restricted, this function has been deleted.

```cpp
CTR()=delete
```
Default constructor: default is restricted, this function has been deleted.

```cpp
CTR(BlockCiphers CipherType)
```
Initialize the Cipher Mode using a block-cipher type name.
 
```cpp
CTR(IBlockCipher* Cipher)
```
Initialize the Cipher Mode using a block-cipher instance.
 
```cpp
~CTR() override
```
Destructor: finalize this class.

```cpp
const size_t BlockSize() override
```
Read Only: The ciphers internal block-size in bytes.

```cpp
const BlockCiphers CipherType() override
```
Read Only: The block ciphers enumeration type name.

```cpp
IBlockCipher* Engine() override
```
Read Only: A pointer to the underlying block-cipher instance.

```cpp
const CipherModes Enumeral() override
```
Read Only: The cipher modes enumeration type name.

```cpp
const bool IsEncryption() override
```
Read Only: The operation mode, returns true if initialized for encryption, false for decryption.

```cpp
const bool IsInitialized() override
```
Read Only: The block-cipher mode has been keyed and is ready to transform data.

```cpp
const bool IsParallel() override
```
Read Only: Processor parallelization availability.

```cpp
const std::vector<SymmetricKeySize> &LegalKeySizes() override
```
Read Only: A vector of allowed cipher-mode input key byte-sizes.

```cpp
const std::string Name() override
```
Read Only: The cipher-modes formal class name.

```cpp
const size_t ParallelBlockSize() override
```
Read Only: Parallel block size; the byte-size of the input/output data arrays passed to a transform that trigger parallel processing.

```cpp
ParallelOptions &ParallelProfile() override
```
Read/Write: Contains parallel and SIMD capability flags and sizes.

```cpp
void DecryptBlock(const std::vector<byte> &Input, std::vector<byte> &Output) override
```
Decrypt a single block of bytes.

```cpp
void DecryptBlock(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset) override
```
Decrypt a block of bytes with offset parameters.

```cpp
void EncryptBlock(const std::vector<byte> &Input, std::vector<byte> &Output) override
```
Encrypt a single block of bytes.

```cpp
void EncryptBlock(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset) override
```
Encrypt a block of bytes using offset parameters.

```cpp
void Initialize(bool Encryption, ISymmetricKey &Parameters) override
```
Initialize the cipher-mode instance.

```cpp
void ParallelMaxDegree(size_t Degree) override
```
Set the maximum number of threads allocated when using multi-threaded processing.

```cpp
void Transform(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset, const size_t Length) override
```
Transform a length of bytes with offset parameters.

## Links
* NIST [SP800-38A](http://csrc.nist.gov/publications/nistpubs/800-38a/sp800-38a.pdf). 
* Handbook of [Applied Cryptography](http://cacr.uwaterloo.ca/hac/about/chap7.pdf) Chapter 7: Block Ciphers. 


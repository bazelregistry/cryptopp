// ppc-crypto.h - written and placed in public domain by Jeffrey Walton

//! \file ppc-crypto.h
//! \brief Support functions for PowerPC and Power8 vector operations
//! \details This header provides an agnostic interface into GCC and
//!   IBM XL C/C++ compilers modulo their different built-in functions
//!   for accessing vector intructions.
//! \since Crypto++ 6.0

#ifndef CRYPTOPP_PPC_CRYPTO_H
#define CRYPTOPP_PPC_CRYPTO_H

#include "config.h"

#if defined(CRYPTOPP_ALTIVEC_AVAILABLE) || defined(CRYPTOPP_DOXYGEN_PROCESSING)
# include <altivec.h>
# undef vector
# undef pixel
# undef bool
#endif

NAMESPACE_BEGIN(CryptoPP)

#if defined(CRYPTOPP_ALTIVEC_AVAILABLE) || defined(CRYPTOPP_DOXYGEN_PROCESSING)

typedef __vector unsigned char      uint8x16_p8;
typedef __vector unsigned int       uint32x4_p8;
typedef __vector unsigned long long uint64x2_p8;

// Use 8x16 for documentation because it is used frequently
#if defined(CRYPTOPP_XLC_VERSION) || defined(CRYPTOPP_DOXYGEN_PROCESSING)
typedef uint8x16_p8 VectorType;
#elif defined(CRYPTOPP_GCC_VERSION)
typedef uint64x2_p8 VectorType;
#endif

//! \brief Reverse a 16-byte array
//! \param src the byte array
//! \details ReverseByteArrayLE reverses a 16-byte array on a little endian
//!   system. It does nothing on a big endian system.
inline void ReverseByteArrayLE(byte src[16])
{
#if defined(CRYPTOPP_XLC_VERSION) && defined(IS_LITTLE_ENDIAN)
	vec_st(vec_reve(vec_ld(0, src)), 0, src);
#elif defined(IS_LITTLE_ENDIAN)
	const uint8x16_p8 mask = {15,14,13,12, 11,10,9,8, 7,6,5,4, 3,2,1,0};
	const uint8x16_p8 zero = {0};
	vec_vsx_st(vec_perm(vec_vsx_ld(0, src), zero, mask), 0, src);
#endif
}

//! \brief Reverse a vector
//! \tparam T a vector type
//! \param src the vector
//! \details Reverse endian swaps the bytes in a vector
template <class T>
inline T Reverse(const T& src)
{
	const uint8x16_p8 mask = {15,14,13,12, 11,10,9,8, 7,6,5,4, 3,2,1,0};
	const uint8x16_p8 zero = {0};
	return vec_perm(src, zero, mask);
}

//! \brief Loads a vector from a byte array
//! \param src the byte array
//! \details Loads a vector in big endian format from a byte array.
//!   VectorLoadBE will swap endianess on little endian systems.
//! \note VectorLoadBE does not require an aligned array.
inline VectorType VectorLoadBE(const uint8_t src[16])
{
#if defined(CRYPTOPP_XLC_VERSION)
	return (VectorType)vec_xl_be(0, (uint8_t*)src);
#else
# if defined(IS_LITTLE_ENDIAN)
	return (VectorType)Reverse(vec_vsx_ld(0, (uint8_t*)src));
# else
	return (VectorType)vec_vsx_ld(0, (uint8_t*)src);
# endif
#endif
}

//! \brief Loads a vector from a byte array
//! \param src the byte array
//! \param off offset into the byte array
//! \details Loads a vector in big endian format from a byte array.
//!   VectorLoadBE will swap endianess on little endian systems.
//! \note VectorLoadBE does not require an aligned array.
inline VectorType VectorLoadBE(int off, const uint8_t src[16])
{
#if defined(CRYPTOPP_XLC_VERSION)
	return (VectorType)vec_xl_be(off, (uint8_t*)src);
#else
# if defined(IS_LITTLE_ENDIAN)
	return (VectorType)Reverse(vec_vsx_ld(off, (uint8_t*)src));
# else
	return (VectorType)vec_vsx_ld(off, (uint8_t*)src);
# endif
#endif
}

//! \brief Stores a vector to a byte array
//! \tparam T a vector type
//! \param src the vector
//! \param dest the byte array
//! \details Stores a vector in big endian format to a byte array.
//!   VectorStoreBE will swap endianess on little endian systems.
//! \note VectorStoreBE does not require an aligned array.
template <class T>
inline void VectorStoreBE(const T& src, uint8_t dest[16])
{
#if defined(CRYPTOPP_XLC_VERSION)
	vec_xst_be((uint8x16_p8)src, 0, (uint8_t*)dest);
#else
# if defined(IS_LITTLE_ENDIAN)
	vec_vsx_st(Reverse((uint8x16_p8)src), 0, (uint8_t*)dest);
# else
	vec_vsx_st((uint8x16_p8)src, 0, (uint8_t*)dest);
# endif
#endif
}

//////////////////////////////////////////////////////////////////

//! \brief Loads a vector from a byte array
//! \param src the byte array
//! \details Loads a vector in big endian format from a byte array.
//!   VectorLoad will swap endianess on little endian systems.
//! \note VectorLoad does not require an aligned array.
inline VectorType VectorLoad(const byte src[16])
{
	return (VectorType)VectorLoadBE((uint8_t*)src);
}

//! \brief Loads a vector from a byte array
//! \param src the byte array
//! \param off offset into the byte array
//! \details Loads a vector in big endian format from a byte array.
//!   VectorLoad will swap endianess on little endian systems.
//! \note VectorLoad does not require an aligned array.
inline VectorType VectorLoad(int off, const byte src[16])
{
	return (VectorType)VectorLoadBE(off, (uint8_t*)src);
}

//! \brief Loads a vector from a byte array
//! \param src the byte array
//! \details Loads a vector from a byte array.
//!   VectorLoadKey does not swap endianess on little endian systems.
//! \note VectorLoadKey does not require an aligned array.
inline VectorType VectorLoadKey(const byte src[16])
{
#if defined(CRYPTOPP_XLC_VERSION)
	return (VectorType)vec_xl(0, (uint8_t*)src);
#else
	return (VectorType)vec_vsx_ld(0, (uint8_t*)src);
#endif
}

//! \brief Loads a vector from a 32-bit word array
//! \param src the 32-bit word array
//! \details Loads a vector from a 32-bit word array.
//!   VectorLoadKey does not swap endianess on little endian systems.
//! \note VectorLoadKey does not require an aligned array.
inline VectorType VectorLoadKey(const word32 src[4])
{
#if defined(CRYPTOPP_XLC_VERSION)
	return (VectorType)vec_xl(0, (uint8_t*)src);
#else
	return (VectorType)vec_vsx_ld(0, (uint8_t*)src);
#endif
}

//! \brief Loads a vector from a byte array
//! \param src the byte array
//! \param off offset into the byte array
//! \details Loads a vector from a byte array.
//!   VectorLoadKey does not swap endianess on little endian systems.
//! \note VectorLoadKey does not require an aligned array.
inline VectorType VectorLoadKey(int off, const byte src[16])
{
#if defined(CRYPTOPP_XLC_VERSION)
	return (VectorType)vec_xl(off, (uint8_t*)src);
#else
	return (VectorType)vec_vsx_ld(off, (uint8_t*)src);
#endif
}

//! \brief Stores a vector to a byte array
//! \tparam T a vector type
//! \param src the vector
//! \param dest the byte array
//! \details Stores a vector in big endian format to a byte array.
//!   VectorStore will swap endianess on little endian systems.
//! \note VectorStoreBE does not require an aligned array.
template<class T>
inline void VectorStore(const T& src, byte dest[16])
{
	return VectorStoreBE(src, (uint8_t*)dest);
}

//! \brief Permutes two vectors
//! \tparam T1 a vector type
//! \tparam T2 a vector type
//! \param vec1 the first vector
//! \param vec2 the second vector
//! \param mask vector mask
//! \details VectorPermute returns a new vector from vec1 and vec2
//!   based on mask. mask is an uint8x16_p8 type vector. The return
//!   vector is the same type as vec1.
template <class T1, class T2>
inline T1 VectorPermute(const T1& vec1, const T1& vec2, const T2& mask)
{
	return (T1)vec_perm(vec1, vec2, (uint8x16_p8)mask);
}

//! \brief XOR two vectors
//! \tparam T1 a vector type
//! \tparam T2 a vector type
//! \param vec1 the first vector
//! \param vec2 the second vector
//! \details VectorXor returns a new vector from vec1 and vec2. The return
//!   vector is the same type as vec1.
template <class T1, class T2>
inline T1 VectorXor(const T1& vec1, const T2& vec2)
{
	return (T1)vec_xor(vec1, (T1)vec2);
}

//! \brief Add two vector
//! \tparam T1 a vector type
//! \tparam T2 a vector type
//! \param vec1 the first vector
//! \param vec2 the second vector
//! \details VectorAdd returns a new vector from vec1 and vec2.
//!   vec2 is cast to the same type as vec1. The return vector
//!   is the same type as vec1.
template <class T1, class T2>
inline T1 VectorAdd(const T1& vec1, const T2& vec2)
{
	return (T1)vec_add(vec1, (T1)vec2);
}

//! \brief Shift two vectors left
//! \tparam T1 a vector type
//! \tparam T2 a vector type
//! \param vec1 the first vector
//! \param vec2 the second vector
//! \details VectorShiftLeft returns a new vector from vec1 and vec2.
//!   Both vec1 and vec2 are cast to uint8x16_p8. The return vector
//!   is the same type as vec1.
//! \note VectorShiftLeft handles the difference between big endian
//!   and little endian internally. Call the function as if on a big
//!   endian machine.
template <int C, class T1, class T2>
inline T1 VectorShiftLeft(const T1& vec1, const T2& vec2)
{
#if defined(IS_LITTLE_ENDIAN)
	return (T1)vec_sld((uint8x16_p8)vec2, (uint8x16_p8)vec1, 16-C);
#else
	return (T1)vec_sld((uint8x16_p8)vec1, (uint8x16_p8)vec2, C);
#endif
}

//! \brief One round of AES encryption
//! \tparam T1 a vector type
//! \tparam T2 a vector type
//! \param state the state vector
//! \param key the subkey vector
//! \details VectorEncrypt performs one round of AES encryption of state
//!   using subkey key. The return vector is the same type as vec1.
template <class T1, class T2>
inline T1 VectorEncrypt(const T1& state, const T2& key)
{
#if defined(CRYPTOPP_XLC_VERSION)
	return (T1)__vcipher((VectorType)state, (VectorType)key);
#elif defined(CRYPTOPP_GCC_VERSION)
	return (T1)__builtin_crypto_vcipher((VectorType)state, (VectorType)key);
#else
	CRYPTOPP_ASSERT(0);
#endif
}

//! \brief Final round of AES encryption
//! \tparam T1 a vector type
//! \tparam T2 a vector type
//! \param state the state vector
//! \param key the subkey vector
//! \details VectorEncryptLast performs the final round of AES encryption
//!   of state using subkey key. The return vector is the same type as vec1.
template <class T1, class T2>
inline T1 VectorEncryptLast(const T1& state, const T2& key)
{
#if defined(CRYPTOPP_XLC_VERSION)
	return (T1)__vcipherlast((VectorType)state, (VectorType)key);
#elif defined(CRYPTOPP_GCC_VERSION)
	return (T1)__builtin_crypto_vcipherlast((VectorType)state, (VectorType)key);
#else
	CRYPTOPP_ASSERT(0);
#endif
}

//! \brief One round of AES decryption
//! \tparam T1 a vector type
//! \tparam T2 a vector type
//! \param state the state vector
//! \param key the subkey vector
//! \details VectorDecrypt performs one round of AES decryption of state
//!   using subkey key. The return vector is the same type as vec1.
template <class T1, class T2>
inline T1 VectorDecrypt(const T1& state, const T2& key)
{
#if defined(CRYPTOPP_XLC_VERSION)
	return (T1)__vncipher((VectorType)state, (VectorType)key);
#elif defined(CRYPTOPP_GCC_VERSION)
	return (T1)__builtin_crypto_vncipher((VectorType)state, (VectorType)key);
#else
	CRYPTOPP_ASSERT(0);
#endif
}

//! \brief Final round of AES decryption
//! \tparam T1 a vector type
//! \tparam T2 a vector type
//! \param state the state vector
//! \param key the subkey vector
//! \details VectorDecryptLast performs the final round of AES decryption
//!   of state using subkey key. The return vector is the same type as vec1.
template <class T1, class T2>
inline T1 VectorDecryptLast(const T1& state, const T2& key)
{
#if defined(CRYPTOPP_XLC_VERSION)
	return (T1)__vncipherlast((VectorType)state, (VectorType)key);
#elif defined(CRYPTOPP_GCC_VERSION)
	return (T1)__builtin_crypto_vncipherlast((VectorType)state, (VectorType)key);
#else
	CRYPTOPP_ASSERT(0);
#endif
}

//! \brief SHA512 Sigma functions
//! \tparam func the function
//! \tparam subfunc the sub-function
//! \tparam T a vector type
//! \param vec the block to transform
//! \details VectorSHA512 selects sigma0, sigma1, Sigma0, Sigma1 based on
//!   func and subfunc. The return vector is the same type as vec.
template <int func, int subfunc, class T>
inline T VectorSHA512(const T& vec)
{
#if defined(CRYPTOPP_XLC_VERSION)
	return (T)__vshasigmad((uint64x2_p8)vec, func, subfunc);
#elif defined(CRYPTOPP_GCC_VERSION)
	return (T)__builtin_crypto_vshasigmad((uint64x2_p8)vec, func, subfunc);
#else
	CRYPTOPP_ASSERT(0);
#endif
}

//! \brief SHA256 Sigma functions
//! \tparam func the function
//! \tparam subfunc the sub-function
//! \tparam T a vector type
//! \param vec the block to transform
//! \details VectorSHA256 selects sigma0, sigma1, Sigma0, Sigma1 based on
//!   func and subfunc. The return vector is the same type as vec.
template <int func, int subfunc, class T>
inline T VectorSHA256(const T& vec)
{
#if defined(CRYPTOPP_XLC_VERSION)
	return (T)__vshasigmaw((uint32x4_p8)vec, func, subfunc);
#elif defined(CRYPTOPP_GCC_VERSION)
	return (T)__builtin_crypto_vshasigmaw((uint32x4_p8)vec, func, subfunc);
#else
	CRYPTOPP_ASSERT(0);
#endif
}

#endif // CRYPTOPP_ALTIVEC_AVAILABLE

NAMESPACE_END

#endif  // CRYPTOPP_PPC_CRYPTO_H
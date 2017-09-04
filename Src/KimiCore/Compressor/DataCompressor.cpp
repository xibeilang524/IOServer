#include "stdafx.h"
#include "KimiCore.h"
#include "KimiCore/DataCompressor.h"

#include "minilzo/minilzo.h"
#include "lz4/lz4.h"
#include "QuickLZ/quicklz.h"
#include "google-snappy/snappy.h"


_KIMI_NAMESPACE_BEGIN

#pragma warning( push )
#pragma warning(disable : 4267)

//////////////////////////////////////////////////////////////////////////
// 数据压缩接口定义
//////////////////////////////////////////////////////////////////////////

ICompressor* ICompressor::CreateCompressor(int nCompressorType) 
{
	ICompressor* pCompressor = NULL;

	switch(nCompressorType)
	{
	case dcLZO:
		pCompressor = new CLZOCompressor();
		break;
	case dcLZ4:
		pCompressor = new CLZ4Compressor();
		break;
	case dcQuickLZ:
		pCompressor = new CQuickLZCompressor();
		break;
	case dcSnappy:
		pCompressor = new CSnappyCompressor();
		break;
	}

	return pCompressor;
}

size_t ICompressor::GetDecompressSize(const BYTE* pSource, size_t nSoureSize)
{
	if (nSoureSize <= sizeof(unsigned long))
		return 0;

	unsigned long* pDWORD = (unsigned long*)pSource;
	return (size_t)(*pDWORD);
}

//////////////////////////////////////////////////////////////////////////
// LZ0压缩算法封装类 - CLZOCompressor
//////////////////////////////////////////////////////////////////////////

CLZOCompressor::CLZOCompressor()
{
	if (lzo_init() == LZO_E_OK)
	{
	}
}

CLZOCompressor::~CLZOCompressor()
{

}

size_t CLZOCompressor::GetCompressSize(size_t nInputSize)
{
	return (nInputSize + nInputSize / 16 + 64 + 3) + sizeof(unsigned long);
}

BOOL CLZOCompressor::Compress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize)
{
	if (workmem_.size() < LZO1X_1_MEM_COMPRESS)
		workmem_.resize(LZO1X_1_MEM_COMPRESS);

	lzo_uint nDestSize = *pDestSize;
	int r = lzo1x_1_compress(pSource, nSoureSize, pDest + sizeof(unsigned long), &nDestSize, &workmem_[0]);

	if (r == LZO_E_OK)
	{
		*pDestSize = nDestSize + sizeof(unsigned long);
		unsigned long* pDWORD = reinterpret_cast<unsigned long*>(pDest);
		*pDWORD = nSoureSize;

		return TRUE;
	}
	else 
		return FALSE;
}

BOOL CLZOCompressor::Decompress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize)
{
	lzo_uint nDestSize = *pDestSize;
	int r = lzo1x_decompress(pSource + sizeof(unsigned long), nSoureSize, pDest, &nDestSize, NULL);
	*pDestSize = nDestSize;

	return (r == LZO_E_OK);
}

//////////////////////////////////////////////////////////////////////////
// LZ4压缩算法封装类 - CLZ4Compressor
//////////////////////////////////////////////////////////////////////////

CLZ4Compressor::CLZ4Compressor()
{

}

CLZ4Compressor::~CLZ4Compressor()
{

}

size_t CLZ4Compressor::GetCompressSize(size_t nInputSize)
{
	return  LZ4_COMPRESSBOUND(nInputSize) + sizeof(unsigned long);
}

BOOL CLZ4Compressor::Compress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize)
{
	int nBytesPassed = LZ4_compress(reinterpret_cast<const char*>(pSource), reinterpret_cast<char*>(pDest + sizeof(unsigned long)), (int)*pDestSize);
	if (nBytesPassed > 0)
	{
		*pDestSize = (size_t)nBytesPassed + sizeof(unsigned long);
		unsigned long* pDWORD = reinterpret_cast<unsigned long*>(pDest);
		*pDWORD = nSoureSize;

		return TRUE;
	}
	else
		return FALSE;
}

BOOL CLZ4Compressor::Decompress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize)
{
	int nBytesPassed = LZ4_decompress_fast(reinterpret_cast<const char*>(pSource + sizeof(unsigned long)), reinterpret_cast<char*>(pDest), (int)*pDestSize);
	//*pDestSize = (size_t)nBytesPassed;

	return (nBytesPassed > 0);
}


//////////////////////////////////////////////////////////////////////////
// QuickLZ压缩算法封装类 - CQuickLZCompressor
//////////////////////////////////////////////////////////////////////////

CQuickLZCompressor::CQuickLZCompressor()
	: state_compress(NULL)
	, state_decompress(NULL)
{
}

CQuickLZCompressor::~CQuickLZCompressor()
{
	if (state_compress)
		free(state_compress);

	if (state_decompress)
		free(state_decompress);
}

size_t CQuickLZCompressor::GetCompressSize(size_t nInputSize)
{
	return  (nInputSize + 400);
}

size_t CQuickLZCompressor::GetDecompressSize(const BYTE* pSource, size_t nSoureSize)
{
	return qlz_size_decompressed(reinterpret_cast<const char*>(pSource));
}

BOOL CQuickLZCompressor::Compress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize)
{
	if (!state_compress)
		state_compress = malloc(sizeof(qlz_state_compress));

	size_t nBytesPassed = qlz_compress(reinterpret_cast<const void*>(pSource), reinterpret_cast<char*>(pDest), *pDestSize, 
		reinterpret_cast<qlz_state_compress*>(state_compress));
	*pDestSize = nBytesPassed;

	return (nBytesPassed > 0);
}

BOOL CQuickLZCompressor::Decompress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize)
{
	if (!state_decompress)
		state_decompress = malloc(sizeof(qlz_state_decompress));

	size_t nBytesPassed = qlz_decompress(reinterpret_cast<const char*>(pSource), reinterpret_cast<void*>(pDest), 
		reinterpret_cast<qlz_state_decompress*>(state_decompress));
	*pDestSize = nBytesPassed;

	return (nBytesPassed > 0);
}

//////////////////////////////////////////////////////////////////////////
// google snappy压缩算法封装类 - CSnappyCompressor
//////////////////////////////////////////////////////////////////////////
CSnappyCompressor::CSnappyCompressor()
{

}

CSnappyCompressor::~CSnappyCompressor()
{

}

size_t CSnappyCompressor::GetCompressSize(size_t nInputSize)
{
	return snappy::MaxCompressedLength(nInputSize);
}

size_t CSnappyCompressor::GetDecompressSize(const BYTE* pSource, size_t nSoureSize)
{
	size_t result;

	if (snappy::GetUncompressedLength(reinterpret_cast<const char*>(pSource), nSoureSize, &result))
		return result;
	else
		return 0;
}

BOOL CSnappyCompressor::Compress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize)
{
	snappy::RawCompress(reinterpret_cast<const char*>(pSource), nSoureSize, reinterpret_cast<char*>(pDest), pDestSize);
	return (*pDestSize > 0);
}

BOOL CSnappyCompressor::Decompress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize)
{
	return snappy::RawUncompress(reinterpret_cast<const char*>(pSource), nSoureSize, reinterpret_cast<char*>(pDest));
}

#pragma warning( pop )

_KIMI_NAMESPACE_END

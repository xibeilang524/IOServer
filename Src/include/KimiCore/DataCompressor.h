#ifndef __KIMI_KERNEL_DATA_COMPRESSOR_H__
#define __KIMI_KERNEL_DATA_COMPRESSOR_H__

#include <vector>

_KIMI_NAMESPACE_BEGIN

class CLZ4Compressor;
class CLZOCompressor;

//////////////////////////////////////////////////////////////////////////
// 数据压缩接口定义 - ICompressor
//////////////////////////////////////////////////////////////////////////
class KIMI_CORE_CLASS ICompressor
{
public:
	enum {
		dcEmpty = 0,
		dcLZO,
		dcLZ4,
		dcQuickLZ,
		dcSnappy
	};
	static ICompressor* CreateCompressor(int nDataCompressType);

public:
	/// 通过提交需要压缩的数据大小，计算最大需要的内存大小
	virtual size_t GetCompressSize(size_t nInputSize) = 0;

	/// 所有压缩后的数据，最前4个字节为压缩前数据长度，该函数可从前4个字节中读取到原始数据长度值
	virtual size_t GetDecompressSize(const BYTE* pSource, size_t nSoureSize);

	/// 压缩数据
	virtual BOOL Compress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize) = 0;

	/// 解压数据，为了性能考虑，pDestSize最好为真正的压缩前数据大小
	virtual BOOL Decompress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize) = 0;
};

#pragma warning( push )
#pragma warning(disable : 4251)

//////////////////////////////////////////////////////////////////////////
// LZ0压缩算法封装类 - CLZOCompressor
//////////////////////////////////////////////////////////////////////////
class KIMI_CORE_CLASS CLZOCompressor : public ICompressor
{
public:
	CLZOCompressor();
	virtual ~CLZOCompressor();

public:
	virtual size_t GetCompressSize(size_t nInputSize);
	virtual BOOL Compress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize);
	virtual BOOL Decompress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize);

private:
	// 压缩字典表
	mutable std::vector<BYTE>  workmem_;
};

#pragma warning( pop )

//////////////////////////////////////////////////////////////////////////
// LZ4压缩算法封装类 - CLZ4Compressor
//////////////////////////////////////////////////////////////////////////
class KIMI_CORE_CLASS CLZ4Compressor : public ICompressor
{
public:
	CLZ4Compressor();
	virtual ~CLZ4Compressor();

public:
	virtual size_t GetCompressSize(size_t nInputSize);
	virtual BOOL Compress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize);
	virtual BOOL Decompress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize);

private:
};

//////////////////////////////////////////////////////////////////////////
// QuickLZ压缩算法封装类 - CQuickLZCompressor
//////////////////////////////////////////////////////////////////////////
class KIMI_CORE_CLASS CQuickLZCompressor : public ICompressor
{
public:
	CQuickLZCompressor();
	virtual ~CQuickLZCompressor();

public:
	virtual size_t GetCompressSize(size_t nInputSize);
	virtual size_t GetDecompressSize(const BYTE* pSource, size_t nSoureSize);
	virtual BOOL Compress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize);
	virtual BOOL Decompress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize);

protected:
	void *state_compress;
	void *state_decompress;
	//qlz_state_compress *state_compress;
	//qlz_state_decompress *state_decompress;
};

//////////////////////////////////////////////////////////////////////////
// google snappy压缩算法封装类 - CSnappyCompressor
//////////////////////////////////////////////////////////////////////////
class KIMI_CORE_CLASS CSnappyCompressor : public ICompressor
{
public:
	CSnappyCompressor();
	virtual ~CSnappyCompressor();

public:
	virtual size_t GetCompressSize(size_t nInputSize);
	virtual size_t GetDecompressSize(const BYTE* pSource, size_t nSoureSize);
	virtual BOOL Compress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize);
	virtual BOOL Decompress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize);
};

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_DATA_COMPRESSOR_H__
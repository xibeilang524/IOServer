#ifndef __KIMI_KERNEL_DYNAMIC_ARRAY_H__
#define __KIMI_KERNEL_DYNAMIC_ARRAY_H__

_KIMI_NAMESPACE_BEGIN

template <class ClassType>
class CDynamicArray			// 数组模板
{
public:
	CDynamicArray();
	CDynamicArray(long InitSize);
	virtual ~CDynamicArray();

public:
	int Error;				// 错误计数器(分配错误,索引错误等)
	void Clear();
	int GetBlockSize() { return BlockSize; };
	int SetBlockSize(int factorial); // 使用价乘值设置块大小，有效值为1-32
	int SetLength(long l);
	int GetLength();
	int SetAutoAlloc(int flag);
	ClassType& operator[](long index);

protected:
	ClassType *TypeData;
	ClassType Nil;
	long DataCount;
	int AutoMode;
	int BlockSize;
	int Factorial;
};

///////////////////////////////////////////////////////////////
// CDynamicArray 实现

template <class ClassType> 
CDynamicArray<ClassType>::CDynamicArray()
{
	TypeData  = 0; 
	DataCount = 0;
	AutoMode  = 0;
	Error     = 0;
	BlockSize = 128;
	Factorial = 7;
}

template <class ClassType> 
CDynamicArray<ClassType>::CDynamicArray(long InitSize)
{
	CDynamicArray::CDynamicArray();
	SetLength(InitSize);
}

template <class ClassType> 
CDynamicArray<ClassType>::~CDynamicArray()
{ 
	Clear();
}

template <class ClassType> 
void CDynamicArray<ClassType>::Clear()
{
	if (0 != TypeData)
	{
		delete[] TypeData; 
		TypeData  = 0;
		DataCount = 0; 
	}
}

template <class ClassType>
int CDynamicArray<ClassType>::SetBlockSize(int factorial)
{
	if (factorial < 5)
		factorial = 5;
	else if (factorial > 31)
		factorial = 31;

	BlockSize = 1 << factorial;
	Factorial = factorial;
	return BlockSize;
}

template <class ClassType> 
int CDynamicArray<ClassType>::SetLength(long NewLen)
{
	if (NewLen <= 0)
		return 0;

	int block1 = (DataCount + BlockSize - 1) >> Factorial,
		block2 = (NewLen + BlockSize - 1) >> Factorial; 

	if (block1 == block2) 
	{ 
		DataCount = NewLen; 
		return 0; 
	}
	if (block1 == 0 || TypeData == 0) 
	{
		TypeData = new ClassType[block2 << Factorial];
		if (!TypeData) 
		{ 
			Error++; 
			return -1; 
		}
		DataCount=NewLen;
	} 
	else 
	{
		ClassType *NewData;
		NewData = new ClassType[block2 << Factorial];
		if (!NewData) 
		{ 
			Error++; 
			return -1; 
		}
		int min = (DataCount <= NewLen) ? DataCount : NewLen;
		for (int i = 0; i < min; i++) 
			NewData[i] = TypeData[i];
		
		delete[] TypeData;
		TypeData = NewData;
		DataCount = NewLen;
	}
	return 0;
}

template <class ClassType> 
int CDynamicArray<ClassType>::GetLength()
{
	return DataCount;
}

template <class ClassType> 
int CDynamicArray<ClassType>::SetAutoAlloc(int flag)
{
	AutoMode = flag;
	return 0;
}

template <class ClassType>
 ClassType& CDynamicArray<ClassType>::operator[](long index)
{
	if (index < 0) 
	{ 
		Error++; 
		return Nil; 
	}
	if (index >= DataCount) 
	{
		if (!AutoMode) 
		{ 
			Error++; 
			return Nil; 
		}
		if (SetLength(index+1)) 
		{ 
			Error++; 
			return Nil; 
		}
	}

	return TypeData[index];
}

_KIMI_NAMESPACE_END

#endif // __KIMI_KERNEL_DYNAMIC_ARRAY_H__
///////////////////////////////////////////////////////////////////
//	TPRICOMM - TPRI公共库
//
// 实现Singleton设计模式模板类
//
// 作者：何清
// 日期：2010-11-11
// mail: heqing@tpri.com.cn
//
//////////////////////////////////////////////////////////////////////////
//
//  使用方法：
// class CServiceManger
// {
//	public:
//		void Run() { ... }
//	private:
//		CServiceManger(void) { ... }
//		virtual ~CServiceManger(void) { ... }
//		DECLARE_SINGLETON_CLASS(CServiceManger);
// };
// typedef Singleton<CServiceManger> CSingletonServiceManger;
//
// int _tmain(int argc, _TCHAR* argv[])
// {
//		CSingletonServiceManger::GetInstance()->Run();
// }
//
///////////////////////////////////////////////////////////////////
// 更新历史:
// 
// @2010-11-11
//   创建文档     
///////////////////////////////////////////////////////////////////

#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include <memory>
#include <afxmt.h>

TRUNK_NAMESPACE_BEGIN

template<class T>
class _GENERAL_CLASS CSingleton
{
public:
	static inline T* GetInstance();
	
private:
	CSingleton(void){}
	CSingleton(const CSingleton&){}
	CSingleton & operator= (const CSingleton &){}
	virtual ~CSingleton(void){}
	
	static std::auto_ptr<T> _instance;
	static CCriticalSection _cs; 
};

template<class T>
std::auto_ptr<T> CSingleton<T>::_instance;

template <class T>
CCriticalSection CSingleton<T>::_cs; 

template<class T>
inline T* CSingleton<T>::GetInstance()
{
	if( 0 == _instance.get() )
	{
		_cs.Lock();
		if( 0 == _instance.get() )
		{
			_instance.reset( new T);
		}
		_cs.Unlock();
	}
	return _instance.get();
}

// 在实现一个Singleton模式的类时，必须在定义类中使用该宏来声明
#define DECLARE_SINGLETON_CLASS( type ) \
	friend class std::auto_ptr< type >;\
	friend class CSingleton< type >;


TRUNK_NAMESPACE_END

#endif // end of __SINGLETON_H__
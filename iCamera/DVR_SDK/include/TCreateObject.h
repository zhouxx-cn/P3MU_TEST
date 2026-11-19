#ifndef	H_CREATE_OBJECT

#define H_CREATE_OBJECT

#include <iostream>
#include "MemoryInterface.h"

#define OUTC_FAIL			printf

namespace	CommonKit
{

	/*! 
	* \class TCreateObject
	* \brief new操作创建对象
	*  TCreatebject<对象>
	*  CTestMemory *cTestMemory = CommonKit::TCreateObject<CTestMemory>::CreateObject(__FILE__, __FUNCTION__, __LINE__);
	*/
	template<class TObject, class TBase = TObject>
	class TCreateObject
	{
	public:
		//构造对象
		static TBase* CreateObject()
		{
			TBase *object = NULL;
#ifdef USE_MEMORY_POOL
			try
			{
				TObject *cObject = (TObject*)MEMORY_MALLOC(sizeof(TObject), __FILE__, __FUNCTION__, __LINE__);
				//cObject->TObject::TObject();
				new(cObject)TObject();
				object = cObject;
			}
			catch (...)
			{
				return object;
			}
#else
			try
			{
				object = new TObject;
			}
			catch (...)
			{
				return object;
			}
#endif
			return object;
		}
		//构造对象
		static TBase* CreateObject(const char *cfile, const char *cfunction, int line)
		{
			TBase *object = NULL;
#ifdef USE_MEMORY_POOL
			try
			{
				TObject *cObject = (TObject*)MEMORY_MALLOC(sizeof(TObject), cfile, cfunction, line);
				//cObject->TObject::TObject();
				new(cObject)TObject();
				object = cObject;
			}
			catch (...)
			{
				return object;
			}
#else
			try
			{
				object = new TObject;
			}
			catch (...)
			{
				return object;
			}
#endif
			return object;
		}
		//构造数组
		static TBase* CreateObject(int iNumber, const char *cfile, const char *cfunction, int line)
		{
			TBase *object = NULL;
#ifdef USE_MEMORY_POOL
			try
			{
				TObject *cObject = (TObject*)MEMORY_MALLOC(sizeof(TObject) * iNumber, cfile, cfunction, line);
				//cObject->TObject::TObject();
				for (int iCount = 0; iCount < iNumber; iCount++)
				{
					new(cObject[iCount])TObject();
				}
				object = cObject;
			}
			catch (...)
			{
				return object;
			}
#else
			try
			{
				object = new TObject[iNumber];
			}
			catch (...)
			{
				return object;
			}
#endif
			return object;
		}
	};

	/*! 
	* \class TCreateObject
	* \brief new操作创建对象
	*  TCreatebject<对象>
	*  CTestMemory *cTestMemory = CommonKit::TCreateObject<CTestMemory>::CreateObject(__FILE__, __FUNCTION__, __LINE__);
	*/
	template<class TObject, class TPara1, class TBase = TObject>
	class TCreateObjectP1
	{
	public:
		//构造对象
		static TBase* CreateObject(TPara1 cPara1)
		{
			TBase *object = NULL;
#ifdef USE_MEMORY_POOL
			try
			{
				TObject *cObject = (TObject*)MEMORY_MALLOC(sizeof(TObject), __FILE__, __FUNCTION__, __LINE__);
				//cObject->TObject::TObject();
				new(cObject)TObject(cPara1);
				object = cObject;
			}
			catch (...)
			{
				return object;
			}
#else
			try
			{
				object = new TObject(cPara1);
			}
			catch (...)
			{
				return object;
			}
#endif
			return object;
		}
		//构造对象
		static TBase* CreateObject(TPara1 cPara1, const char *cfile, const char *cfunction, int line)
		{
			TBase *object = NULL;
#ifdef USE_MEMORY_POOL
			try
			{
				TObject *cObject = (TObject*)MEMORY_MALLOC(sizeof(TObject), cfile, cfunction, line);
				//cObject->TObject::TObject();
				new(cObject)TObject(cPara1);
				object = cObject;
			}
			catch (...)
			{
				return object;
			}
#else
			try
			{
				object = new TObject(cPara1);
			}
			catch (...)
			{
				return object;
			}
#endif
			return object;
		}
	};

	/*! 
	* \class TCreateObject
	* \brief new操作创建对象
	*  TCreatebject<对象>
	*  CTestMemory *cTestMemory = CommonKit::TCreateObject<CTestMemory>::CreateObject(__FILE__, __FUNCTION__, __LINE__);
	*/
	template<class TObject, class TPara1, class TPara2, class TBase = TObject>
	class TCreateObjectP2
	{
	public:
		//构造对象
		static TBase* CreateObject(TPara1 cPara1, TPara2 cPara2)
		{
			TBase *object = NULL;
#ifdef USE_MEMORY_POOL
			try
			{
				TObject *cObject = (TObject*)MEMORY_MALLOC(sizeof(TObject), __FILE__, __FUNCTION__, __LINE__);
				//cObject->TObject::TObject();
				new(cObject)TObject(cPara1, cPara2);
				object = cObject;
			}
			catch (...)
			{
				return object;
			}
#else
			try
			{
				object = new TObject(cPara1, cPara2);
			}
			catch (...)
			{
				return object;
			}
#endif
			return object;
		}
		//构造对象
		static TBase* CreateObject(TPara1 cPara1, TPara2 cPara2, const char *cfile, const char *cfunction, int line)
		{
			TBase *object = NULL;
#ifdef USE_MEMORY_POOL
			try
			{
				TObject *cObject = (TObject*)MEMORY_MALLOC(sizeof(TObject), cfile, cfunction, line);
				//cObject->TObject::TObject();
				new(cObject)TObject(cPara1, cPara2);
				object = cObject;
			}
			catch (...)
			{
				return object;
			}
#else
			try
			{
				object = new TObject(cPara1, cPara2);
			}
			catch (...)
			{
				return object;
			}
#endif
			return object;
		}
	};

	/*! 
	* \class TCreateObject
	* \brief new操作创建对象
	*  TCreatebject<对象>
	*  CTestMemory *cTestMemory = CommonKit::TCreateObject<CTestMemory>::CreateObject(__FILE__, __FUNCTION__, __LINE__);
	*/
	template<class TObject, class TPara1, class TPara2, class TPara3, class TBase = TObject>
	class TCreateObjectP3
	{
	public:
		//构造对象
		static TBase* CreateObject(TPara1 cPara1, TPara2 cPara2, TPara3 cPara3)
		{
			TBase *object = NULL;
#ifdef USE_MEMORY_POOL
			try
			{
				TObject *cObject = (TObject*)MEMORY_MALLOC(sizeof(TObject), __FILE__, __FUNCTION__, __LINE__);
				//cObject->TObject::TObject();
				new(cObject)TObject(cPara1, cPara2, cPara3);
				object = cObject;
			}
			catch (...)
			{
				return object;
			}
#else
			try
			{
				object = new TObject(cPara1, cPara2, cPara3);
			}
			catch (...)
			{
				return object;
			}
#endif
			return object;
		}
		//构造对象
		static TBase* CreateObject(TPara1 cPara1, TPara2 cPara2, TPara3 cPara3, const char *cfile, const char *cfunction, int line)
		{
			TBase *object = NULL;
#ifdef USE_MEMORY_POOL
			try
			{
				TObject *cObject = (TObject*)MEMORY_MALLOC(sizeof(TObject), cfile, cfunction, line);
				//cObject->TObject::TObject();
				new(cObject)TObject(cPara1, cPara2, cPara3);
				object = cObject;
			}
			catch (...)
			{
				return object;
			}
#else
			try
			{
				object = new TObject(cPara1, cPara2, cPara3);
			}
			catch (...)
			{
				return object;
			}
#endif
			return object;
		}
	};

	/*! 
	* \class TDestroyObject
	* \brief delete操作销毁对象
	*  TDestroyObject<对象>
	*  CommonKit::TDestroyObject<CTestMemory>::DestroyObject(cTestMemory);
	*/
	template<class TObject>
	class TDestroyObject
	{
	public:
		//析构对象
		static void DestroyObject(TObject *object)
		{
			if (NULL == object)
			{
				OUTC_FAIL("[NetSDK] %s %s %d object is null.\n", __FILE__, __FUNCTION__, __LINE__);
				return;
			}
#ifdef USE_MEMORY_POOL
			object->~TObject();
			MEMORY_FREE(object);
#else
			try
			{
				delete object;
			}
			catch (...)
			{
			}
#endif
		}
	};

}

#endif

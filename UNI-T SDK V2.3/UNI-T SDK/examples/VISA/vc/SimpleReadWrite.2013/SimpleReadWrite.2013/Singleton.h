/*
 *使用方法：
 通过以下语句：CSingleton<类名>::Instance()，获取指向某类实例的指针。
 **/
template<class T>
class CSingleton
{
public:
	static T* Instance()
	{
		if (NULL == m_pInstance)
		{
			// 二次检查  
			if (NULL == m_pInstance)
			{
				m_pInstance = new T;
				atexit(Destory);
			}
		}

		return m_pInstance;
	}

protected:
	CSingleton() {} //防止实例  
	CSingleton(const CSingleton&) {} //防止拷贝构造一个实例  
	CSingleton& operator=(const CSingleton&){} //防止赋值出另一个实例  

	virtual ~CSingleton()
	{
	}

	static void Destory()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

private:
	static T* m_pInstance;
};

template<class T> T* CSingleton<T>::m_pInstance = NULL;
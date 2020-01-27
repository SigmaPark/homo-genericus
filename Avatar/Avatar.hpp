#pragma once


namespace sgm
{


	template<class T>
	class Avatar
	{
	public:
		Avatar(T& t) : pval(&t){}
		Avatar(T&&) = delete;

		operator T&(){  return *pval;  }
		auto value()-> T&{  return *pval;  }
		

	private:
		T* pval;
	};


}
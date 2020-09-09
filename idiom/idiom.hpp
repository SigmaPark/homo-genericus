#pragma once

#ifndef _SGM_IDIOM_
#define _SGM_IDIOM_


#ifndef SGM_MACROPACK
	#define SGM_MACROPACK(...) __VA_ARGS__
#else
	#error SGM_MACROPACK was already defined somewhere else.
#endif


namespace sgm
{

	struct Freezer_Violation;
	template<class T> class Freezer;

}
//========//========//========//========//=======#//========//========//========//========//=======#


struct sgm::Freezer_Violation
{
public:
	auto what() const-> wchar_t const*{  return L"Freezer_Violation";  }


private:
	template<class T> friend class Freezer;

	Freezer_Violation() = default;
};


template<class T>
class sgm::Freezer 
{
public:
	Freezer(T const& t) : _t(t), _frozen(false){}
	Freezer(T&& t) : _t( static_cast<T&&>(t) ), _frozen(false){}

	void freeze(){  _frozen = true;  }
	bool frozen() const{  return _frozen;  }
	
	operator T const&() const{  return _t;  }

	auto value()-> T&
	{
		if(frozen())
			throw Freezer_Violation();
		else
			return _t;  
	}


private:
	T _t;
	bool _frozen;
};
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{
	template<bool B, class...ARGS>
	static bool then(ARGS&&...){  return B;  }
}


#endif
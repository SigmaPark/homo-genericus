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


	template<class ITR1, class ITR2> struct Dual_iterator;


	struct Over_instantiation_Violation;
	template<class instance_t, size_t NOF_INSTANCE> struct Limited;

}
//========//========//========//========//=======#//========//========//========//========//=======#


#ifndef _SGM_VIOLATION
	#define _SGM_VIOLATION(NAME)	\
		struct sgm::NAME	\
		{	\
		public:	\
			auto what() const-> wchar_t const*{  return L###NAME;  }	\
			\
		protected:	\
			\
			NAME() = default;	\
		}


		_SGM_VIOLATION(Freezer_Violation);
		_SGM_VIOLATION(Over_instantiation_Violation);

	#undef _SGM_VIOLATION
#else
	#error _SGM_VIOLATION was already defined somewhere else.
#endif
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T>
class sgm::Freezer : public Freezer_Violation
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


template<class instance_t, size_t NOF_INSTANCE>
struct sgm::Limited : Over_instantiation_Violation
{
private:
	enum Count : long long{UP = 1, READ = 0, DOWN = -1};


	static auto _instance_count(Count cnt)-> size_t
	{
		static size_t nof_instance = 0;

		if (cnt == UP && nof_instance >= NOF_INSTANCE)
			throw Over_instantiation_Violation();
		else
			return nof_instance += cnt;
	}


public:
	static auto nof_instance()-> size_t{  return _instance_count(READ);  }


protected:
	Limited(){  _instance_count(UP);  }
	~Limited(){  _instance_count(DOWN);  }

	Limited(Limited const&){  _instance_count(UP);  }
};


template<class instance_t>
struct sgm::Limited<instance_t, 0>{  Limited() = delete;  };
//========//========//========//========//=======#//========//========//========//========//=======#


template<class ITR1, class ITR2> 
struct sgm::Dual_iterator
{
	ITR1 _1;  
	ITR2 _2;


	Dual_iterator(ITR1 itr1, ITR2 itr2) : _1(itr1), _2(itr2){}

	
	auto operator++(int)-> Dual_iterator
	{
		auto const itr = *this;

		_1++,  _2++;

		return itr;
	}

	auto operator++()-> Dual_iterator&
	{
		++_1,  ++_2;

		return *this;
	}
};
//========//========//========//========//=======#//========//========//========//========//=======#


namespace sgm
{

	template<bool B, class...ARGS>
	static bool then(ARGS&&...){  return B;  }


	template<class ITR1, class ITR2>
	static auto Dual_iteration(ITR1 itr1, ITR2 itr2)-> Dual_iterator<ITR1, ITR2>
	{
		return {itr1, itr2};
	}

}


#endif // end of #ifndef _SGM_IDIOM_
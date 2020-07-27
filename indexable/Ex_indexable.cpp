#include <cassert>
#include <array>
#include <vector>

#include "..\Avatar\Avatar.hpp"
#include <future>

#define USE_STL_VECTOR_AND_ARRAY
#include "indexable.hpp"
#undef USE_STL_VECTOR_AND_ARRAY
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#


using namespace sgm;


struct Tutorial : No_Making
{
private:
	template<class CON1, class CON2>
	static bool same_iterables(CON1&& con1, CON2&& con2)
	{
		auto itr1 = con1.cbegin();
		auto itr2 = con2.cbegin();

		bool res = con1.size() == con2.size();

		while(itr1 != con1.cend() && res)
			res = *itr1++ == *itr2++;

		return res;
	}


public:
	template<unsigned> static void Case();


	template<> static void Case<0>()
	{
		indexable<float> const ix{5, 2, 1};

		assert( same_iterables(ix, indexable<float, 3>{5, 2, 1}) );
	}


	template<> static void Case<1>()
	{
		indexable<double> ix1, ix2 = ix1, ix1_{4, 3}, ix2_(ix1_.begin(), ix1_.end());

		assert
		(	ix1.empty() && ix2.empty() && ix1_.size() == 2
		&&	same_iterables(ix1_, ix2_)
		);


		indexable<float> ix5(4), ix6(4, 7.f);

		assert
		(	ix5.capacity() == 4 && ix5.size() == 0 
		&&	same_iterables(ix6, indexable<float>{7.f, 7.f, 7.f, 7.f})
		);


		indexable<double, 5> ix3{1, 3, 5, 7, 9}, ix4 = ix3 ;

		assert
		(	same_iterables(ix3, ix4)
		&&	ix3.size() == 5
		);
	}


	template<> static void Case<2>()
	{
		indexable<float> ix1{1, 2, 3}, ix2 = ix1, ix3 = ix1;

		ix1.~indexable();

		assert
		(	ix1.size() == 0 && ix2.size() != 0
		&&	same_iterables(ix2, ix3)
		);
	}
	//========//========//========//========//=======#//========//========//========//========//===

	template
	<	class T, class _raw_t = std::remove_reference_t<T>
	,	bool DEFAULT_CONSTRUCTABLE = std::is_default_constructible<_raw_t>::value
	,	bool COPY_CONSTRUCTABLE = std::is_copy_constructible<_raw_t>::value
	,	bool MOVE_CONSTRUCTABLE = std::is_move_constructible<_raw_t>::value
	,	bool COPY_ASSIGNABLE = std::is_copy_assignable<_raw_t>::value
	,	bool MOVE_ASSIGNABLE = std::is_move_assignable<_raw_t>::value
	>
	struct Special_Member_info
	{
		enum : bool
		{	IS_DEFAULT_CONSTRUCTABLE = DEFAULT_CONSTRUCTABLE
		,	IS_COPY_CONSTRUCTABLE = COPY_CONSTRUCTABLE
		,	IS_MOVE_CONSTRUCTABLE = MOVE_CONSTRUCTABLE
		,	IS_COPY_ASSIGNABLE = COPY_ASSIGNABLE
		,	IS_MOVE_ASSIGNABLE = MOVE_ASSIGNABLE
		};
	};

	template<bool...> struct BinaryFlags : No_Making{};


	template<class T>
	struct _core_part
	{
		T* _core;
		size_t _size;
	};


	template<class T>
	struct iterative_methods : _core_part<T>
	{
		auto cbegin() const-> T const&{  return _core[0];  };
		auto cend() const-> T const&{  return _core[_size - 1];  }
		
		auto begin() const-> T const&{  return cbegin();  };
		auto end() const-> T const&{  return cend();  }

		auto begin()-> T&{  return _core[0];  };
		auto end()-> T&{  return _core[_size - 1];  }
	};


	//template<class T, size_t SIZE>
	//class _Core
	//{
	//private:
	//	template<class T, size_t S> struct ixData : No_Making{  using type = T[S];  };
	//	template<class T> struct ixData<T, 0> : No_Making{  using type = T*;  };

	//public:
	//	using core_t = typename ixData<T, SIZE>::type;

	//	core_t _core;
	//};

	
	template<class T>
	class _CommonMethods
	{
	public:
		//

		//auto operator[](size_t idx)-> T&{  return _core[idx];  }
		//auto operator[](size_t idx) const-> T const&{  return _core[idx];  }

		//auto cdata() const-> T const*{  return _core;  }
		//auto data() const-> T const*{  return cdata();  }
		//auto data()-> T*{  return _core;  }
	};


	template<class T, size_t SIZE = 0>
	class Container// : public _CommonMethods< Container<T, SIZE> >
	{
	private:
		template<class T, size_t S> struct ixData : No_Making{  using type = T[S];  };
		template<class T> struct ixData<T, 0> : No_Making{  using type = T*;  };

	public:
		using core_t = typename ixData<T, SIZE>::type;

		core_t _core;

		Container() = default;
	};


	template<> static void Case<3>()
	{
		Container<int, 3> con{1, 2, 3};

		
	}

};
//========//========//========//========//=======#//========//========//========//========//=======#



template<unsigned N>
struct Static_Par
{
	template<class F>
	static void For(F&& f)
	{
		auto fut = std::async(f, N - 1);

		Static_Par<N - 1>::For(f);

		fut.get();
	}
};

template<>
struct Static_Par<0>
{
	template<class F> static void For(F&&){}
};


template<class F>
static void Dynamic_Par_For(F&& f, unsigned d)
{
	auto fut = std::async(f, --d);

	if(d > 0)
		Dynamic_Par_For(f, d);

	fut.get();
}


struct _Concurrent_Helper : No_Making
{
protected:
	struct _Ranger
	{
		_Ranger(size_t idx_begin, size_t idx_end, unsigned nof_task)
		:	_idx_begin(idx_begin), _idx_end(idx_end), _nof_task( static_cast<size_t>(nof_task) )
		{
			assert(idx_begin <= idx_end && L"invalid index range.\n");

			_total_size = idx_end - idx_begin,
			_loop_q = _total_size / _nof_task,
			_loop_r = _total_size % _nof_task;
		}


		indexable<size_t, 2> operator()(unsigned task_id) const
		{
			auto const 
				d = static_cast<size_t>(task_id),
				begin0 = d * _loop_q + (d < _loop_r ? d : _loop_r),
				end0 = begin0 + _loop_q + (d < _loop_r ? 1 : 0);

			return indexable<size_t, 2>{begin0 + _idx_begin, end0 + _idx_begin};
		}


	private:
		size_t _idx_begin, _idx_end, _nof_task, _total_size, _loop_q, _loop_r;
	};

};


template<unsigned NOF_TASK = unsigned(-1)>
struct Concurrent : _Concurrent_Helper
{
	template<class F>
	static void Loop(size_t const idx_begin, size_t const idx_end, F&& func)
	{
		Static_Par<NOF_TASK>::For
		(	[&func, idx_begin, idx_end](unsigned d)
			{
				_Ranger const ranger(idx_begin, idx_end, NOF_TASK);
				auto const range = ranger(d);

				func(range[0], range[1]);
			}
		);
	}

	template<  class CON, class F, class = std::enable_if_t< is_iterable<CON>::value >  >
	static void Loop(CON&& con, F&& func)
	{
		Loop( 0, con.size(), std::forward<F>(func) );
	}
};


template<>
struct Concurrent<unsigned(-1)> : _Concurrent_Helper
{
	template<class F>
	static void Loop
	(	size_t const idx_begin, size_t const idx_end, F&& func
	,	unsigned const nof_task = std::thread::hardware_concurrency()
	)
	{
		Dynamic_Par_For
		(	[&func, idx_begin, idx_end, nof_task](unsigned d)
			{
				_Ranger const ranger(idx_begin, idx_end, nof_task);
				auto const range = ranger(d);

				func(range[0], range[1]);
			}
		,	nof_task
		);
	}

	template<  class CON, class F, class = std::enable_if_t< is_iterable<CON>::value >  >
	static void Loop
	(	CON&& con, F&& func
	,	unsigned const nof_task = std::thread::hardware_concurrency()
	)
	{
		Loop( 0, con.size(), std::forward<F>(func), nof_task );
	}
};


#if 1
int main()
{
	Tutorial::Case<0>();
	Tutorial::Case<1>();
	Tutorial::Case<2>();
	Tutorial::Case<3>();

	indexable<size_t, 998> ix;

	auto func
	=	[&ix](size_t const idx_begin, size_t idx_end)
		{
			while(idx_end-->idx_begin)
				ix[idx_end] = idx_end;
		};

	//enum : size_t{NOF_TASK = 4};
	//Concurrent<NOF_TASK>::Loop(0, ix.size(), func);

	Concurrent<>::Loop(0, ix.size(), func, 4);

	for(size_t idx = 0; idx < ix.size(); ++idx)
		assert(ix[idx] == idx);

	return 0;
}
#endif
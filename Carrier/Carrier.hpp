#pragma once

#ifndef _SGM_CARRIER_
#define _SGM_CARRIER_

#if defined(_MSC_VER) && _MSC_VER < 1800
	#error C++11 or higher version language support is required.
#endif

namespace sgm
{

	template<typename T, size_t N>
	class Static_CArr
	{
	public:
		using value_type = T;
		enum : size_t { SIZE = N };

		Static_CArr() = default;

		Static_CArr(T const arr[])
		{
			for(size_t idx = 0; idx < N; idx++)
				_arr[idx] = arr[idx];
		}

		auto operator[](size_t idx) const-> T const&{  return _arr[idx];  }
		auto at(size_t idx)-> T&{  return _arr[idx];  }
		auto size() const-> size_t{  return N;  }

	private:
		T _arr[N];
	};


	template<typename T, typename...TYPES, size_t SIZE = sizeof...(TYPES)>
	static auto make_SttCArr(TYPES&&...types)-> Static_CArr<T, SIZE>
	{
		T arr[SIZE] = { static_cast<T>(types)... };

		return sgm::Static_CArr<T, SIZE>(arr);
	}


	template<typename T, size_t N, typename STD_CON>
	static auto SttCArr_from(STD_CON&& con)-> Static_CArr<T, N>
	{
		T arr[N];

		struct Cycle{ size_t idx; decltype(con.cbegin()) itr; };

		for
		(	Cycle c{0, con.cbegin()}
		;	c.itr != con.cend()
		;	arr[c.idx++] = *(c.itr++)
		);

		return Static_CArr<T, N>(arr);
	}
	//========//========//========//========//=======#//========//========//========//========//===


	template<typename T>
	class Dynamic_CArr
	{
	public:	
		using value_type = T;

		Dynamic_CArr(Dynamic_CArr const& cpy) : _arr(nullptr), _n(cpy.size())
		{
			_arr = new T[_n];

			for(size_t idx = 0; idx < _n; idx++)
				_arr[idx] = cpy._arr[idx];
		}

		Dynamic_CArr(Dynamic_CArr&& temp) : _arr(nullptr), _n(temp.size())
		{
			_arr = temp._arr;
			temp._arr = nullptr;
		}

		~Dynamic_CArr()
		{	
			if(_arr != nullptr)
				delete[] _arr;
		}

		auto operator[](size_t idx) const-> T const&{  return _arr[idx];  }
		auto at(size_t idx)-> T&{  return _arr[idx];  }
		auto size() const-> size_t{  return _n;  }
		
	private:
		template<typename _T, typename...TYPES>
		friend auto make_DnmCArr(TYPES&&...types)-> Dynamic_CArr<_T>;

		template<typename _T, typename STD_CON>
		friend auto DnmCArr_from(STD_CON&& con)-> Dynamic_CArr<_T>;

		Dynamic_CArr(T const arr[], size_t n) : _arr( const_cast<T*>(arr) ), _n(n){}
		
		T* _arr;
		size_t const _n;
	};


	template<typename _T, typename...TYPES>
	static auto make_DnmCArr(TYPES&&...types)-> Dynamic_CArr<_T>
	{
		enum : size_t{N = sizeof...(TYPES)};

		_T	temp[N] = { static_cast<_T>(types)... },
			*parr = new _T[N];

		for(size_t idx = 0; idx < N; idx++)
			parr[idx] = temp[idx];

		return Dynamic_CArr<_T>(parr, N);
	}


	template<typename _T, typename STD_CON>
	static auto DnmCArr_from(STD_CON&& con)-> Dynamic_CArr<_T>
	{
		size_t const n = con.size();
		_T* parr = new _T[n];

		struct Cycle{ size_t idx; decltype(con.cbegin()) itr; };

		for
		(	Cycle c{0, con.cbegin()}
		;	c.itr != con.cend()
		;	parr[c.idx++] = *(c.itr++)
		);

		return Dynamic_CArr<_T>(parr, n);
	}
	//========//========//========//========//=======#//========//========//========//========//===
}

#endif
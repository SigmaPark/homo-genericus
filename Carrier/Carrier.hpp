#pragma once

#ifndef _SGM_CARRIER_
#define _SGM_CARRIER_

#if defined(_MSC_VER) && _MSC_VER < 1800
	#error C++11 or higher version language support is required.
#endif

/**	warning when c-style array was declared without initialization.
	disable because we don't have no method to do it.
*/
#pragma warning( disable : 4701)

#include <initializer_list>

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{

	enum class Carry_t { SHALLOW, DEEP };

	template<typename, size_t, Carry_t = Carry_t::SHALLOW> class Static_CArr;
	template<typename, Carry_t = Carry_t::SHALLOW> class Dynamic_CArr;
	//========//========//========//========//=======#//========//========//========//========//===


	template<bool> class Boolean_Flag;
	template<> class Boolean_Flag<true>{  public: enum : bool{ value = true };  };
	template<> class Boolean_Flag<false>{  public: enum : bool{ value = false };  };
	//========//========//========//========//=======#//========//========//========//========//===

	template<typename T>
	class is_mutable
	{
	private:
		template<typename Q>
		static auto get_type(Q const*){  return Boolean_Flag<false>();  }

		template<typename Q>
		static auto get_type(Q*){  return Boolean_Flag<true>();  }
		
	public:
		enum : bool{ value = decltype(  get_type( (T*) nullptr )  )::value };
	};
	//========//========//========//========//=======#//========//========//========//========//===


	template<typename Ref_t, typename T>
	class same_mutability
	{
	private:
		template<typename Q>
		static auto get_type(Q const*){  return *(T const*)nullptr;  }

		template<typename Q>
		static auto get_type(Q*){  return *(T*) nullptr;  }

	public:
		using type = decltype(  get_type( (Ref_t*) nullptr )  );
	};
	//========//========//========//========//=======#//========//========//========//========//===


	template<typename T> 
	class Proxy_Ref
	{
	public:
		Proxy_Ref() : _just_refered(true), _pval(nullptr){}
		Proxy_Ref(T const& t) : _just_refered(true), _pval(&t){}
		Proxy_Ref(T&& t) : _just_refered(false), _pval( new T(t) ){}

		Proxy_Ref(Proxy_Ref const& cpy) 
		:	_just_refered(cpy._just_refered)
		,	_pval( cpy._just_refered ? cpy._pval : new T(*cpy._pval) )
		{}

		Proxy_Ref(Proxy_Ref&& temp) 
		:	_just_refered(temp._just_refered)
		,	_pval( new T(*temp._pval) )
		{}

		~Proxy_Ref()
		{
			release_if_deep(),
			_pval = nullptr;
		}

		bool is_empty() const{  return _pval == nullptr;  }
		bool is_deep() const{  return !_just_refered;  }

		operator T const&() const{  return *_pval;  }


		auto operator=(T const& t)-> T const&
		{  
			release_if_deep(),
			_pval = &t;

			return *_pval;
		}

		auto operator=(T&& t)-> T const&
		{
			release_if_deep(),
			_pval = new T(t),
			_just_refered = false;

			return *_pval;
		}

		template<typename _T>
		void deep(_T&& _t)
		{
			release_if_deep(),
			_pval = new T(_t),
			_just_refered = false;
		}

		template<typename _T>
		void shallow(_T&& _t)
		{
			release_if_deep(),
			_pval = &static_cast<T const&>(_t);
		}

	private:
		bool _just_refered;
		T const* _pval;

		void release_if_deep()
		{
			if (is_deep())
				delete _pval;

			_just_refered = true;
		}

	};	// End of class Proxy_Ref
	//========//========//========//========//=======#//========//========//========//========//===


	template<typename CARR>
	class CArr_iterator
	{
	public:
		CArr_iterator(CARR const* pcarr, size_t idx) : _pcarr(pcarr), _idx(idx){}

		auto operator++()-> CArr_iterator{  return CArr_iterator(_pcarr, ++_idx);  }
		auto operator++(int)-> CArr_iterator{  _idx++; return *this;  }
		bool operator!=(CArr_iterator aitr) const{  return aitr._idx != _idx;  }
		bool operator==(CArr_iterator aitr) const{  return aitr._idx == _idx;  }
		
		auto operator*() const-> Proxy_Ref<typename CARR::value_t>{  return _pcarr->at(_idx);  }

	private:
		CARR const* _pcarr;
		size_t _idx;
	};


	template<typename CARR>
	class CArr_const_iterator
	{
	public:
		CArr_const_iterator(CARR const* pcarr, size_t idx) : _pcarr(pcarr), _idx(idx) {}

		auto operator++()-> CArr_const_iterator{  return CArr_const_iterator(_pcarr, ++_idx);  }
		auto operator++(int)-> CArr_const_iterator{  _idx++; return *this;  }
		bool operator!=(CArr_const_iterator aitr) const{  return aitr._idx != _idx;  }
		bool operator==(CArr_const_iterator aitr) const{  return aitr._idx == _idx;  }

		auto operator*() const-> typename CARR::value_t const&{  return (*_pcarr)[_idx];  }

	private:
		CARR const* _pcarr;
		size_t _idx;
	};
	//========//========//========//========//=======#//========//========//========//========//===


	class _Copy_Method
	{
	public:
		_Copy_Method() = delete;


		template<typename CON, typename T>
		static void iterative_auto_copy(CON const& con, T _a[])
		{
			size_t idx = 0;
			auto itr = con.begin();

			for
			(
			;	itr != con.end()
			;	_a[idx] = *itr, idx++, itr++
			);
		}


		template<typename CON, typename T>
		static void iterative_deep_copy(CON const& con, Proxy_Ref<T> _a[])
		{
			size_t idx = 0;
			auto itr = con.begin();

			for
			(
			;	itr != con.end()
			;	_a[idx].deep(*itr), idx++, itr++
			);
		}


		template<typename CON, typename T>
		static void indexed_auto_copy(CON const& con, T _a[], size_t offset = 0)
		{
			for
			(	size_t idx = offset, size = con.size()
			;	idx < size
			;	_a[idx - offset] = con[idx], idx++
			);
		}


		template<typename CON, typename T>
		static void indexed_deep_copy(CON const& con, Proxy_Ref<T> _a[], size_t offset = 0)
		{
			for
			(	size_t idx = offset, size = con.size()
			;	idx < size
			;	_a[idx - offset].deep(con[idx]), idx++
			);
		}


		template<typename CARR, typename ITR>
		static void record_from(CARR const* pcarr, ITR itr)
		{
			for
			(	auto citr = pcarr->begin()
			;	citr != pcarr->end()
			;	*itr = *citr, itr++, citr++ 
			);
		}

	};	// End of class _Copy_Method
	//========//========//========//========//=======#//========//========//========//========//===


	template<typename T, size_t N>
	class Static_CArr<T, N, Carry_t::SHALLOW>
	{
	private:
		using _internal_t = Proxy_Ref<T>;

		_internal_t _arr[N];

		
	public:
		using value_t = T;
		using proxy_t = Proxy_Ref<T>;
		using const_iterator_t = CArr_const_iterator<Static_CArr>;
		using iterator_t = CArr_iterator<Static_CArr>;

		enum : size_t { SIZE = N };
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		Static_CArr() = default;

		template<typename Q>
		Static_CArr(std::initializer_list<Q>&& iL)
		{
			if(iL.size() != SIZE)
				throw !(bool) "size of the object is not matched";

			_Copy_Method::iterative_deep_copy(iL, _arr);
		}

		Static_CArr(Static_CArr<T, N, Carry_t::DEEP> const& Lval)
		{
			_Copy_Method::iterative_auto_copy(Lval, _arr);
		}

		Static_CArr(Static_CArr<T, N, Carry_t::DEEP>&& temp)
		{
			_Copy_Method::indexed_deep_copy(temp, _arr);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		template<typename CON>
		auto operator=(CON& cpy)-> Static_CArr const&
		{
			return *this = static_cast<CON const&>(cpy);
		}

		template<typename CON>
		auto operator=(CON const& cpy)-> Static_CArr const&
		{
			if(cpy.size() != SIZE)
				throw !(bool) "size of the object is not matched";
			
			_Copy_Method::iterative_auto_copy(cpy, _arr);

			return *this;
		}

		template<typename CON>
		auto operator=(CON&& temp)-> Static_CArr const&
		{
			if(temp.size() != SIZE)
				throw !(bool) "size of the object is not matched";

			_Copy_Method::iterative_deep_copy(temp, _arr);

			return *this;
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		auto operator[](size_t idx) const-> value_t const&{  return _arr[idx];  }
		auto at(size_t idx) const-> proxy_t{  return _arr[idx];  }	// Be aware of dangling

		auto size() const-> size_t{  return SIZE;  }

		auto cbegin() const-> const_iterator_t{  return const_iterator_t(this, 0);  }
		auto cend() const-> const_iterator_t{  return const_iterator_t(this, SIZE);  }

		auto begin() const-> iterator_t{  return iterator_t(this, 0);  }
		auto end() const-> iterator_t{  return iterator_t(this, SIZE);  }

		template<typename ITR>
		void record_from(ITR itr) const{  _Copy_Method::record_from(this, itr);  }
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		template<typename Q>
		static auto from_iterable(Q& Lval)-> Static_CArr
		{	
			return from_iterable( static_cast<Q const&>(Lval) );
		}

		template<typename Q>
		static auto from_iterable(Q const& Lval)-> Static_CArr
		{
			if(Lval.size() != SIZE)
				throw !(bool) "size of the object is not matched.";

			Static_CArr stt;

			_Copy_Method::iterative_auto_copy(Lval, stt._arr);
				
			return stt;
		}

		template<typename Q> 
		static auto from_iterable(Q&& temp)-> Static_CArr
		{
			if(temp.size() != SIZE)
				throw !(bool) "size of the object is not matched.";

			Static_CArr stt;

			_Copy_Method::iterative_deep_copy(temp, stt._arr);
				
			return stt;
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		template<typename Q>
		static auto from_indexed(Q& Lval, size_t offset = 0)-> Static_CArr
		{
			return from_indexed( static_cast<Q const&>(Lval, offset) );
		}

		template<typename Q> 
		static auto from_indexed(Q const& Lval, size_t offset = 0)-> Static_CArr
		{
			if(SIZE != Lval.size() - offset)
				throw !(bool) "size of the object is not matched.";

			Static_CArr stt;

			_Copy_Method::indexed_auto_copy(Lval, stt._arr, offset);

			return stt;
		}

		template<typename Q> 
		static auto from_indexed(Q&& temp, size_t offset = 0)-> Static_CArr
		{
			if(SIZE != temp.size() - offset)
				throw !(bool) "size of the object is not matched.";

			Static_CArr stt;

			_Copy_Method::indexed_deep_copy(temp, stt._arr, offset);

			return stt;
		}
		
	};	// End of class Static_CArr<T, N, Carry_t::SHALLOW>
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<typename T, size_t N>
	class Static_CArr<T, N, Carry_t::DEEP>
	{
	private:
		using _internal_t = T;

		_internal_t _arr[N];
		

	public:
		using value_t = T;
		using const_iterator_t = CArr_const_iterator<Static_CArr>;
		using iterator_t = CArr_iterator<Static_CArr>;
		enum : size_t { SIZE = N };
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		Static_CArr() = default;

		template<typename Q>
		Static_CArr(std::initializer_list<Q>&& iL)
		{
			if(SIZE != iL.size())
				throw !(bool) "size of the object is not matched.";

			_Copy_Method::iterative_auto_copy(iL, _arr);
		}

		Static_CArr(Static_CArr<T, N, Carry_t::SHALLOW> const& Lval)
		{
			_Copy_Method::iterative_auto_copy(Lval, _arr);
		}

		Static_CArr(Static_CArr<T, N, Carry_t::SHALLOW>&& temp)
		{
			_Copy_Method::iterative_auto_copy(temp, _arr);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		template<typename CON>
		auto operator=(CON&& con)-> Static_CArr const&
		{
			if(SIZE != con.size())
				throw !(bool) "size of the object is not matched.";	
				
			_Copy_Method::iterative_auto_copy(con, _arr);

			return *this;
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		auto operator[](size_t idx) const-> value_t const&{  return _arr[idx];  }
		auto at(size_t idx) const-> _internal_t&{  return _arr[idx];  }	

		auto size() const-> size_t{  return SIZE;  }

		auto cbegin() const-> const_iterator_t{  return const_iterator_t(this, 0);  }
		auto cend() const-> const_iterator_t{  return const_iterator_t(this, SIZE);  }

		auto begin() const-> iterator_t{  return iterator_t(this, 0);  }
		auto end() const-> iterator_t{  return iterator_t(this, SIZE);  }

		template<typename ITR>
		void record_from(ITR itr) const{  _Copy_Method::record_from(this, itr);  }
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		template<typename Q>
		static auto from_iterable(Q&& Uval)-> Static_CArr
		{
			if(SIZE != Uval.size())
				throw !(bool) "size of the object is not matched.";

			Static_CArr stt;

			_Copy_Method::iterative_auto_copy(Uval, stt._arr);
				
			return stt;
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		template<typename Q> 
		static auto from_indexed(Q&& Uval, size_t offset = 0)-> Static_CArr
		{
			if(SIZE != Uval.size() - offset)
				throw !(bool) "size of the object is not matched.";

			Static_CArr stt;

			_Copy_Method::indexed_auto_copy(Uval, stt._arr, offset);

			return stt;
		}
		
	};	// End of class Static_CArr<T, N, Carry_t::DEEP>
	//========//========//========//========//=======#//========//========//========//========//===


	template<typename T>
	class Dynamic_CArr<T, Carry_t::SHALLOW>
	{
	private:
		using _internal_t = Proxy_Ref<T>;

		_internal_t* _arr;
		size_t _n;

		Dynamic_CArr(_internal_t arr[], size_t n) : _arr(arr), _n(n) {}

		
	public:
		using value_t = T;
		using proxy_t = Proxy_Ref<T>;
		using const_iterator_t = CArr_const_iterator<Dynamic_CArr>;
		using iterator_t = CArr_iterator<Dynamic_CArr>;
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		template<typename Q>
		Dynamic_CArr(std::initializer_list<Q>&& iL) : _arr(nullptr), _n(iL.size())
		{
			_arr = new _internal_t[_n],
			_Copy_Method::iterative_deep_copy(iL, _arr);
		}

		template<Carry_t C>
		Dynamic_CArr(Dynamic_CArr<T, C> const& cpy) : _arr(nullptr), _n(cpy.size())
		{
			_arr = new _internal_t[_n],
			_Copy_Method::iterative_auto_copy(cpy, _arr);
		}

		template<Carry_t C>
		Dynamic_CArr(Dynamic_CArr<T, C>&& temp) : _arr(nullptr), _n(temp.size())
		{
			_arr = temp._arr,
			temp._arr = nullptr;
		}

		~Dynamic_CArr()
		{
			delete[] _arr,
			_arr = nullptr;
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		template<typename CON>
		auto operator=(CON& con)-> Dynamic_CArr const&
		{
			return *this = static_cast<CON const&>(con);
		}

		template<typename CON>
		auto operator=(CON const& cpy)-> Dynamic_CArr const&
		{
			if(cpy.size() != _n)
				throw !(bool) "size of the object is not matched.";

			_Copy_Method::iterative_auto_copy(cpy, _arr);

			return *this;
		}

		template<typename CON>
		auto operator=(CON&& temp)-> Dynamic_CArr const&
		{
			if(temp.size() != _n)
				throw !(bool) "size of the object is not matched.";

			_Copy_Method::iterative_deep_copy(temp, _arr);

			return *this;
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		auto operator[](size_t idx) const-> value_t const&{ return _arr[idx]; }
		auto at(size_t idx) const-> proxy_t{  return _arr[idx];  }	// Be aware of dangling

		auto size() const-> size_t{ return _n; }

		auto cbegin() const-> const_iterator_t{  return const_iterator_t(this, 0);  }
		auto cend() const-> const_iterator_t{  return const_iterator_t(this, _n);  }

		auto begin() const-> iterator_t{  return iterator_t(this, 0);  }
		auto end() const-> iterator_t{  return iterator_t(this, _n);  }

		template<typename ITR>
		void record_from(ITR itr) const{  _Copy_Method::record_from(this, itr);  }
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		template<typename Q>
		static auto from_iterable(Q& Lval)-> Dynamic_CArr
		{	
			return from_iterable( static_cast<Q const&>(Lval) );
		}

		template<typename Q>
		static auto from_iterable(Q const& Lval)-> Dynamic_CArr
		{
			size_t const n = Lval.size();
			_internal_t* arr = new _internal_t[n];

			_Copy_Method::iterative_auto_copy(Lval, arr);
				
			return Dynamic_CArr(arr, n);
		}

		template<typename Q> 
		static auto from_iterable(Q&& temp)-> Dynamic_CArr
		{
			size_t const n = temp.size();
			_internal_t* arr = new _internal_t[n];

			_Copy_Method::iterative_deep_copy(temp, arr);
				
			return Dynamic_CArr(arr, n);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		template<typename Q>
		static auto from_indexed(Q& Lval, size_t offset = 0)-> Dynamic_CArr
		{
			return from_indexed( static_cast<Q const&>(Lval, offset) );
		}

		template<typename Q> 
		static auto from_indexed(Q const& Lval, size_t offset = 0)
		->	Dynamic_CArr
		{
			size_t const n = Lval.size() - offset;
			_internal_t* arr = new _internal_t[n];

			_Copy_Method::indexed_auto_copy(Lval, arr, offset);

			return Dynamic_CArr(arr, n);
		}

		template<typename Q> 
		static auto from_indexed(Q&& temp, size_t offset = 0)-> Dynamic_CArr
		{
			size_t const n = temp.size() - offset;
			_internal_t* arr = new _internal_t[n];

			_Copy_Method::indexed_deep_copy(temp, arr, offset);

			return Dynamic_CArr(arr, n);
		}

	};	// End of class Dynamic_CArr<T, Carry_t::SHALLOW>
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<typename T>
	class Dynamic_CArr<T, Carry_t::DEEP>
	{
	private:
		using _internal_t = T;

		_internal_t* _arr;
		size_t _n;

		Dynamic_CArr(_internal_t arr[], size_t n) : _arr(arr), _n(n) {}

		template<typename U>
		void alloc_and_copy(U&& Uval)
		{
			_arr = new _internal_t[_n],
			_Copy_Method::iterative_auto_copy(Uval, _arr);
		}

		
	public:
		using value_t = T;
		using const_iterator_t = CArr_const_iterator<Dynamic_CArr>;
		using iterator_t = CArr_iterator<Dynamic_CArr>;

		template<typename Q>
		Dynamic_CArr(std::initializer_list<Q>&& iL) : _arr(nullptr), _n(iL.size())
		{
			alloc_and_copy(iL);
		}

		Dynamic_CArr(Dynamic_CArr const& cpy) : _arr(nullptr), _n(cpy.size())
		{
			alloc_and_copy(cpy);
		}

		Dynamic_CArr(Dynamic_CArr&& temp) : _arr(nullptr), _n(temp.size())
		{
			_arr = temp._arr,
			temp._arr = nullptr;
		}

		~Dynamic_CArr()
		{
			delete[] _arr,
			_arr = nullptr;
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		template<typename CON>
		auto operator=(CON&& con)-> Dynamic_CArr const&
		{
			if(con.size() != _n)
				throw !(bool) "size of the object is not matched.";

			_Copy_Method::iterative_auto_copy(con, _arr);

			return *this;
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		auto operator[](size_t idx) const-> value_t const&{ return _arr[idx]; }
		auto at(size_t idx) const-> _internal_t&{  return _arr[idx];  }

		auto size() const-> size_t{ return _n; }

		auto cbegin() const-> const_iterator_t{  return const_iterator_t(this, 0);  }
		auto cend() const-> const_iterator_t{  return const_iterator_t(this, _n);  }

		auto begin() const-> iterator_t{  return iterator_t(this, 0);  }
		auto end() const-> iterator_t{  return iterator_t(this, _n);  }

		template<typename ITR>
		void record_from(ITR itr) const{  _Copy_Method::record_from(this, itr);  }
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		template<typename Q> 
		static auto from_iterable(Q&& Uval)-> Dynamic_CArr
		{
			size_t const n = Uval.size();
			_internal_t* arr = new _internal_t[n];

			_Copy_Method::iterative_auto_copy(Uval, arr);
				
			return Dynamic_CArr(arr, n);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		template<typename Q> 
		static auto from_indexed(Q&& Uval, size_t offset = 0)-> Dynamic_CArr
		{
			size_t const n = Uval.size() - offset;
			_internal_t* arr = new _internal_t[n];

			_Copy_Method::indexed_auto_copy(Uval, arr, offset);

			return Dynamic_CArr(arr, n);
		}

	};	// End of class Dynamic_CArr<T, Carry_t::DEEP>
	//========//========//========//========//=======#//========//========//========//========//===


	template<typename T>
	class _Original_type
	{
	private:
		template<typename Q> class typer			{  public: using type = Q;  };
		template<typename Q> class typer<Q const>	{  public: using type = Q;  };
		template<typename Q> class typer<Q&>			{  public: using type = Q;  };
		template<typename Q> class typer<Q const&>	{  public: using type = Q;  };
		template<typename Q> class typer<Q&&>		{  public: using type = Q;  };

	public:
		using type = typename typer<T>::type;
	};

	template<typename T>
	using Original_t = typename _Original_type<T>::type;
	//========//========//========//========//=======#//========//========//========//========//===


	template<typename T>
	class Carrier
	{
	public:
		Carrier() : _pp(nullptr), size_t(0){}

		template<typename CON>
		Carrier(CON&& con) 
		:	_pp( new T const*[con.size()] )
		,	_n(con.size())
		{
			auto p = _pp[0];
			auto itr = con.begin();

			for(; itr != con.end(); ++itr, ++p)
				p = static_cast<T const*>(&*itr);
		}

		Carrier(Carrier const& cpy) : _pp( new T const*[cpy._n] ), _n(cpy._n)
		{
			for(size_t idx = 0; idx < _n; idx++)
				_pp[idx] = cpy._pp[idx];
		}

		Carrier(Carrier&& cpy) : _pp(cpy._pp), _n(cpy._n){  cpy._pp = nullptr;  }
		~Carrier(){  delete[] _pp, _pp = nullptr;  }


		template<typename CON>
		auto operator=(CON&& con)-> Carrier const&
		{
			if (con.size() != _n)
				delete[] _pp,
				_pp = new T const*[con.size()];
			
			auto p = _pp[0];
			auto itr = con.begin();

			for(; itr != con.end(); ++itr, ++p)
				p = static_cast<T const*>(&*itr);
			
			return *this;
		}

		auto operator=(Carrier const& cpy)-> Carrier const&
		{
			if (cpy._n != _n)
				delete[] _pp,
				_pp = new T const*[cpy._n];

			for(size_t idx = 0; idx < _n; idx++)
				_pp[idx] = cpy._pp[idx];

			return *this;
		}

		auto size() const-> size_t{  return _n;  }
		auto operator[](size_t idx) const-> T const&{  return *_pp[idx];  }

	private:
		T const** _pp;
		size_t _n;
	};
	//========//========//========//========//=======#//========//========//========//========//===


	class CArrier_Trait
	{
	public:
		CArrier_Trait() = delete;

	#define _CARR_TPACK(...) __VA_ARGS__

	#define _CARR_DECLARE_TEMPLATE(sig, name, spec, bool_value)		\
		template<sig>	\
		class name spec	\
		{	\
		public:	\
			name() = delete;	\
			\
			enum : bool { value = bool_value };	\
		}

	#define _CARR_DECLARE_FALSE_TEMPLATE(name) \
		_CARR_DECLARE_TEMPLATE(typename, name, /* no spec */, false)


		_CARR_DECLARE_FALSE_TEMPLATE(is_Static);

		_CARR_DECLARE_TEMPLATE
		(	_CARR_TPACK(typename T, size_t N, Carry_t CT)
		,	is_Static, _CARR_TPACK(< Static_CArr<T, N, CT> >), true
		);


		_CARR_DECLARE_FALSE_TEMPLATE(is_Dynamic);
		
		_CARR_DECLARE_TEMPLATE
		(	_CARR_TPACK(typename T, Carry_t CT)
		,	is_Dynamic, _CARR_TPACK(< Dynamic_CArr<T, CT> >), true
		);

		_CARR_DECLARE_TEMPLATE
		(	typename CArr
		,	is, /* no spec */, is_Dynamic<CArr>::value || is_Static<CArr>::value
		);


		_CARR_DECLARE_FALSE_TEMPLATE(is_Shallow);

		_CARR_DECLARE_TEMPLATE
		(	_CARR_TPACK(typename T)
		,	is_Shallow, _CARR_TPACK(< Dynamic_CArr<T, Carry_t::SHALLOW> >), true
		);

		_CARR_DECLARE_TEMPLATE
		(	_CARR_TPACK(typename T, size_t N)
		,	is_Shallow, _CARR_TPACK(< Static_CArr<T, N, Carry_t::SHALLOW> >), true
		);


		_CARR_DECLARE_FALSE_TEMPLATE(is_Deep);

		_CARR_DECLARE_TEMPLATE
		(	_CARR_TPACK(typename T)
		,	is_Deep, _CARR_TPACK(< Dynamic_CArr<T, Carry_t::DEEP> >), true
		);

		_CARR_DECLARE_TEMPLATE
		(	_CARR_TPACK(typename T, size_t N)
		,	is_Deep, _CARR_TPACK(< Static_CArr<T, N, Carry_t::DEEP> >), true
		);


	#undef _CARR_DECLARE_FALSE_TEMPLATE
	#undef _CARR_DECLARE_TEMPLATE
	#undef _CARR_TPACK

	};	// End of class CArrier_Traits
	//========//========//========//========//=======#//========//========//========//========//===
}

#endif
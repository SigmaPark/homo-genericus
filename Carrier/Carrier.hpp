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

/**	No STL headers!
*/


namespace sgm
{

	enum class Carry_t { SHALLOW, DEEP };

	template<typename, size_t, Carry_t = Carry_t::SHALLOW> class Static_CArr;
	template<typename, Carry_t = Carry_t::SHALLOW> class Dynamic_CArr;
	//========//========//========//========//=======#//========//========//========//========//===


	class CArrier
	{
	public:
		CArrier() = delete;

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


		_CARR_DECLARE_FALSE_TEMPLATE(Made_Static);

		_CARR_DECLARE_TEMPLATE
		(	_CARR_TPACK(typename T, size_t N, Carry_t CT)
		,	Made_Static, _CARR_TPACK(< Static_CArr<T, N, CT> >), true
		);


		_CARR_DECLARE_FALSE_TEMPLATE(Made_Dynamic);
		
		_CARR_DECLARE_TEMPLATE
		(	_CARR_TPACK(typename T, Carry_t CT)
		,	Made_Dynamic, _CARR_TPACK(< Dynamic_CArr<T, CT> >), true
		);

		_CARR_DECLARE_TEMPLATE
		(	typename CArr
		,	Made, /* no spec */, Made_Dynamic<CArr>::value || Made_Static<CArr>::value
		);


		_CARR_DECLARE_FALSE_TEMPLATE(Made_Shallow);

		_CARR_DECLARE_TEMPLATE
		(	_CARR_TPACK(typename T)
		,	Made_Shallow, _CARR_TPACK(< Dynamic_CArr<T, Carry_t::SHALLOW> >), true
		);

		_CARR_DECLARE_TEMPLATE
		(	_CARR_TPACK(typename T, size_t N)
		,	Made_Shallow, _CARR_TPACK(< Static_CArr<T, N, Carry_t::SHALLOW> >), true
		);


		_CARR_DECLARE_FALSE_TEMPLATE(Made_Deep);

		_CARR_DECLARE_TEMPLATE
		(	_CARR_TPACK(typename T)
		,	Made_Deep, _CARR_TPACK(< Dynamic_CArr<T, Carry_t::DEEP> >), true
		);

		_CARR_DECLARE_TEMPLATE
		(	_CARR_TPACK(typename T, size_t N)
		,	Made_Deep, _CARR_TPACK(< Static_CArr<T, N, Carry_t::DEEP> >), true
		);


	#undef _CARR_DECLARE_FALSE_TEMPLATE
	#undef _CARR_DECLARE_TEMPLATE
	#undef _CARR_TPACK

	};	// End of class CArrier
	//========//========//========//========//=======#//========//========//========//========//===


	template<typename T> 
	class Proxy_CRef
	{
	public:
		Proxy_CRef() : _just_refered(true), _pval(nullptr){}
		Proxy_CRef(T const& t) : _just_refered(true), _pval(&t){}
		Proxy_CRef(T&& t) : _just_refered(false), _pval( new T(t) ){}

		Proxy_CRef(Proxy_CRef const& copy) 
		:	_just_refered(copy._just_refered)
		,	_pval( copy._just_refered ? copy._pval : new T(*copy._pval) )
		{}

		Proxy_CRef(Proxy_CRef&& temp) 
		:	_just_refered(temp._just_refered)
		,	_pval( new T(*temp._pval) )
		{}

		~Proxy_CRef()
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
			release_if_deep();

			_pval = new T(t);

			_just_refered = false;

			return *_pval;
		}

		template<typename _T>
		void deep(_T&& _t)
		{
			release_if_deep();

			_pval = new T(_t);

			_just_refered = false;
		}

		template<typename _T>
		void shallow(_T&& _t)
		{
			release_if_deep();

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

	};	// End of class Proxy_CRef
	//========//========//========//========//=======#//========//========//========//========//===


	template<typename CARR>
	class const_iterator_t
	{
	public:
		using value_t = typename CARR::value_type;

		const_iterator_t(CARR const& carr, size_t idx) : _carr(carr), _idx(idx){}

		auto operator++()-> const_iterator_t{  return const_iterator_t(_carr, ++_idx);  }
		auto operator++(int)-> const_iterator_t const&{  _idx++; return *this;  }
		bool operator!=(const_iterator_t const& citr) const{  return citr._idx != _idx;  }
		bool operator==(const_iterator_t const& citr) const{  return citr._idx == _idx;  }
		auto operator*() const-> value_t const&{  return _carr[_idx];  }

	private:
		CARR const& _carr;
		size_t _idx;
	};
	//========//========//========//========//=======#//========//========//========//========//===


	template<typename T, size_t N>
	class Static_CArr<T, N, Carry_t::SHALLOW>
	{
	private:
		using _internal_t = Proxy_CRef<T>;

		_internal_t _arr[N];

		
	public:
		using value_type = T;
		using iterator_type = const_iterator_t<Static_CArr>;

		enum : size_t { SIZE = N };

		Static_CArr() = default;

		auto operator[](size_t idx) const-> value_type const&{  return _arr[idx];  }
		auto size() const-> size_t{  return SIZE;  }

		auto begin() const-> iterator_type{  return iterator_type(*this, 0);  }
		auto end() const-> iterator_type{  return iterator_type(*this, SIZE);  }
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
				throw !static_cast<bool>("size of the object is not matched.");

			Static_CArr stt;

			struct Cycle{  size_t idx; decltype(Lval.cbegin()) itr;  };

			for
			(	Cycle c{0, Lval.cbegin()}
			;	c.itr != Lval.cend() && c.idx < SIZE
			;	stt._arr[c.idx++] = *(c.itr++)
			);
				
			return stt;
		}

		template<typename Q> 
		static auto from_iterable(Q&& temp)-> Static_CArr
		{
			if(temp.size() != SIZE)
				throw !static_cast<bool>("size of the object is not matched.");

			Static_CArr stt;

			struct Cycle{  size_t idx; decltype(temp.begin()) itr;  };

			for
			(	Cycle c{0, temp.begin()}
			;	c.itr != temp.end() && c.idx < SIZE
			;	(stt._arr[c.idx++]).deep( *(c.itr++) )
			);
				
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
				throw !static_cast<bool>("size of the object is not matched.");

			Static_CArr stt;

			for(size_t idx = 0; idx < SIZE; idx++)
				stt._arr[idx] = Lval[idx + offset];

			return stt;
		}

		template<typename Q> 
		static auto from_indexed(Q&& temp, size_t offset = 0)-> Static_CArr
		{
			if(SIZE != temp.size() - offset)
				throw !static_cast<bool>("size of the object is not matched.");

			Static_CArr stt;

			for(size_t idx = 0; idx < SIZE; idx++)
				(stt._arr[idx]).deep(temp[idx + offset]);

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
		using value_type = T;
		using iterator_type = const_iterator_t<Static_CArr>;
		enum : size_t { SIZE = N };

		Static_CArr() = default;

		auto operator[](size_t idx) const-> value_type const&{  return _arr[idx];  }
		auto size() const-> size_t{  return SIZE;  }

		auto begin() const-> iterator_type{  return iterator_type(*this, 0);  }
		auto end() const-> iterator_type{  return iterator_type(*this, SIZE);  }
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		template<typename Q>
		static auto from_iterable(Q&& Uval)-> Static_CArr
		{
			if(SIZE != Uval.size())
				throw !static_cast<bool>("size of the object is not matched.");

			Static_CArr stt;

			struct Cycle{  size_t idx; decltype(Uval.begin()) itr;  };

			for
			(	Cycle c{0, Uval.begin()}
			;	c.itr != Uval.end()
			;	stt._arr[c.idx++] = *(c.itr++)
			);
				
			return stt;
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		template<typename Q> 
		static auto from_indexed(Q&& Uval, size_t offset = 0)-> Static_CArr
		{
			if(SIZE != Uval.size() - offset)
				throw !static_cast<bool>("size of the object is not matched.");

			Static_CArr stt;

			for(size_t idx = 0; idx < SIZE; idx++)
				stt._arr[idx] = Uval[idx + offset];

			return stt;
		}
		
	};	// End of class Static_CArr<T, N, Carry_t::DEEP>
	//========//========//========//========//=======#//========//========//========//========//===


	template<typename T>
	class Dynamic_CArr<T, Carry_t::SHALLOW>
	{
	private:
		using _internal_t = Proxy_CRef<T>;

		_internal_t* _arr;
		size_t _n;

		Dynamic_CArr(_internal_t arr[], size_t n) : _arr(arr), _n(n) {}

		
	public:
		using value_type = T;
		using iterator_type = const_iterator_t<Dynamic_CArr>;

		Dynamic_CArr(Dynamic_CArr const& cpy) : _arr(nullptr), _n(cpy.size())
		{
			_arr = new _internal_t[_n];

			for(size_t idx =0; idx < _n; idx++)
				_arr[idx] = cpy._arr[idx];
		}

		Dynamic_CArr(Dynamic_CArr&& temp) : _arr(nullptr), _n(temp.size())
		{
			_arr = temp._arr;

			temp._arr = nullptr;
		}

		~Dynamic_CArr()
		{
			delete[] _arr;

			_arr = nullptr;
		}

		auto operator[](size_t idx) const-> value_type const&{ return _arr[idx]; }
		auto size() const-> size_t{ return _n; }

		auto begin() const-> iterator_type{  return iterator_type(*this, 0);  }
		auto end() const-> iterator_type{  return iterator_type(*this, _n);  }
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

			struct Cycle{  size_t idx; decltype(Lval.cbegin()) itr;  };

			for
			(	Cycle c{0, Lval.cbegin()}
			;	c.itr != Lval.cend()
			;	arr[c.idx++] = *(c.itr++)
			);
				
			return Dynamic_CArr(arr, n);
		}

		template<typename Q> 
		static auto from_iterable(Q&& temp)-> Dynamic_CArr
		{
			size_t const n = temp.size();
			_internal_t* arr = new _internal_t[n];

			struct Cycle{  size_t idx; decltype(temp.begin()) itr;  };

			for
			(	Cycle c{0, temp.begin()}
			;	c.itr != temp.end()
			;	(arr[c.idx++]).deep( *(c.itr++) )
			);
				
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

			for(size_t idx = 0; idx < n; idx++)
				arr[idx] = Lval[idx + offset];

			return Dynamic_CArr(arr, n);
		}

		template<typename Q> 
		static auto from_indexed(Q&& temp, size_t offset = 0)-> Dynamic_CArr
		{
			size_t const n = temp.size() - offset;
			_internal_t* arr = new _internal_t[n];

			for(size_t idx = 0; idx < n; idx++)
				(arr[idx]).deep(temp[idx + offset]);

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

		
	public:
		using value_type = T;
		using iterator_type = const_iterator_t<Dynamic_CArr>;

		Dynamic_CArr(Dynamic_CArr const& cpy) : _arr(nullptr), _n(cpy.size())
		{
			_arr = new _internal_t[_n];

			for(size_t idx =0; idx < _n; idx++)
				_arr[idx] = cpy._arr[idx];
		}

		Dynamic_CArr(Dynamic_CArr&& temp) : _arr(nullptr), _n(temp.size())
		{
			_arr = temp._arr;

			temp._arr = nullptr;
		}

		~Dynamic_CArr()
		{
			delete[] _arr;

			_arr = nullptr;
		}

		auto operator[](size_t idx) const-> value_type const&{ return _arr[idx]; }
		auto size() const-> size_t{ return _n; }

		auto begin() const-> iterator_type{  return iterator_type(*this, 0);  }
		auto end() const-> iterator_type{  return iterator_type(*this, _n);  }
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		template<typename Q> 
		static auto from_iterable(Q&& Uval)-> Dynamic_CArr
		{
			size_t const n = Uval.size();
			_internal_t* arr = new _internal_t[n];

			struct Cycle{  size_t idx; decltype(Uval.begin()) itr;  };

			for
			(	Cycle c{0, Uval.begin()}
			;	c.itr != Uval.end()
			;	arr[c.idx++] = *(c.itr++)
			);
				
			return Dynamic_CArr(arr, n);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		template<typename Q> 
		static auto from_indexed(Q&& Uval, size_t offset = 0)-> Dynamic_CArr
		{
			size_t const n = Uval.size() - offset;
			_internal_t* arr = new _internal_t[n];

			for(size_t idx = 0; idx < n; idx++)
				arr[idx] = Uval[idx + offset];

			return Dynamic_CArr(arr, n);
		}

	};	// End of class Dynamic_CArr<T, Carry_t::DEEP>


}

#endif
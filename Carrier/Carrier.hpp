#pragma once

#ifndef _SGM_CARRIER_
#define _SGM_CARRIER_

#include <cstdlib>
#include <cassert>
#include <new>
#include "..\interface_Traits\interface_Traits.hpp"


////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#


namespace sgm
{


	template<class T, class Host_t, bool MUTABLE, bool REVERSE> class CArr_iterator;


	template<class T>
	class _CArr_iterator_Helper : No_Making
	{
		template<class, class, bool, bool> friend class CArr_iterator;


		template
		<	class H1, class H2, bool M1, bool M2, bool R1, bool R2
		,	class itr1_t = CArr_iterator<T, H1, M1, R1>
		,	class itr2_t = CArr_iterator<T, H2, M2, R2>
		>
		static auto _Substitution
		(	CArr_iterator<T, H1, M1, R1>& itr1, CArr_iterator<T, H2, M2, R2> itr2
		)->	itr1_t&
		{
			static_assert
			(	!itr1_t::EVENTUALLY_MUTABLE || itr2_t::EVENTUALLY_MUTABLE
			,	"cannot bind immutable iterator to mutable one."
			);
			
			itr1._arr = itr2._arr, itr1._idx = itr2._idx;
		
			return itr1;
		}

	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---



	template<class T, class Host_t, bool MUTABLE, bool REVERSE>
	class CArr_iterator
	{	
		template<class _T> friend class _CArr_iterator_Helper;

		enum class dir_t : bool {F, B};

		using iter_t = CArr_iterator;

		//enum : bool{EVENTUALLY_MUTABLE = MUTABLE && _RTH<Host_t>::is_Const ? false : true};
		enum : bool{EVENTUALLY_MUTABLE = MUTABLE && is_immutable<Host_t>::value ? false : true};

	public:	
		CArr_iterator(T* arr, size_t idx) : _arr(arr), _idx(idx){}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		template<class _H, bool _M, bool _R>
		auto operator=(CArr_iterator<T, _H, _M, _R> itr)-> iter_t&
		{
			return _CArr_iterator_Helper<T>::_Substitution(*this, itr);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


	#ifndef _MUTABLE_AND_IMMUTABLE_MEMBERS
		#define _MUTABLE_AND_IMMUTABLE_MEMBERS(NAME, ARGS, RES, ...)	\
			auto NAME (ARGS)	\
			->	std::conditional_t<EVENTUALLY_MUTABLE, RES, const RES>	{__VA_ARGS__} \
			\
			auto NAME (ARGS) const-> const RES						{__VA_ARGS__}
	#else
		#error _MUTABLE_AND_IMMUTABLE_MEMBERS was already defined somewhere else.
	#endif

		//	itr++	(rvalue) 
		auto operator++(int) const-> iter_t
		{
			iter_t const itr = *this;
			
			_idx = shifted(_idx);
			
			return itr;		
		}

		//	++itr	(lvalue)
		_MUTABLE_AND_IMMUTABLE_MEMBERS(operator++, , iter_t&, return *this += 1;)

		//	itr--	(rvalue)
		auto operator--(int) const-> iter_t
		{
			iter_t const itr = *this;

			_idx = shifted(_idx, dir_t::B);

			return itr;
		}

		//	--itr	(lvalue)
		_MUTABLE_AND_IMMUTABLE_MEMBERS(operator--, , iter_t&, return *this -= 1;)
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------
		

		bool operator!=(iter_t const itr) const{  return _idx != itr._idx;  }	
		bool operator==(iter_t const itr) const{  return _idx == itr._idx;  }	
		
		bool operator<(iter_t const itr) const{  return Less(*this, itr);  }
		bool operator>(iter_t const itr) const{  return Less(itr, *this);  }

		bool operator<=(iter_t const itr) const{  return *this < itr || *this == itr;  }
		bool operator>=(iter_t const itr) const{  return *this > itr || *this == itr;  }
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		_MUTABLE_AND_IMMUTABLE_MEMBERS( operator*, , T&, return (*this)[0]; )
		
		_MUTABLE_AND_IMMUTABLE_MEMBERS
		(	operator[], size_t interval, T&
		,	return *( _arr + shifted(_idx, dir_t::F, interval) - (REVERSE ? 1 : 0) );
		)
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		auto operator+(size_t interval) const-> iter_t
		{
			return iter_t( _arr, shifted(_idx, dir_t::F, interval) );
		}

		auto operator-(size_t interval) const-> iter_t
		{
			return iter_t( _arr, shifted(_idx, dir_t::B, interval) );
		}

		_MUTABLE_AND_IMMUTABLE_MEMBERS
		(	operator+=, size_t interval, iter_t&
		,	_idx = shifted(_idx, dir_t::F, interval);

			return *this;
		)

		_MUTABLE_AND_IMMUTABLE_MEMBERS
		(	operator-=, size_t interval, iter_t&
		,	_idx = shifted(_idx, dir_t::B, interval);

			return *this;
		)


	#undef _MUTABLE_AND_IMMUTABLE_MEMBERS
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		auto operator-(iter_t itr) const-> signed long long
		{
			bool const has_greater_idx = _idx > itr._idx ? true : false;
			size_t const du = has_greater_idx ? _idx - itr._idx : itr._idx - _idx;

			assert(du <= LLONG_MAX && L"the difference exceeds maximum capacity.");

			return 
			!REVERSE && has_greater_idx || REVERSE && !has_greater_idx 
			?	(signed long long) du
			:	-(signed long long) du;
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


	private:	
		T* _arr;	
		size_t mutable _idx;	
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		template<bool _REVERSE> static size_t _shifted(size_t idx, dir_t dir, size_t interval);

		template<> static size_t _shifted<false>	(size_t idx, dir_t dir, size_t interval)
		{  
			return dir == dir_t::F ? idx + interval : idx - interval;
		}

		template<> static size_t _shifted<true>	(size_t idx, dir_t dir, size_t interval)
		{
			return dir == dir_t::B ? idx + interval : idx - interval;
		}

		static size_t shifted(size_t idx, dir_t dir = dir_t::F, size_t interval = 1)
		{
			return _shifted<REVERSE>(idx, dir, interval); 
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------
		
		template<bool _REVERSE> static bool _less(iter_t const itr1, iter_t const itr2);

		template<> static bool _less<false> (iter_t const itr1, iter_t const itr2)
		{
			return itr1._idx < itr2._idx;
		}

		template<> static bool _less<true> (iter_t const itr1, iter_t const itr2)
		{
			return itr1._idx > itr2._idx;
		}

		static bool Less(iter_t const itr1, iter_t const itr2)
		{  
			return _less<REVERSE>(itr1, itr2);
		}

	};// end of template class CArr_iterator
	//========//========//========//========//=======#//========//========//========//========//===
	
	
	template<class> class Carrier;

	template<class T>
	class _CArr_Helper : No_Making
	{
		friend class Carrier<T>;

		using value_t = std::decay_t<T>;
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		template<class ITR1, class ITR2>
		struct _tied
		{
		public:
			_tied(ITR1 itr1, ITR2 itr2) : _1st(itr1), _2nd(itr2){}

			auto operator++()-> _tied&
			{
				_1st++, _2nd++;

				return *this;
			}

			auto operator++(int)-> _tied&
			{
				_tied res(_1st, _2nd);

				++(*this);

				return res;
			}

			ITR1 _1st;
			ITR2 _2nd;
		};

		template<class ITR1, class ITR2>
		static auto make_tied(ITR1 itr1, ITR2 itr2)-> _tied<ITR1, ITR2>
		{  
			return _tied<ITR1, ITR2>(itr1, itr2);  
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		static auto _alloc(size_t capa)-> value_t*
		{
			return static_cast<value_t*>(  std::malloc( sizeof(value_t) * capa )  );
		}


		template<class...ARGS>
		static auto _fill(size_t size, value_t* const arr, ARGS const&...args)-> value_t*
		{
			for(size_t idx = 0; idx < size; idx++)
				new(arr + idx) value_t(args...);

			return arr;
		}


		static auto _remove(size_t size, value_t* const arr)-> value_t*
		{
			for(size_t idx = 0; idx < size; idx++)
				(arr + idx)->~value_t();

			return arr;
		}


		static auto _copy(size_t size, value_t* const arr)-> value_t*
		{
			value_t* rarr = _alloc(size);

			for(size_t idx = 0; idx < size; idx++)
				new(rarr + idx) value_t(arr[idx]);

			return rarr;
		}

		template<  class CON, class = std::enable_if_t< is_iterable<CON>::value >  >
		static auto _copy(size_t size, CON&& con)-> value_t*
		{
			value_t* arr = _alloc(size);

			using elem_t
			=	std::conditional_t
				<	std::is_lvalue_reference<decltype(con)>::value, value_t const&, value_t&& 
				>;

			for( auto tied = make_tied<size_t>(0, con.begin()); tied._1st < size; ++tied )
				new(arr + tied._1st) value_t( (elem_t)*tied._2nd );

			return arr;
		}

	};// end of template class _CArr_Helper
	//========//========//========//========//=======#//========//========//========//========//===


	template<class T>
	class Carrier
	{
	public:
		using value_t = std::decay_t<T>;
		using method_t = _CArr_Helper<T>;
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		Carrier() : _capa(0), _size(0), _arr(nullptr){}

		explicit Carrier(size_t capa) : _capa(capa), _size(0), _arr( method_t::_alloc(capa) ){}
		explicit Carrier(int capa) : Carrier( size_t(capa) ){}
		explicit Carrier(unsigned capa) : Carrier( size_t(capa) ){}

		template<class...ARGS>
		Carrier(size_t size, ARGS const&...args) 
		:	_capa(size), _size(size)
		,	_arr(  method_t::_fill( size, method_t::_alloc(size), args... )  )
		{}


		template
		<	class ITR
		,	class 
			=	std::enable_if_t
				<	!std::is_integral<ITR>::value && is_iterator<ITR>::value 
				>
		>
		Carrier(ITR bi, ITR ei) : Carrier(  static_cast<size_t>( std::distance(bi, ei) )  )
		{
			for(auto itr = bi; itr != ei; ++itr)
				*this >> *itr;
		}

		Carrier(Carrier const& ca)
		:	_capa(ca._capa), _size(ca._size)
		,	_arr
			(	ca._size > 0
				?	method_t::_copy(ca._size, ca._arr)
				:
				ca._capa > 0
				?	method_t::_alloc(ca._capa)
				:	nullptr
			)
		{}

		Carrier(Carrier&& ca) : _capa(ca._capa), _size(ca._size), _arr(ca._arr)
		{
			ca._size = ca._capa = 0, ca._arr = nullptr;
		}

	#ifdef _INITIALIZER_LIST_
		template<class Q>
		Carrier(std::initializer_list<Q>&& con)
		:	_capa(con.size()), _size(_capa), _arr(  method_t::_copy( _capa, std::move(con) )  )
		{}
	#endif

		template
		<	class CON
		,	class 
			=	std::enable_if_t
				<	is_iterable<CON>::value
				&&	!std::is_same< std::decay_t<CON>, Carrier >::value  
				> 
		>
		Carrier(CON&& con)
		:	_capa(con.size()), _size(_capa)
		,	_arr(  method_t::_copy( _capa, std::forward<CON>(con) )  )
		{}


		~Carrier()
		{
			if(_arr != nullptr)
				_arr = method_t::_remove(_size, _arr),
				std::free(_arr), _arr = nullptr;
					
			_size = _capa = 0;
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		auto operator=(Carrier const& ca)-> Carrier&{  return _equal(ca);  }
		 
		auto operator=(Carrier&& ca)-> Carrier&
		{
			this->~Carrier();

			_capa = ca._capa, _size = ca._size, _arr = ca._arr,
			ca._size = ca._capa = 0, 
			ca._arr = nullptr;

			return *this;
		}

		template
		<	class CON
		,	class 
			=	std::enable_if_t
				<	is_iterable<CON>::value
				&&	!std::is_same< std::decay_t<CON>, Carrier >::value  
				>  
		>
		auto operator=(CON&& con)-> Carrier&{  return _equal( std::forward<CON>(con) );  }
		 

		auto operator[](size_t idx) const-> value_t const&{  return _arr[idx];  }
		auto operator[](size_t idx)-> value_t&{  return _arr[idx];  }


		auto operator>>(value_t const& val)-> Carrier&{  return emplace_back(val);  }
		auto operator>>(value_t&& val)-> Carrier&		{  return emplace_back( std::move(val) );  }


		template
		<	class CON
		,	class 
			=	std::enable_if_t
				<	is_iterable<CON>::value
				&&	!std::is_same< std::decay_t<CON>, Carrier >::value  
				> 
		> 
		operator CON() const{  return std::decay_t<CON>(begin(), end());  }
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		auto capacity() const-> size_t	{  return _capa;  }
		auto size() const-> size_t		{  return _size;  }

		bool no_capacity() const	{  return _capa == 0;  }
		bool no_element() const	{  return _size == 0;  }


		template<class...ARGS>
		auto emplace_back(ARGS&&...args)-> Carrier&
		{
			assert(_size < _capa && L"can't emplace_back : out of index");

			new(_arr + _size) value_t( std::forward<ARGS>(args)... ), _size++;

			return *this;
		}


		auto pop_back(size_t n = 1)-> Carrier&
		{
			assert(_size >= n && L"can't pop_back : not enough elements to pop");

			while(n-->0)
				_size--, (_arr + _size)->~value_t();

			return *this;
		}


		template<bool M>
		auto pop_back_from(CArr_iterator<value_t, Carrier, M, false> ei)-> Carrier&
		{
			return 
			pop_back
			(	static_cast<size_t>
				(	CArr_iterator<value_t, Carrier, M, false>(_arr, _size) - ei
				)
			);
		}

		template<bool M>
		auto pop_back_from(CArr_iterator<value_t, Carrier, M, true> ei)-> Carrier&
		{
			return 
			pop_back
			(	static_cast<size_t>
				(	CArr_iterator<value_t, Carrier, M, true>(_arr, 0) - ei
				)
			);
		}


		auto clear()-> Carrier&
		{
			_arr = method_t::_remove(_size, _arr), _size = 0;

			return *this;
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		template<class H> using iterator_t = CArr_iterator<value_t, H, true, false>;
		template<class H> using citerator_t = CArr_iterator<value_t, H, false, false>;
		template<class H> using riterator_t = CArr_iterator<value_t, H, true, true>;
		template<class H> using criterator_t = CArr_iterator<value_t, H, false, true>;

	#ifndef _DECL_CARR_ITERATOR
		#define _DECL_CARR_ITERATOR(HEAD, FUNC, C, IDX)	\
			auto HEAD##FUNC() C -> HEAD##iterator_t<Carrier C>		\
			{	\
				return HEAD##iterator_t<Carrier C>(_arr, IDX);		\
			}
	#else
		#error _DECL_CARR_ITERATOR was already defined somewhere else.
	#endif

		_DECL_CARR_ITERATOR( , begin, , 0)
		_DECL_CARR_ITERATOR( , begin, const, 0)
		_DECL_CARR_ITERATOR( c, begin, , 0)
		_DECL_CARR_ITERATOR( c, begin, const, 0)

		_DECL_CARR_ITERATOR( , end, , _size)
		_DECL_CARR_ITERATOR( , end, const, _size)
		_DECL_CARR_ITERATOR( c, end, , _size)
		_DECL_CARR_ITERATOR( c, end, const, _size)

		_DECL_CARR_ITERATOR( r, begin, , _size)
		_DECL_CARR_ITERATOR( r, begin, const, _size)
		_DECL_CARR_ITERATOR( cr, begin, , _size)
		_DECL_CARR_ITERATOR( cr, begin, const, _size)

		_DECL_CARR_ITERATOR( r, end, , 0)
		_DECL_CARR_ITERATOR( r, end, const, 0)
		_DECL_CARR_ITERATOR( cr, end, , 0)
		_DECL_CARR_ITERATOR( cr, end, const, 0)

	#undef _DECL_CARR_ITERATOR
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		auto cdata() const-> value_t const*	{  return _arr;  }
		
		auto data() const-> value_t const*	{  return cdata();  }
		auto data()-> value_t*				{  return _arr;  }



	private:
		size_t _capa, _size;
		value_t* _arr;


		template<class CON>
		auto _equal(CON&& con)-> Carrier&
		{
			static_assert(is_iterable<CON>::value, "CON is not iterable type.");

			{
				size_t const con_size = con.size();

				if(_capa < con_size)
					this->~Carrier(),
					_arr = method_t::_alloc(con_size),
					_size = 0, _capa = con_size;
			}
			{
				using elem_t
				=	std::conditional_t
					<	std::is_lvalue_reference<decltype(con)>::value
					,	value_t const&, value_t&& 
					>;

				auto tied = method_t::template make_tied<size_t>(0, con.begin());

				for(; tied._1st < _size && tied._2nd != con.end(); ++tied)
					_arr[tied._1st] = elem_t(*tied._2nd);

				for(; tied._1st < _capa && tied._2nd != con.end(); ++tied)
					emplace_back( elem_t(*tied._2nd) );
			}
			return *this;
		}

	};	// end of class Carrier<T>
	//========//========//========//========//=======#//========//========//========//========//===


}	// end of namespace sgm

#endif // end of #ifndef _SGM_CARRIER_

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

#ifdef _ITERATOR_
namespace std
{

	template<class T, class Host_t, bool MUTABLE, bool REVERSE>
	struct iterator_traits< sgm::CArr_iterator<T, Host_t, MUTABLE, REVERSE> >
	{
		using iterator_category = std::random_access_iterator_tag;
		using value_type = T;
		using difference_type = signed long long;
		
		using pointer 
		=	std::conditional_t< MUTABLE && !sgm::is_immutable<Host_t>::value, T*, T const* >;
		
		using reference 
		=	std::conditional_t< MUTABLE && !sgm::is_immutable<Host_t>::value, T&, T const& >;
	};

}
#endif
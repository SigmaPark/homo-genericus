#pragma once

#ifndef _SGM_CARRIER_
#define _SGM_CARRIER_

#if defined(_MSC_VER) && _MSC_VER < 1800
	#error C++11 or higher version of language support is required.
#endif

#include <cstdlib>
#include "..\Type_Analysis\Type_Analysis.hpp"

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#


namespace sgm
{


#ifdef _CSTDDEF_
	using std::size_t;
#endif


	template<class T, bool REVERSE> class CArr_iterator;


	template<class T, bool REVERSE>
	class CArr_iterator
	{	
	private:
		enum class dir_t : bool { F, B };

		using iter_t = CArr_iterator;



	public:	
		CArr_iterator(T* arr, size_t idx) : _arr(arr), _idx(idx){}	

	#ifndef _MUTABLE_AND_IMMUTABLE_MEMBERS
		#define _MUTABLE_AND_IMMUTABLE_MEMBERS(NAME, ARGS, RES, ...)	\
			auto NAME (ARGS)->				RES{__VA_ARGS__}	\
			auto NAME (ARGS) const-> const	RES{__VA_ARGS__}
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
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

	};// end of template class CArr_iterator
	//========//========//========//========//=======#//========//========//========//========//===
	
	
	template<class> class Carrier;

	template<class T>
	class _CArr_Helper
	{
	public:
		_CArr_Helper() = delete;

	private:
		friend class Carrier<T>;
		using value_t = _Original_t<T>;
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		template<class ITR>
		class _tied
		{
		public:
			_tied(size_t idx, ITR itr) : idx(idx), itr(itr){}

			auto operator++(int)-> _tied
			{
				idx++, itr++;

				return *this;
			}
		
			size_t idx;
			ITR itr;
		};

		template<class ITR>
		static auto make_tied(size_t idx, ITR itr)-> _tied<ITR>
		{  
			return _tied<ITR>(idx, itr);  
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

		template<class CON>
		static auto _copy(size_t size, CON&& con)-> value_t*
		{
			value_t* arr = _alloc(size);

			using elem_t 
			=	_Selective_t< _RTH<decltype(con)>::is_LRef, value_t const&, value_t&& >;

			for(auto tied = make_tied(0, con.begin()); tied.idx < size; tied++)
				new(arr + tied.idx) value_t( (elem_t)*tied.itr );

			return arr;
		}

	};// end of template class _CArr_Helper
	//========//========//========//========//=======#//========//========//========//========//===


	template<class T>
	class Carrier
	{
	public:
		using value_t = _Original_t<T>;
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

		Carrier(Carrier& ca) : Carrier( static_cast<Carrier const&>(ca) ){}

		Carrier(Carrier&& ca) : _capa(ca._capa), _size(ca._size), _arr(ca._arr)
		{
			ca._size = ca._capa = 0, ca._arr = nullptr;
		}

	#ifdef _INITIALIZER_LIST_
		template<class Q>
		Carrier(std::initializer_list<Q>&& con)
		:	_capa(con.size()), _size(_capa), _arr(  method_t::_copy( _capa, _Move(con) )  )
		{}
	#endif

		template<class CON>
		Carrier(CON&& con)
		:	_capa(con.size()), _size(_capa)
		,	_arr(  method_t::_copy( _capa, _Forward<CON>(con) )  )
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
		auto operator=(Carrier& ca)-> Carrier&{  return _equal(ca);  }

		auto operator=(Carrier&& ca)-> Carrier&
		{
			this->~Carrier();

			_capa = ca._capa, _size = ca._size, _arr = ca._arr,
			ca._size = ca._capa = 0, ca._arr = nullptr;

			return *this;
		}

		template<class CON>
		auto operator=(CON&& con)-> Carrier&{  return _equal( _Forward<CON>(con) );  }


		auto operator[](size_t idx) const-> value_t const&{  return _arr[idx];  }
		auto operator[](size_t idx)-> value_t&{  return _arr[idx];  }


		auto operator>>(value_t const& val)-> Carrier&{  return emplace_back(val);  }
		auto operator>>(value_t&& val)-> Carrier&		{  return emplace_back( _Move(val) );  }
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		auto capacity() const-> size_t	{  return _capa;  }
		auto size() const-> size_t		{  return _size;  }

		bool no_capacity() const	{  return _capa == 0;  }
		bool no_element() const	{  return _size ==0;  }


		template<class...ARGS>
		auto emplace_back(ARGS&&...args)-> Carrier&
		{
		#ifdef _DEBUG
			if(_size >= _capa)
				throw !(bool) L"can't emplace_back : out of index";
		#endif
			new(_arr +_size) value_t( _Forward<ARGS>(args)... ), _size++;

			return *this;
		}


		auto pop_back()-> Carrier&
		{
		#ifdef _DEBUG
			if(no_element())
				throw !(bool) L"can't pop_back : no element to pop";
		#endif
			_size--, (_arr + _size)->~value_t();

			return *this;
		}


		auto clear()-> Carrier&
		{
			_arr = method_t::_remove(_size, _arr), _size = 0;

			return *this;
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

		using iterator_t = CArr_iterator<value_t, false>;

		auto begin()-> iterator_t				{  return iterator_t(_arr, 0);  }
		auto begin() const-> iterator_t const	{  return iterator_t(_arr, 0);  }
		auto cbegin()-> iterator_t const			{  return iterator_t(_arr, 0);  }
		auto cbegin() const-> iterator_t const	{  return iterator_t(_arr, 0);  }

		auto end()-> iterator_t				{  return iterator_t(_arr, _size);  }
		auto end() const-> iterator_t const		{  return iterator_t(_arr, _size);  }
		auto cend()-> iterator_t const			{  return iterator_t(_arr, _size);  }
		auto cend() const-> iterator_t const		{  return iterator_t(_arr, _size);  }


		using riterator_t = CArr_iterator<value_t, true>;

		auto rbegin()-> riterator_t				{  return riterator_t(_arr, _size);  }
		auto rbegin() const-> riterator_t const	{  return riterator_t(_arr, _size);  }
		auto crbegin()-> riterator_t const		{  return riterator_t(_arr, _size);  }
		auto crbegin() const-> riterator_t const	{  return riterator_t(_arr, _size);  }

		auto rend()-> riterator_t				{  return riterator_t(_arr, 0);  }
		auto rend() const-> riterator_t const	{  return riterator_t(_arr, 0);  }
		auto crend()-> riterator_t const			{  return riterator_t(_arr, 0);  }
		auto crend() const-> riterator_t const	{  return riterator_t(_arr, 0);  }
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

	private:
		size_t _capa, _size;
		value_t* _arr;


		template<class CON>
		auto _equal(CON&& con)-> Carrier&
		{
			{
				size_t const con_size = con.size();

				if(_capa < con_size)
					this->~Carrier(),
					_arr = method_t::_alloc(con_size),
					_size = 0, _capa = con_size;
			}
			{
				using elem_t
				=	_Selective_t< _RTH<decltype(con)>::is_LRef, value_t const&, value_t&& >;

				auto tied = method_t::make_tied(0, con.begin());

				for(; tied.idx < _size && tied.itr != con.end(); tied++)
					_arr[tied.idx] = elem_t(*tied.itr);

				for(; tied.idx < _capa && tied.itr != con.end(); tied++)
					emplace_back( elem_t(*tied.itr) );
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
	template<class T, bool FRONT>
	struct iterator_traits< sgm::CArr_iterator<T, FRONT> >
	{
		using iterator_category = std::random_access_iterator_tag;
		using value_type = T;
		using difference_type = void;
		using pointer = T*;
		using reference = T&;
	};
}
#endif
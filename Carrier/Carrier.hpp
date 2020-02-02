#pragma once

#ifndef _SGM_CARRIER_
#define _SGM_CARRIER_

namespace sgm
{


	template<class T>
	class _Original_Helper
	{
	private:
		template<class Q> struct _Original			{  using type = Q;  };
		template<class Q> struct _Original<Q const>	{  using type = Q;  };
		template<class Q> struct _Original<Q&>		{  using type = Q;  };
		template<class Q> struct _Original<Q const&>	{  using type = Q;  };
		template<class Q> struct _Original<Q&&>		{  using type = Q;  };

	public:
		_Original_Helper() = delete;

		using type = typename _Original<T>::type;
	};

	//	Similar to std::decay_t
	template<class T>
	using _Original_t = typename _Original_Helper<T>::type;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#
	
	
	template<bool B, class true_t, class false_t> class _Selective;

	template<class true_t, class false_t>
	class _Selective<true, true_t, false_t>
	{
	public:
		_Selective() = delete;

		using type = true_t;
	};

	template<class true_t, class false_t>
	class _Selective<false, true_t, false_t>
	{
	public:
		_Selective() = delete;

		using type = false_t;
	};

	//	Similar to std::conditional_t
	template<bool B, class true_t, class false_t>
	using _Selective_t = typename _Selective<B, true_t, false_t>::type;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


	template<class T>
	class _Ref_Traits_Helper
	{
	private:
		template<class Q> struct _CVL			{  enum : bool{C = false, V = true, L = true};  };
		template<class Q> struct _CVL<Q const>	{  enum : bool{C = true, V = true, L = true};	};
		template<class Q> struct _CVL<Q&>		{  enum : bool{C = false, V = false, L = true};	};
		template<class Q> struct _CVL<Q const&>	{  enum : bool{C = true, V = false, L = true};	};
		template<class Q> struct _CVL<Q&&>		{  enum : bool{C = false, V = false, L = false};	};


	public:
		_Ref_Traits_Helper() = delete;

		enum : bool{is_Const = _CVL<T>::C, is_Val = _CVL<T>::V, is_LRef = _CVL<T>::L};
	};

	template<class T>
	using _RTH = _Ref_Traits_Helper<T>;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


	template<class T, class Q>
	static auto _Forward(Q&& q)
	->	_Selective_t	
		<	_RTH<decltype(q)>::is_Val	
		,	_Selective_t< _RTH<decltype(q)>::is_Const, T const, T >	
		,	_Selective_t	
			<	_RTH<decltype(q)>::is_LRef	
			,	_Selective_t< _RTH<decltype(q)>::is_Const, T const&, T& >	
			,	T&&	
			>	
		>
	{
		return
		static_cast
		<	_Selective_t	
			<	_RTH<decltype(q)>::is_Val	
			,	_Selective_t< _RTH<decltype(q)>::is_Const, T const, T >	
			,	_Selective_t	
				<	_RTH<decltype(q)>::is_LRef	
				,	_Selective_t< _RTH<decltype(q)>::is_Const, T const&, T& >	
				,	T&&	
				>	
			>
		>
		(q);
	}


	template<class T>
	static auto _Move(T&& t)-> _Original_t<decltype(t)>&&
	{
		return static_cast< _Original_t<decltype(t)>&& >(t);
	}
	//========//========//========//========//=======#//========//========//========//========//=======#


	template<class T>
	class CArr_iterator
	{
	public:
		CArr_iterator(T* arr, size_t idx) : _arr(arr), _idx(idx){}

		auto operator++()-> CArr_iterator{  return CArr_iterator(_arr, ++_idx);  }
		auto operator++() const-> CArr_iterator const{  return CArr_iterator(_arr, ++_idx);  }

		auto operator++(int)-> CArr_iterator{  _idx++; return *this;  }
		auto operator++(int) const-> CArr_iterator const{  _idx++; return *this;  }

		bool operator!=(CArr_iterator const itr) const{  return _idx != itr._idx;  }
		bool operator==(CArr_iterator const itr) const{  return _idx == itr._idx;  }

		auto operator*()-> T&{  return _arr[_idx];  }
		auto operator*() const-> T const&{  return _arr[_idx];  }
		

	private:
		T* _arr;
		size_t _idx;
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


	template<class T>
	class Carrier
	{
	public:
		using value_t = T;
		using index_t = size_t;

		Carrier() : _arr(nullptr), _n(0){}
		Carrier(Carrier const& carr) : _arr( allocation(carr) ), _n(carr._n){}
		Carrier(Carrier&& carr) : _arr(carr._arr), _n(carr._n){  carr._arr = nullptr;  }

		template<class CON>
		Carrier(CON&& a) : _arr(  allocation( _Forward<CON>(a) )  ), _n(a.size()){}

		~Carrier(){  delete[] _arr, _arr = nullptr;  }


		auto operator=(Carrier const& carr)-> Carrier const&
		{
			op_equal(carr);

			return *this;
		}

		auto operator=(Carrier&& carr)-> Carrier const&
		{
			delete[] _arr, _arr = carr._arr, carr._arr = nullptr;

			_n = carr._n;

			return *this;
		}

		template<class CON>
		auto operator=(CON&& con)-> Carrier const&
		{
			op_equal( _Forward<CON>(con) );

			return *this;
		}


		auto operator[](index_t idx) const-> T const&
		{  
			index_check(idx); 
			
			return _arr[idx];  
		}

		auto operator[](index_t idx)-> T&
		{
			index_check(idx); 
			
			return _arr[idx];
		}


		bool is_empty() const{  return _arr == nullptr;  }
		auto size() const-> size_t{  return _n;  }
		void clear(){  delete[] _arr, _arr = nullptr, _n = 0;  }
		
		
		using iterator_t = CArr_iterator<value_t>;

		auto begin()-> iterator_t{  return iterator_t(_arr, 0);  }
		auto end()-> iterator_t{  return iterator_t(_arr, _n);  }
		auto begin() const-> iterator_t const{  return iterator_t(_arr, 0);  }
		auto end() const-> iterator_t const{  return iterator_t(_arr, _n);  }

		auto cbegin()-> iterator_t const{  return iterator_t(_arr, 0);  }
		auto cend()-> iterator_t const{  return iterator_t(_arr, _n);  }
		auto cbegin() const-> iterator_t const{  return iterator_t(_arr, 0);  }
		auto cend() const-> iterator_t const{  return iterator_t(_arr, _n);  }


	private:
		T* _arr;
		size_t _n;


		template<class Q>
		static auto allocation(Q&& q)-> T*
		{
			T* arr = new T[q.size()];

			auto itr = q.begin();
			index_t idx = 0;

			for(; itr != q.end(); itr++, idx++)
				arr[idx] = _Selective_t< _RTH<decltype(q)>::is_LRef, T const&, T&& >(*itr);

			return arr;
		}


		template<class CON>
		void op_equal(CON&& con)
		{
			size_t const con_size = con.size();

			if(con_size == 0)
				return;
			else if(_n != con_size)
				_n = con_size,
				delete[] _arr,
				_arr = new T[_n];

			auto itr = con.begin();
			index_t idx = 0;

			for(; idx < _n; idx++, itr++)
				_arr[idx] = _Selective_t< _RTH<decltype(con)>::is_LRef, T const&, T&& >(*itr);
		}


		void index_check(index_t idx) const
		{
			if(idx >= _n)
				throw !(bool) L"index is out of valid range.";
		}

	};	// end of class Carrier<T>
	//========//========//========//========//=======#//========//========//========//========//=======#


}	// end of namespace sgm

#endif	// end of #ifndef _SGM_CARRIER_
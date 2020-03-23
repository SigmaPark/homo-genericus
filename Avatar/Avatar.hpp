#pragma once

#ifndef _SGM_AVATAR_
#define _SGM_AVATAR_

#include <cassert>
#include "..\Type_Analysis\Type_Analysis.hpp"
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{


	template<class> struct is_Avatar; 


	template
	<	class T, class M
	,	bool IS_NESTED_AVATAR = is_Avatar<T>::value
	,	bool IS_IMMUTABLE 
		=	is_immutable<T>::value 
			||	(	IS_NESTED_AVATAR 
				&&	std::is_same< typename is_Avatar<T>::M_type, invar >::value
				)
	,	bool IS_REFERENCE = std::is_reference<T>::value
	>
	struct _Avatar_T_Helper;


	template< class T, class M, class = _Avatar_T_Helper<T, M> > class Avatar_t;
	template<class T> using Avatar = Avatar_t<T, Var>;
	template<class T> using constAvatar = Avatar_t<T, invar>;

	

	template<class T, class M, bool IS_NESTED_AVATAR, bool IS_IMMUTABLE, bool IS_REFERENCE>
	struct _Avatar_T_Helper : No_Making
	{
		using Parent_t
		=	std::conditional_t
			<	IS_NESTED_AVATAR
			,	std::conditional_t
				<	IS_IMMUTABLE
				,	Avatar_t< typename is_Avatar<T>::Q_type, invar >
				,	Avatar_t< typename is_Avatar<T>::Q_type, typename is_Avatar<T>::M_type >
				>
			,	std::conditional_t
				<	IS_IMMUTABLE
				,	Avatar_t< std::decay_t<T>, invar >
				,	std::conditional_t
					<	IS_REFERENCE
					,	Avatar_t< std::decay_t<T>, M >
					,	std::false_type
					>
				>	
			>;
	};


	template< class T, class M, bool...BS>
	class Avatar_t< T, M, _Avatar_T_Helper<T, M, BS...> > 
	:	public _Avatar_T_Helper<T, M, BS...>::Parent_t
	{
	public: 
		template<class Q> 
		Avatar_t(Q&& q) : _Avatar_T_Helper<T, M, BS...>::Parent_t( std::forward<Q>(q) ){}
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---

	template<class T>
	class Avatar_t< T, invar, _Avatar_T_Helper<T, invar, false, false, false> >
	{
	protected:
		T const* _cpval;
		enum class State { YET, OWNING, GONE } _state;


	public:
		using value_t = T;

		bool is_yet() const	{  return _state == State::YET;  }
		bool is_owning() const	{  return _state == State::OWNING;  }
		bool has_gone() const	{  return _state == State::GONE;  }

		Avatar_t() : _cpval(nullptr), _state(State::YET){}
		Avatar_t(T const& t) : _cpval(&t), _state(State::OWNING){}
		Avatar_t(T&&) = delete;

		~Avatar_t(){  _cpval = nullptr, _state = State::GONE;  }


		auto operator=(Avatar_t)-> Avatar_t = delete;
		auto operator=(T)-> Avatar_t = delete;


		auto operator()(Avatar_t avt)-> Avatar_t
		{
			_cpval = avt._cpval, _state = avt._state;  

			return *this;
		}

		auto operator()(T const& t)-> Avatar_t{  _cpval = &t;  return *this;  }
		void operator()(T&&) = delete;


		auto value() const-> T const&	{  return *this;  }
		operator T const&() const		{  return value();  }


		template<class Q>
		bool operator==(Q&& q) const{  return *_cpval == std::forward<Q>(q);  }

		template<class Q>
		bool operator!=(Q&& q) const{  return !( *this == std::forward<Q>(q) );  }

		bool operator==(Avatar_t avt) const
		{  
			assert(!has_gone() && !avt.has_gone() && L"Avatar_t was released already");

			return *this == *avt._cpval;  
		}

		bool operator!=(Avatar_t avt) const{  return !(*this == avt);  }
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---

	template<class T>
	class Avatar_t< T, Var, _Avatar_T_Helper<T, Var, false, false, false> >
	:	public Avatar_t< T, invar, _Avatar_T_Helper<T, invar, false, false, false> >
	{
	private:
		T* _pval;
		

		using const_Avatar_t = Avatar_t<T, invar>;

		static auto _avt_state(Avatar_t avt)-> typename const_Avatar_t::State
		{
			return 
			avt.is_owning() 
			?	const_Avatar_t::State::OWNING
			:	
			avt.is_yet()
			?	const_Avatar_t::State::YET
			:	const_Avatar_t::State::GONE;
		}


	public:	
		Avatar_t() : const_Avatar_t(), _pval(nullptr){}
		Avatar_t(T const& t) : const_Avatar_t(t), _pval(nullptr){}
		Avatar_t(T& t) : const_Avatar_t(t), _pval(&t){}
		Avatar_t(T&&) = delete;

		~Avatar_t(){  _pval = nullptr;  }

		template
		<	class Q
		,	class 
			=	std::enable_if_t
				<	std::is_convertible<Q, T>::value 
				&&	!std::is_same< std::decay_t<Q>, Avatar_t >::value
				>
		>
		auto operator=(Q&& q)-> Avatar_t
		{  
			assert(is_owning() && L"Avatar_t has nothing");

			*_pval = std::forward<Q>(q);

			return *this; 
		}

		auto operator=(const_Avatar_t cavt)-> Avatar_t{  return *this = cavt.value();  }

		auto operator()(Avatar_t avt)-> Avatar_t
		{
			_cpval = _pval = avt._pval,
			_state = _avt_state(avt);
			
			return *this;
		}

		auto operator()(T& t)-> Avatar_t
		{
			_cpval = _pval = &t, 
			_state = State::OWNING;

			return *this;
		}


		auto value()-> T&{  return *_pval;  }
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---
	

	template<class T>
	struct is_Avatar
	{
	private:
		template<class Q> struct Eval : public std::false_type
		{
			using Q_type = Q;
			using M_type = void;
		};

		template<class Q, class M, class...TYPES>
		struct Eval< Avatar_t<Q, M, TYPES...> > : public std::true_type
		{
			using Q_type = Q;
			using M_type = M;
		};

	public: 
		enum : bool{  value = Eval< std::decay_t<T> >::value  };

		using Q_type = typename Eval< std::decay_t<T> >::Q_type;
		using M_type = typename Eval< std::decay_t<T> >::M_type;
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<class T> 
	static auto make_Avatar(T&& t)-> Avatar< std::remove_reference_t<T> >
	{
		static_assert
		(	std::is_lvalue_reference<decltype(t)>::value && !is_Avatar<T>::value
		,	"no rvalue and no nested Avatar_t"
		);

		return Avatar< std::remove_reference_t<T> >(t);
	}




} // end of namespace sgm

#endif // end of #ifndef _SGM_AVATAR_


#if 0

template<class T, ...ARGS> 
class Avatar : public DirectProxy<Avatar>
{
//...
	using Self_t = typename DirectProxy<Avatar>::template Proxy<T, ARGS...>;
	using value_t = typename DirectProxy<Avatar>::template Raw<T, ARGS...>;
};

//...

Avatar<  Avatar< Avatar<int> >  > nested1;

static_assert( std::is_same< decltype(nested1), Avatar<int> >::value );

Avatar< Avatar< Avatar<int>&& > const& > nested2;

static_assert( std::is_same< decltype(nested2), Avatar<int> const
>::value );


#endif
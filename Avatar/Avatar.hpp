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
	,	bool IS_REFERENCE = std::is_reference<T>::value
	,	bool IS_IMMUTABLE = is_immutable<T>::value
	,	bool IS_NESTED_AVATAR = is_Avatar< std::decay_t<T> >::value
	>
	struct _Avatar_T_Helper;

	template< class T, class M = invar, class = _Avatar_T_Helper<T, M> > class Avatar;
	
	

	template<class T, class M, bool IS_REFERENCE, bool IS_IMMUTABLE, bool IS_NESTED_AVATAR>
	struct _Avatar_T_Helper : No_Making
	{
		static_assert
		(	std::is_same<M, invar>::value || !IS_IMMUTABLE
		,	"sgm::Var cannot be used with immutable type."
		);

		using Parent_t
		=	std::conditional_t
			<	IS_REFERENCE
			,	Avatar< std::remove_reference_t<T>, M >
			,	std::conditional_t
				<	IS_IMMUTABLE
				,	Avatar< std::decay_t<T>, invar >
				,	std::conditional_t
					<	IS_NESTED_AVATAR
					,	Avatar< typename is_Avatar<T>::value_t, M >
					,	std::false_type
					>
				>
			>;
	};


	template< class T, class M, bool...BS>
	class Avatar< T, M, _Avatar_T_Helper<T, M, BS...> > 
	:	public _Avatar_T_Helper<T, M, BS...>::Parent_t
	{
	public: 
		template<class Q> 
		Avatar(Q&& q) : _Avatar_T_Helper<T, M, BS...>::Parent_t( std::forward<Q>(q) ){}
	};


	template<class T>
	class Avatar< T, invar, _Avatar_T_Helper<T, invar, false, false, false> >
	{
	protected:
		T const* _cpval;
		enum class State { YET, OWNING, GONE } _state;


	public:
		using value_t = T;

		bool is_yet() const	{  return _state == State::YET;  }
		bool is_owning() const	{  return _state == State::OWNING;  }
		bool has_gone() const	{  return _state == State::GONE;  }

		Avatar() : _cpval(nullptr), _state(State::YET){}
		Avatar(T const& t) : _cpval(&t), _state(State::OWNING){}
		Avatar(T&&) = delete;

		~Avatar(){  _cpval = nullptr, _state = State::GONE;  }

		auto operator=(Avatar)-> Avatar = delete;
		auto operator=(T)-> Avatar = delete;


		auto operator()(Avatar avt)-> Avatar
		{
			_cpval = avt._cpval, _state = avt._state;  

			return *this;
		}

		auto operator()(T const& t)-> Avatar{  _cpval = &t;  return *this;  }
		void operator()(T&&) = delete;


		auto value() const-> T const&	{  return *this;  }
		operator T const&() const		{  return value();  }

		template<class Q>
		bool operator==(Q&& q) const{  return *_cpval == std::forward<Q>(q);  }

		template<class Q>
		bool operator!=(Q&& q) const{  return !( *this == std::forward<Q>(q) );  }

		bool operator==(Avatar avt) const
		{  
			assert(!has_gone() && !avt.has_gone() && L"Avatar was released already");

			return *this == *avt._cpval;  
		}

		bool operator!=(Avatar avt) const{  return !(*this == avt);  }
	};


	template<class T>
	class Avatar< T, Var, _Avatar_T_Helper<T, Var, false, false, false> >
	:	public Avatar< T, invar, _Avatar_T_Helper<T, invar, false, false, false> >
	{
	private:
		T* _pval;
		

		using const_Avatar_t = Avatar<T, invar>;

		static auto _avt_state(Avatar avt)-> typename const_Avatar_t::State
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
		Avatar() : const_Avatar_t(), _pval(nullptr){}
		Avatar(T const& t) : const_Avatar_t(t), _pval(nullptr){}
		Avatar(T& t) : const_Avatar_t(t), _pval(&t){}
		Avatar(T&&) = delete;

		~Avatar(){  _pval = nullptr;  }

		template
		<	class Q
		,	class 
			=	std::enable_if_t
				<	std::is_convertible<Q, T>::value 
				&&	!std::is_same< std::decay_t<Q>, Avatar >::value
				>
		>
		auto operator=(Q&& q)-> Avatar
		{  
			assert(is_owning() && L"Avatar has nothing");

			*_pval = std::forward<Q>(q);

			return *this; 
		}

		auto operator=(const_Avatar_t cavt)-> Avatar{  return *this = cavt.value();  }

		auto operator()(Avatar avt)-> Avatar
		{
			_cpval = _pval = avt._pval,
			_state = _avt_state(avt);
			
			return *this;
		}

		auto operator()(T& t)-> Avatar
		{
			_cpval = _pval = &t, 
			_state = State::OWNING;

			return *this;
		}


		auto value()-> T&{  return *_pval;  }


	};

	
	template<class T> struct is_Avatar : public std::false_type
	{
		using value_t = T;

		using type = T;
		using Mutability_t = void;
	};
	
	template<class T, class M, class...TYPES> 
	struct is_Avatar< Avatar<T, M, TYPES...> > : public std::true_type
	{ 
		using value_t = std::conditional_t< std::is_same<M, Var>::value, T, T const >;
		
		using type = Avatar<T, M, TYPES...>;
		using Mutability_t = M;
	};



	template<class M = invar, class T> 
	static auto make_Avatar(T& t)-> Avatar<T, M>{  return Avatar<T, M>(t);  }




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
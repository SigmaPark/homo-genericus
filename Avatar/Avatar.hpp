#pragma once

#ifndef _SGM_AVATAR_
#define _SGM_AVATAR_

#include <cassert>
#include "..\Type_Analysis\Type_Analysis.hpp"
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{


	SGM_DECL_PROXY_TEMPLATE_CLASS(Avatar);


#if 1
	template<class T, class M>
	class Avatar_t< T, M, Avatar_T_Helper<T, M, false, false, false> >
	{
		static_assert
		(	std::is_same< T, std::decay_t<T> >::value && std::is_base_of<Mutability, M>::value
		,	""
		);

	public:
		enum : bool{IS_CONST = std::is_same<M, invar>::value || is_immutable<T>::value};

		using value_t = std::conditional_t< IS_CONST, std::add_const_t<T>, T >;


		bool is_yet() const		{  return _state == State::YET;  }
		bool is_owning() const	{  return _state == State::OWNING;  }
		bool has_gone() const	{  return _state == State::GONE;  }


		Avatar_t() : _pval(nullptr), _state(State::YET){}

		template< class Q, class = std::enable_if_t< !is_Avatar<Q>::value >  >
		Avatar_t(Q& q) : _pval(&q), _state(State::OWNING){}

		Avatar_t(T&&) = delete;

		template<  class M, class = std::enable_if_t< IS_CONST || !Avatar_t<T, M>::IS_CONST >  >
		Avatar_t(Avatar_t<T, M> const& avt) : _pval(&avt.value()), _state( get_state(avt) ){} 

		~Avatar_t(){  _pval = nullptr, _state = State::GONE;  }


		auto value() const-> T const&	{  return *_pval;  }
		operator T const&() const		{  return value();  }

		template< class = std::enable_if_t<!IS_CONST> >
		auto value()-> T&{  return *_pval;  }


		auto operator=(Avatar_t const& avt)-> Avatar_t&
		{
			static_assert(!IS_CONST, "cannot bind to const Avatar_t");

			assert(is_yet() && !avt.has_gone() || is_owning() && avt.is_owning());

			*_pval = avt.value(), _state = avt._state;

			return *this;
		}


		template
		<	class Q
		,	class 
			=	std::enable_if_t
				<	!IS_CONST && !is_Avatar<Q>::value && std::is_convertible<Q, T>::value
				>  
		>
		auto operator=(Q&& q)-> Avatar_t&
		{
			assert(is_owning() && L"Avatar_t has nothing");

			*_pval = std::forward<Q>(q);

			return *this;
		}


		template<class M>
		auto operator()(Avatar_t<T, M> avt)-> Avatar_t
		{
			static_assert
			(	IS_CONST || !Avatar_t<T, M>::IS_CONST, "cannot bind to const Avatar_t"
			);

			_pval = &avt.value(), _state = get_state(avt);

			return *this;
		}

		template< class Q, class = std::enable_if_t< !is_Avatar<Q>::value >  >
		auto operator()(Q& q)-> Avatar_t
		{
			static_assert
			(	IS_CONST || !is_immutable<decltype(q)>::value, "cannot bind to const Avatar_t"
			);

			assert(!has_gone() && L"Avatar_t was released already");

			_pval = &q, _state = State::OWNING;

			return *this;
		}


		template<class Q>
		bool operator==(Q&& q) const{  return *_pval == std::forward<Q>(q);  }

		template<class Q>
		bool operator!=(Q&& q) const{  return !( *this == std::forward<Q>(q) );  }


		template<class M>
		bool operator==(Avatar_t<T, M> avt) const
		{  
			assert(!has_gone() && !avt.has_gone() && L"Avatar_t was released already");

			return *this == avt.value(); 
		}

		template<class M>
		bool operator!=(Avatar_t<T, M> avt) const{  return !(*this == avt);  }


	private:
		value_t* _pval;
		enum class State{YET, OWNING, GONE} _state;


		template<class M>
		static auto get_state(Avatar_t<T, M> avt)-> State
		{
			return 
			avt.is_owning() ? State::OWNING : (avt.is_yet() ? State::YET : State::GONE);
		}
	};

#else
	template<class T>
	class Avatar_t< T, invar, Avatar_T_Helper<T, invar, false, false, false> >
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


		auto value() const-> T const&	{  return *_cpval;  }
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
	class Avatar_t< T, Var, Avatar_T_Helper<T, Var, false, false, false> >
	:	public Avatar_t< T, invar, Avatar_T_Helper<T, invar, false, false, false> >
	{
	private:
		T* _pval;
		

		using const_Avatar_t = constAvatar<T>;

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
		Avatar_t(T& t) : const_Avatar_t(t), _pval(&t){}
		
		Avatar_t(T const&) = delete;
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
#endif	

	template<class T> 
	static auto make_Avatar(T&& t)-> Avatar< std::remove_reference_t<T> >
	{
		static_assert(!is_Avatar<T>::value, "no nested Avatar_t");

		return Avatar< std::remove_reference_t<T> >( std::forward<T>(t) );
	}


} // end of namespace sgm

#endif // end of #ifndef _SGM_AVATAR_
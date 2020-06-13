#pragma once

#ifndef _SGM_AVATAR_
#define _SGM_AVATAR_

#include <cassert>
#include "..\Type_Analysis\Type_Analysis.hpp"
////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{


	SGM_DECL_PROXY_TEMPLATE_CLASS(Avatar);


	template<class T>
	struct is_constAvatar : No_Making
	{
	private:
		template<class T>
		struct AvtMutability : No_Making
		{
			using type 
			=	std::conditional_t< is_immutable<T>::value && is_Avatar<T>::value, invar, void >;
		};

		template<class T, class M, bool...BS>
		struct AvtMutability<  Avatar_t< T, M, Avatar_T_Helper<T, M, BS...> >  > : No_Making
		{
			using type 
			=	std::conditional_t
				<	std::is_same<M, invar>::value || is_immutable<T>::value
				,	invar
				,	std::conditional_t
					<	is_Avatar<T>::value
					,	typename AvtMutability< std::remove_reference_t<T> >::type
					,	Var
					>
				>;
		};


	public:
		enum : bool
		{	value 
			=	std::is_same
				<	typename AvtMutability< std::remove_reference_t<T> >::type, invar					 
				>::value
		};
	};
	//========//========//========//========//=======#//========//========//========//========//===


	template<class T, class M>
	class Avatar_t< T, M, Avatar_T_Helper<T, M, false, false, false> >
	{
	public:
		enum : bool
		{	IS_CONST 
			=	is_constAvatar
				<	Avatar_t< T, M, Avatar_T_Helper<T, M, false, false, false> >  
				>::value
		};

		using value_t = std::conditional_t< IS_CONST, std::add_const_t<T>, T >;


		bool is_yet() const	{  return _state == State::YET;  }
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
	//========//========//========//========//=======#//========//========//========//========//===


	template<  class T, class = std::enable_if_t< !is_Avatar<T>::value >  >
	static auto make_Avatar(T& t) SGM_DECLTYPE_AUTO(  Avatar< std::remove_reference_t<T> >(t)  )

	template<  class T, class = std::enable_if_t< is_Avatar<T>::value >  >
	static auto make_Avatar(T t) SGM_DECLTYPE_AUTO(  Avatar<typename T::value_t>(t.value())  )


} // end of namespace sgm

#endif // end of #ifndef _SGM_AVATAR_
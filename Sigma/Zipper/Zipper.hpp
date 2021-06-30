#pragma once

#ifndef _SGM_ZIPPER_
#define _SGM_ZIPPER_


#include "..\Family\Family.hpp"
#include "..\interface_Traits\interface_Traits.hpp"


namespace sgm
{
	
	template<class...CS>  class cZipper;
	template<class...CS>  class Zipper;

	template<class FAM, bool IS_MUTABLE>  class Zip_iterator;

	struct _itr_fam_traits;
	
}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class FAM, bool IS_MUTABLE>  
class sgm::Zip_iterator
{
	static_assert(is_Family<FAM>::value);

public:
	Zip_iterator(FAM fam) : _itr_fam(fam){}


	auto operator*() const{  return _deref(*this);  }
	auto operator*(){  return _deref(*this);  }

	decltype(auto) operator++(){  return _step<true>();  }
	decltype(auto) operator--(){  return _step<false>();  }
	auto operator++(int){  auto const clone = *this;  return ++*this,  clone;  }
	auto operator--(int){  auto const clone = *this;  return --*this,  clone;  }

	auto operator!=(Zip_iterator const zi) const
	->	bool{  return std::get<0>(this->_itr_fam) != std::get<0>(zi._itr_fam);  }

	auto operator==(Zip_iterator const zi) const-> bool{  return !(*this != zi);  }


private:
	mutable FAM _itr_fam;


	template<class ME, class...ARGS>
	static auto _deref([[maybe_unused]] ME &me, ARGS&&...args)
	{
		size_t constexpr IDX = sizeof...(ARGS);

		if constexpr(IDX == std::tuple_size<FAM>::value)
			return Family< remove_aleph_t<ARGS>... >( Forward<ARGS>(args)... );
		else
			return
			_deref
			(	me,	Forward<ARGS>(args)...
			,	*std::get<IDX>
				(	static_cast< Selective_t<IS_MUTABLE, FAM, FAM const> >(me._itr_fam)
				)
			);
	}


	template<bool PLUS, size_t IDX = 0>
	auto _step()-> Zip_iterator&
	{
		if constexpr(IDX == std::tuple_size<FAM>::value)
			return *this;
		else
		{
			if constexpr(PLUS)
				std::get<IDX>(_itr_fam)++; 
			else
				std::get<IDX>(_itr_fam)--;

			return _step<PLUS, IDX+1>();
		}
	}
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class...CS>  
class sgm::cZipper
{
	static_assert( (is_iterable<CS>::value &&...) );

	using _itr_fam_t = Family<  Decay_t< decltype(Declval<CS const>().begin()) >...  >;
	using _iter_t = Zip_iterator<_itr_fam_t, false>;

public:
	template<class..._CS>
	cZipper(_CS&&...args) : _fam( Forward<_CS>(args)... ), _size(_get_size()){}

	auto cbegin() const-> _iter_t{  return _get_iter<true>(*this);  }
	auto begin() const{  return cbegin();  }

	auto cend() const-> _iter_t{  return _get_iter<false>(*this);  }
	auto end() const{  return cend();  }

	auto size() const{  return _size;  }

protected:
	Family<CS...> _fam;

	template<bool WANT_BEGIN, class ME, class...ARGS>
	static auto _get_iter([[maybe_unused]] ME &me, ARGS&&...args)
	{
		size_t constexpr IDX = sizeof...(ARGS);

		if constexpr( IDX == sizeof...(CS) )
			return Make_Family( Forward<ARGS>(args)... );
		else if constexpr(WANT_BEGIN)
			return _get_iter<true>( me, Forward<ARGS>(args)..., std::get<IDX>(me._fam).begin() );
		else
			return _get_iter<false>( me, Forward<ARGS>(args)..., std::get<IDX>(me._fam).end() );
	}


private:
	size_t _size;

	template<size_t IDX = 0>
	auto _get_size() const
	{
		if constexpr( IDX == sizeof...(CS) )
			return std::get<0>(_fam).size();
		else if constexpr
		(	is_random_access_iterator
			<	decltype(Declval< Nth_t<IDX, CS...> >().begin())  
			>::	value
		)
			return std::get<IDX>(_fam).size();
		else
			return _get_size<IDX+1>();
	}
};


template<class...CS>
class sgm::Zipper : public cZipper<CS...>
{
	using _cz = cZipper<CS...>;
	using _itr_fam_t = Family<  Decay_t< decltype(Declval<CS>().begin()) >...  >;
	using _iter_t = Zip_iterator<_itr_fam_t, true>;

public:
	template<class..._CS>  Zipper(_CS&&...args) : _cz( Forward<_CS>(args)... ){}
	
	auto begin()-> _iter_t{  return _cz::_get_iter<true>(*this);  }
	auto end()-> _iter_t{  return _cz::_get_iter<false>(*this);  }
};


namespace sgm
{
	template<class..._CS>  cZipper(_CS&&...)-> cZipper< remove_aleph_t<_CS&&>... >;
	template<class..._CS>  Zipper(_CS&&...)-> Zipper< remove_aleph_t<_CS&&>... >;
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#include <iterator>


struct sgm::_itr_fam_traits : No_Making
{
private:
	template<class...ITRS>
	using _tag_t
	=	Selective_t
		<	(is_random_access_iterator<ITRS>::value &&...)
		,	std::random_access_iterator_tag
		,	Selective_t
			<	(Has_Operator_Post_Decrease<ITRS>::value &&...)
			,	std::bidirectional_iterator_tag
			,	std::forward_iterator_tag
			>
		>;	

	template<  class...ITRS, class = Guaranteed_t< (is_iterator<ITRS>::value &&...) >  >
	/* Declaration Only */ static auto _tag(Family<ITRS...>) noexcept-> _tag_t<ITRS...>;

public:
	template<class FAM>  using tag_t = decltype( _tag(Declval<FAM>()) );
};


template<bool M, class FAM>
struct std::iterator_traits< sgm::Zip_iterator<FAM, M> >
{
	using iterator_category = typename sgm::_itr_fam_traits::template tag_t<FAM>;
	using value_type = decay_t<  decltype(*sgm::Declval< sgm::Zip_iterator<FAM, M> >())  >;
	using difference_type = ptrdiff_t;
	using pointer = conditional_t<M, value_type, value_type const>*;
	using reference = remove_pointer_t<pointer>&;	
};

#endif // end of #ifndef _SGM_ZIP_
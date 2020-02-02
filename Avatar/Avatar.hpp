#pragma once

#ifndef _SGM_AVATAR_
#define _SGM_AVATAR_

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{


	template<class T>
	class Avatar
	{
		enum class State{ TEMPORARY, REFERENCING, DEAD };


	public:
		bool is_temporary() const{  return _state == State::TEMPORARY;  }
		bool is_released() const{  return _state == State::DEAD;  }


		Avatar() : _pval(nullptr), _state(State::TEMPORARY){}
		Avatar(T& t) : _pval(&t), _state(State::REFERENCING){}
		Avatar(T&& t) : _pval(  new T( static_cast<T&&>(t) )  ), _state(State::TEMPORARY){}

		~Avatar()
		{
			if(is_temporary())
				delete _pval, _pval = nullptr;

			_state = State::DEAD;
		}

		Avatar(Avatar const& avt) 
		:	_pval( avt.is_temporary() ? new T(*avt._pval) : avt._pval )
		,	_state(avt.is_released() ? State::TEMPORARY : avt._state)
		{
			if(is_released()) 
				throw !(bool) L"Avatar was released already";
		}

		Avatar(Avatar&& avt) 
		:	_pval(avt._pval)
		,	_state(avt.is_released() ? State::TEMPORARY : avt._state)
		{
			if(is_released())
				throw !(bool) L"Avatar was released already";
			else if(is_temporary())
				avt._pval = nullptr;
			
		}

		
		operator T const&() const{  return _value_impl(*this);  }
		auto value()-> T&{  return _value_impl(*this);  }
		auto value() const-> T const&{  return _value_impl(*this);  }


		auto operator=(T& t)-> Avatar const&
		{  
			if (is_released())
				throw !(bool) L"Avatar was released already";
			else if(is_temporary())
				delete _pval, _state = State::REFERENCING;

			_pval = &t;

			return *this;
		}

		auto operator=(T&& t) const-> Avatar const&
		{
			if (is_released())
				throw !(bool) L"Avatar was released already";
			else if(is_temporary())
			{ 
				if(_pval == nullptr)
					_pval = new T( static_cast<T&&>(t) );
				else
					*_pval = static_cast<T&&>(t);

				return *this;
			}
			else throw !(bool) L"having a reference already.";
		}

		auto operator=(Avatar const& avt)-> Avatar const&
		{
			if(is_released() || avt.is_released())
				throw !(bool) L"Avatar was released already";

			_pval = avt.is_temporary() ? new T(*avt._pval) : avt._pval,
			_state = avt._state;

			return *this;
		}

		auto operator=(Avatar&& avt)-> Avatar const&
		{
			if (is_released() || avt.is_released())
				throw !(bool) L"Avatar was released already";
			
			_pval = avt._pval, _state = avt._state;

			if(is_temporary())
				avt._pval = nullptr;

			return *this;
		}


		bool operator==(T const& t) const{  return *_pval == t;  }
		bool operator==(T&& t) const{  return *_pval == static_cast<T&&>(t);  }
		bool operator!=(T const& t) const{  return !(*this == t);  }
		bool operator!=(T&& t) const{  return !( *this == static_cast<T&&>(t) );  }

		bool operator==(Avatar avt) const
		{  
			if(is_released() || avt.is_released()) 
				throw !(bool) L"Avatar was released already";

			return *this == *avt._pval;  
		}

		bool operator!=(Avatar avt) const{  return !(*this == avt);  }


	private:
		T mutable* _pval;
		State mutable _state;


		template<class AVT>
		static auto _value_impl(AVT& avt)-> decltype(avt.value())
		{
			if(avt.is_released())
				throw !(bool) L"Avatar was released already";
			else if(avt._pval == nullptr)
				avt._pval = new T(), avt._state = State::TEMPORARY;	

			return *avt._pval;
		}

	};	// end of class Avatar<T>


}	// end of namespace sgm

#endif	// end of #ifndef _SGM_AVATAR_
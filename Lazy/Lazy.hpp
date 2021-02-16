#pragma once


#ifndef _SGM_LAZY_
#define _SGM_LAZY_

#include <new>


namespace sgm
{

	template<class F>
	struct Lazy
	{
		using type = decltype(  ( *static_cast<F*>(nullptr) )()  );
	
		Lazy(F f) : _f(f){}
		
		~Lazy(){  destruct();  }
	
		auto operator*() const-> type const&{  return _deref(this);  }
		auto operator*()-> type&{  return _deref(this);  }
	
	
		void destruct() noexcept
		{
			if(_is_evaluated)
			{
				(**this).~type();
	
				for(auto& t : _buf)
					t = char(0);		
	
				_is_evaluated = false;
			}
		}
	
	
	private:
		F _f;
		char _buf[sizeof(type)] = {0, };
		bool _is_evaluated = false;
	
	
		template<class THIS>
		static decltype(auto) _deref(THIS p)
		{
			if(!p->_is_evaluated)
				new(p->_buf) type(p->_f()),  p->_is_evaluated = true;
	
			return reinterpret_cast<decltype(**p)>(*p->_buf);
		}
	};	

}

#endif
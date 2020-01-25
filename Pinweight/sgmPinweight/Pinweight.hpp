#pragma once


namespace sgm
{


	template<class T>
	class Pinweight
	{
	public:
		Pinweight() : pval(new T()), pcount( new size_t(1) ){}
		Pinweight(T const& t) : pval( new T(t) ), pcount( new size_t(1) ){}		
		Pinweight(T&& t) : pval(  new T( static_cast<T&&>(t) )  ), pcount( new size_t(1) ){}

		Pinweight(Pinweight const& pw) : pval(pw.pval), pcount( _pcount_up(pw.pcount) ){}

		Pinweight(Pinweight&& pw) : pval(pw.pval), pcount(pw.pcount)
		{
			pw.pval = nullptr, pw.pcount = nullptr;
		}

		~Pinweight(){  _my_pcount_down();  }

	
		auto operator=(T const& t)-> Pinweight const&
		{  
			_my_pcount_down();

			pval = new T(t),
			pcount = new size_t(1);

			return *this;  	
		}

		auto operator=(T&& t)-> Pinweight const&
		{
			_my_pcount_down();

			pval = new T( static_cast<T&&>(t) ),
			pcount = new size_t(1);

			return *this;			
		}

		auto operator=(Pinweight const& pw)-> Pinweight const&
		{
			if(pval == pw.pval)
				return *this;

			_my_pcount_down(), _pcount_up(pw.pcount);

			pval = pw.pval, pcount = pw.pcount;

			return *this;
		}

		auto operator=(Pinweight&& pw)-> Pinweight const&
		{
			if(pval == pw.pval)
				return *this;

			_my_pcount_down();

			pval = pw.pval, pw.pval = nullptr,
			pcount = pw.pcount, pw.pcount = nullptr;

			return *this;
		}


		operator T const&() const{  return *pval;  }

		auto value()-> T&{  return *pval;  }
		auto value() const-> T const&{  return *pval;  }
		auto operator*()-> T&{  return *pval;  }
		auto operator*() const-> T const&{  return *pval;  }

		bool share_with(Pinweight const& pw) const{  return pval == pw.pval;  }

	
	private:
		T* pval;
		size_t* pcount;


		static auto _pcount_up(size_t* pn)-> size_t*
		{
			if(pn != nullptr)
				(*pn)++;

			return pn;
		}

		void _my_pcount_down()
		{
			if(pcount == nullptr)
				return;
			else if( --(*pcount) == 0 )
				delete pval, pval = nullptr,
				delete pcount, pcount = nullptr;
		}
	};


}
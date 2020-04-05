//#include "Type_Analysis.hpp"
#include "..\interface_Traits\interface_Traits.hpp"
#include <cassert>


namespace
{


	SGM_HAS_MEMFUNC(memfunc);
	SGM_HAS_MEMBER(memnum);
	SGM_HAS_MEMBER(stt_calc);
	SGM_HAS_MEMBER(sttmem);

	SGM_HAS_MEMBER(value);
	SGM_HAS_NESTED_TYPE(ID);
	SGM_HAS_NESTED_TYPE(type);


	class UnitTest
	{
	public:
		UnitTest() = delete;


		static void Case01()
		{
			static_assert(std::is_same<sgm::Pointerless_t<int**const**>, int>::value, "");
		}


		static void Case02()
		{
			static_assert
			(	sgm::Has_Type<int>::among<double, int, int const>::value
			,	""
			);
		}


		static void Case03()
		{
			static_assert
			(	sgm::Check_All<std::is_integral>::for_all<size_t, int, unsigned>::value
			,	""
			);

			static_assert
			(	sgm::Check_All<std::is_integral>::for_any<size_t*, int*, unsigned>::value
			,	""
			);
		}


		template<class T>
		struct CAA
		{
			CAA(T t = T()) : _t(t){}

			auto operator=(CAA)-> CAA = delete;
			auto operator=(T const&)-> CAA = delete;

			auto value() const-> T const&{  return _t;  };
		
		protected:
			T _t;
		};

		template<class T>
		struct AA : CAA<T>
		{
			AA(T t = T()) : CAA(t){}

			auto operator=(AA aa)-> AA{  return *this = CAA<T>(aa);  }
			auto operator=(CAA<T> caa)-> AA{  _t = caa.value(); return *this;  }
			auto operator=(T const& t)-> AA{  _t = t; return *this;  }

			auto value()-> T&{  return _t;  }
		};


		static void Case04()
		{
			AA<int> aa1, aa2(40);

			aa1 = aa2;

			aa2.value() = 65;

			assert(aa1.value() == 40 && aa2.value() ==65);

			CAA<int> caa1(10), caa2(23);

			//caa1.value() = 100;

			aa1 = caa1;

			assert(aa1.value() == 10);
		}

		//--------//--------//--------//--------//-------#//--------//--------//--------//--------
		struct BB
		{
			//BB& operator=(BB) = delete;

			bool operator==(BB) const{  return true;  }
			bool operator!=(BB) const{  return false;  }

			static auto stt_calc(int, double)-> int{  return 0;  }

			int memnum;
			auto memfunc(int)-> double{  return 0;  }

			static int sttmem;

			using type = double;

			template<class Q>
			using value_t = std::add_pointer_t<Q>;

			enum {value = 0};
			enum class ID{NUMBER = 123};
		};


		template<class, class T>
		struct is_comparable
		{
			static_assert
			(	std::integral_constant<T, false>::value
			,	"2nd template parameter needs to be function type."
			);
		};

		template<class C, class FUNC, class...ARGS>
		struct is_comparable<C, FUNC(ARGS...)>
		{
		private:
			template<class T>
			static auto _check(T*)
			->	typename std::is_same
				<	decltype( std::declval<T>().operator=(std::declval<ARGS>()...) )
				,	FUNC
				>::	type;


			template<class>
			static auto _check(...)-> std::false_type;

			using type = decltype( _check<C>(nullptr) );

		public:
			enum : bool{  value = type::value  };
		};


		static_assert
		(	(	is_comparable<BB, BB&(BB)>::value
			&&	std::is_convertible< decltype(BB::stt_calc), int(*)(int, double) >::value
			)
		,	""
		);

		static_assert 
		(	(	Has_Member_memnum<BB>::value
			&&	Has_MemFunc_memfunc<BB, int>::value
			&&	Has_Member_stt_calc<BB>::value
			&&	Has_Member_sttmem<BB>::value
			&&	Has_Member_value<BB>::value
			&&	Has_NestedType_type<BB>::value
			&&	Has_NestedType_ID<BB>::value
			)
		,	"Something Wrong"
		);
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		static void Case05()
		{
			BB bb;

			auto sfunc = bb.stt_calc;

			assert( sfunc(3, 0.14) == 0 );

			enum {VV = sgm::is_Ordered<BB>::value};

			auto Lamf = [](int)-> double{  return 0;  };

			std::is_function<decltype(Lamf)(int)>::value;
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

	};

	int UnitTest::BB::sttmem = 30;


}

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

int main()
{
	UnitTest::Case04();
	UnitTest::Case05();

	return 0;
}



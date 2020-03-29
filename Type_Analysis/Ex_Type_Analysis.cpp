#include "Type_Analysis.hpp"
#include <cassert>


namespace
{
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
		(	is_comparable<BB, BB&(BB)>::value
		,	""
		);
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------





		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


	};
}

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

int main()
{
	UnitTest::Case04();

	return 0;
}



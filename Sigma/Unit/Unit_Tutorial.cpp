#include "Unit.hpp"
#include <cassert>
#include <iostream>

namespace
{
	using namespace sgm;

	
	class Tutorial
	{
	public:
		template<unsigned> static void Case();

		template<>
		static void Case<1>()
		{
			
		}

		template<>
		static void Case<2>()
		{
			
		}


		Tutorial() : _Everything_is_Fine(false)
		{
			Case<1>();
			Case<2>();

			_Everything_is_Fine = true;
		}

		operator bool() const{  return _Everything_is_Fine;  }


	private:
		bool _Everything_is_Fine;
	};


}

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

int main()
{
	if(Tutorial())
		std::wcout << L"Every tutorial case worked well.\n";
		
	return 0;
}
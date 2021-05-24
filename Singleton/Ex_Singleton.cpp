#include "Singleton/Singleton.h"
#include <iostream>

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

class AAA : sgm::Singleton<AAA>
{
public:
	int x = 30;
	char const* str = "foo";

	AAA() {  std::cout << "constructor" << '\n';  }
	~AAA(){  std::cout << "destructor" << '\n';  }

	void show() const
	{
		std::cout
		<<	"x = " << x << '\n'
		<<	"str = " << str << '\n';
	}
};
//========//========//========//========//=======#//========//========//========//========//=======#

int main()
{
	try
	{
		AAA aaa;
	}
	catch(bool)
	{
		std::cout 
		<<	"Exception : unauthorized access.\n"
		<<	"Use Singleton<T>::init method instead.\n";
	}

	std::cout << sgm::Singleton<AAA>::empty() << '\n';

	sgm::Singleton<AAA>::init();

	std::cout << sgm::Singleton<AAA>::empty() << '\n';

	sgm::Singleton<AAA>::write
	(	[](AAA& a){  a.x = 22;  }, std::execution::par 
	);

	sgm::Singleton<AAA>::read().show();

	sgm::Singleton<AAA>::clear();

	std::cout << sgm::Singleton<AAA>::empty() << '\n';

	sgm::Singleton<AAA>::init();

	sgm::Singleton<AAA>::write().x = 400;

	sgm::Singleton<AAA>::read(std::execution::par).show();

	return 0;
}
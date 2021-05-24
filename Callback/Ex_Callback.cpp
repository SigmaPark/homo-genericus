#include <iostream>
#include <string>
#include <array>
#include <type_traits>
#include <execution>


template<typename...Ts> 
class overloaded : public Ts...
{
public:
	using Ts::operator()...;
};


template<typename... Ts> 
overloaded(Ts...)-> overloaded<Ts...>;



int main()
{
	using std::cout, std::endl;

	double const d = 4.4;

	//	���غҰ�2 : �̰� �׳� auto ovl Ȥ�� auto const ovl�� �����ϸ� main�� ���� �� ���ܹ߻�
	auto const& ovl	
	=   overloaded
	    {   [](auto arg){  std::cout << "auto type : " << arg << endl;  }
	    ,   [](double const& arg) {  cout << "double type : " << arg << endl;  }	
	    ,   [](std::string arg){  cout << "string type : " << arg.c_str() << endl;  }
	    };

	ovl(3);
	ovl(d);
	ovl(3.2);
	ovl( std::string("str") );

	return 0;
}
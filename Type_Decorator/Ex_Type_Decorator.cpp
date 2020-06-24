#include "Type_Decorator.hpp"

struct const_type : sgm::Type_Decorator
{
	template<class T>
	using type = std::add_const_t<T>;
};

struct pointer_type : sgm::Type_Decorator
{
	template<class T>
	using type = std::add_pointer_t<T>;
};

int main()
{
	sgm::Decorated_t<int, const_type, pointer_type, const_type> p = nullptr;

	return 0;  
}
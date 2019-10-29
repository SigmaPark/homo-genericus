#include <iostream>
#include <list>

int main()
{
	std::cout << "Hello, Azure\n";

	std::cout << "I expect you to boost me up.\n";

	std::list<int> li;

	for (int n = 1; li.size() < 10; n++)
		if (n % 3 == 0)
			li.emplace_back(n);

	for(auto const& x : li)
		std::cout << x << ' ';

	std::cout << '\n';

	return 0;
}
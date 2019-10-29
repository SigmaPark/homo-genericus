#include <iostream>
#include "Recursion/Recursion.h"

static void Example(int n){  std::cout << "Example " << n << "." << '\n';  }


static void Lines(int L)
{
	SGM_RECURSION(dec)
	{
		auto const d = dec - 1;

		std::cout << "---------" << L - d;

		self(d);

	ESCAPE_IF(dec == 0)
		std::cout << '\n';
	}(L);
}


int main()
{
	Example(1);
	{
		auto const factorial_5
		=	SGM_RECURSION(n, res)
			{
				self(n - 1, n*res);

			ESCAPE_IF(n <= 1)
				return res;
			}( (size_t)5, (size_t)1 );

		std::cout << "factorial(5) is " << factorial_5 << '\n';
	}
	Lines(8);

	Example(2);
	{
		auto const fibonacci_10
		=	SGM_RECURSION(n, prev, next)
			{
				auto const
					New_prev = next,
					New_next = prev + next;

				self(n - 1, New_prev, New_next);

			ESCAPE_IF(n <= 1)
				return next;
			}(10, 0, 1);

		std::cout << "fibonacci(10) is " << fibonacci_10 << '\n';
	}
	Lines(8);

	Example(3);
	{
		auto Collatz_sequence
		=	SGM_RECURSION(n, steps)
			{
				std::cout << n << " -> ";

				if(n%2 == 1)
					self(3*n + 1, steps + 1);
				else
					self(n/2, steps + 1);

			ESCAPE_IF(n <= 1)
				std::cout 
				<<	1 << '\n'
				<<	"total " << steps << " steps. \n";
			};

		int const x = 22;

		std::cout << "Collatz_sequence(" << x << ") : ";

		Collatz_sequence(x, 0);
	}
	Lines(8);

	return 0;
}



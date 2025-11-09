#include "Poiya_Problem.hpp"
#include <cmath>


void sgm::test::PrimeFactors::_update_until(std::size_t const n)
{
	for(auto x = _primes.back() + 2;  x <= n;  x += 2)
		for
		(	auto itr = _primes.cbegin()
		;	x >= *itr * *itr || ( _primes.emplace_back(x),  false )
		;	++itr
		)
			if(x % *itr == 0)
				break;
}


auto sgm::test::PrimeFactors::primes_under(std::size_t const n)-> std::deque<std::size_t> const&
{
	if(n > _primes.back())
		_update_until(n);
		
	return _primes;
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


auto sgm::test::Poiya_rate::rof_odd_factored(std::size_t const n)-> long double
{
	if(_nof_odd_factors.size() <= n)
		_update_until(n);

	return static_cast<long double>(_nof_odd_factors[n]) / (n - 1);
}


auto sgm::test::Poiya_rate::_nof_factors(std::size_t n)-> std::size_t
{
	auto const sqn = static_cast<std::size_t>( std::sqrt(n) );
	auto const& prime_numbers = _primes.primes_under(sqn + 1);
	
	auto const end 
	=	[&prime_numbers, sqn]
		{
			auto itr = prime_numbers.cbegin();

			while(itr != prime_numbers.cend() && *itr <= sqn)
				++itr;

			return itr == prime_numbers.cend() ? itr : ++itr;
		}();

	std::size_t nof_prime_factors = 0;

	for(auto itr = prime_numbers.cbegin(); n != 1 && itr != end;)
		if(auto d = *itr;  n % d == 0)
			n /= d,
			++nof_prime_factors;
		else
			++itr;

	return nof_prime_factors == 0 ? std::size_t(1) : nof_prime_factors;		
}


void sgm::test::Poiya_rate::_update_until(std::size_t const n)
{
	if(_nof_odd_factors.size() <= n)
		for(auto k = _nof_odd_factors.size();  k <= n;  ++k)
			_nof_odd_factors.emplace_back
			(	_nof_factors(k) % 2 == 1 
			?	_nof_odd_factors.back() + 1
			:	_nof_odd_factors.back()
			);
}


auto sgm::test::Poiya_Problem(std::size_t const n)-> long double
{
	return Poiya_rate{}.rof_odd_factored(n);
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#
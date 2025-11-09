#pragma once
#ifndef _SGM_SPEC_POIYA_PROBLEM_
#define _SGM_SPEC_POIYA_PROBLEM_


#include <cstddef>
#include <deque>


namespace sgm
{
	namespace test
	{

		auto Poiya_Problem(std::size_t const n)-> long double;

		class PrimeFactors;
		class Poiya_rate;	

	}
}
//========//========//========//========//=======#//========//========//========//========//=======#


class sgm::test::PrimeFactors
{
public:
	PrimeFactors() : _primes({2, 3}){}

	auto primes_under(std::size_t const n)-> std::deque<std::size_t> const&;


private:
	std::deque<std::size_t> _primes;

	void _update_until(std::size_t const n);
};


class sgm::test::Poiya_rate
{
public:
	Poiya_rate() : _primes(), _nof_odd_factors({0, 0, 1}){}

	auto rof_odd_factored(std::size_t const n)-> long double;

private:
	PrimeFactors _primes;
	std::deque<std::size_t> _nof_odd_factors;


	auto _nof_factors(std::size_t n)-> std::size_t;
	void _update_until(std::size_t n);
};

#endif // end of #ifndef _SGM_SPEC_POIYA_PROBLEM_
#pragma once

#include "indexable_interface.hpp"
#include <vector>
#include <array>

namespace sgm
{
	

	template<class T, DxSize_t SIZE>
	class indexable_impl
	{
		using core_t 
		=	std::conditional_t
			<	SIZE == DxSize::DYNAMIC, std::vector<T>, std::array<T, SIZE>
			>;

		core_t _core;

	protected:
		indexable_impl() = default;

		template<  class ITR, class = std::enable_if_t< is_iterator<ITR>::value >  >
		indexable_impl(ITR bi, ITR ei) : _core(bi, ei){}


		template
		<	class S
		,	class = std::enable_if_t< std::is_integral<S>::value && SIZE == DxSize::DYNAMIC >
		>
		indexable_impl(S size) 
		:	_core
			(	[size]()-> core_t
				{
					core_t res;

					res.reserve(size);

					return res;
				}()
			)
		{}


		template
		<	class S, class...ARGS
		,	class 
			=	std::enable_if_t
				<	std::is_integral<S>::value && SIZE == DxSize::DYNAMIC 
				&&	sizeof...(ARGS) != 0
				>
		>
		indexable_impl(S size, ARGS const&...args) : _core( size, T(args...) ){}


		template<class Q>
		indexable_impl(std::initializer_list<Q>&& con) : _core(con.begin(), con.end()){}


	private:



	};


}
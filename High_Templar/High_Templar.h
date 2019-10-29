/*	함수형 프로그래밍의 higher ordre function들을 template 형태로 제공한다
*/
#pragma once

#ifndef _SGM_HIGH_TEMPLAR_
#define _SGM_HIGH_TEMPLAR_

#ifdef _MSC_VER
	static_assert(_MSC_VER >= 1914, "C++17 or higher version language support is required.");
#endif

#include <algorithm>
#include <iterator>
#include <execution>

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{


	template<typename _T = void>
	class identity
	{
	public:
		template<typename T>
		auto operator()(T const& t)
		{
			if constexpr(std::is_same_v<_T, void>)
				return t;
			else
				return static_cast<_T>(t);
		}
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	#ifdef _ARRAY_
	template<size_t D, size_t OFFSET, typename index_t, typename...Ns>
	static auto constexpr _indices_tail(Ns... ns)
	{	
		if constexpr(D == 0)
			return std::array<index_t, sizeof...(Ns)>{ static_cast<index_t>(ns)... };
		else
			return _indices_tail<D - 1, OFFSET, index_t>( ns..., sizeof...(Ns) + OFFSET );
	}

	template<size_t N, size_t OFFSET = 0, typename index_t = size_t>
	static auto constexpr indices(){  return _indices_tail<N, OFFSET, index_t>();  }
	#endif


	#ifdef _VECTOR_
	template<typename T, typename POL>
	static auto indices(size_t const s, T const offset, POL&& pol)
	{
		std::vector<T> res(s);

		std::for_each
		(	std::move(pol)
		,	std::begin(res), std::end(res)
		,	[&res, offset](auto& x)
			{
				x = static_cast<T>(  std::distance( &*std::begin(res), &x )  ) + offset;
			}
		);

		return res;
	}

	template<typename _2ND>
	static auto indices(size_t const s, _2ND&& second)
	{
		if constexpr(std::is_execution_policy_v< std::decay_t<_2ND> >)
			return indices( s, size_t(0), std::move(second) );
		else
			return indices( s, std::forward<_2ND>(second), std::execution::seq);
	}

	inline auto indices(size_t const s)
	{
		return indices( s, size_t(0), std::execution::seq );
	}
	#endif

	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<typename T>
	class Templar
	{
	private:
		class _proxy : public T{  friend class Templar<T>;  };

	public:
		Templar() = delete;

		template<typename CON, typename F, typename POL, typename Y>
		static auto Morph(CON&& con, F&& f, POL&& pol, Y&& dftval)
		{
			return 
			_proxy::_Morph
			(	std::forward<CON>(con), std::forward<F>(f)
			,	std::move(pol), std::forward<Y>(dftval)
			);
		}

		template<typename CON, typename F, typename POL>
		static auto Filter(CON&& con, F&& f, POL&& pol)
		{
			return 
			_proxy::_Filter
			(	std::forward<CON>(con), std::forward<F>(f), std::move(pol) 
			);
		}
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---

	
	template<typename CON>
	class Container_traits;


#define SPECIALIZE_CONTAINER_TRAITS(con)	\
	template<typename...ARGS>	\
	class Container_traits< std::con<ARGS...> >	\
	{	\
	public:		\
		Container_traits() = delete;	\
		\
		using Templar_t = std_##con;	\
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	class Looper
	{
	public:
		Looper() = delete;

		template<typename CON, typename F, typename POL, typename res_t>
		static auto _Transform(CON&& con, F&& f, POL&& pol, res_t&& res)
		{
			std::transform
			(	std::move(pol)
			,	std::cbegin(con), std::cend(con), std::begin(res)
			,	std::forward<F>(f)
			);

			return std::move(res);
		}


		template<typename CON, typename F, typename res_t>
		static auto _Emplace(CON&& con, F&& f, res_t&& res)
		{
			for(auto const& t : con)
				res.emplace( f(t) );

			return std::move(res);
		}


		template<typename CON, typename F, typename res_t>
		static auto _Emplace_Back(CON&& con, F&& f, res_t&& res)
		{
			for(auto const& t : con)
				res.emplace_back( f(t) );

			return std::move(res);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------/


		template<typename CON, typename F, typename POL, typename res_t>
		static auto _Copy_and_Erase_if(CON&& con, F&& f, POL&& pol, res_t&& res)
		{
			auto NewEnd 
			=	std::copy_if
				(	std::move(pol), std::cbegin(con), std::cend(con), std::begin(res)
				,	std::forward<F>(f)
				);

			res.erase( NewEnd, std::end(res) );

			return std::move(res);
		}


		template<typename CON, typename F, typename res_t>
		static auto _Back_Insert_if(CON&& con, F&& f, res_t&& res)
		{
			std::copy_if
			(	std::cbegin(con), std::cend(con), std::back_insert_iterator(res)
			,	std::forward<F>(f)
			);

			return std::move(res);
		}


		template<typename CON, typename F, typename res_t>
		static auto _Emplace_if(CON&& con, F&& f, res_t&& res)
		{
			for(auto const& t : con)
				if( f(t) )
					res.emplace(t);

			return std::move(res);
		}

	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	#ifdef _VECTOR_
	class std_vector : public Templar<std_vector>
	{
	public:
		std_vector() = delete;

		template<typename...TYPES>
		using container_t = std::vector<TYPES...>;

	private:
		template<typename T>
		static auto _ReservedVector(size_t N)
		{
			container_t<T> res{};

			res.reserve(N);

			return res;
		}

	protected:
		template<typename CON, typename F, typename POL, typename Y>
		static auto _Morph
		(	CON&& con, F&& f, [[maybe_unused]] POL&& pol, [[maybe_unused]] Y&& dftval
		)
		{
			if constexpr(std::is_same_v<POL, std::execution::sequenced_policy>)
				return 
				Looper::_Emplace_Back
				(	std::forward<CON>(con), std::forward<F>(f)
				,	_ReservedVector<Y>(con.size())
				);
			else
				return
				Looper::_Transform
				(	std::forward<CON>(con), std::forward<F>(f), std::move(pol)
				,	container_t<Y>( con.size(), std::forward<Y>(dftval) )
				);
		}


		template
		<	typename CON, typename F, typename POL
		,	typename X = typename std::decay_t<CON>::value_type
		>
		static auto _Filter(CON&& con, F&& f, [[maybe_unused]] POL&& pol)
		{
			if constexpr (std::is_same_v<POL, std::execution::sequenced_policy>)
				return
				[&con, &f]
				{
					auto res 
					=	Looper::_Back_Insert_if
						(	std::forward<CON>(con), std::forward<F>(f)
						,	_ReservedVector<X>(con.size())
						);

					res.shrink_to_fit();

					return res;
				}();
			else
				return
				Looper::_Copy_and_Erase_if
				(	std::forward<CON>(con), std::forward<F>(f), std::move(pol)
				,	container_t<X>( con.size(), *std::cbegin(con) )
				);
		}
	};

	SPECIALIZE_CONTAINER_TRAITS(vector);
	#endif
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	#ifdef _LIST_
	class std_list : public Templar<std_list>
	{
	public:
		std_list() = delete;

		template<typename...TYPES>
		using container_t = std::list<TYPES...>;

	protected:
		template<typename CON, typename F, typename POL, typename Y>
		static auto _Morph(CON&& con, F&& f, POL&&, Y&&)
		{
			return 
			Looper::_Emplace_Back
			(	std::forward<CON>(con), std::forward<F>(f), container_t<Y>()
			);
		}


		template
		<	typename CON, typename F, typename POL
		,	typename X = typename std::decay_t<CON>::value_type
		>
		static auto _Filter(CON&& con, F&& f, POL&& pol)
		{
			if constexpr (std::is_same_v<POL, std::execution::sequenced_policy>)
				return
				Looper::_Back_Insert_if
				(	std::forward<CON>(con), std::forward<F>(f)
				,	container_t<X>()
				);
			else
				return
				Looper::_Copy_and_Erase_if
				(	std::forward<CON>(con), std::forward<F>(f), std::move(pol)
				,	container_t<X>( con.size(), *std::cbegin(con) )
				);
		}
	};

	SPECIALIZE_CONTAINER_TRAITS(list);
	#endif
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	#ifdef _ARRAY_
	template<size_t N>
	class std_array : public Templar< std_array<N> >
	{
	public:
		std_array() = delete;

		template<typename T>
		using container_t = std::array<T, N>;

	protected:
		template<typename CON, typename F, typename POL, typename Y>
		static auto _Morph(CON&& con, F&& f, POL&& pol, Y&&)
		{
			if(con.size() != N)
				std::cout << "Exception : array size doesn't agree with input. \n",
				throw false;

			return
			Looper::_Transform
			(	std::forward<CON>(con), std::forward<F>(f), std::move(pol)
			,	container_t<Y>()
			);
		}


		template<typename CON, typename F, typename POL>
		static auto constexpr _Filter(CON&& con, F&& f, POL&& pol)
		{
			static_assert(false, "std_array does not offer Filter method.");
		}

	};


	template<size_t N, typename T>
	class Container_traits< std::array<T, N> >
	{
	public:
		Container_traits() = delete;

		using Templar_t = std_array<N>;
	};
	#endif
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	#ifdef _DEQUE_
	class std_deque : public Templar<std_deque>
	{
	public:
		std_deque() = delete;

		template<typename...TYPES>
		using container_t = std::deque<TYPES...>;

	protected:
		template<typename CON, typename F, typename POL, typename Y>
		static auto _Morph
		(	CON&& con, F&& f, [[maybe_unused]] POL&& pol, [[maybe_unused]] Y&& dftval
		)
		{
			if constexpr(std::is_same_v<POL, std::execution::sequenced_policy>)
				return
				Looper::_Emplace_Back
				(	std::forward<CON>(con), std::forward<F>(f), container_t<Y>()
				);
			else
				return
				Looper::_Transform
				(	std::forward<CON>(con), std::forward<F>(f), std::move(pol)
				,	container_t<Y>( con.size(), std::forward<Y>(dftval) )
				);
		}


		template
		<	typename CON, typename F, typename POL
		,	typename X = typename std::decay_t<CON>::value_type
		>
		static auto _Filter(CON&& con, F&& f, [[maybe_unused]] POL&& pol)
		{
			if constexpr(std::is_same_v<POL, std::execution::sequenced_policy>)
				return
				Looper::_Back_Insert_if
				(	std::forward<CON>(con), std::forward<F>(f)
				,	container_t<X>()
				);
			else
				return
				Looper::_Copy_and_Erase_if
				(	std::forward<CON>(con), std::forward<F>(f), std::move(pol)
				,	container_t<X>( con.size(), *std::cbegin(con) )
				);
		}
	};

	SPECIALIZE_CONTAINER_TRAITS(deque);
	#endif
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	#ifdef _FORWARD_LIST_
	class std_forward_list : public Templar<std_forward_list>
	{
	public:
		std_forward_list() = delete;

		template<typename...TYPES>
		using container_t = std::forward_list<TYPES...>;

	protected:
		template<typename CON, typename F, typename POL, typename Y>
		static auto _Morph(CON&& con, F&& f, POL&&, Y&&)
		{
			static_assert
			(	std::is_same_v<POL, std::execution::sequenced_policy>
			,	"std_forward_list does not offer concurrent method"
			);

			container_t<Y> res{};

			for
			(	auto [itr1, itr2] = std::pair( std::cbegin(con), res.cbefore_begin() )
			;	itr1 != std::cend(con)
			;	itr1++, itr2++
			)
				res.emplace_after( itr2, f(*itr1) );

			return res;
		}

		template
		<	typename CON, typename F, typename POL
		,	typename X = typename std::decay_t<CON>::value_type
		>
		static auto _Filter(CON&& con, F&& f, POL&&)
		{
			static_assert
			(	std::is_same_v<POL, std::execution::sequenced_policy>
			,	"std_forward_list does not offer concurrent method"
			);
			
			container_t<X> res{};

			for
			(	auto [itr1, itr2] = std::pair( std::cbegin(con), res.cbefore_begin() )
			;	itr1 != std::cend(con)
			;	itr1++
			)
				if( f(*itr1) )
					res.emplace_after(itr2, *itr1),
					itr2++;

			return res;
		}
		
	};

	SPECIALIZE_CONTAINER_TRAITS(forward_list);
	#endif
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---



	template< template<typename...> class TC >
	class emplace_single : public Templar< emplace_single<TC> >
	{
	public:
		emplace_single() = delete;

		template<typename T>
		using container_t = TC<T>;

	protected:
		template<typename CON, typename F, typename POL, typename Y>
		static auto _Morph(CON&& con, F&& f, POL&&, Y&&)
		{
			static_assert
			(	std::is_same_v<POL, std::execution::sequenced_policy>
			,	"associative/applicative container does not offer concurrent method"
			);

			return
			Looper::_Emplace
			(	std::forward<CON>(con), std::forward<F>(f), container_t<Y>()
			);
		}


		template
		<	typename CON, typename F, typename POL
		,	typename X = typename std::decay_t<CON>::value_type
		>
		static auto _Filter(CON&& con, F&& f, POL&&)
		{
			static_assert
			(	std::is_same_v<POL, std::execution::sequenced_policy>
			,	"associative/applicative container does not offer concurrent method"
			);

			return 
			Looper::_Emplace_if
			(	std::forward<CON>(con), std::forward<F>(f), container_t<X>()
			);
		}
	};


	template< template<typename...> class TC >
	class emplace_pair : public Templar< emplace_pair<TC> >
	{
	public:
		emplace_pair() = delete;

		template<typename T>
		using container_t = TC<typename T::first_type, typename T::second_type>;

	protected:
		template<typename CON, typename F, typename POL, typename Y>
		static auto _Morph(CON&& con, F&& f, POL&&, Y&&)
		{
			static_assert
			(	std::is_same_v<POL, std::execution::sequenced_policy>
			,	"associative container does not offer concurrent method"
			);

			return 
			Looper::_Emplace
			(	std::forward<CON>(con), std::forward<F>(f), container_t<Y>()
			);
		}

		
		template
		<	typename CON, typename F, typename POL
		,	typename X = typename std::decay_t<CON>::value_type
		>
		static auto _Filter(CON&& con, F&& f, POL&&)
		{
			static_assert
			(	std::is_same_v<POL, std::execution::sequenced_policy>
			,	"associative container does not offer concurrent method"
			);

			return 
			Looper::_Emplace_if
			(	std::forward<CON>(con), std::forward<F>(f), container_t<X>()
			);
		}
	};


	#ifdef _SET_
	class std_set : public emplace_single<std::set>{};
	class std_multiset : public emplace_single<std::multiset>{};

	SPECIALIZE_CONTAINER_TRAITS(set);
	SPECIALIZE_CONTAINER_TRAITS(multiset);
	#endif


	#ifdef _MAP_
	class std_map : public emplace_pair<std::map>{};
	class std_multimap : public emplace_pair<std::multimap>{};

	SPECIALIZE_CONTAINER_TRAITS(map);
	SPECIALIZE_CONTAINER_TRAITS(multimap);
	#endif


	#ifdef _UNORDERED_SET_
	class std_unordered_set : public emplace_single<std::unordered_set>{};
	class std_unordered_multiset : public emplace_single<std::unordered_multiset>{};

	SPECIALIZE_CONTAINER_TRAITS(unordered_set);
	SPECIALIZE_CONTAINER_TRAITS(unordered_multiset);
	#endif


	#ifdef _UNORDERED_MAP_
	class std_unordered_map : public emplace_pair<std::unordered_map>{};
	class std_unordered_multimap : public emplace_pair<std::unordered_multimap>{};

	SPECIALIZE_CONTAINER_TRAITS(unordered_map);
	SPECIALIZE_CONTAINER_TRAITS(unordered_multimap);
	#endif


	#ifdef _QUEUE_
	class std_queue : public emplace_single<std::queue>{};
	class std_priority_queue : public emplace_single<std::priority_queue>{};

	SPECIALIZE_CONTAINER_TRAITS(queue);
	SPECIALIZE_CONTAINER_TRAITS(priority_queue);
	#endif


	#ifdef _STACK_
	class std_stack : public emplace_single<std::stack>{};

	SPECIALIZE_CONTAINER_TRAITS(stack);
	#endif
	//========//========//========//========//=======#//========//========//========//========//===


	/*	함수형 프로그래밍의 고차함수 Map과 동일한 역할
	*/
	template
	<	typename CON, typename F
	,	typename POL = std::execution::sequenced_policy
	,	typename Y
		=	std::invoke_result_t
			<	std::decay_t<F>, typename std::decay_t<CON>::value_type 
			>
	>
	static auto Morph(CON&& con, F&& f, POL pol = std::execution::seq, Y&& dftval = Y())
	{
		return
		Container_traits< std::decay_t<CON> >::Templar_t::Morph
		(	std::forward<CON>(con), std::forward<F>(f)
		,	std::move(pol), std::forward<Y>(dftval)
		);
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	/*	컨테이너 캐스팅
	*/
	template<typename outCON, typename CON, typename POL = std::execution::sequenced_policy>
	static auto Repack(CON&& con, POL pol = std::execution::seq)
	{
		return 
		outCON::Morph
		(	std::forward<CON>(con), sgm::identity<>(), std::move(pol) 
		,	static_cast< typename std::decay_t<CON>::value_type >( *std::cbegin(con) )
		);
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	/*	함수형 프로그래밍의 고차함수 Filter
	*/
	template<typename CON, typename F, typename POL = std::execution::sequenced_policy>
	static auto Filter(CON&& con, F&& f, POL pol = std::execution::seq)
	{
		return
		Container_traits< std::decay_t<CON> >::Templar_t::Filter
		(	std::forward<CON>(con), std::forward<F>(f), std::move(pol)
		);
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	/*	함수형 프로그래밍의 고차함수 Reduce
	*/
	template<typename CON, typename F, typename POL, typename T>
	static auto Fold(CON&& con, F&& f, POL&& pol, T&& init)
	{
		static_assert
		(	std::is_execution_policy_v< std::decay_t<POL> >
		,	"POL is not execution policy type"
		);

		return 
		std::reduce
		(	std::move(pol), std::cbegin(con), std::cend(con), std::forward<T>(init)
		,	std::forward<F>(f)
		);
	}

	template<typename CON, typename F, typename _3RD>
	static auto Fold(CON&& con, F&& f, _3RD&& third)
	{
		if constexpr(std::is_execution_policy_v< std::decay_t<_3RD> >)
			return
			std::reduce
			(	std::move(third)
			,	std::next( std::cbegin(con) ), std::cend(con), *std::cbegin(con)
			,	std::forward<F>(f)
			);
		else
			return
			std::reduce
			(	std::cbegin(con), std::cend(con), std::forward<_3RD>(third)
			,	std::forward<F>(f)
			);
	}

	template<typename CON, typename _2ND>
	static auto Fold(CON&& con, _2ND&& second)
	{
		if constexpr(std::is_execution_policy_v< std::decay_t<_2ND> >) 
			return std::reduce( std::move(second), std::cbegin(con), std::cend(con) );
		else
			return 
			std::reduce
			(	std::next( std::cbegin(con) ), std::cend(con), *std::cbegin(con)
			,	std::forward<_2ND>(second)
			);
	}

	template<typename CON>
	static auto Fold(CON&& con)
	{
		return std::reduce( std::cbegin(con), std::cend(con) );
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	/*	정렬
	*/
	template<typename CON, typename F, typename POL>
	static auto Sort(CON con, F&& f, [[maybe_unused]] POL&& pol)
	{
		if constexpr
		(	std::is_same_v
			<	typename std::iterator_traits<decltype( std::begin(con) )>::iterator_category
			,	std::random_access_iterator_tag
			>
		)
			std::sort( std::move(pol), std::begin(con), std::end(con), std::forward<F>(f) );
		else if constexpr
		(	false
			#ifdef _LIST_
		||	std::is_same_v
			<	typename Container_traits< std::decay_t<CON> >::Templar_t, std_list 
			>
			#endif
			#ifdef _FORWARD_LIST_
		||	std::is_same_v
			<	typename Container_traits< std::decay_t<CON> >::Templar_t, std_forward_list
			>
			#endif
		)
			con.sort( std::forward<F>(f) );
		else
			static_assert(false, "no suitable sorting method is found");

		return con;
	}

	template<typename CON, typename _2ND>
	static auto Sort(CON&& con, _2ND&& second)
	{
		if constexpr(std::is_execution_policy_v< std::decay_t<_2ND> >) 
			return Sort( std::forward<CON>(con), std::less<>(), std::move(second) );
		else
			return
			Sort( std::forward<CON>(con), std::forward<_2ND>(second), std::execution::seq );
	}

	template<typename CON>
	static auto Sort(CON&& con)
	{
		return Sort( std::forward<CON>(con), std::less<>(), std::execution::seq);
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	#ifdef _LIST_
	template<typename T, typename F>
	class _Ranker_Helper
	{
	private:
		template<typename CON, typename F, typename elem_t>
		friend auto Rankers(CON&& con, size_t n, F&& f);

		#ifdef _ARRAY_
		template<size_t SIZE, typename CON, typename F, typename elem_t>
		friend auto Rankers(CON&& con, F&& f);
		#endif
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------/

		_Ranker_Helper(size_t _n, F&& _comp)
		:	n(_n), comp( std::forward<F>(_comp) ), count(0), rankers()
		{}

		void operator()(T const& t)
		{
			if(count < n)
			{
				rankers.emplace_back(t);

				if(++count == n)
					rankers.sort(comp);
			}
			else if( comp(t, rankers.back()) )
			{
				auto low_itr = std::lower_bound(rankers.cbegin(), rankers.cend(), t, comp);

				rankers.emplace(low_itr, t),
				rankers.pop_back();
			}
		}

		auto get_rankers() const-> std::list<T> const&
		{
			if(count != n)
				rankers.sort(comp);

			return rankers;
		}


		size_t const n;
		F&& comp;
		size_t mutable count;
		std::list<T> mutable rankers;
	};


	template
	<	typename CON, typename F = decltype(std::less<>())
	,	typename elem_t = typename std::decay_t<CON>::value_type
	>
	static auto Rankers(CON&& con, size_t n, F&& f = std::less<>())
	{
		size_t const 
			con_size	= con.size(), 
			size		= con_size < n ? con_size : n;

		_Ranker_Helper<size_t, F> helper( size, std::forward<F>(f) );

		for(auto const& t : con)
			helper(t);

		if constexpr
		(	std::is_same_v
			<	typename Container_traits< std::decay_t<CON> >::Templar_t, std_list
			>
		)
			return helper.get_rankers();
		else
			return 
			Repack<  typename Container_traits< std::decay_t<CON> >::Templar_t  >
			(	helper.get_rankers()
			);
	}
	#endif


	#if defined _LIST_ && defined _ARRAY_
	template
	<	size_t SIZE, typename CON, typename F = decltype(std::less<>())
	,	typename elem_t = typename std::decay_t<CON>::value_type
	>
	static auto Rankers(CON&& con, F&& f = std::less<>())
	{
		size_t const
			con_size	= con.size(),
			size		= con_size < SIZE ? con_size : SIZE;

		_Ranker_Helper<elem_t, F> helper( size, std::forward<F>(f) );

		for (auto const& t : con)
			helper(t);

		return Repack< std_array<SIZE> >(helper.get_rankers());
	}
	#endif


	template<typename CON, typename F = decltype(std::less<>())>
	static auto Top_Ranker(CON&& con, F&& f = std::less<>())
	{
		return *std::min_element( std::cbegin(con), std::cend(con), std::forward<F>(f) );
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---

	
	template<typename CON>
	static auto Take(CON&& con, signed int n)
	{
		if( auto gap = abs(n); n > 0 )
			return std::decay_t<CON>(  std::cbegin(con), std::next( std::cbegin(con), gap )  );
		else
			return std::decay_t<CON>(  std::prev( std::cend(con), gap ), std::cend(con)  );
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


}

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

#endif
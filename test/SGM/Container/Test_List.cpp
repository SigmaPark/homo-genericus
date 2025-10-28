/*  SPDX-FileCopyrightText: (c) 2022 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "SGM/Container/List.hpp"
#include "Test_List.hpp"


using sgm::h2u::Are_Equivalent_Ranges;
using sgm::h2u::Specimen;


static void Construction()
{
	{
		sgm::List<Specimen> Li;

		SGM_H2U_ASSERT(Li.is_empty() && Li.size() == 0);
	}
	{
		sgm::List<Specimen> Li{Specimen(1), Specimen(3), Specimen(5)};

		SGM_H2U_ASSERT(Li.size() == 3);
		
		SGM_H2U_ASSERT
		(	Are_Equivalent_Ranges
			(	Li
			,	std::initializer_list<Specimen>{Specimen(1), Specimen(3), Specimen(5)}
			)
		);
	}
}


static void Emplacement()
{
	{
		sgm::List<Specimen> Li;

		Li.emplace_back( Specimen(1) );

		Li.emplace_back(3);

		Li.emplace_front( Specimen(5) );

		Li.emplace_front(7);

		SGM_H2U_ASSERT
		(	Are_Equivalent_Ranges
			(	Li
			,	std::initializer_list<Specimen>
				{	Specimen(7), Specimen(5), Specimen(1), Specimen(3)
				}
			)
		);
	}
	{
		sgm::List<Specimen> Li{Specimen(1), Specimen(3), Specimen(5), Specimen(7)};

		auto itr = Li.end();

		auto new_itr = Li.emplace_prev( itr, Specimen(9) );

		SGM_H2U_ASSERT( *new_itr == Specimen(9) );

		SGM_H2U_ASSERT
		(	Are_Equivalent_Ranges
			(	Li
			,	std::initializer_list<Specimen>
				{	Specimen(1), Specimen(3), Specimen(5), Specimen(7), Specimen(9)
				}
			)
		);
	}
	{
		sgm::List<Specimen> Li{Specimen(1), Specimen(3), Specimen(5), Specimen(7)};
		
		auto itr = Li.rend();

		auto new_itr = Li.emplace_prev( itr, Specimen(9) );

		SGM_H2U_ASSERT( *new_itr == Specimen(9) );

		SGM_H2U_ASSERT
		(	Are_Equivalent_Ranges
			(	Li
			,	std::initializer_list<Specimen>
				{	Specimen(9), Specimen(1), Specimen(3), Specimen(5), Specimen(7)
				}
			)
		);
	}
	{
		sgm::List<Specimen> Li{Specimen(1), Specimen(3), Specimen(5), Specimen(7)};

		auto itr = sgm::Next(Li.begin());

		auto new_itr = Li.emplace_next( itr, Specimen(9) );

		SGM_H2U_ASSERT( *new_itr == Specimen(9) );

		SGM_H2U_ASSERT
		(	Are_Equivalent_Ranges
			(	Li
			,	std::initializer_list<Specimen>
				{	Specimen(1), Specimen(3), Specimen(9), Specimen(5), Specimen(7)
				}
			)
		);
	}
	{
		sgm::List<Specimen> Li{Specimen(1), Specimen(3), Specimen(5), Specimen(7)};
		
		auto itr = Li.rbegin();

		auto new_itr = Li.emplace_next( itr, Specimen(9) );

		SGM_H2U_ASSERT( *new_itr == Specimen(9) );

		SGM_H2U_ASSERT
		(	Are_Equivalent_Ranges
			(	Li
			,	std::initializer_list<Specimen>
				{	Specimen(1), Specimen(3), Specimen(5), Specimen(9), Specimen(7)
				}
			)
		);
	}
}


static void Pop()
{
	sgm::List<Specimen> Li
	{	Specimen(1), Specimen(3), Specimen(5), Specimen(7), Specimen(9), Specimen(11)
	};
	
	{
		Li.pop_back();

		SGM_H2U_ASSERT
		(	Are_Equivalent_Ranges
			(	Li
			,	std::initializer_list<Specimen>
				{	Specimen(1), Specimen(3), Specimen(5), Specimen(7), Specimen(9)
				}
			)
		);		
	}
	{
		Li.pop_front();
	
		SGM_H2U_ASSERT
		(	Are_Equivalent_Ranges
			(	Li
			,	std::initializer_list<Specimen>
				{	Specimen(3), Specimen(5), Specimen(7), Specimen(9)
				}
			)
		);
	}
	{
		auto citr = sgm::Next(Li.begin());

		SGM_H2U_ASSERT( *citr == Specimen(5) );

		auto itr = Li.pop(citr);

		SGM_H2U_ASSERT( *itr == Specimen(7) );

		SGM_H2U_ASSERT
		(	Are_Equivalent_Ranges
			(	Li
			,	std::initializer_list<Specimen>
				{	Specimen(3), Specimen(7), Specimen(9)
				}
			)
		);
	}
}


static void Pop_Range()
{
	{
		sgm::List<Specimen> Li
		{	Specimen(1), Specimen(3), Specimen(5), Specimen(7), Specimen(9), Specimen(11)
		};
		
		auto first = sgm::Next(Li.begin()),  last = sgm::Next(first, 3);

		SGM_H2U_ASSERT( *first == Specimen(3) && *last == Specimen(9) );

		auto itr = Li.pop(first, last);

		SGM_H2U_ASSERT(itr == last);

		SGM_H2U_ASSERT
		(	Are_Equivalent_Ranges
			(	Li
			,	std::initializer_list<Specimen>{Specimen(1), Specimen(9), Specimen(11)}
			)
		);
	}
	{
		sgm::List<Specimen> Li
		{	Specimen(1), Specimen(3), Specimen(5), Specimen(7), Specimen(9), Specimen(11)
		};

		auto first = sgm::Next(Li.begin()),  last = first;

		SGM_H2U_ASSERT( *first == Specimen(3) && *last == Specimen(3) );

		auto itr = Li.pop(first, last);

		SGM_H2U_ASSERT(itr == last);

		SGM_H2U_ASSERT
		(	Are_Equivalent_Ranges
			(	Li
			,	sgm::List<Specimen>
				{	Specimen(1), Specimen(3), Specimen(5), Specimen(7)
				,	Specimen(9), Specimen(11)
				}
			)
		);
	}
}


namespace sgm
{
	namespace _Test_List_Allocator_detail
	{
		template<class T>
		class Test_Allocator : public Allocator< sgm::List_Node<T> >
		{
		private:
			using _base_t = Allocator< sgm::List_Node<T> >;


		public:
			Test_Allocator(sgm::List_Node<T>* node_arr) : _node_arr(node_arr), _idx(0){}

			auto allocate(size_t)-> sgm::List_Node<T>*{  return _node_arr + _idx;  }

			void deallocate(sgm::List_Node<T>*, size_t){}

			template<class Q, class...ARGS>
			void construct(Q* p, ARGS&&...args)
			{
				SGM_CRTP_OVERRIDE(construct, <Q, ARGS&&...>);

				_base_t::construct( p, Forward<ARGS&&>(args)... );

				++_idx;
			}

			template<class Q>
			void destroy(Q* p){  p->~Q();  }


		private:
			sgm::List_Node<T>* _node_arr;
			size_t _idx;
		};
	}
}


static void Allocator()
{
	size_t constexpr list_node_byte_size_v = sizeof(sgm::List_Node<Specimen>);
	size_t constexpr max_nof_node_in_buffer_v = 10;

	wchar_t buffer[list_node_byte_size_v * max_nof_node_in_buffer_v] = {0, };
	auto node_arr = reinterpret_cast< sgm::List_Node<Specimen>* >(buffer);

	auto Li 
	=	sgm::List<Specimen>::by
		(	sgm::_Test_List_Allocator_detail::Test_Allocator<Specimen>(node_arr)
		);


	Li.emplace_back(1);

	SGM_H2U_ASSERT( node_arr[0].value == Specimen(1) );

	Li.emplace_back(3);

	SGM_H2U_ASSERT( node_arr[1].value == Specimen(3) );

	Li.emplace_front(5);

	SGM_H2U_ASSERT( node_arr[2].value == Specimen(5) );

	Li.emplace_front(7);

	SGM_H2U_ASSERT( node_arr[3].value == Specimen(7) );

	Li.emplace_next( sgm::Next(Li.begin()), Specimen(9) );

	SGM_H2U_ASSERT( node_arr[4].value == Specimen(9) );

	SGM_H2U_ASSERT
	(	Are_Equivalent_Ranges
		(	Li
		,	std::initializer_list<Specimen>
			{	Specimen(7), Specimen(5), Specimen(9), Specimen(1), Specimen(3)
			}
		)
	);
}


static void End_iterator()
{
	sgm::List<Specimen> Li{Specimen(1), Specimen(3), Specimen(5)};
	
	{
		auto itr = Li.end();

		SGM_H2U_ASSERT( &*Li.rbegin() == &*sgm::Prev(itr) );
	}
	{
		auto itr = Li.rend();

		SGM_H2U_ASSERT( &*Li.begin() == &*sgm::Prev(itr) );
	}
}


static void Swap()
{
	{
		sgm::List<Specimen> 
			Li0{Specimen(1), Specimen(3), Specimen(5)},
			Li1{Specimen(7), Specimen(9)};

		Li0.swap(Li1);

		SGM_H2U_ASSERT
		(	Are_Equivalent_Ranges
			(	Li0
			,	sgm::List<Specimen>{Specimen(7), Specimen(9)}
			)
		&&	Are_Equivalent_Ranges
			(	Li1
			,	sgm::List<Specimen>{Specimen(1), Specimen(3), Specimen(5)}
			)
		);
	}
	{
		sgm::List<Specimen> 
			Li0{Specimen(1), Specimen(3), Specimen(5)},
			Li1{Specimen(7), Specimen(9)};

		sgm::Swap(Li0, Li1);

		SGM_H2U_ASSERT
		(	Are_Equivalent_Ranges
			(	Li0
			,	sgm::List<Specimen>{Specimen(7), Specimen(9)}
			)
		&&	Are_Equivalent_Ranges
			(	Li1
			,	sgm::List<Specimen>{Specimen(1), Specimen(3), Specimen(5)}
			)
		);
	}
}


static void Size_of_List()
{
	static_assert( sizeof(sgm::List<Specimen>) <= 3*sizeof(std::uint64_t*), "" );
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


struct Performance : sgm::Unconstructible
{
	static void push_iterate_and_pop();
};


#include <list>
#include <chrono>


void Performance::push_iterate_and_pop()
{
#if 0
	using namespace std::chrono;
	
	int constexpr N = 100'000'000;
	{
	#if 0
		std::size_t constexpr list_node_byte_size_v = sizeof(sgm::List_Node<Specimen>);

		auto* buffer = new wchar_t[list_node_byte_size_v * N];
		//[list_node_byte_size_v * N] = {0, };
		auto node_arr = reinterpret_cast< sgm::List_Node<Specimen>* >(buffer);

		auto list	
		=	sgm::List<Specimen>::by
			(	sgm::_Test_List_Allocator_detail
				::	Test_Allocator<sgm::h2u::Specimen>(node_arr)
			);
	#else
		sgm::List<sgm::h2u::Specimen> list;
	#endif

		std::wcout << "\tsgm::List\n";

		{
			std::wcout << "\t\templace_back : ";

			auto const start_tp = system_clock::now();

			for( int i = 0;  i < N;  list.emplace_back(i++) );

			auto const time = system_clock::now() - start_tp;

			std::wcout << duration_cast<milliseconds>(time).count() << " millisec.\n";
		}
		{
			sgm::h2u::Specimen s;

			std::wcout << "\t\titerate : ";

			auto const start_tp = system_clock::now();

			for(auto const& e : list)
				s = e;

			auto const time = system_clock::now() - start_tp;

			std::wcout << duration_cast<milliseconds>(time).count() << " millisec.\n";
		}
		{
			std::wcout << "\t\tpop_back : ";

			auto const start_tp = system_clock::now();

			list.pop(list.begin(), list.end());
			//for(int i = 0;  i < N;  list.pop_back(),  ++i);

			auto const time = system_clock::now() - start_tp;

			std::wcout << duration_cast<milliseconds>(time).count() << " millisec.\n";
		}

		SGM_H2U_ASSERT(list.is_empty());

		//delete[] buffer;
	}	

	std::wcout << '\n';

	{
		std::wcout << "\tstd::list\n";

		std::list<sgm::h2u::Specimen> list;

		{
			std::wcout << "\t\templace_back : ";

			auto const start_tp = system_clock::now();

			for( int i = 0;  i < N;  list.emplace_back(i++) );

			auto const time = system_clock::now() - start_tp;

			std::wcout << duration_cast<milliseconds>(time).count() << " millisec.\n";
		}
		{
			sgm::h2u::Specimen s;

			std::wcout << "\t\titerate : ";

			auto const start_tp = system_clock::now();

			for(auto const& e : list)
				s = e;

			auto const time = system_clock::now() - start_tp;

			std::wcout << duration_cast<milliseconds>(time).count() << " millisec.\n";
		}
		{
			std::wcout << "\t\tpop_back : ";

			auto const start_tp = system_clock::now();

			for(int i = 0;  i < N;  list.pop_front(),  ++i);

			auto const time = system_clock::now() - start_tp;

			std::wcout << duration_cast<milliseconds>(time).count() << " millisec.\n";
		}

		SGM_H2U_ASSERT(list.empty());
	}
#endif
}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_HOW2USE_TESTS(sgm::h2u::Test_, List, /**/)
{	::Construction
,	::Emplacement
,	::Pop
,	::Pop_Range
,	::Allocator
,	::End_iterator
,	::Swap
,	::Size_of_List
,	::Performance::push_iterate_and_pop
};

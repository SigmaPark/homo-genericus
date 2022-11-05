/*  SPDX-FileCopyrightText: (c) 2022 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "List.hpp"
#include "Test_List.hpp"


using sgm::spec::is_True;
using sgm::spec::Specimen;


static void Construction()
{
	{
		sgm::List<Specimen> Li;

		is_True(Li.is_empty() && Li.size() == 0);
	}
	{
		sgm::List<Specimen> Li{Specimen(1), Specimen(3), Specimen(5)};

		is_True(Li.size() == 3);
		
		sgm::spec::Are_Equivalent_Ranges
		(	Li
		,	std::initializer_list<Specimen>{Specimen(1), Specimen(3), Specimen(5)}
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

		sgm::spec::Are_Equivalent_Ranges
		(	Li
		,	std::initializer_list<Specimen>
			{	Specimen(7), Specimen(5), Specimen(1), Specimen(3)
			}
		);
	}
	{
		sgm::List<Specimen> Li{Specimen(1), Specimen(3), Specimen(5), Specimen(7)};

		auto itr = Li.end();

		auto new_itr = Li.emplace_prev( itr, Specimen(9) );

		is_True( *new_itr == Specimen(9) );

		sgm::spec::Are_Equivalent_Ranges
		(	Li
		,	std::initializer_list<Specimen>
			{	Specimen(1), Specimen(3), Specimen(5), Specimen(7), Specimen(9)
			}
		);
	}
	{
		sgm::List<Specimen> Li{Specimen(1), Specimen(3), Specimen(5), Specimen(7)};
		
		auto itr = Li.rend();

		auto new_itr = Li.emplace_prev( itr, Specimen(9) );

		is_True( *new_itr == Specimen(9) );

		sgm::spec::Are_Equivalent_Ranges
		(	Li
		,	std::initializer_list<Specimen>
			{	Specimen(9), Specimen(1), Specimen(3), Specimen(5), Specimen(7)
			}
		);
	}
	{
		sgm::List<Specimen> Li{Specimen(1), Specimen(3), Specimen(5), Specimen(7)};

		auto itr = sgm::Next(Li.begin());

		auto new_itr = Li.emplace_next( itr, Specimen(9) );

		is_True( *new_itr == Specimen(9) );

		sgm::spec::Are_Equivalent_Ranges
		(	Li
		,	std::initializer_list<Specimen>
			{	Specimen(1), Specimen(3), Specimen(9), Specimen(5), Specimen(7)
			}
		);
	}
	{
		sgm::List<Specimen> Li{Specimen(1), Specimen(3), Specimen(5), Specimen(7)};
		
		auto itr = Li.rbegin();

		auto new_itr = Li.emplace_next( itr, Specimen(9) );

		is_True( *new_itr == Specimen(9) );

		sgm::spec::Are_Equivalent_Ranges
		(	Li
		,	std::initializer_list<Specimen>
			{	Specimen(1), Specimen(3), Specimen(5), Specimen(9), Specimen(7)
			}
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

		sgm::spec::Are_Equivalent_Ranges
		(	Li
		,	std::initializer_list<Specimen>
			{	Specimen(1), Specimen(3), Specimen(5), Specimen(7), Specimen(9)
			}
		);		
	}
	{
		Li.pop_front();
	
		sgm::spec::Are_Equivalent_Ranges
		(	Li
		,	std::initializer_list<Specimen>
			{	Specimen(3), Specimen(5), Specimen(7), Specimen(9)
			}
		);
	}
	{
		auto citr = sgm::Next(Li.begin());

		is_True( *citr == Specimen(5) );

		auto itr = Li.pop(citr);

		is_True( *itr == Specimen(7) );

		sgm::spec::Are_Equivalent_Ranges
		(	Li
		,	std::initializer_list<Specimen>
			{	Specimen(3), Specimen(7), Specimen(9)
			}
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

		is_True( *first == Specimen(3) && *last == Specimen(9) );

		auto itr = Li.pop(first, last);

		is_True(itr == last);

		sgm::spec::Are_Equivalent_Ranges
		(	Li
		,	std::initializer_list<Specimen>{Specimen(1), Specimen(9), Specimen(11)}
		);
	}
	{
		sgm::List<Specimen> Li
		{	Specimen(1), Specimen(3), Specimen(5), Specimen(7), Specimen(9), Specimen(11)
		};

		auto first = sgm::Next(Li.begin()),  last = first;

		is_True( *first == Specimen(3) && *last == Specimen(3) );

		auto itr = Li.pop(first, last);

		is_True(itr == last);

		sgm::spec::Are_Equivalent_Ranges
		(	Li
		,	sgm::List<Specimen>
			{	Specimen(1), Specimen(3), Specimen(5), Specimen(7), Specimen(9), Specimen(11)
			}
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

	unsigned char buffer[list_node_byte_size_v * max_nof_node_in_buffer_v] = {0, };
	auto node_arr = reinterpret_cast< sgm::List_Node<Specimen>* >(buffer);

	auto Li 
	=	sgm::List<Specimen>::by
		(	sgm::_Test_List_Allocator_detail::Test_Allocator<Specimen>(node_arr)
		);


	Li.emplace_back(1);

	is_True( node_arr[0].value == Specimen(1) );

	Li.emplace_back(3);

	is_True( node_arr[1].value == Specimen(3) );

	Li.emplace_front(5);

	is_True( node_arr[2].value == Specimen(5) );

	Li.emplace_front(7);

	is_True( node_arr[3].value == Specimen(7) );

	Li.emplace_next( sgm::Next(Li.begin()), Specimen(9) );

	is_True( node_arr[4].value == Specimen(9) );

	sgm::spec::Are_Equivalent_Ranges
	(	Li
	,	std::initializer_list<Specimen>
		{	Specimen(7), Specimen(5), Specimen(9), Specimen(1), Specimen(3)
		}
	);
}


static void End_iterator()
{
	sgm::List<Specimen> Li{Specimen(1), Specimen(3), Specimen(5)};
	
	{
		auto itr = Li.end();

		is_True( &*Li.rbegin() == &*sgm::Prev(itr) );
	}
	{
		auto itr = Li.rend();

		is_True( &*Li.begin() == &*sgm::Prev(itr) );
	}
}


static void Swap()
{
	{
		sgm::List<Specimen> 
			Li0{Specimen(1), Specimen(3), Specimen(5)},
			Li1{Specimen(7), Specimen(9)};

		Li0.swap(Li1);

		sgm::spec::Are_Equivalent_Ranges
		(	Li0
		,	sgm::List<Specimen>{Specimen(7), Specimen(9)}
		);
		
		sgm::spec::Are_Equivalent_Ranges
		(	Li1
		,	sgm::List<Specimen>{Specimen(1), Specimen(3), Specimen(5)}
		);
	}
	{
		sgm::List<Specimen> 
			Li0{Specimen(1), Specimen(3), Specimen(5)},
			Li1{Specimen(7), Specimen(9)};

		sgm::Swap(Li0, Li1);

		sgm::spec::Are_Equivalent_Ranges
		(	Li0
		,	sgm::List<Specimen>{Specimen(7), Specimen(9)}
		);
		
		sgm::spec::Are_Equivalent_Ranges
		(	Li1
		,	sgm::List<Specimen>{Specimen(1), Specimen(3), Specimen(5)}
		);
	}
}


static void Size_of_List()
{
	static_assert( sizeof(sgm::List<Specimen>) <= 3*sizeof(std::uint64_t*), "" );
}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_SPECIFICATION_TEST(sgm::spec::Test_, List, /**/)
{	::Construction
,	::Emplacement
,	::Pop
,	::Pop_Range
,	::Allocator
,	::End_iterator
,	::Swap
,	::Size_of_List
};

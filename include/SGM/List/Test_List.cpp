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
#if 0
	{
		sgm::List<Specimen> Li;

		is_True(Li.is_empty() && Li.size() == 0);
	}
	{
		sgm::List<Specimen> Li{Specimen(1), Specimen(3), Specimen(5)};

		is_True(Li.size() == 3);
		
		sgm::spec::Are_Equivalent
		(	Li
		,	std::initializer_list<Specimen>{Specimen(1), Specimen(3), Specimen(5)}
		);
	}
	{
		
	}
#endif
}


static void Emplacement()
{
#if 0
	sgm::List<Specimen> Li;

	Li.emplace_back( Specimen(1) );

	Li.emplace_back(3);

	Li.emplace_front( Specimen(5) );

	Li.emplace_front(7);

	{
		auto citr = sgm::Next(Li.begin());

		auto itr = Li.emplace( citr, Specimen(9) );

		is_True( *itr == Specimen(9) );
	}

	sgm::spec::Are_Equivalent
	(	Li
	,	std::initializer_list<Specimen>
		{	Specimen(7), Specimen(5), Specimen(9), Specimen(1), Specimen(3)
		}
	);
#endif
}


static void Pop()
{
#if 0
	sgm::List<Specimen> Li
	{	Specimen(1), Specimen(3), Specimen(5), Specimen(7), Specimen(9), Specimen(11)
	};
	
	{
		auto itr = Li.pop_back();

		is_True(itr == Li.end());

		sgm::spec::Are_Equivalent
		(	Li
		,	std::initializer_list<Specimen>
			{	Specimen(1), Specimen(3), Specimen(5), Specimen(7), Specimen(9)
			}
		);		
	}
	{
		auto itr = Li.pop_front();

		is_True(itr == Li.begin());

		sgm::spec::Are_Equivalent
		(	Li
		,	std::initializer_list<Specimen>
			{	Specimen(3), Specimen(5), Specimen(7), Specimen(9)
			}
		);
	}
	{
		auto citr = sgm::Next(Li.begin());

		is_True( *citr = Specimen(5) );

		auto itr = Li.pop(citr);

		is_True( *itr == Specimen(7) );

		sgm::spec::Are_Equivalent
		(	Li
		,	std::initializer_list<Specimen>
			{	Specimen(3), Specimen(5), Specimen(7), Specimen(9)
			}
		);
	}
#endif
}


static void Pop_Range()
{
#if 0
	{
		sgm::List<Specimen> Li
		{	Specimen(1), Specimen(3), Specimen(5), Specimen(7), Specimen(9), Specimen(11)
		};
		
		auto first = sgm::Next(Li.begin()),  last = sgm::Next(first, 3);

		is_True( *first == Specimen(3) && *last == Specimen(9) );

		auto itr = Li.pop(first, last);

		is_True(itr == last);

		sgm::spec::Are_Equivalent
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

		sgm::spec::Are_Equivalent
		(	Li
		,	sgm::List<Specimen>
			{	Specimen(1), Specimen(3), Specimen(5), Specimen(7), Specimen(9), Specimen(11)
			}
		);
	}
#endif
}


namespace sgm
{
	namespace _Test_List_Allocator_detail
	{
	#if 0
		template<class T>
		class Test_Allocator : public Allocator< sgm::List_Node<T> >
		{
		private:
			using _base_t = Allocator< sgm::List_Node<T> >;

		public:
			Test_Allocator(sgm::List_Node<T>* node_arr) : _node_arr(node_arr), _idx(0){}


			template<class Q, class...ARGS>
			void construct(Q*, ARGS&&...args)
			{
				_base_t::construct( _node_arr+_idx, Forward<ARGS&&>(args)... );
			}


		private:
			sgm::List_Node<T>* _node_arr;
			size_t _idx;
		};
	#endif
	}
}


static void Allocator()
{
#if 0
	size_t constexpr list_node_byte_size_v = sizeof(sgm::List_Node<Specimen>);
	size_t constexpr max_nof_node_in_buffer_v = 10;

	unsigned char buffer[list_node_byte_size_v * max_nof_node_in_buffer_v] = {0, };
	auto node_arr = reinterpret_cast< sgm::List_Node<Specimen>* >(buffer);

	auto Li 
	=	sgm::List<Specimen>::by
		(	sgm::_Test_List_Allocator_detail::Test_Allocator<Specimen>(node_arr)
		);


	Li.emplace_back(1);

	is_True( node_arr[0].value() == Specimen(1) );

	Li.emplace_back(3);

	is_True( node_arr[1].value() == Specimen(3) );

	Li.emplace_front(5);

	is_True( node_arr[2].value() == Specimen(5) );

	Li.emplace_front(7);

	is_True( node_arr[3].value() == Specimen(7) );

	Li.emplace( sgm::Next(Li.begin()), Specimen(9) );

	is_True( node_arr[4].value() == Specimen(9) );

	sgm::spec::Are_Equivalent
	(	Li
	,	std::initializer_list<Specimen>
		{	Specimen(7), Specimen(5), Specimen(9), Specimen(1), Specimen(3)
		}
	);
#endif
}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_SPECIFICATION_TEST(sgm::spec::Test_, List, /**/)
{	::Construction
,	::Emplacement
,	::Pop
,	::Pop_Range
,	::Allocator
};

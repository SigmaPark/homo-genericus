/*  SPDX-FileCopyrightText: (c) 2022 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Test_Queue.hpp"
#include "Queue.hpp"


using sgm::spec::Specimen;


struct Queue_Contents : sgm::Unconstructible
{
	static void Constructions();
	static void Push_and_Pop();
	static void Clear();
	static void Construction_by_Allocator();
	static void Push_and_Pop_by_Allocator();
};


void Queue_Contents::Constructions()
{
	{
		sgm::Queue<Specimen> qu;

		sgm::spec::is_True(qu.is_empty());
	}
}


void Queue_Contents::Push_and_Pop()
{
	{
		sgm::Queue<Specimen> qu;

		qu.push( Specimen(1) );
		qu.push( Specimen(3) );

		sgm::spec::is_True(qu.size() == 2 && qu.front() == 1 && qu.back() == 3);

		qu.push( Specimen(5) );

		sgm::spec::is_True(qu.size() == 3 && qu.front() == 1 && qu.back() == 5);

		qu.pop();

		sgm::spec::is_True(qu.size() == 2 && qu.front() == 3 && qu.back() == 5);

		qu.pop();

		sgm::spec::is_True(qu.size() == 1 && qu.front() == 5 && qu.back() == 5);
	}
}


void Queue_Contents::Clear()
{
	{
		sgm::Queue<Specimen> qu;

		qu.push( Specimen(1) );
		qu.push( Specimen(3) );
		qu.push( Specimen(5) );

		sgm::spec::is_True(qu.size() == 3 && qu.front() == 1 && qu.back() == 5);

		qu.clear();

		sgm::spec::is_True(qu.is_empty());
	}
}


namespace sgm
{
	namespace _Test_Queue_Allocator_detail
	{

		template<class T>
		class Test_Allocator : public Allocator< List_Node<T> >
		{
		private:
			using _base_t = Allocator< List_Node<T> >;


		public:
			Test_Allocator(List_Node<T>* node_arr) : _node_arr(node_arr), _idx(0){}

			auto allocate(size_t)-> List_Node<T>*{  return _node_arr + _idx;  }

			void deallocate(List_Node<T>*, size_t){}

			template<class Q, class...ARGS>
			void construct(Q* p, ARGS&&...args)
			{
				_base_t::construct( p, Forward<ARGS&&>(args)... );

				++_idx;
			}

			template<class Q>
			void destroy(Q* p){  p->~Q();  }


		private:
			List_Node<T>* _node_arr;
			size_t _idx;
		};

	}
}


void Queue_Contents::Construction_by_Allocator()
{
	{
		std::size_t constexpr list_node_byte_size_v = sizeof(sgm::List_Node<Specimen>);
		std::size_t constexpr max_nof_node_in_buffer_v = 10;

		unsigned char buffer[list_node_byte_size_v * max_nof_node_in_buffer_v] = {0, };
		auto node_arr = reinterpret_cast< sgm::List_Node<Specimen>* >(buffer);
		
		auto qu 
		=	sgm::Queue<Specimen>::by
			(	sgm::_Test_Queue_Allocator_detail::Test_Allocator<Specimen>(node_arr) 
			);

		sgm::spec::is_True(qu.is_empty());
	}
}


void Queue_Contents::Push_and_Pop_by_Allocator()
{
	{
		std::size_t constexpr list_node_byte_size_v = sizeof(sgm::List_Node<Specimen>);
		std::size_t constexpr max_nof_node_in_buffer_v = 10;

		unsigned char buffer[list_node_byte_size_v * max_nof_node_in_buffer_v] = {0, };
		auto node_arr = reinterpret_cast< sgm::List_Node<Specimen>* >(buffer);

		auto qu 
		=	sgm::Queue<Specimen>::by
			(	sgm::_Test_Queue_Allocator_detail::Test_Allocator<Specimen>(node_arr) 
			);

		qu.push( Specimen(1) );
		qu.push( Specimen(3) );

		sgm::spec::is_True
		(	qu.size() == 2 && qu.front() == 1 && qu.back() == 3
		&&	node_arr[0].value == 1 && node_arr[1].value == 3
		);

		qu.push( Specimen(5) );

		sgm::spec::is_True
		(	qu.size() == 3 && qu.front() == 1 && qu.back() == 5
		&&	node_arr[0].value == 1 && node_arr[1].value == 3 && node_arr[2].value == 5
		);

		qu.pop();

		sgm::spec::is_True
		(	qu.size() == 2 && qu.front() == 3 && qu.back() == 5
		&&	node_arr[0].value == Specimen::State::DESTRUCTION 
		&&	node_arr[1].value == 3
		&&	node_arr[2].value == 5
		);

		qu.pop();

		sgm::spec::is_True
		(	qu.size() == 1 && qu.front() == 5 && qu.back() == 5
		&&	node_arr[0].value == Specimen::State::DESTRUCTION 
		&&	node_arr[1].value == Specimen::State::DESTRUCTION
		&&	node_arr[2].value == 5
		);
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#
#if 0

struct Circular_Queue_Contents : sgm::Unconstructible
{
	static void Constructions();
	static void Push_and_Pop();
	static void Clear();
	static void Construction_by_Allocator();
	static void Push_and_Pop_by_Allocator();
};


void Circular_Queue_Contents::Constructions()
{
	{
		sgm::Circular_Queue<Specimen> qu(5);

		sgm::spec::is_True(qu.is_empty());
	}
}


void Circular_Queue_Contents::Push_and_Pop()
{
	{
		sgm::Circular_Queue<Specimen> qu(5);

		qu.push( Specimen(1) );
		qu.push( Specimen(3) );

		sgm::spec::is_True(qu.size() == 2 && qu.front() == 1 && qu.back() == 3);

		qu.push( Specimen(5) );

		sgm::spec::is_True(qu.size() == 3 && qu.front() == 1 && qu.back() == 5);

		qu.pop();

		sgm::spec::is_True(qu.size() == 2 && qu.front() == 3 && qu.back() == 5);

		qu.pop();

		sgm::spec::is_True(qu.size() == 1 && qu.front() == 5 && qu.back() == 5);
	}
}


void Circular_Queue_Contents::Clear()
{
	{
		sgm::Circular_Queue<Specimen> qu(5);

		qu.push( Specimen(1) );
		qu.push( Specimen(3) );
		qu.push( Specimen(5) );

		sgm::spec::is_True(qu.size() == 3 && qu.front() == 1 && qu.back() == 5);

		qu.clear();

		sgm::spec::is_True(qu.is_empty());
	}
}


namespace sgm
{
	namespace _Test_Circular_Queue_Allocator_detail
	{

		template<class T>
		class Test_Allocator : public Allocator<T>
		{
		private:
			using _base_t = Allocator<T>;


		public:
			Test_Allocator(T* buffer_arr) : _buffer_arr(buffer_arr), _idx(0){}

			auto allocate(size_t)-> T*{  return _buffer_arr + _idx;  }

			void deallocate(T*, size_t){}

			template<class Q, class...ARGS>
			void construct(Q* p, ARGS&&...args)
			{
				_base_t::construct( p, Forward<ARGS&&>(args)... );

				++_idx;
			}

			template<class Q>
			void destroy(Q* p){  p->~Q();  }


		private:
			T* _buffer_arr;
			size_t _idx;
		};

	}
}


void Circular_Queue_Contents::Construction_by_Allocator()
{
	{
		std::size_t constexpr byte_size_v = sizeof(Specimen);
		std::size_t constexpr max_nof_buffer_v = 10;

		unsigned char buffer[byte_size_v * max_nof_buffer_v] = {0, };
		auto buffer_arr = reinterpret_cast<Specimen*>(buffer);
		
		auto qu 
		=	sgm::Circular_Queue<Specimen>::by
			(	sgm::_Test_Circular_Queue_Allocator_detail::Test_Allocator<Specimen>(buffer_arr)
			);

		sgm::spec::is_True(qu.is_empty());
	}
}


void Circular_Queue_Contents::Push_and_Pop_by_Allocator()
{
	{
		std::size_t constexpr byte_size_v = sizeof(Specimen);
		std::size_t constexpr max_nof_buffer_v = 10;

		unsigned char buffer[byte_size_v * max_nof_buffer_v] = {0, };
		auto buffer_arr = reinterpret_cast<Specimen*>(buffer);
		
		auto qu 
		=	sgm::Circular_Queue<Specimen>::by
			(	sgm::_Test_Circular_Queue_Allocator_detail::Test_Allocator<Specimen>(buffer_arr)
			);

		qu.push( Specimen(1) );
		qu.push( Specimen(3) );

		sgm::spec::is_True
		(	qu.size() == 2 && qu.front() == 1 && qu.back() == 3
		&&	buffer_arr[0].value == 1 && buffer_arr[1].value == 3
		);

		qu.push( Specimen(5) );

		sgm::spec::is_True
		(	qu.size() == 3 && qu.front() == 1 && qu.back() == 5
		&&	buffer_arr[0].value == 1 && buffer_arr[1].value == 3 && buffer_arr[2].value == 5
		);

		qu.pop();

		sgm::spec::is_True
		(	qu.size() == 2 && qu.front() == 3 && qu.back() == 5
		&&	buffer_arr[0].value == Specimen::State::DESTRUCTION 
		&&	buffer_arr[1].value == 3
		&&	buffer_arr[2].value == 5
		);

		qu.pop();

		sgm::spec::is_True
		(	qu.size() == 1 && qu.front() == 5 && qu.back() == 5
		&&	buffer_arr[0].value == Specimen::State::DESTRUCTION 
		&&	buffer_arr[1].value == Specimen::State::DESTRUCTION
		&&	buffer_arr[2].value == 5
		);
	}
}
#endif
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_SPECIFICATION_TEST(sgm::spec::Test_, Queue, /**/)
{	::Queue_Contents::Constructions
,	::Queue_Contents::Push_and_Pop
,	::Queue_Contents::Clear
,	::Queue_Contents::Construction_by_Allocator
,	::Queue_Contents::Push_and_Pop_by_Allocator
//,	::Circular_Queue_Contents::Constructions
//,	::Circular_Queue_Contents::Push_and_Pop
//,	::Circular_Queue_Contents::Clear
//,	::Circular_Queue_Contents::Construction_by_Allocator
//,	::Circular_Queue_Contents::Push_and_Pop_by_Allocator
};
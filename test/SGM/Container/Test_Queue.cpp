/*  SPDX-FileCopyrightText: (c) 2022 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "SGM/Container/Queue.hpp"
#include "Test_Queue.hpp"


using sgm::h2u::Specimen;


struct Queue_Contents : sgm::Unconstructible
{
	static void Construction();
	static void Push_and_Pop();
	static void Clear();
	static void Construction_by_Allocator();
	static void Push_and_Pop_by_Allocator();
};


void Queue_Contents::Construction()
{
	using namespace sgm::h2u;

	mdo << Title(L"Basic Queue Construction");
	
	mdo 
	<<	L"A Queue can be constructed using its default constructor, which creates an empty "
	<<	L"container. The newly constructed queue should report itself as empty through the "
	<<	L"is_empty() method." << newl;

	sgm::Queue<Specimen> qu;

	SGM_H2U_ASSERT(qu.is_empty());
}


void Queue_Contents::Push_and_Pop()
{
	using namespace sgm::h2u;

	mdo << Title(L"FIFO Operations");
	
	mdo 
	<<	L"The Queue implements First-In-First-Out (FIFO) semantics where elements are added "
	<<	L"at the back and removed from the front. This demonstration shows how push() adds "
	<<	L"elements sequentially while pop() removes them in the same order they were inserted."
	<<	newl;

BEGIN_CODE_BLOCK(queue_basic_usage)
	sgm::Queue<Specimen> qu;

	qu.push( Specimen(1) );
	qu.push( Specimen(3) );

	SGM_H2U_ASSERT(qu.size() == 2 && qu.front() == 1 && qu.back() == 3);

	qu.push( Specimen(5) );

	SGM_H2U_ASSERT(qu.size() == 3 && qu.front() == 1 && qu.back() == 5);

	qu.pop();

	SGM_H2U_ASSERT(qu.size() == 2 && qu.front() == 3 && qu.back() == 5);

	qu.pop();

	SGM_H2U_ASSERT(qu.size() == 1 && qu.front() == 5 && qu.back() == 5);
END_CODE_BLOCK_AND_LOAD(queue_basic_usage)
}


void Queue_Contents::Clear()
{
	using namespace sgm::h2u;

	mdo << Title(L"Queue Clearing");

	mdo 
	<<	L"The clear() operation provides an efficient way to remove all elements from the "
	<<	L"queue at once. After clearing, the queue returns to its initial empty state." 
	<<	newl;

	sgm::Queue<Specimen> qu;

	qu.push( Specimen(1) );
	qu.push( Specimen(3) );
	qu.push( Specimen(5) );

	SGM_H2U_ASSERT(qu.size() == 3 && qu.front() == 1 && qu.back() == 5);

	qu.clear();

	SGM_H2U_ASSERT(qu.is_empty());
}


BEGIN_CODE_BLOCK(queue_test_allocator_decl)
namespace sgm
{
	namespace _Test_Queue_Allocator_detail
	{

		// Custom allocator for Queue testing using pre-allocated List_Node buffer
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
END_CODE_BLOCK(queue_test_allocator_decl)


void Queue_Contents::Construction_by_Allocator()
{
	using namespace sgm::h2u;

	mdo << Title(L"Custom Allocator Construction");

	mdo
	<<	L"Queues can be constructed with custom allocators to control memory allocation "
	<<	L"behavior. This is particularly useful in embedded systems or real-time applications "
	<<	L"where dynamic allocation must be avoided. Since our Queue implementation is built "
	<<	L"upon sgm::List<T>, custom allocators must allocate List_Node<T> objects rather than "
	<<	L"the element type T directly. This is a fundamental requirement for any Queue allocator."
	<<	newl
	<<	Load_code_block(L"queue_test_allocator_decl")
	<<	newl;

BEGIN_CODE_BLOCK(queue_custom_allocator)
	std::size_t constexpr list_node_byte_size_v = sizeof(sgm::List_Node<Specimen>);
	std::size_t constexpr max_nof_node_in_buffer_v = 10;

	wchar_t buffer[list_node_byte_size_v * max_nof_node_in_buffer_v] = {0, };
	auto node_arr = reinterpret_cast< sgm::List_Node<Specimen>* >(buffer);

	auto qu
	=	sgm::Queue<Specimen>::by
		(	sgm::_Test_Queue_Allocator_detail::Test_Allocator<Specimen>(node_arr)
		);

	SGM_H2U_ASSERT(qu.is_empty());
END_CODE_BLOCK_AND_LOAD(queue_custom_allocator)
}


void Queue_Contents::Push_and_Pop_by_Allocator()
{
	using namespace sgm::h2u;

	mdo << Title(L"Custom Allocator Operations", 2);

	mdo 
	<<	L"This test demonstrates how queues with custom allocators behave during normal "
	<<	L"operations. We can verify that elements are stored in our pre-allocated buffer "
	<<	L"and that destructors are properly called when elements are removed." 
	<<	newl;

	std::size_t constexpr list_node_byte_size_v = sizeof(sgm::List_Node<Specimen>);
	std::size_t constexpr max_nof_node_in_buffer_v = 10;

	wchar_t buffer[list_node_byte_size_v * max_nof_node_in_buffer_v] = {0, };
	auto node_arr = reinterpret_cast< sgm::List_Node<Specimen>* >(buffer);

	auto qu
	=	sgm::Queue<Specimen>::by
		(	sgm::_Test_Queue_Allocator_detail::Test_Allocator<Specimen>(node_arr)
		);

	qu.push( Specimen(1) );
	qu.push( Specimen(3) );

	SGM_H2U_ASSERT
	(	qu.size() == 2 && qu.front() == 1 && qu.back() == 3
	&&	node_arr[0].value == 1 && node_arr[1].value == 3
	);

	qu.push( Specimen(5) );

	SGM_H2U_ASSERT
	(	qu.size() == 3 && qu.front() == 1 && qu.back() == 5
	&&	node_arr[0].value == 1 && node_arr[1].value == 3 && node_arr[2].value == 5
	);

	qu.pop();

	SGM_H2U_ASSERT
	(	qu.size() == 2 && qu.front() == 3 && qu.back() == 5
	&&	node_arr[0].value == Specimen::State::DESTRUCTION
	&&	node_arr[1].value == 3
	&&	node_arr[2].value == 5
	);

	qu.pop();

	SGM_H2U_ASSERT
	(	qu.size() == 1 && qu.front() == 5 && qu.back() == 5
	&&	node_arr[0].value == Specimen::State::DESTRUCTION
	&&	node_arr[1].value == Specimen::State::DESTRUCTION
	&&	node_arr[2].value == 5
	);
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


struct Circular_Queue_Contents : sgm::Unconstructible
{
	static void Construction();
	static void Push_and_Pop();
	static void Push_and_Pop2();
	static void Copy_Construction();
	static void Move_Construction();
	static void Copy_Assignment();
	static void Move_Assignment();
	static void Clear();
	static void Construction_by_Allocator();
	static void Push_and_Pop_by_Allocator();
};


void Circular_Queue_Contents::Construction()
{
	using namespace sgm::h2u;

	mdo << Title(L"Circular Queue Construction");

	mdo
	<<	L"Unlike the standard Queue, Circular_Queue requires a fixed capacity to be specified "
	<<	L"during construction. This capacity determines the maximum number of elements the "
	<<	L"queue can hold before it becomes full. Built on arrays rather than linked lists, "
	<<	L"Circular_Queue offers superior memory efficiency through contiguous storage and "
	<<	L"enhanced iteration performance due to better cache locality."
	<<	newl;

	sgm::Circular_Queue<Specimen> qu(5);

	SGM_H2U_ASSERT(qu.is_empty());
}


void Circular_Queue_Contents::Push_and_Pop()
{
	using namespace sgm::h2u;

	mdo << Title(L"Circular Queue FIFO Operations");

	mdo 
	<<	L"Circular queues maintain the same FIFO semantics as standard queues but operate "
	<<	L"within a fixed-size buffer. This example shows the basic push and pop operations "
	<<	L"while staying within the capacity limits." 
	<<	newl;

BEGIN_CODE_BLOCK(circular_queue_basic)
	sgm::Circular_Queue<Specimen> qu(5);

	qu.push( Specimen(1) );
	qu.push( Specimen(3) );

	SGM_H2U_ASSERT(qu.size() == 2 && qu.front() == 1 && qu.back() == 3);

	qu.push( Specimen(5) );

	SGM_H2U_ASSERT(qu.size() == 3 && qu.front() == 1 && qu.back() == 5);

	qu.pop();

	SGM_H2U_ASSERT(qu.size() == 2 && qu.front() == 3 && qu.back() == 5);

	qu.pop();

	SGM_H2U_ASSERT(qu.size() == 1 && qu.front() == 5 && qu.back() == 5);
END_CODE_BLOCK_AND_LOAD(circular_queue_basic)
}


void Circular_Queue_Contents::Push_and_Pop2()
{
	using namespace sgm::h2u;

	mdo << Title(L"Advanced Circular Buffer Operations", 2);

	mdo 
	<<	L"This test explores the circular nature of the buffer when it reaches capacity. "
	<<	L"It also demonstrates method chaining, where operations can be linked together for "
	<<	L"more concise code. Watch how the queue wraps around when space becomes available."
	<<	newl;

BEGIN_CODE_BLOCK(circular_queue_chaining)
	sgm::Circular_Queue<Specimen> qu(3);

	qu.push(1).push(3).push(5);

	SGM_H2U_ASSERT(qu.size() == 3 && qu.is_full());

	qu.pop().pop();

	SGM_H2U_ASSERT(qu.size() == 1 && qu.front() == 5);

	qu.push(7).push(9);

	SGM_H2U_ASSERT
	(	qu.size() == 3 && qu.is_full() && qu.front() == 5 && qu.back() == 9
	);

	qu.pop();

	SGM_H2U_ASSERT
	(	qu.size() == 2 && qu.front() == 7 && qu.back() == 9
	);
END_CODE_BLOCK_AND_LOAD(circular_queue_chaining)
}


void Circular_Queue_Contents::Copy_Construction()
{
	using namespace sgm::h2u;

	mdo << Title(L"Copy Semantics");

	mdo 
	<<	L"Circular queues support standard copy construction, which creates an independent "
	<<	L"copy with the same capacity and current state. The copied queue can be manipulated "
	<<	L"without affecting the original." 
	<<	newl;

	sgm::Circular_Queue<Specimen> qu(5);

	qu.push( Specimen(1) );
	qu.push( Specimen(3) );
	qu.push( Specimen(5) );
	qu.pop();

	sgm::Circular_Queue<Specimen> qu2 = qu;

	SGM_H2U_ASSERT(qu2.max_size() == 5 && qu2.size() == 3 - 1);
	SGM_H2U_ASSERT(qu2.front() == 3);

	qu2.pop();

	SGM_H2U_ASSERT(qu2.size() == 3 - 2);
	SGM_H2U_ASSERT(qu2.front() == 5);

	qu2.pop();

	SGM_H2U_ASSERT(qu2.is_empty());

	SGM_H2U_ASSERT
	(	qu.size() == 3 - 1
	&&	qu.front() == 3 && qu.back() == 5
	);
}


void Circular_Queue_Contents::Move_Construction()
{
	using namespace sgm::h2u;

	mdo << Title(L"Move Semantics", 2);

	mdo 
	<<	L"Move construction provides efficient ownership transfer by taking control of the "
	<<	L"source queue's resources. The moved-from queue is left in a valid but empty state "
	<<	L"with zero capacity." 
	<<	newl;

	sgm::Circular_Queue<Specimen> qu(5);

	qu.push( Specimen(1) );
	qu.push( Specimen(3) );
	qu.push( Specimen(5) );
	qu.pop();

	sgm::Circular_Queue<Specimen> qu2 = sgm::Move(qu);

	SGM_H2U_ASSERT(qu2.max_size() == 5 && qu2.size() == 3 - 1);
	SGM_H2U_ASSERT(qu2.front() == 3);

	qu2.pop();

	SGM_H2U_ASSERT(qu2.size() == 3 - 2);
	SGM_H2U_ASSERT(qu2.front() == 5);

	qu2.pop();

	SGM_H2U_ASSERT(qu2.is_empty());

	SGM_H2U_ASSERT
	(	qu.is_empty() && qu.max_size() == 0
	);
}


void Circular_Queue_Contents::Copy_Assignment()
{
	using namespace sgm::h2u;

	mdo << Title(L"Copy Assignment", 2);

	mdo 
	<<	L"Copy assignment allows replacing an existing queue's contents with those of "
	<<	L"another. This operation updates both capacity and current elements, making the "
	<<	L"target queue an independent copy of the source." 
	<<	newl;

	sgm::Circular_Queue<Specimen> qu(5), qu2(1);

	qu.push( Specimen(1) );
	qu.push( Specimen(3) );
	qu.push( Specimen(5) );
	qu.pop();

	qu2 = qu;

	SGM_H2U_ASSERT(qu2.max_size() == 5 && qu2.size() == 3 - 1);
	SGM_H2U_ASSERT(qu2.front() == 3);

	qu2.pop();

	SGM_H2U_ASSERT(qu2.size() == 3 - 2);
	SGM_H2U_ASSERT(qu2.front() == 5);

	qu2.pop();

	SGM_H2U_ASSERT(qu2.is_empty());

	SGM_H2U_ASSERT
	(	qu.size() == 3 - 1
	&&	qu.front() == 3 && qu.back() == 5
	);
}


void Circular_Queue_Contents::Move_Assignment()
{
	using namespace sgm::h2u;

	mdo << Title(L"Move Assignment", 2);

	mdo 
	<<	L"Move assignment efficiently transfers ownership from one queue to another without "
	<<	L"copying the actual elements. The source queue becomes empty while the target gains "
	<<	L"all resources." 
	<<	newl;

	sgm::Circular_Queue<Specimen> qu(5), qu2(1);

	qu.push( Specimen(1) );
	qu.push( Specimen(3) );
	qu.push( Specimen(5) );
	qu.pop();

	qu2 = sgm::Move(qu);

	SGM_H2U_ASSERT(qu2.max_size() == 5 && qu2.size() == 3 - 1);
	SGM_H2U_ASSERT(qu2.front() == 3);

	qu2.pop();

	SGM_H2U_ASSERT(qu2.size() == 3 - 2);
	SGM_H2U_ASSERT(qu2.front() == 5);

	qu2.pop();

	SGM_H2U_ASSERT(qu2.is_empty());

	SGM_H2U_ASSERT
	(	qu.is_empty() && qu.max_size() == 0
	);
}


void Circular_Queue_Contents::Clear()
{
	using namespace sgm::h2u;

	mdo << Title(L"Circular Queue Clearing");

	mdo 
	<<	L"The clear operation removes all elements while preserving the queue's original "
	<<	L"capacity. This allows the queue to be reused without requiring reconstruction."
	<<	newl;

	sgm::Circular_Queue<Specimen> qu(3);

	SGM_H2U_ASSERT(qu.max_size() == 3);

	qu.push( Specimen(1) );
	qu.push( Specimen(3) );
	qu.push( Specimen(5) );

	SGM_H2U_ASSERT
	(	qu.size() == 3
	&&	qu.front() == 1 && qu.back() == 5
	&&	qu.is_full()
	);

	qu.clear();

	SGM_H2U_ASSERT(qu.is_empty());
}


BEGIN_CODE_BLOCK(circular_queue_test_allocator_decl)
namespace sgm
{
	namespace _Test_Circular_Queue_Allocator_detail
	{

		// Custom allocator for Circular_Queue testing using pre-allocated element buffer
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
END_CODE_BLOCK(circular_queue_test_allocator_decl)


void Circular_Queue_Contents::Construction_by_Allocator()
{
	using namespace sgm::h2u;

	mdo << Title(L"Circular Queue with Custom Allocator");

	mdo 
	<<	L"Circular queues can also utilize custom allocators, but unlike the standard queue, "
	<<	L"they allocate elements directly rather than list nodes. This provides more "
	<<	L"predictable memory layout and access patterns." 
	<<	newl
	<<	Load_code_block(L"circular_queue_test_allocator_decl")
	<<	newl;

BEGIN_CODE_BLOCK(circular_queue_custom_allocator)
	std::size_t constexpr byte_size_v = sizeof(Specimen);
	std::size_t constexpr max_nof_buffer_v = 10;

	wchar_t buffer[byte_size_v * max_nof_buffer_v] = {0, };
	auto buffer_arr = reinterpret_cast<Specimen*>(buffer);

	auto qu
	=	sgm::Circular_Queue<Specimen>::by
		(	sgm::_Test_Circular_Queue_Allocator_detail::Test_Allocator<Specimen>(buffer_arr)
		,	5
		);

	SGM_H2U_ASSERT(qu.is_empty());
END_CODE_BLOCK_AND_LOAD(circular_queue_custom_allocator)
}


void Circular_Queue_Contents::Push_and_Pop_by_Allocator()
{
	using namespace sgm::h2u;

	mdo << Title(L"Custom Allocator Operations with Verification", 2);

	mdo
	<<	L"This comprehensive test validates that circular queues properly manage elements "
	<<	L"within the custom-allocated buffer. By examining the buffer contents directly, we "
	<<	L"can confirm that destructors are called and memory is properly managed."
	<<	newl;

BEGIN_CODE_BLOCK(circular_queue_custom_allocator_2)
	std::size_t constexpr byte_size_v = sizeof(Specimen);
	std::size_t constexpr max_nof_buffer_v = 10;

	wchar_t buffer[byte_size_v * max_nof_buffer_v] = {0, };
	auto buffer_arr = reinterpret_cast<Specimen*>(buffer);

	auto qu
	=	sgm::Circular_Queue<Specimen>::by
		(	sgm::_Test_Circular_Queue_Allocator_detail::Test_Allocator<Specimen>(buffer_arr)
		,	5
		);

	qu.push( Specimen(1) );
	qu.push( Specimen(3) );

	SGM_H2U_ASSERT
	(	qu.size() == 2 && qu.front() == 1 && qu.back() == 3
	&&	buffer_arr[0] == 1 && buffer_arr[1] == 3
	);

	qu.push( Specimen(5) );

	SGM_H2U_ASSERT
	(	qu.size() == 3 && qu.front() == 1 && qu.back() == 5
	&&	buffer_arr[0] == 1
	&&	buffer_arr[1] == 3
	&&	buffer_arr[2] == 5
	);

	qu.pop();

	SGM_H2U_ASSERT
	(	qu.size() == 2 && qu.front() == 3 && qu.back() == 5
	&&	buffer_arr[0] == Specimen::State::DESTRUCTION
	&&	buffer_arr[1] == 3
	&&	buffer_arr[2] == 5
	);

	qu.pop();

	SGM_H2U_ASSERT
	(	qu.size() == 1 && qu.front() == 5 && qu.back() == 5
	&&	buffer_arr[0] == Specimen::State::DESTRUCTION
	&&	buffer_arr[1] == Specimen::State::DESTRUCTION
	&&	buffer_arr[2] == 5
	);
END_CODE_BLOCK_AND_LOAD(circular_queue_custom_allocator_2)
}
//========//========//========//========//=======#//========//========//========//========//=======#


struct Performance : sgm::Unconstructible
{
	static void Push_and_Pop();
};


#include <queue>
#include <chrono>


void Performance::Push_and_Pop()
{
#if 0
	using namespace std::chrono;
	
	int constexpr N = 100'000'000;

	{
		sgm::Queue<sgm::h2u::Specimen> qu;

		std::wcout << "\tsgm::Queue\n";

		{
			std::wcout << "\t\tpush : ";

			auto const start_tp = system_clock::now();

			for( int i = 0;  i < N;  qu.push(i++) );

			auto const time = system_clock::now() - start_tp;

			std::wcout << duration_cast<milliseconds>(time).count() << " millisec.\n";
		}
		{
			std::wcout << "\t\tpop : ";

			auto const start_tp = system_clock::now();

			for(int i = 0;  i < N;  qu.pop(),  ++i);

			auto const time = system_clock::now() - start_tp;

			std::wcout << duration_cast<milliseconds>(time).count() << " millisec.\n";
		}

		SGM_H2U_ASSERT(qu.is_empty());
	}	

	std::wcout << '\n';

	{
		std::wcout << "\tstd::queue\n";

		std::queue<sgm::h2u::Specimen> qu;

		{
			std::wcout << "\t\tpush : ";

			auto const start_tp = system_clock::now();

			for( int i = 0;  i < N;  qu.push(i++) );

			auto const time = system_clock::now() - start_tp;

			std::wcout << duration_cast<milliseconds>(time).count() << " millisec.\n";
		}
		{
			std::wcout << "\t\tpop : ";

			auto const start_tp = system_clock::now();

			for(int i = 0;  i < N;  qu.pop(),  ++i);

			auto const time = system_clock::now() - start_tp;

			std::wcout << duration_cast<milliseconds>(time).count() << " millisec.\n";
		}

		SGM_H2U_ASSERT(qu.empty());
	}
#endif
}


SGM_HOW2USE_TESTS(sgm::h2u::Test_, Queue, /**/)
{	::Queue_Contents::Construction
,	::Queue_Contents::Push_and_Pop
,	::Queue_Contents::Clear
,	::Queue_Contents::Construction_by_Allocator
,	::Queue_Contents::Push_and_Pop_by_Allocator
,	::Circular_Queue_Contents::Construction
,	::Circular_Queue_Contents::Push_and_Pop
,	::Circular_Queue_Contents::Push_and_Pop2
,	::Circular_Queue_Contents::Copy_Construction
,	::Circular_Queue_Contents::Move_Construction
,	::Circular_Queue_Contents::Copy_Assignment
,	::Circular_Queue_Contents::Move_Assignment
,	::Circular_Queue_Contents::Clear
,	::Circular_Queue_Contents::Construction_by_Allocator
,	::Circular_Queue_Contents::Push_and_Pop_by_Allocator
,	::Performance::Push_and_Pop
};
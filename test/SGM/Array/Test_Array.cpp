/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "SGM/Array/Array.hpp"
#include "Test_Array.hpp"
#include <vector>
#include <chrono>


using sgm::spec::Specimen;
using sgm::spec::Are_Equivalent_Ranges;
using sgm::spec::Are_All_Equivalent_to;


struct Static : sgm::Unconstructible
{
	static void Construction();
	static void No_Move_Construction();
	static void Assignment();
	static void Move_Assignment();
	static void Destruction();
	static void Element();
	static void Swap();
	static void Type_Conversion_into_iterable();
};


struct Dynamic : sgm::Unconstructible
{
	static void Construction();
	static void Copy_Construction();
	static void Move_Construction();
	static void Construction_from_immutable_elements();
	static void Assignment();
	static void Move_Assignment();
	static void Clear_and_Destruction();
	static void Element();
	static void Swap();
	static void Type_Conversion_into_iterable();
	static void Push();
	static void Pop();
};
//========//========//========//========//=======#//========//========//========//========//=======#


void Static::Construction()
{
	sgm::Array<Specimen, 3>
		arr1,
		arr2{Specimen(2), Specimen(4), Specimen(6)},
		arr3 = arr2;

	SGM_SPEC_ASSERT
	(	Are_All_Equivalent_to(arr1, Specimen::State::DEFAULT_CONSTRUCTION)
	&&	Are_All_Equivalent_to(arr2, Specimen::State::MANUAL_CONSTRUCTION)
	&&	Are_Equivalent_Ranges
		(	arr2
		,	sgm::Array<Specimen>{Specimen(2), Specimen(4), Specimen(6)} 
		)
	&&	Are_All_Equivalent_to(arr3, Specimen::State::COPY_CONSTRUCTION)
	&&	Are_Equivalent_Ranges(arr2, arr3)
	);
}


void Static::No_Move_Construction()
{
	sgm::Array<Specimen, 3> 
		_arr1{Specimen(2), Specimen(4), Specimen(6)},
		arr1 = std::move(_arr1);

	SGM_SPEC_ASSERT
	(	Are_All_Equivalent_to(arr1, Specimen::State::COPY_CONSTRUCTION)
	&&	Are_Equivalent_Ranges
		(	arr1
		,	sgm::Array<Specimen>{Specimen(2), Specimen(4), Specimen(6)} 
		)
	);
}


void Static::Assignment()
{
	sgm::Array<Specimen, 3> 
		arr1{Specimen(2), Specimen(4), Specimen(6)}, 
		arr2;

	arr2 = arr1;

	SGM_SPEC_ASSERT
	(	Are_Equivalent_Ranges(arr2, arr1)
	&&	Are_All_Equivalent_to(arr2, Specimen::State::COPY_ASSIGNMENT)
	);
	

	std::vector<Specimen> 
		vec1{Specimen(777)}, 
		vec2{Specimen(-2), Specimen(-4), Specimen(-6), Specimen(-8)};

	arr1 = vec1,  arr2 = vec2;
	
	SGM_SPEC_ASSERT
	(	arr1.size() != vec1.size()
	&&	Are_Equivalent_Ranges
		(	arr1
		,	std::vector<Specimen>{Specimen(777), Specimen(4), Specimen(6)} 
		)
	&&	arr2.size() != vec2.size()
	&&	Are_Equivalent_Ranges
		(	arr2
		,	std::vector<Specimen>{Specimen(-2), Specimen(-4), Specimen(-6)} 
		)
	);
}


void Static::Move_Assignment()
{
	sgm::Array<Specimen, 3> 
		_arr{Specimen(2), Specimen(4), Specimen(6)}, 
		arr1;

	arr1 = std::move(_arr);
	
	SGM_SPEC_ASSERT
	(	Are_Equivalent_Ranges
		(	arr1
		,	std::vector<Specimen>{Specimen(2), Specimen(4), Specimen(6)} 
		)
	&&	Are_All_Equivalent_to(arr1, Specimen::State::MOVE_ASSIGNMENT)
	&&	_arr.size() != 0
	&&	Are_All_Equivalent_to(_arr, Specimen::State::MOVE_AWAY)
	);
	

	sgm::Array<Specimen, 3> arr2{Specimen(3), Specimen(6), Specimen(9)};

	arr2 = {Specimen(-3), Specimen(-6), Specimen(-9)};
	
	SGM_SPEC_ASSERT
	(	Are_Equivalent_Ranges
		(	arr2
		,	std::vector<Specimen>{Specimen(-3), Specimen(-6), Specimen(-9)} 
		)
	);
	

	sgm::Array<Specimen, 3> arr3;
	std::vector<Specimen> 
		vec2{Specimen(777)},
		vec3{Specimen(-2), Specimen(-4), Specimen(-6), Specimen(-8)};

	arr2 = std::move(vec2),  arr3 = std::move(vec3);
	
	SGM_SPEC_ASSERT
	(	Are_Equivalent_Ranges
		(	arr2
		,	std::vector<Specimen>{Specimen(777), Specimen(-6), Specimen(-9)} 
		)
	&&	Are_Equivalent_Ranges
		(	arr3, std::vector<Specimen>{Specimen(-2), Specimen(-4), Specimen(-6)} 
		)
	&&	Are_All_Equivalent_to(arr3, Specimen::State::MOVE_ASSIGNMENT)
	);
}


void Static::Destruction()
{
	sgm::Array<Specimen, 3> arr1{Specimen(2), Specimen(4), Specimen(6)};
	
	arr1.~Array();
	
	SGM_SPEC_ASSERT
	(	arr1.size() != 0
	&&	Are_All_Equivalent_to(arr1, Specimen::State::DESTRUCTION)
	); 
}


void Static::Element()
{
	sgm::Array<Specimen, 3> arr1{Specimen(2), Specimen(4), Specimen(6)};

	SGM_SPEC_ASSERT
	(	arr1[0] == Specimen(2) && arr1[1] == Specimen(4) && arr1[2] == Specimen(6) 
	);

	arr1[1] = arr1[0],  arr1[2] = Specimen(-6);
	
	SGM_SPEC_ASSERT
	(	arr1[1] == Specimen(2) && arr1[1] == Specimen::State::COPY_ASSIGNMENT
	&&	arr1[2] == Specimen(-6) && arr1[2] == Specimen::State::MOVE_ASSIGNMENT
	);

	SGM_SPEC_ASSERT(arr1.front() == *arr1.begin() && arr1.back() == *arr1.rbegin());
}


void Static::Swap()
{
	sgm::Array<Specimen, 3> 
		arr1{Specimen(1), Specimen(3), Specimen(5)}, 
		arr2{Specimen(2), Specimen(4), Specimen(6)};

	arr1.swap(arr2);
	
	SGM_SPEC_ASSERT
	(	Are_Equivalent_Ranges
		(	arr1
		,	sgm::Array<Specimen>{Specimen(2), Specimen(4), Specimen(6)} 
		)
	&&	Are_Equivalent_Ranges
		(	arr2
		,	sgm::Array<Specimen>{Specimen(1), Specimen(3), Specimen(5)} 
		)
	);
	

	std::swap(arr1, arr2);

	SGM_SPEC_ASSERT
	(	Are_Equivalent_Ranges
		(	arr1
		,	sgm::Array<Specimen>{Specimen(1), Specimen(3), Specimen(5)} 
		)
	&&	Are_Equivalent_Ranges
		(	arr2
		,	sgm::Array<Specimen>{Specimen(2), Specimen(4), Specimen(6)} 
		)
	);
}


void Static::Type_Conversion_into_iterable()
{
	sgm::Array<Specimen, 3> 
		arr1{Specimen(1), Specimen(3), Specimen(5)}, 
		arr2{Specimen(2), Specimen(4), Specimen(6)};

	std::vector<Specimen> vec = arr1;

	SGM_SPEC_ASSERT
	(	Are_Equivalent_Ranges(vec, arr1)
	&&	Are_All_Equivalent_to(vec, Specimen::State::COPY_CONSTRUCTION)
	);
	

	vec = sgm::iterable_cast< std::vector<Specimen> >(arr2);

	SGM_SPEC_ASSERT
	(	Are_Equivalent_Ranges(vec, arr2)
	&&	Are_All_Equivalent_to(vec, Specimen::State::COPY_CONSTRUCTION)
	);
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


void Dynamic::Construction()
{
	sgm::Array<Specimen>
		arr1,
		arr2(3),
		arr3( 3, Specimen(555) ),
		arr4{Specimen(2), Specimen(4), Specimen(6)},
		arr5(arr4.begin(), arr4.end()),
		arr6(arr4.rbegin(), arr4.rend());

	SGM_SPEC_ASSERT
	(	arr1.is_null()
	&&	arr2.capacity() == 3 && arr2.is_empty()
	&&	arr3.capacity() == 3 && arr3.size() == 3 
	&&	Are_All_Equivalent_to( arr3, Specimen(555) )
	&&	Are_Equivalent_Ranges
		(	arr4
		,	std::initializer_list<Specimen>{Specimen(2), Specimen(4), Specimen(6)} 
		)
	&&	Are_Equivalent_Ranges(arr5, arr4)
	&&	Are_All_Equivalent_to(arr5, Specimen::State::COPY_CONSTRUCTION)
	&&	Are_Equivalent_Ranges
		(	arr6
		,	std::initializer_list<Specimen>{Specimen(6), Specimen(4), Specimen(2)} 
		)
	);
}


void Dynamic::Copy_Construction()
{
	std::vector<Specimen> Lvalue_iterable{Specimen(2), Specimen(4), Specimen(6)};

	sgm::Array<Specimen>
		arr1{Specimen(1), Specimen(3), Specimen(5)},
		arr2 = arr1,
		arr3 = Lvalue_iterable;

	SGM_SPEC_ASSERT
	(	Are_Equivalent_Ranges(arr2, arr1)
	&&	Are_All_Equivalent_to(arr2, Specimen::State::COPY_CONSTRUCTION)
	&&	Are_Equivalent_Ranges(arr3, Lvalue_iterable)
	&&	Are_All_Equivalent_to(arr3, Specimen::State::COPY_CONSTRUCTION)
	);
}


void Dynamic::Move_Construction()
{	
	sgm::Array<Specimen> _arr1{Specimen(1), Specimen(3), Specimen(5)};

	Specimen::State const _prev_arr1_state = _arr1.front().state();
	auto* const internal_data_address = _arr1.cdata();

	sgm::Array<Specimen>
		arr1 = std::move(_arr1),
		arr2 = std::vector<Specimen>{Specimen(2), Specimen(4), Specimen(6)};

	SGM_SPEC_ASSERT
	(	Are_All_Equivalent_to(arr1, _prev_arr1_state)
	&&	arr1.cdata() == internal_data_address
	&&	Are_Equivalent_Ranges
		(	arr1
		,	std::initializer_list<Specimen>{Specimen(1), Specimen(3), Specimen(5)}
		)
	&&	_arr1.is_null()
	&&	Are_Equivalent_Ranges
		(	arr2
		,	std::initializer_list<Specimen>{Specimen(2), Specimen(4), Specimen(6)}
		)
	&&	Are_All_Equivalent_to(arr2, Specimen::State::MOVE_CONSTRUCTION)
	);
}


void Dynamic::Construction_from_immutable_elements()
{
	struct Foo{  auto operator=(Foo const&)-> Foo& = delete;  };

	sgm::Array<Foo>{Foo{}, Foo{}};
}


void Dynamic::Assignment()
{
	std::vector<Specimen> vec{Specimen(2), Specimen(4), Specimen(6), Specimen(8)};
	sgm::Array<Specimen> arr1( 2, Specimen(1) ),  arr2( 6, Specimen(-3) );

	arr1 = vec,  arr2 = vec;

	SGM_SPEC_ASSERT
	(	Are_Equivalent_Ranges(arr1, vec)
	&&	Are_Equivalent_Ranges(arr2, vec)
	&&	Are_All_Equivalent_to(arr1, Specimen::State::COPY_CONSTRUCTION)
	&&	Are_All_Equivalent_to(arr2, Specimen::State::COPY_ASSIGNMENT)
	&&	arr2.capacity() == 6
	);
	

	sgm::Array<Specimen> arr3(6);

	arr3 = {Specimen(1), Specimen(4)};
	SGM_SPEC_ASSERT(arr3.capacity() == 6 && arr3.size() == 2);

	arr3 = vec;

	SGM_SPEC_ASSERT
	(	Are_Equivalent_Ranges(arr3, vec)
	&&	Are_N_Equivalent_to(arr3.cbegin(), 2, Specimen::State::COPY_ASSIGNMENT)
	&&	Are_N_Equivalent_to(arr3.crbegin(), 2, Specimen::State::COPY_CONSTRUCTION)
	&&	arr3.capacity() == 6
	);


	sgm::Array<Specimen> arr4(10), arr5(6);
	
	arr4 = {Specimen(3), Specimen(6)};
	arr5 = {Specimen(1), Specimen(3), Specimen(5), Specimen(7)};

	SGM_SPEC_ASSERT
	(	arr4.capacity() == 10 && arr4.size() == 2
	&&	arr5.capacity() == 6 && arr5.size() == 4
	);

	arr5 = arr4;

	SGM_SPEC_ASSERT
	(	Are_Equivalent_Ranges(arr5, arr4)
	&&	arr5.capacity() == 10 && arr5.size() == 2
	&&	Are_All_Equivalent_to(arr5, Specimen::State::COPY_CONSTRUCTION)
	);
}


void Dynamic::Move_Assignment()
{
	sgm::Array<Specimen> arr1(10), arr2(6);
	
	arr1 = {Specimen(3), Specimen(6)};
	arr2 = {Specimen(2), Specimen(4), Specimen(6), Specimen(8)};

	SGM_SPEC_ASSERT
	(	arr1.capacity() == 10 && arr1.size() == 2
	&&	arr2.capacity() == 6 && arr2.size() == 4
	);

	auto* const internal_data_address = arr1.cdata();

	arr2 = std::move(arr1);
	
	SGM_SPEC_ASSERT
	(	arr2.capacity() == 10 && arr2.size() == 2
	&&	arr2.cdata() == internal_data_address
	&&	arr1.is_null()
	);
}


void Dynamic::Clear_and_Destruction()
{
	sgm::Array<Specimen> 
		arr1{Specimen(2), Specimen(4), Specimen(6), Specimen(8)}, arr2 = arr1;

	arr1.clear(),  arr2.~Array();

	SGM_SPEC_ASSERT
	(	arr1.size() == 0 && arr1.is_empty() && arr1.capacity() == 4 && !arr1.is_null()
	&&	arr2.size() == 0 && arr2.is_empty() && arr2.capacity() == 0 && arr2.is_null()
	);
}


void Dynamic::Element()
{
	sgm::Array<Specimen> arr1{2, 4, 6};

	SGM_SPEC_ASSERT
	(	arr1[0] == Specimen(2) && arr1[1] == Specimen(4) && arr1[2] == Specimen(6) 
	);

	arr1[1] = arr1[0],  arr1[2] = Specimen(-6);

	SGM_SPEC_ASSERT
	(	arr1[1] == Specimen(2) && arr1[1] == Specimen::State::COPY_ASSIGNMENT
	&&	arr1[2] == Specimen(-6) && arr1[2] == Specimen::State::MOVE_ASSIGNMENT
	);

	SGM_SPEC_ASSERT(arr1.front() == *arr1.begin() && arr1.back() == *arr1.rbegin());
}


void Dynamic::Swap()
{
	sgm::Array<Specimen> 
		arr1{Specimen(1), Specimen(3), Specimen(5)}, 
		arr2{Specimen(2), Specimen(4)};

	auto
		*const internal_data_address1 = arr1.cdata(),
		*const internal_data_address2 = arr2.cdata();

	arr1.swap(arr2);

	SGM_SPEC_ASSERT
	(	Are_Equivalent_Ranges
		(	arr1
		,	sgm::Array<Specimen>{Specimen(2), Specimen(4)} 
		)
	&&	Are_Equivalent_Ranges
		(	arr2
		,	sgm::Array<Specimen>{Specimen(1), Specimen(3), Specimen(5)} 
		)
	&&	arr1.cdata() == internal_data_address2 && arr2.cdata() == internal_data_address1
	);

	std::swap(arr1, arr2);
	
	SGM_SPEC_ASSERT
	(	arr1.cdata() == internal_data_address1 && arr2.cdata() == internal_data_address2
	&&	Are_Equivalent_Ranges
		(	arr1
		,	sgm::Array<Specimen>{Specimen(1), Specimen(3), Specimen(5)} 
		)
	&&	Are_Equivalent_Ranges
		(	arr2
		,	sgm::Array<Specimen>{Specimen(2), Specimen(4)} 
		)
	);	
}


void Dynamic::Type_Conversion_into_iterable()
{
	sgm::Array<Specimen> 
		arr1{Specimen(1), Specimen(3), Specimen(5)}, 
		arr2{Specimen(2), Specimen(4), Specimen(6)};
	
	std::vector<Specimen> vec = arr1;

	SGM_SPEC_ASSERT
	(	Are_Equivalent_Ranges(vec, arr1)
	&&	Are_All_Equivalent_to(vec, Specimen::State::COPY_CONSTRUCTION)
	);

	vec = sgm::iterable_cast< std::vector<Specimen> >(arr2);
	
	SGM_SPEC_ASSERT
	(	Are_Equivalent_Ranges(vec, arr2)
	&&	Are_All_Equivalent_to(vec, Specimen::State::COPY_CONSTRUCTION)
	);
}


void Dynamic::Push()
{
	sgm::Array<Specimen> arr1(100);
	auto current_size = arr1.size(); 

	SGM_SPEC_ASSERT(current_size == 0);

	arr1.emplace_back(3);

	SGM_SPEC_ASSERT
	(	arr1.size() == (current_size += 1)
	&&	arr1.back() == Specimen(3) && arr1.back() == Specimen::State::MANUAL_CONSTRUCTION 
	);


	Specimen val = Specimen(100);

	arr1.emplace_back(5).emplace_back(7).emplace_back(val).emplace_back( Specimen(99) );

	SGM_SPEC_ASSERT
	(	Are_Equivalent_Ranges
		(	arr1
		,	std::vector<Specimen>{Specimen(3), Specimen(5), Specimen(7), val, Specimen(99)}
		)
	);

	SGM_SPEC_ASSERT
	(	arr1.size() == (current_size += 4)
	&&	arr1.rbegin()[0] == Specimen::State::MOVE_CONSTRUCTION
	&&	arr1.rbegin()[1] == Specimen::State::COPY_CONSTRUCTION
	&&	arr1.rbegin()[2] == Specimen::State::MANUAL_CONSTRUCTION
	);

	arr1 >> val;

	SGM_SPEC_ASSERT
	(	arr1.size() == (current_size += 1)
	&&	arr1.back() == val
	);

	arr1 >> Specimen(-1) >> val;

	SGM_SPEC_ASSERT
	(	arr1.size() == (current_size += 2)
	&&	arr1.rbegin()[0] == val && arr1.rbegin()[0] == Specimen::State::COPY_CONSTRUCTION
	&&	arr1.rbegin()[1] == -1 && arr1.rbegin()[1] == Specimen::State::MOVE_CONSTRUCTION
	);


	sgm::Array<Specimen> 
		arr2{Specimen(-1), Specimen(-3), Specimen(-5), Specimen(-7)},
		arr3{Specimen(10), Specimen(20)};

	arr1.merge_back(arr2.begin(), arr2.end());

	SGM_SPEC_ASSERT
	(	arr1.size() == (current_size += arr2.size())
	&&	arr1.rbegin()[0] == -7 && arr1.rbegin()[0] == Specimen::State::COPY_CONSTRUCTION
	&&	arr1.rbegin()[1] == -5 && arr1.rbegin()[1] == Specimen::State::COPY_CONSTRUCTION
	&&	arr1.rbegin()[2] == -3 && arr1.rbegin()[2] == Specimen::State::COPY_CONSTRUCTION
	&&	arr1.rbegin()[3] == -1 && arr1.rbegin()[3] == Specimen::State::COPY_CONSTRUCTION
	);

	arr1.merge_back(arr3.begin(), arr3.end()).merge_back(arr3.rbegin(), arr3.rend());

	SGM_SPEC_ASSERT
	(	arr1.size() == (current_size += 2*arr3.size())
	&&	arr1.rbegin()[0] == 10 && arr1.rbegin()[0] == Specimen::State::COPY_CONSTRUCTION
	&&	arr1.rbegin()[1] == 20 && arr1.rbegin()[1] == Specimen::State::COPY_CONSTRUCTION
	&&	arr1.rbegin()[2] == 20 && arr1.rbegin()[2] == Specimen::State::COPY_CONSTRUCTION
	&&	arr1.rbegin()[3] == 10 && arr1.rbegin()[3] == Specimen::State::COPY_CONSTRUCTION
	);
}


void Dynamic::Pop()
{
	size_t const capa = 100;

	sgm::Array<Specimen> arr1(capa);
	auto fn = [](size_t const x)-> int{  return 3*static_cast<int>(x) + 1;  };

	for(size_t n = 0;  n < arr1.capacity();  arr1 >> fn(n + 1),  ++n);


	size_t current_size = arr1.size();

	SGM_SPEC_ASSERT(current_size == capa);

	arr1.pop_back();

	SGM_SPEC_ASSERT
	(	arr1.size() == (current_size -= 1) 
	&&	arr1.back() == fn(current_size)
	);

	arr1.pop_back().pop_back();

	SGM_SPEC_ASSERT
	(	arr1.size() == (current_size -= 2) 
	&&	arr1.back() == fn(current_size)
	);

	arr1.pop_back(3);

	SGM_SPEC_ASSERT
	(	arr1.size() == (current_size -= 3) 
	&&	arr1.back() == fn(current_size)
	);

	arr1.pop_back(3).pop_back(2).pop_back(1);

	SGM_SPEC_ASSERT
	(	arr1.size() == (current_size -= 3 + 2 + 1) 
	&&	arr1.back() == fn(current_size)
	);


	auto itr = arr1.end() - 4;

	arr1.pop_back_from(itr);

	SGM_SPEC_ASSERT
	(	arr1.size() == (current_size -= 4) 
	&&	arr1.back() == fn(current_size)
	);


	auto 
		itr1 = arr1.end() - 2, 
		itr2 = itr1 - 3, 
		itr3 = itr2 - 1;

	arr1.pop_back_from(itr1).pop_back_from(itr2).pop_back_from(itr3);

	SGM_SPEC_ASSERT
	(	arr1.size() == (current_size -= 2 + 3 + 1) 
	&&	arr1.back() == fn(current_size)
	);

	arr1.clear();

	SGM_SPEC_ASSERT(arr1.is_empty() && arr1.capacity() == capa);
}
//========//========//========//========//=======#//========//========//========//========//=======#


struct Performance : sgm::Unconstructible
{
	static void insertion();
	static void random_access();
};


void Performance::insertion()
{
#if 0
	std::size_t const N = 100'000'000;

	std::wcout << "insertion test\n";
	
	{
		std::vector<sgm::spec::Specimen> vec;

		vec.reserve(N);
		
		for(size_t n = 10;  n-->0;)
		{
			{
				std::wcout << "\tstd::vector emplace_back : ";
				
				auto start_tp = std::chrono::system_clock::now();

				for(std::size_t i = 0;  i < N;  ++i)
					vec.emplace_back( static_cast<int>(i) );

				auto time = (std::chrono::system_clock::now() - start_tp).count();

				std::wcout << time << " nanosec\n";
			}
			{
				std::wcout << "\tstd::vector pop_back : ";
				auto start_tp = std::chrono::system_clock::now();

				for(std::size_t i = 0;  i < N;  ++i)
					vec.pop_back();

				auto time = (std::chrono::system_clock::now() - start_tp).count();

				std::wcout << time << " nanosec\n";
			}
		}
	}
	{
		sgm::Array<sgm::spec::Specimen> arr(N);
		
		for(size_t n = 10; n-->0;)
		{
			{
				std::wcout << "\tsgm::Array emplace_back : ";
				auto start_tp = std::chrono::system_clock::now();

				for(std::size_t i = 0;  i < N;  ++i)
					arr.emplace_back( static_cast<int>(i) );

				auto time = (std::chrono::system_clock::now() - start_tp).count();

				std::wcout << time << " nanosec.\n";
			}
			{
				std::wcout << "\tsgm::Array pop_back : ";
				auto start_tp = std::chrono::system_clock::now();

				arr.pop_back(N);

				auto time = (std::chrono::system_clock::now() - start_tp).count();

				std::wcout << time << " nanosec\n";
			}
		}
	}
#endif
}


void Performance::random_access()
{
#if 0
	std::size_t const N = 1'000'000;

	std::wcout << "random access test\n";
	
	{
		std::vector<sgm::spec::Specimen> vec;
		sgm::spec::Specimen s;

		vec.reserve(N);

		for(std::size_t i = 0;  i < N;  ++i)
			vec.emplace_back( static_cast<int>(i) );

		std::wcout << "\tstd::vector random_access : ";

		auto start_tp = std::chrono::system_clock::now();
		
		for(std::size_t n = 10'000;  n-->0; )
			for(std::size_t i = 0;  i < N;  ++i)
				s = vec[i];
		
		auto time 
		=	std::chrono::duration_cast<std::chrono::milliseconds>
			(	std::chrono::system_clock::now() - start_tp
			) .	count();

		std::wcout << time << " millisec\n";		
	}
	{
		sgm::Array<sgm::spec::Specimen> arr(N);
		sgm::spec::Specimen s;

		for(std::size_t i = 0;  i < N;  ++i)
			arr.emplace_back( static_cast<int>(i) );

		std::wcout << "\tsgm::Array random_access : ";

		auto start_tp = std::chrono::system_clock::now();
		
		for(std::size_t n = 10'000;  n-->0; )
			for(std::size_t i = 0;  i < N;  ++i)
				s = arr[i];
			
		auto time 
		=	std::chrono::duration_cast<std::chrono::milliseconds>
			(	std::chrono::system_clock::now() - start_tp
			) .	count();

		std::wcout << time << " millisec\n";		
	}
#endif
}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_SPECIFICATION_TEST(sgm::spec::Test_, Array, /**/)
{	::Static::Construction
,	::Static::No_Move_Construction
,	::Static::Assignment
,	::Static::Move_Assignment
,	::Static::Destruction
,	::Static::Element
,	::Static::Swap
,	::Static::Type_Conversion_into_iterable
,	::Dynamic::Construction
,	::Dynamic::Copy_Construction
,	::Dynamic::Move_Construction
,	::Dynamic::Construction_from_immutable_elements
,	::Dynamic::Assignment
,	::Dynamic::Move_Assignment
,	::Dynamic::Clear_and_Destruction
,	::Dynamic::Element
,	::Dynamic::Swap
,	::Dynamic::Type_Conversion_into_iterable
,	::Dynamic::Push
,	::Dynamic::Pop
,	::Performance::insertion
,	::Performance::random_access
};
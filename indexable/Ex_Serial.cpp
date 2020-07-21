#include "Serial.hpp"
#include <vector>
//========//========//========//========//=======#//========//========//========//========//=======#


using namespace sgm;


struct Tutorial : No_Making
{
private:
	template
	<	class CON1, class CON2
	,	class = std::enable_if_t< is_iterable<CON1>::value && is_iterable<CON2>::value >
	>
	static bool have_the_same(CON1&& con1, CON2&& con2)
	{
		auto itr1 = con1.begin();
		auto itr2 = con2.begin();

		bool res = con1.size() == con2.size();

		while (itr1 != con1.end() && res)
			res = *itr1++ == *itr2++;

		return res;
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	template<  class ITR, class T, class = std::enable_if_t< is_iterator<ITR>::value >  >
	static bool are_all_same_to(ITR const bi, size_t const n, T const& t)
	{
		return
		(	n == 0
		||	*bi == t && are_all_same_to( std::next(bi), n - 1, t )
		);
	}


	template< class CON, class T, class = std::enable_if_t< is_iterable<CON>::value >  >
	static bool are_all_same_to(CON const& con, T const& t)
	{
		return are_all_same_to(con.begin(), con.size(), t);
	}


	template<  class ITR, class T, class = std::enable_if_t< is_iterator<ITR>::value >  >
	static bool are_all_same_to(ITR const bi, ITR const ei, T const& t)
	{
		return
		(	bi == ei
		||	*bi == t && are_all_same_to( std::next(bi), ei, t )
		);
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


public:
	struct Specimen
	{
		enum State
		{	DEFAULT_CONSTRUCTION
		,	MANUAL_CONSTRUCTION
		,	COPY_CONSTRUCTION
		,	MOVE_CONSTRUCTION
		,	COPY_ASSIGNMENT
		,	MOVE_ASSIGNMENT
		,	DESTRUCTED
		} state;

		int val;


		Specimen() : state(DEFAULT_CONSTRUCTION), val(0){}
		Specimen(int n, int n2 = 0) : state(MANUAL_CONSTRUCTION), val(n + n2){}
		Specimen(Specimen const& spec) : state(COPY_CONSTRUCTION), val(spec.val){}
		
		Specimen(Specimen&& spec) throw() : state(MOVE_CONSTRUCTION), val(spec.val)
		{
			spec.~Specimen();
		}

		~Specimen(){  state = DESTRUCTED,  val = 0;  }

		auto operator=(Specimen const& spec)-> Specimen&
		{
			state = COPY_ASSIGNMENT,  val = spec.val;
			
			return *this;
		}

		auto operator=(Specimen&& spec) throw()-> Specimen&
		{
			state = MOVE_ASSIGNMENT,  val = spec.val;
			
			spec.~Specimen();

			return *this;
		}

		bool operator==(Specimen const& spec) const{  return val == spec.val;  }
		bool operator==(int const n) const{  return val == n;  }
		bool operator==(State const s) const{  return state == s;  }

		template<class T>
		bool operator!=(T const& t) const{  return !(*this == t);  }
	};
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


	struct Dynamic : No_Making
	{
		static void Construction()
		{
			using type = Specimen;
			using iterable_t = std::initializer_list<type>;

			Serial<type>
				sr1,
				sr2(3),
				sr3(3, 180),
				sr4{3, 6, 9},
				sr5(sr4.begin(), sr4.end()),
				sr6(sr4.rbegin(), sr4.rend());

			assert
			(	sr1.is_null()
			&&	sr2.capacity() == 3 && sr2.is_empty()
			&&	sr3.capacity() == 3 && sr3.size() == 3 && are_all_same_to( sr3, type(180) )
			&&	have_the_same( sr4, iterable_t{type(3), type(6), type(9)} )
			&&	have_the_same(sr5, sr4) && are_all_same_to(sr5, type::State::COPY_CONSTRUCTION)
			&&	have_the_same( sr6, iterable_t{type(9), type(6), type(3)} )
			);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------
		

		static void Copy_Construction()
		{
			using type = Specimen;

			std::vector<type> Lvalue_iterable{-2, 1, 4};

			Serial<type>
				sr1{-1, 2, 5},
				sr2 = sr1,
				sr3 = Lvalue_iterable;

			assert
			(	have_the_same(sr2, sr1) && are_all_same_to(sr2, type::State::COPY_CONSTRUCTION)
			&&	have_the_same(sr3, Lvalue_iterable) 
			&&	are_all_same_to(sr3, type::State::COPY_CONSTRUCTION)
			);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		static void Move_Construction()
		{	
			using type = Specimen;
			using iterable_t = std::initializer_list<type>;

			Serial<type> _sr1{3, 6, 9};

			assert( are_all_same_to(_sr1, type::State::MANUAL_CONSTRUCTION) );

			type::State const _prev_sr1_state = _sr1.front().state;
			auto* const internal_data_address = _sr1.cdata();

			Serial<type>
				sr1 = std::move(_sr1),
				sr2 = std::vector<type>{type(-2), type(-5), type(-8)};

			assert
			(	!are_all_same_to(sr1, type::State::MOVE_CONSTRUCTION)
			&&	are_all_same_to(sr1, _prev_sr1_state) && sr1.cdata() == internal_data_address
			&&	have_the_same( sr1, iterable_t{type(3), type(6), type(9)} )
			&&	_sr1.is_null()
			&&	have_the_same( sr2, iterable_t{type(-2), type(-5), type(-8)} )
			&&	are_all_same_to(sr2, type::State::MOVE_CONSTRUCTION)
			);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		static void Assignment()
		{
			using type = Specimen;

			std::vector<type> vec{2, 5, 8, 11};
			Serial<type> sr1( 2, type(1) ),  sr2( 6, type(-3) );

			sr1 = vec,  sr2 = vec,
			assert
			(	have_the_same(sr1, vec) && have_the_same(sr2, vec)
			&&	!are_all_same_to(sr1, type::State::COPY_ASSIGNMENT)
			&&	are_all_same_to(sr1, type::State::COPY_CONSTRUCTION)
			&&	are_all_same_to(sr2, type::State::COPY_ASSIGNMENT)
			&&	sr2.capacity() == 6
			);


			Serial<type> sr3(6);

			sr3 = {type(1), type(4)},
			assert(sr3.capacity() == 6 && sr3.size() == 2);

			sr3 = vec,
			assert
			(	have_the_same(sr3, vec)
			&&	are_all_same_to(sr3.cbegin(), 2, type::State::COPY_ASSIGNMENT)
			&&	are_all_same_to(sr3.crbegin(), 2, type::State::COPY_CONSTRUCTION)
			&&	sr3.capacity() == 6
			);


			Serial<type> sr4(10), sr5(6);
			
			sr4 = {type(3), type(6)},
			sr5 = {type(-4), type(6), type(-8), type(10)},
			assert
			(	sr4.capacity() == 10 && sr4.size() == 2
			&&	sr5.capacity() == 6 && sr5.size() == 4
			);

			sr5 = sr4,
			assert
			(	have_the_same(sr5, sr4)
			&&	sr5.capacity() == 10 && sr5.size() == 2
			&&	are_all_same_to(sr5, type::State::COPY_CONSTRUCTION)
			&&	!are_all_same_to(sr5, type::State::COPY_ASSIGNMENT)
			);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		static void Move_Assignment()
		{
			using type = Specimen;

			Serial<type> sr1(10), sr2(6);
			
			sr1 = {type(3), type(6)},
			sr2 = {type(-4), type(6), type(-8), type(10)},
			assert
			(	sr1.capacity() == 10 && sr1.size() == 2
			&&	sr2.capacity() == 6 && sr2.size() == 4
			);

			auto* const internal_data_address = sr1.cdata();

			sr2 = std::move(sr1),
			assert
			(	sr2.capacity() == 10 && sr2.size() == 2
			&&	sr2.cdata() == internal_data_address
			&&	sr1.is_null()
			);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		static void Clear_and_Destruction()
		{
			using type = Specimen;

			Serial<type> sr1{1, -2, 3, -4}, sr2 = sr1;

			sr1.clear(),  sr2.~Serial(),
			assert
			(	sr1.size() == 0 && sr1.is_empty() && sr1.capacity() == 4 && !sr1.is_null()
			&&	sr2.size() == 0 && sr2.is_empty() && sr2.capacity() == 0 && sr2.is_null()
			);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		static void Type_Conversion_into_iterable()
		{
			using type = Specimen;
			using iterable_t = std::vector<type>;

			Serial<type> sr1{0, 5, 10}, sr2{10, 20, 30};
			iterable_t itb = sr1;

			assert
			(	have_the_same(itb, sr1)
			&&	are_all_same_to(itb, type::State::COPY_CONSTRUCTION)
			);

			itb = static_cast<iterable_t>(sr2),
			assert
			(	have_the_same(itb, sr2)
			&&	are_all_same_to(itb, type::State::COPY_CONSTRUCTION)
			);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		static void Swap()
		{
			using type = Specimen;

			Serial<type> sr1{12, 3, 45}, sr2{6789};

			auto
				*const internal_data_address1 = sr1.cdata(),
				*const internal_data_address2 = sr2.cdata();

			sr1.swap(sr2),
			assert
			(	have_the_same(sr1, Serial<type>{6789})
			&&	have_the_same(sr2, Serial<type>{12, 3, 45})
			&&	sr1.cdata() == internal_data_address2 && sr2.cdata() == internal_data_address1
			);

			std::swap(sr1, sr2),
			assert
			(	have_the_same(sr1, Serial<type>{12, 3, 45})
			&&	have_the_same(sr2, Serial<type>{6789})
			&&	sr1.cdata() == internal_data_address1 && sr2.cdata() == internal_data_address2
			);	
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		static void Element()
		{
			using type = Specimen;

			Serial<type> sr1{2, 4, 6};

			assert( sr1[0] == type(2) && sr1[1] == type(4) && sr1[2] == type(6) );

			sr1[1] = sr1[0],  sr1[2] = type(60),
			assert
			(	sr1[1] == type(2) && sr1[1] == type::State::COPY_ASSIGNMENT
			&&	sr1[2] == type(60) && sr1[2] == type::State::MOVE_ASSIGNMENT
			);

			assert(sr1.front() == *sr1.begin() && sr1.back() == *sr1.rbegin());


			Serial<type> const sr2{3, 6, 9};

			static_assert
			(	(	is_immutable<decltype(sr2[1])>::value
				&&	is_immutable<decltype(sr2.front())>::value
				&&	is_immutable<decltype(sr2.back())>::value
				&&	is_immutable<decltype(*sr2.begin())>::value
				&&	is_immutable<decltype(*sr2.rbegin())>::value	
				)
			,	"access interface may cause unexpected modification."
			);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		static void Push()
		{
			using type = Specimen; 

			type Lvalue = type(77);
			Serial<type> sr1(20);
			auto current_size = sr1.size(); 

			assert(current_size == 0);

			sr1.emplace_back(1, 2),
			assert
			(	sr1.size() == (current_size += 1)
			&&	sr1.back() == type(1, 2) && sr1.back() == type::State::MANUAL_CONSTRUCTION 
			);

			sr1.emplace_back(2, 3).emplace_back(7).emplace_back(Lvalue).emplace_back( type(99) ),
			assert
			(	sr1.size() == (current_size += 4)
			&&	have_the_same
				(	sr1, std::vector<type>{type(1, 2), type(2, 3), type(7), Lvalue, type(99)}
				)
			&&	sr1.rbegin()[0] == type::State::MOVE_CONSTRUCTION
			&&	sr1.rbegin()[1] == type::State::COPY_CONSTRUCTION
			&&	sr1.rbegin()[2] == type::State::MANUAL_CONSTRUCTION
			);

			sr1 >> Lvalue, 
			assert
			(	sr1.size() == (current_size += 1)
			&&	sr1.back() == Lvalue
			);

			sr1 >> type(-1) >> Lvalue,
			assert
			(	sr1.size() == (current_size += 2)
			&&	sr1.rbegin()[0] == Lvalue && sr1.rbegin()[0] == type::State::COPY_CONSTRUCTION
			&&	sr1.rbegin()[1] == -1 && sr1.rbegin()[1] == type::State::MOVE_CONSTRUCTION
			);


			Serial<type> 
				sr2{type(-1), type(-3), type(-5), type(-7)},
				sr3{type(11), type(22)};

			sr1.merge_back(sr2.begin(), sr2.end()),
			assert
			(	sr1.size() == (current_size += sr2.size())
			&&	sr1.rbegin()[0] == -7 && sr1.rbegin()[0] == type::State::COPY_CONSTRUCTION
			&&	sr1.rbegin()[1] == -5 && sr1.rbegin()[1] == type::State::COPY_CONSTRUCTION
			&&	sr1.rbegin()[2] == -3 && sr1.rbegin()[2] == type::State::COPY_CONSTRUCTION
			&&	sr1.rbegin()[3] == -1 && sr1.rbegin()[3] == type::State::COPY_CONSTRUCTION
			);

			sr1.merge_back(sr3.begin(), sr3.end()).merge_back(sr3.rbegin(), sr3.rend()),
			assert
			(	sr1.size() == (current_size += 2*sr3.size())
			&&	sr1.rbegin()[0] == 11 && sr1.rbegin()[0] == type::State::COPY_CONSTRUCTION
			&&	sr1.rbegin()[1] == 22 && sr1.rbegin()[1] == type::State::COPY_CONSTRUCTION
			&&	sr1.rbegin()[2] == 22 && sr1.rbegin()[2] == type::State::COPY_CONSTRUCTION
			&&	sr1.rbegin()[3] == 11 && sr1.rbegin()[3] == type::State::COPY_CONSTRUCTION
			);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		static void Pop()
		{
			using type = Specimen;
			enum{INITIAL_CAPACITY = 40};

			Serial<type> sr1(INITIAL_CAPACITY);
			auto fn = [](size_t const x)-> int{  return 2*static_cast<int>(x) + 1;  };

			for(size_t n = 0;  n < sr1.capacity();  sr1 >> fn(n + 1),  ++n);


			size_t current_size = sr1.size();

			assert(current_size == INITIAL_CAPACITY);

			sr1.pop_back(),
			assert
			(	sr1.size() == (current_size -= 1) && sr1.back() == fn(current_size)
			);

			sr1.pop_back().pop_back(),
			assert
			(	sr1.size() == (current_size -= 2) && sr1.back() == fn(current_size)
			);

			sr1.pop_back(3),
			assert
			(	sr1.size() == (current_size -= 3) && sr1.back() == fn(current_size)
			);

			sr1.pop_back(3).pop_back(2).pop_back(1),
			assert
			(	sr1.size() == (current_size -= 3 + 2 + 1) && sr1.back() == fn(current_size)
			);


			auto itr = sr1.end() - 4;

			sr1.pop_back_from(itr), 
			assert
			(	sr1.size() == (current_size -= 4) && sr1.back() == fn(current_size)
			);


			auto 
				itr1 = sr1.end() - 2, 
				itr2 = itr1 - 3, 
				itr3 = itr2 - 1;

			sr1.pop_back_from(itr1).pop_back_from(itr2).pop_back_from(itr3),
			assert
			(	sr1.size() == (current_size -= 2 + 3 + 1) && sr1.back() == fn(current_size)
			);

			sr1.clear(),
			assert(sr1.is_empty() && sr1.capacity() == INITIAL_CAPACITY);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

	};
	//========//========//========//========//=======#//========//========//========//========//===


	struct Static : No_Making
	{
		static void Construction()
		{
			using type = Specimen;

			Serial<type, 3> 
				sr1,
				sr2{4, 8, 12},
				sr3 = sr2;

			assert
			(	are_all_same_to(sr1, type::State::DEFAULT_CONSTRUCTION)
			&&	are_all_same_to(sr2, type::State::MANUAL_CONSTRUCTION)
			&&	have_the_same(sr2, Serial<type>{4, 8, 12})
			&&	are_all_same_to(sr3, type::State::COPY_CONSTRUCTION)
			&&	have_the_same(sr2, sr3)
			);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		static void No_Move_Construction()
		{
			using type = Specimen;

			Serial<type, 3> _sr1{4, 8, 12}, sr1 = std::move(_sr1);

			assert
			(	!are_all_same_to(sr1, type::State::MOVE_CONSTRUCTION)
			&&	!are_all_same_to(_sr1, type::State::DESTRUCTED)
			&&	are_all_same_to(sr1, type::State::COPY_CONSTRUCTION)
			&&	have_the_same(sr1, Serial<type>{4, 8, 12})
			);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		static void Assignment()
		{
			using type = Specimen;

			Serial<type, 3> sr1{2, 4, 6}, sr2;

			sr2 = sr1,
			assert
			(	have_the_same(sr2, sr1) && are_all_same_to(sr2, type::State::COPY_ASSIGNMENT)
			);

			std::vector<type> vec1{10}, vec2{-2, -4, -6, -8};

			sr1 = vec1,  sr2 = vec2,
			assert
			(	sr1.size() != vec1.size() && have_the_same(sr1, std::vector<type>{10, 4, 6})
			&&	sr2.size() != vec2.size() && have_the_same(sr2, std::vector<type>{-2, -4, -6})
			);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		static void Move_Assignment()
		{
			using type = Specimen;

			Serial<type, 3> _sr{2, 5, 8}, sr1;

			sr1 = std::move(_sr),
			assert
			(	have_the_same(sr1, std::vector<type>{2, 5, 8})
			&&	are_all_same_to(sr1, type::State::MOVE_ASSIGNMENT)
			&&	_sr.size() != 0 && are_all_same_to(_sr, type::State::DESTRUCTED)
			);


			Serial<type, 3> sr2{3, 6, 9};

			sr2 = {30, 60, 90},
			assert
			(	have_the_same(sr2, std::vector<type>{30, 60, 90})
			&&	are_all_same_to(sr2, type::State::MOVE_ASSIGNMENT)
			);


			Serial<type, 3> sr3;
			std::vector<type> vec2{10}, vec3{-2, -4, -6, -8};

			sr2 = std::move(vec2),  sr3 = std::move(vec3),
			assert
			(	have_the_same(sr2, std::vector<type>{10, 60, 90})
			&&	are_all_same_to(sr2, type::State::MOVE_ASSIGNMENT)
			&&	have_the_same(sr3, std::vector<type>{-2, -4, -6})
			&&	are_all_same_to(sr3, type::State::MOVE_ASSIGNMENT)
			);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		static void Destruction()
		{
			using type = Specimen;

			Serial<type, 3> sr1{1, -2, 3};
			
			sr1.~Serial(),
			assert( sr1.size() != 0 && are_all_same_to(sr1, type::State::DESTRUCTED) );
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		static void Type_Conversion_into_iterable()
		{
			using type = Specimen;
			using iterable_t = std::vector<type>;

			Serial<type, 3> sr1{0, 5, 10}, sr2{10, 20, 30};
			iterable_t itb = sr1;

			assert
			(	have_the_same(itb, sr1)
			&&	are_all_same_to(itb, type::State::COPY_CONSTRUCTION)
			);

			itb = static_cast<iterable_t>(sr2),
			assert
			(	have_the_same(itb, sr2)
			&&	are_all_same_to(itb, type::State::COPY_CONSTRUCTION)
			);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		static void Swap()
		{
			using type = Specimen;

			Serial<type, 3> sr1{12, 3, 45}, sr2{6, 78, 910};

			sr1.swap(sr2),
			assert
			(	have_the_same(sr1, Serial<type>{6, 78, 910})
			&&	have_the_same(sr2, Serial<type>{12, 3, 45})
			);

			std::swap(sr1, sr2),
			assert
			(	have_the_same(sr1, Serial<type>{12, 3, 45})
			&&	have_the_same(sr2, Serial<type>{6, 78, 910})
			);	
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------


		static void Element()
		{
			using type = Specimen;

			Serial<type, 3> sr1{2, 4, 6};

			assert( sr1[0] == type(2) && sr1[1] == type(4) && sr1[2] == type(6) );

			sr1[1] = sr1[0],  sr1[2] = type(60),
			assert
			(	sr1[1] == type(2) && sr1[1] == type::State::COPY_ASSIGNMENT
			&&	sr1[2] == type(60) && sr1[2] == type::State::MOVE_ASSIGNMENT
			);

			assert(sr1.front() == *sr1.begin() && sr1.back() == *sr1.rbegin());


			Serial<type, 3> const sr2{3, 6, 9};

			static_assert
			(	(	is_immutable<decltype(sr2[1])>::value
				&&	is_immutable<decltype(sr2.front())>::value
				&&	is_immutable<decltype(sr2.back())>::value
				&&	is_immutable<decltype(*sr2.begin())>::value
				&&	is_immutable<decltype(*sr2.rbegin())>::value	
				)
			,	"access interface may cause unexpected modification."
			);
		}
		//--------//--------//--------//--------//-------#//--------//--------//--------//--------

	};


};
//========//========//========//========//=======#//========//========//========//========//=======#


int main()
{
	Tutorial::Dynamic::Construction();
	Tutorial::Dynamic::Copy_Construction();
	Tutorial::Dynamic::Move_Construction();
	Tutorial::Dynamic::Assignment();
	Tutorial::Dynamic::Move_Assignment();
	Tutorial::Dynamic::Clear_and_Destruction();
	Tutorial::Dynamic::Type_Conversion_into_iterable();
	Tutorial::Dynamic::Swap();
	Tutorial::Dynamic::Element();
	Tutorial::Dynamic::Push();
	Tutorial::Dynamic::Pop();

	Tutorial::Static::Construction();
	Tutorial::Static::No_Move_Construction();
	Tutorial::Static::Assignment();
	Tutorial::Static::Move_Assignment();
	Tutorial::Static::Destruction();
	Tutorial::Static::Type_Conversion_into_iterable();
	Tutorial::Static::Swap();
	Tutorial::Static::Element();

	return 0;
}

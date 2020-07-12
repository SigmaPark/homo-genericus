#include "Serial.hpp"
#include <vector>
#include <array>

using namespace sgm;


struct Tutorial : No_Making
{
private:
	template<class CON1, class CON2>
	static bool same_iterables(CON1&& con1, CON2&& con2)
	{
		auto itr1 = con1.begin();
		auto itr2 = con2.begin();

		bool res = con1.size() == con2.size();

		while (itr1 != con1.end() && res)
			res = *itr1++ == *itr2++;

		return res;
	}

	
	struct NoCopy_NoSubstitution
	{
		NoCopy_NoSubstitution() = default;

		NoCopy_NoSubstitution(NoCopy_NoSubstitution const&) = delete;
		NoCopy_NoSubstitution(NoCopy_NoSubstitution&&){}
		
		auto operator=(NoCopy_NoSubstitution const&)->NoCopy_NoSubstitution& = delete;
	};


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
		Specimen(int n) : state(MANUAL_CONSTRUCTION), val(n){}
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
	//========//========//========//========//=======#//========//========//========//========//===


	struct Dynamic : No_Making
	{
		static void Construction1()
		{
			using type = Specimen;

			Serial<type>
				sr1,
				sr2(3),
				sr3(3, 180),
				sr4{3, 6, 9},
				sr5 = sr4,
				sr6 = std::vector<type>{type(2), type(5), type(8)},
				sr7(sr6.begin(), sr6.end());

			assert
			(	sr1.is_null()
			&&	sr2.capacity() == 3 && sr2.is_empty()
			&&	sr3.capacity() == 3 && sr3.size() == 3
			&&	same_iterables( sr3, std::array<type, 3>{type(180), type(180), type(180)} )
			&&	same_iterables( sr4, std::array<type, 3>{type(3), type(6), type(9)} )
			&&	same_iterables(sr5, sr4)
			&&	same_iterables( sr5, Serial<type::State>(3, type::State::COPY_CONSTRUCTION) )
			&&	same_iterables( sr6, std::array<type, 3>{type(2), type(5), type(8)} )
			&&	same_iterables( sr6, Serial<type::State>(3, type::State::MOVE_CONSTRUCTION) )
			&&	same_iterables(sr7, sr6)
			&&	same_iterables( sr7, Serial<type::State>(3, type::State::COPY_CONSTRUCTION) )
			);
		}


		static void Construction2()
		{	
			using type = Specimen;

			Serial<type> _sr1{3, 6, 9};

			assert
			(	same_iterables( _sr1, Serial<type::State>(3, type::State::MANUAL_CONSTRUCTION) )
			);

			auto sr1 = std::move(_sr1);

			assert
			(	!same_iterables( sr1, Serial<type::State>(3, type::State::MOVE_CONSTRUCTION) )
			&&	same_iterables( sr1, Serial<type::State>(3, type::State::MANUAL_CONSTRUCTION) )
			&&	same_iterables( sr1, std::array<type, 3>{type(3), type(6), type(9)} )
			&&	_sr1.is_null()
			);			
		}


		static void Assignment()
		{
			using type = Specimen;

			Serial<type> 
				sr1{2, 5, 8}, 
				sr2, 
				sr3( 3, type(10) );

			sr2 = sr1,
			sr3 = sr1;

			assert
			(	same_iterables(sr2, sr1)
			&&	!same_iterables( sr2, Serial<type::State>(3, type::State::COPY_ASSIGNMENT) )
			&&	same_iterables( sr2, Serial<type::State>(3, type::State::COPY_CONSTRUCTION) )
			&&	same_iterables(sr3, sr1)
			&&	same_iterables( sr3, Serial<type::State>(3, type::State::COPY_ASSIGNMENT) )
			);
		}


	};


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
			(	same_iterables( sr1, Serial<type::State>(3, type::State::DEFAULT_CONSTRUCTION) )
			&&	same_iterables( sr2, Serial<type::State>(3, type::State::MANUAL_CONSTRUCTION) )
			&&	same_iterables(sr2, Serial<int>{4, 8, 12})
			&&	same_iterables( sr3, Serial<type::State>(3, type::State::COPY_CONSTRUCTION) )
			&&	same_iterables(sr2, sr3)
			);

			Serial<type, 3> _sr4{4, 8, 12}, sr4 = std::move(_sr4);

			assert
			(	!same_iterables( sr4, Serial<type::State>(3, type::State::MOVE_CONSTRUCTION) )
			&&	!same_iterables( _sr4, Serial<type::State>(3, type::State::DESTRUCTED) )
			&&	same_iterables( sr4, Serial<type::State>(3, type::State::COPY_CONSTRUCTION) )
			&&	same_iterables(sr4, Serial<int>{4, 8, 12})
			);
		}
	};


	template<unsigned> static void Case();


	template<> static void Case<0>()
	{
		Serial<float> const sr{5, 2, 1};

		assert( same_iterables(sr, Serial<float, 3>{5, 2, 1}) );
	}


	template<> static void Case<1>()
	{
		Serial<double> sr1, sr2 = sr1, sr1_{4, 3}, sr2_(sr1_.begin(), sr1_.end());

		assert
		(	sr1.is_empty() && sr2.is_empty() && sr1_.size() == 2
		&&	same_iterables(sr1_, sr2_)
		);


		Serial<float> sr5(4), sr6(4, 7.f);

		assert
		(	sr5.capacity() == 4 && sr5.size() == 0 
		&&	same_iterables(sr6, Serial<float>{7.f, 7.f, 7.f, 7.f})
		);


		Serial<double, 5> sr3{1, 3, 5, 7, 9}, sr4 = sr3;

		assert
		(	same_iterables(sr3, sr4)
		&&	sr3.size() == 5
		);
	}


	template<> static void Case<2>()
	{
		Serial<float> sr1{1, 2, 3}, sr2 = sr1, sr3 = sr1;

		sr1.~Serial(),
		assert
		(	sr1.size() == 0 && sr2.size() != 0
		&&	same_iterables(sr2, sr3)
		);
	}
	//========//========//========//========//=======#//========//========//========//========//===


	template<> static void Case<3>()
	{
		using type = NoCopy_NoSubstitution;

		Serial<type> sr(4);

		sr >> type() >> type() >> type() >> type(),  assert(sr.size() == 4);

		sr.pop_back().pop_back(),  assert(sr.size() == 4 - 2);

		sr = Serial<type>(3),  assert(sr.size() == 0 && sr.capacity() == 3);

		for
		(	sr = Serial<type>(3)
		;	sr.size() != sr.capacity()
		;	sr >> type()
		);

		assert(sr.size() == 3);

		sr.clear(),  assert(sr.size() == 0 && sr.capacity() == 3 && sr.is_empty());
	}
	//========//========//========//========//=======#//========//========//========//========//===
	
	
	template<> static void Case<4>()
	{
		Serial<float> sr1{4, 2}, sr2{9, 7, 5, 3, 1}, sr3{3, 0, -3};

		sr1 = sr2,  assert( same_iterables(sr1, sr2) );
		sr2 = sr3,  assert( same_iterables(sr2, sr3) && sr2.capacity() == 5 );

		sr3 = {7, 5},  
		assert( same_iterables(sr3, Serial<float>{7, 5}) && sr3.capacity() == 2 );

		Serial<int> sr4{12, 3, 45}, sr5{6789};

		sr4.swap(sr5),
		assert
		(	same_iterables(sr4, Serial<int>{6789})
		&&	same_iterables(sr5, Serial<int>{12, 3, 45})
		);

		std::swap(sr4, sr5),
		assert
		(	same_iterables(sr5, Serial<int>{6789})
		&&	same_iterables(sr4, Serial<int>{12, 3, 45})
		);

		Serial<int, 2> sr6{2, 4}, sr7{6, 8};

		sr6.swap(sr7),
		assert
		(	same_iterables(sr6, Serial<int>{6, 8}) 
		&&	same_iterables(sr7, Serial<int>{2, 4})
		);
	}
	//========//========//========//========//=======#//========//========//========//========//===


};
//========//========//========//========//=======#//========//========//========//========//=======#



int main()
{
	Tutorial::Case<0>();
	Tutorial::Case<1>();
	Tutorial::Case<2>();
	Tutorial::Case<3>();
	Tutorial::Case<4>();

	Tutorial::Dynamic::Construction1();
	Tutorial::Dynamic::Construction2();
	Tutorial::Dynamic::Assignment();

	Tutorial::Static::Construction();

	return 0;
}

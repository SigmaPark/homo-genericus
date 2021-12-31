/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_SPECIFICATION_
#define _SGM_SPECIFICATION_


#include "../Exception/Exception.hpp"
#include "../iterable/iterable.hpp"
#include <initializer_list>
#include <iostream>


namespace sgm
{
    namespace spec
    {

        class Specimen;

    }
}
//========//========//========//========//=======#//========//========//========//========//=======#


class sgm::spec::Specimen
{
public:
    enum class State
    {   DEFAULT_CONSTRUCTION
    ,   MANUAL_CONSTRUCTION
    ,   COPY_CONSTRUCTION
    ,   MOVE_CONSTRUCTION
    ,   COPY_ASSIGNMENT
    ,   MOVE_ASSIGNMENT
    ,   DESTRUCTION
    ,   MOVE_AWAY
    };


    Specimen() : _state(State::DEFAULT_CONSTRUCTION), _value(0){}
    Specimen(int val) : _state(State::MANUAL_CONSTRUCTION), _value(val){}
    Specimen(Specimen const& s) : _state(State::COPY_CONSTRUCTION), _value(s.value()){}
    
    Specimen(Specimen&& s) noexcept 
    :   _state(State::MOVE_CONSTRUCTION), _value(s.value()){  s._state = State::MOVE_AWAY;  }

    ~Specimen(){  value() = -1,  _state = State::DESTRUCTION;  }


    auto operator=(Specimen const& s)-> Specimen&
    {
        _state = State::COPY_ASSIGNMENT,  value() = s.value();

        return *this;        
    }

    auto operator=(Specimen&& s) noexcept-> Specimen&
    {
        _state = State::MOVE_ASSIGNMENT,  value() = s.value();

        s._state = State::MOVE_AWAY;

        return *this;
    }


    auto state() const-> State{  return _state;  }

    auto value() const-> int{  return _value;  }
    auto value()-> int&{  return _value;  }

    auto operator==(Specimen const& s) const-> bool{  return value() == s.value();  }
    auto operator==(State const t) const-> bool{  return state() == t;  }

    template<class T>
    auto operator!=(T t) const-> bool{  return !(*this == t);  }


private:
    State _state;
    int _value;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{
    namespace spec
    {

        inline static void is_True(bool const b, Exception const& e = {}) noexcept(false)
        {
            if(!b)
                throw e;
        }

        inline static void is_False(bool const b, Exception const& e = {}) noexcept(false)
        {
            is_True(!b, e);
        }


        template<class RG, class TEST>
        static void Are_All_True(RG const& rg, TEST&& test) noexcept(false)
        {
            for(auto const& x : rg)
                is_True( test(x) );
        }


        template<class ITR, class TEST>
        static void Are_N_True(ITR itr, size_t n, TEST&& test) noexcept(false)
        {
            while(n-->0)
                is_True( test(*itr++) );
        }


        template<class RG1, class RG2, class EQ>
        static void Are_Equivalent_Ranges(RG1 const& rg1, RG2 const& rg2, EQ&& eq) 
        noexcept(false)
        {
            auto itr1 = Begin(rg1);
            auto itr2 = Begin(rg2);
            auto const end1 = End(rg1);
            auto const end2 = End(rg2);

            for( ;  itr1 != end1 && itr2 != end2 && eq(*itr1, *itr2);  itr1++,  itr2++ );

            is_True(itr1 == end1 && itr2 == end2);
        }

        template<class RG1, class RG2>
        static void Are_Equivalent_Ranges(RG1 const& rg1, RG2 const& rg2) noexcept(false)
        {
            using _T1 = decltype( *Begin(rg1) );
            using _T2 = decltype( *Begin(rg2) );

            Are_Equivalent_Ranges
            (   rg1, rg2, [](_T1 const& t1, _T2 const& t2)-> bool{  return t1 == t2;  }
            );
        }


        template<class RG, class T, class EQ>
        static void Are_All_Equivalent_to(RG const& rg, T const& t, EQ&& eq) noexcept(false)
        {
            using _T = decltype( *Begin(rg) );

            Are_All_True( rg, [t, eq](_T const& _t)-> bool{  return eq(_t, t);  } );
        }

        template<class RG, class T>
        static void Are_All_Equivalent_to(RG const& rg, T const& t) noexcept(false)
        {
            using _T = decltype( *Begin(rg) );
            
            Are_All_Equivalent_to
            (   rg, t, [](_T const& t1, T const& t2)-> bool{  return t1 == t2;  }  
            );
        }

        template<class ITR, class T, class EQ>
        static void Are_N_Equivalent_to(ITR const itr, size_t const n, T const& t, EQ&& eq)
        noexcept(false)
        {
            using _T = decltype(*itr);

            Are_N_True( itr, n, [t, eq](_T const& _t)-> bool{  return eq(_t, t);  } );            
        }

        template<class ITR, class T>
        static void Are_N_Equivalent_to(ITR const itr, size_t const n, T const& t)
        noexcept(false)
        {
            using _T = decltype(*itr);

            Are_N_Equivalent_to
            (   itr, n, t, [](_T const& t1, T const& t2)-> bool{  return t1 == t2;  }   
            );            
        }

    }
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#define SGM_SPECIFICATION_TEST(PREFIX, TITLE, SUFFIX)   \
    struct __##TITLE##_Helper   \
    {   \
        static std::initializer_list<void(*)()> test_list;   \
    };  \
    \
    void PREFIX##TITLE##SUFFIX::test()  \
    {   \
        try     \
        {   \
            for(auto _test : __##TITLE##_Helper::test_list)  \
                _test(); \
            \
            std::cout << #TITLE << " test complete.\n"; \
            \
        }   \
        catch(sgm::Exception const& e)  \
        {   \
            std::cout << #TITLE << " test failed.\n\t" << e.what() << std::endl;    \
        }   \
        catch(...)  \
        {   \
            std::cout << #TITLE << " test failed.\n\t" << "unexpected error.\n";    \
        }   \
    }   \
    \
    std::initializer_list<void(*)()> __##TITLE##_Helper::test_list =


#define SGM_SPECIFICATION_CLASS(PREFIX, TITLE, SUFFIX)  \
    struct PREFIX##TITLE##SUFFIX    \
    {   \
        PREFIX##TITLE##SUFFIX() = delete;   \
        \
        static void test(); \
    }


#endif // end of #ifndef _SGM_SPECIFICATION_
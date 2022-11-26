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
#include <limits>
#include <string>


namespace sgm
{
    namespace spec
    {

        class Specimen;
        class Specimen_Logger;
        class Specimen_Log_Guard;

    }
}
//========//========//========//========//=======#//========//========//========//========//=======#


class sgm::spec::Specimen_Logger
{
public:
    Specimen_Logger() = default;
    virtual ~Specimen_Logger() = default;

    virtual void log(std::string const& log_message) = 0;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


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


    Specimen() : _state(State::DEFAULT_CONSTRUCTION), _value(0)
    {
        _Log("default_construction");
    }
    
    Specimen(int val) : _state(State::MANUAL_CONSTRUCTION), _value(val)
    {
        _Log("manual_construction");
    }
    
    Specimen(Specimen const& s) : _state(State::COPY_CONSTRUCTION), _value(s.value())
    {
        _Log("copy_construction");
    }
    
    Specimen(Specimen&& s) noexcept : _state(State::MOVE_CONSTRUCTION), _value(s.value())
    {
        s._state = State::MOVE_AWAY;  

        _Log("move_construction");
    }

    ~Specimen()
    {
        value() = -1,  _state = State::DESTRUCTION; 
        
        _Log("destruction");
    }


    auto operator=(Specimen const& s)-> Specimen&
    {
        _state = State::COPY_ASSIGNMENT,  value() = s.value();

        _Log("copy_assignment");

        return *this;        
    }

    auto operator=(Specimen&& s) noexcept-> Specimen&
    {
        _state = State::MOVE_ASSIGNMENT,  value() = s.value();

        s._state = State::MOVE_AWAY;

        _Log("move_assignment");

        return *this;
    }


    auto state() const-> State{  return _state;  }

    auto value() const-> int{  return _value;  }
    auto value()-> int&{  return _value;  }

    auto operator==(Specimen const& s) const-> bool{  return value() == s.value();  }
    auto operator==(State const t) const-> bool{  return state() == t;  }

    template<class T>
    auto operator!=(T t) const-> bool{  return !(*this == t);  }


    static void Begin_log(Specimen_Logger& logger){  _Logger_ptr() = Address_of(logger);  }
    static void End_log(){  _Logger_ptr() = nullptr;  }


private:
    State _state;
    int _value;


    static auto _Logger_ptr()-> Specimen_Logger*&
    {
        static Specimen_Logger* ptr = nullptr;

        return ptr;
    }


    static void _Log(std::string const& log_str)
    {
        if(_Logger_ptr() != nullptr)
            _Logger_ptr()->log(log_str);
    }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class sgm::spec::Specimen_Log_Guard
{
public:
    Specimen_Log_Guard(Specimen_Logger& logger){  Specimen::Begin_log(logger);  }
    ~Specimen_Log_Guard(){  Specimen::End_log();  }
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


#define _SGM_DOUBLE_UNDERBAR_MACRO_HELPER(MACRO)  __##MACRO##__

#define BEGIN_CODE_BLOCK(TAG) /* nothing */
#define END_CODE_BLOCK(TAG) /* nothing */
#define END_CODE_BLOCK_AND_LOAD(TAG)  sgm::spec::mdo << sgm::spec::Load_code_block(#TAG);
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{ 
    namespace spec
    {

	    auto HTML_tag(std::string const& contents, std::string const& tag)-> std::string;
	    auto Load_image(std::string const& image_name, size_t const image_width = 0)-> std::string;
	    auto Empty_lines(size_t nof_empty_lines = 1)-> std::string;
        auto Title(std::string const& title, unsigned const level = 1)-> std::string;

	    auto Load_description_file(std::string const& filename) noexcept(false)-> std::string;
        auto Load_code_block(std::string const code_block_tag) noexcept(false)-> std::string;
	    

	    class md_guard;
	    class html_block_guard;
	    class md_block_guard;


	    class _tabless_description;
	    class _code_description;

	    class _MD_Stream;


        template
        <   class T, class _T = Decay_t<T>
        ,   int 
            =   (   is_Same<_T, _tabless_description>::value 
                ||  is_Same<_T, _code_description>::value
                ) ? 1
            :   is_Same<_T, bool>::value ? 2
            :   is_Convertible<_T, double>::value ? 3
            :   is_Pointer<_T>::value ? 4
            :   /* otherwise */ 0
        >
        struct _MD_Stream_Helper;


        class _Singleton_MD_Streamer;
	    class _MD_Stream_Guard;

	    auto _Code_writing(std::string const& code, std::string const &lang = "")-> std::string;
    
    }
}


auto operator ""_code(char const* str, size_t)-> sgm::spec::_code_description;
auto operator ""_mdo(char const* str, size_t)-> sgm::spec::_tabless_description;
//========//========//========//========//=======#//========//========//========//========//=======#



class sgm::spec::_tabless_description
{
public:
	_tabless_description(std::string&& s);

private:
    template<class T, class _T, int>
	friend struct sgm::spec::_MD_Stream_Helper;

	std::string _str;

	static auto _tabless_string(std::string&& s)-> std::string;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class sgm::spec::_code_description
{
public:
	_code_description(std::string&& s);

private:
    template<class T, class _T, int>
    friend struct sgm::spec::_MD_Stream_Helper;

	std::string _str;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class sgm::spec::_MD_Stream
{
public:
	_MD_Stream(_MD_Stream const&) = delete;
	auto operator=(_MD_Stream const&)-> _MD_Stream& = delete;

	static auto instance()-> _MD_Stream&;

    void open(std::string const working_filepath);
    bool is_open() const;

    auto ever_used() const-> bool;
    auto working_filepath() const-> std::string const&;
    auto md_filepath() const-> std::string const&;
    auto md_materials_dir() const-> std::string const&;

	void close();
	void print_and_close();

	template<class T>
	auto operator<<(T&& t)-> _MD_Stream&;


private:
	_MD_Stream();
	~_MD_Stream();


	struct _Contents;

	std::string _working_filepath, _md_filepath, _md_materials_dir;
	_Contents *_pcnts;

	void _push(std::string const& str);
	void _push(std::string&& str);
};


template<class T>
auto sgm::spec::_MD_Stream::operator<<(T&& t)-> _MD_Stream&
{
    _push(  _MD_Stream_Helper<T>::calc( Forward<T>(t) )  );

	return *this;
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


template<class T, class _T, int>
struct sgm::spec::_MD_Stream_Helper : Unconstructible
{
    template<class Q>
    static auto calc(Q&& q)-> std::string{  return Forward<Q>(q);  }    
};

template<class T, class _T>
struct sgm::spec::_MD_Stream_Helper<T, _T, 1> : Unconstructible
{
    template<class Q>
    static auto calc(Q&& q)-> SGM_DECLTYPE_AUTO(  q._str  )
};

template<class T, class _T>
struct sgm::spec::_MD_Stream_Helper<T, _T, 2> : Unconstructible
{
    static auto calc(_T const b)-> std::string{  return b ? "true" : "false";  }
};

template<class T, class _T>
struct sgm::spec::_MD_Stream_Helper<T, _T, 3> : Unconstructible
{
    template<class Q>
    static auto calc(Q const s)-> std::string{  return std::to_string(s);  }
};

template<class T, class _T>
struct sgm::spec::_MD_Stream_Helper<T, _T, 4> : Unconstructible
{
    template<class P>
    static auto calc(P const p)
    ->  std::string{  return std::to_string( reinterpret_cast<long long>(p) );  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class sgm::spec::_Singleton_MD_Streamer
{
public:
    template<class T>
    auto operator<<(T&& t) const-> _MD_Stream&{  return _MD_Stream::instance() << Forward<T>(t);  }

    auto operator->() const-> _MD_Stream*{  return &_MD_Stream::instance();  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{
    namespace spec
    {

	    static _Singleton_MD_Streamer const mdo = {};

	    static std::string const newl = "  \n";
	    static std::string const empty_line = Empty_lines(1);

    }
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class sgm::spec::_MD_Stream_Guard
{
public:
	_MD_Stream_Guard(std::string working_filepath);
	~_MD_Stream_Guard();

	bool is_successful;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class sgm::spec::md_guard
{
public:
	md_guard(std::string begin);
	md_guard(std::string begin, std::string end);
	~md_guard();

private:
	std::string _end;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class sgm::spec::md_block_guard : public md_guard
{
public:
	md_block_guard(std::string s = "");
};


class sgm::spec::html_block_guard
{
public:
	html_block_guard(std::string const& tags);
	~html_block_guard();

private:
	std::string _end = {};

	static auto _bracket(std::string const& s)-> std::string;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


#define SGM_SPECIFICATION_TEST(PREFIX, TITLE, SUFFIX)   \
    struct __##TITLE##_Helper   \
    {   \
        static std::initializer_list<void(*)()> test_list;   \
    };  \
    \
    void PREFIX##TITLE##SUFFIX::test()  \
    {   \
        sgm::spec::_MD_Stream_Guard guard( _SGM_DOUBLE_UNDERBAR_MACRO_HELPER(FILE) );  \
        \
        try     \
        {   \
            \
            for(auto _test : __##TITLE##_Helper::test_list)  \
                _test(); \
            \
            guard.is_successful = true;    \
            std::cout << #TITLE << " test complete.\n"; \
            \
        }   \
        catch(sgm::Exception const& e)  \
        {   \
            std::cout << '\n' << #TITLE << " test failed.\n\t" << e.what() << std::endl;    \
            guard.is_successful = false;    \
        }   \
        catch(...)  \
        {   \
            std::cout << '\n' << #TITLE << " test failed.\n\t" << "unexpected error.\n";    \
            guard.is_successful = false;    \
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
/*  SPDX-FileCopyrightText: (c) 2020 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#pragma once
#ifndef _SGM_SPECIFICATION_
#define _SGM_SPECIFICATION_


#include "SGM/iterable/iterable.hpp"
#include <string>
#include <iostream>


namespace sgm
{
    namespace spec
    {

        class Specimen;
        class Specimen_Logger;
        class Specimen_Log_Guard;

    }
}

#define BEGIN_CODE_BLOCK(TAG) /* nothing */
#define END_CODE_BLOCK(TAG) /* nothing */

#define END_CODE_BLOCK_AND_LOAD(TAG)  \
    sgm::spec::mdo << sgm::spec::Load_code_block( sgm::spec::_Mbs_to_Wcs(#TAG) );
//========//========//========//========//=======#//========//========//========//========//=======#


class sgm::spec::Specimen_Logger
{
public:
    Specimen_Logger() = default;
    virtual ~Specimen_Logger() = default;

    virtual void log(std::wstring const& log_message) = 0;
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
        _Log(L"default_construction");
    }
    
    Specimen(int val) : _state(State::MANUAL_CONSTRUCTION), _value(val)
    {
        _Log(L"manual_construction");
    }
    
    Specimen(Specimen const& s) : _state(State::COPY_CONSTRUCTION), _value(s.value())
    {
        _Log(L"copy_construction");
    }
    
    Specimen(Specimen&& s) noexcept : _state(State::MOVE_CONSTRUCTION), _value(s.value())
    {
        s._state = State::MOVE_AWAY;  

        _Log(L"move_construction");
    }

    ~Specimen()
    {
        if(_state == State::DESTRUCTION)
            return;

        value() = -1,  _state = State::DESTRUCTION; 
        
        _Log(L"destruction");
    }


    auto operator=(Specimen const& s)-> Specimen&
    {
        _state = State::COPY_ASSIGNMENT,  value() = s.value();

        _Log(L"copy_assignment");

        return *this;        
    }

    auto operator=(Specimen&& s) noexcept-> Specimen&
    {
        _state = State::MOVE_ASSIGNMENT,  value() = s.value();

        s._state = State::MOVE_AWAY;

        _Log(L"move_assignment");

        return *this;
    }


    auto state() const noexcept-> State{  return _state;  }

    auto value() const noexcept-> int{  return _value;  }
    auto value() noexcept-> int&{  return _value;  }

    auto operator==(Specimen const& s) const noexcept-> bool{  return value() == s.value();  }
    auto operator==(State const t) const noexcept-> bool{  return state() == t;  }

    template<class T>
    auto operator!=(T t) const noexcept-> bool{  return !(*this == t);  }


    static void Begin_log(Specimen_Logger& logger) noexcept
    {  
        _Logger_ptr() = Address_of(logger);  
    }
    
    static void End_log() noexcept{  _Logger_ptr() = nullptr;  }


private:
    State _state;
    int _value;


    static auto _Logger_ptr() noexcept-> Specimen_Logger*&
    {
        static Specimen_Logger* ptr = nullptr;

        return ptr;
    }


    static void _Log(std::wstring const& log_str)
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

        template<class RG, class TEST>
        static auto Are_All_True(RG const& rg, TEST&& test)-> bool
        {
            for(auto const& x : rg)
                if( !test(x) )
                    return false;

            return true;
        }


        template<class ITR, class TEST>
        static auto Are_N_True(ITR itr, size_t n, TEST&& test)-> bool
        {
            while(n-->0)
                if( !test(*itr++) )
                    return false;

            return true;
        }


        template<class RG1, class RG2, class EQ>
        static auto Are_Equivalent_Ranges(RG1 const& rg1, RG2 const& rg2, EQ&& eq)-> bool
        {
            auto itr1 = Begin(rg1);
            auto itr2 = Begin(rg2);
            auto const end1 = End(rg1);
            auto const end2 = End(rg2);

            for( ;  itr1 != end1 && itr2 != end2 && eq(*itr1, *itr2);  itr1++,  itr2++ );

            return itr1 == end1 && itr2 == end2;
        }

        template<class RG1, class RG2>
        static auto Are_Equivalent_Ranges(RG1 const& rg1, RG2 const& rg2)-> bool
        {
            using _T1 = decltype( *Begin(rg1) );
            using _T2 = decltype( *Begin(rg2) );

            return 
            Are_Equivalent_Ranges
            (   rg1, rg2, [](_T1 const& t1, _T2 const& t2)-> bool{  return t1 == t2;  }
            );
        }


        template<class RG, class T, class EQ>
        static auto Are_All_Equivalent_to(RG const& rg, T const& t, EQ&& eq)-> bool
        {
            using _T = decltype( *Begin(rg) );

            return Are_All_True( rg, [t, eq](_T const& _t)-> bool{  return eq(_t, t);  } );
        }

        template<class RG, class T>
        static auto Are_All_Equivalent_to(RG const& rg, T const& t)-> bool
        {
            using _T = decltype( *Begin(rg) );
            
            return
            Are_All_Equivalent_to
            (   rg, t, [](_T const& t1, T const& t2)-> bool{  return t1 == t2;  }  
            );
        }

        template<class ITR, class T, class EQ>
        static auto Are_N_Equivalent_to(ITR const itr, size_t const n, T const& t, EQ&& eq)
        ->  bool
        {
            using _T = decltype(*itr);

            return Are_N_True( itr, n, [t, eq](_T const& _t)-> bool{  return eq(_t, t);  } );            
        }

        template<class ITR, class T>
        static auto Are_N_Equivalent_to(ITR const itr, size_t const n, T const& t)-> bool
        {
            using _T = decltype(*itr);

            return
            Are_N_Equivalent_to
            (   itr, n, t, [](_T const& t1, T const& t2)-> bool{  return t1 == t2;  }   
            );            
        }


        enum class Test_Assertion_Failure{};

    }
}


#define SGM_SPEC_ASSERT(...) \
    [](bool const assertion_pass) noexcept(false)-> void \
    {   \
        if(assertion_pass)  \
            return; \
        \
        auto const file_path \
        =   sgm::spec::_Mbs_to_Wcs  \
            (   _SGM_DOUBLE_UNDERBAR_MACRO_HELPER(FILE) \
            );  \
        \
        auto const error_line \
        =   std::to_wstring( _SGM_DOUBLE_UNDERBAR_MACRO_HELPER(LINE) );  \
        \
        auto const log_msg \
        =   (   std::wstring{L"[Failure case] \n"} \
            +   L"  File : " + file_path + L'\n'  \
            +   L"  Line : " + error_line + L'\n' \
            +   L"  " + sgm::spec::_Mbs_to_Wcs(#__VA_ARGS__) \
            +   L"\n\n" \
            );  \
        \
        std::wcout << log_msg; \
        \
        throw sgm::spec::Test_Assertion_Failure{};  \
    }( static_cast<bool>(__VA_ARGS__) )
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{ 
    namespace spec
    {

	    auto HTML_tag(std::wstring const& contents, std::wstring const& tag)-> std::wstring;
	    auto Load_image(std::wstring const& img_name, size_t const img_width = 0)-> std::wstring;
	    auto Empty_lines(size_t nof_empty_lines = 1)-> std::wstring;
        auto Title(std::wstring const& title, unsigned const level = 1)-> std::wstring;

	    auto Load_description_file(std::wstring const& filename) noexcept(false)-> std::wstring;
        auto Load_code_block(std::wstring const code_block_tag) noexcept(false)-> std::wstring;

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

	    auto _Code_writing(std::wstring const& code, std::wstring const &lang = L"")
        ->  std::wstring;


    #pragma warning(push)
    #pragma warning(disable : 4996)
    	static auto _Mbs_to_Wcs(std::string const& mbs)-> std::wstring
    	{
    		std::wstring res(mbs.size(), L'\0');
    
    		mbstowcs(res.data(), mbs.c_str(), res.size());
    		
    		return res;
    	}
    	
    	
    	static auto _Wcs_to_Mbs(std::wstring const& wcs)-> std::string
    	{
    		std::string res(wcs.size(), '\0');
    
    		wcstombs(res.data(), wcs.c_str(), res.size());
    
    		return res;
    	}
    #pragma warning(pop)
    
    }
}


auto operator ""_code(wchar_t const* str, size_t)-> sgm::spec::_code_description;
auto operator ""_mdo(wchar_t const* str, size_t)-> sgm::spec::_tabless_description;
//========//========//========//========//=======#//========//========//========//========//=======#



class sgm::spec::_tabless_description
{
public:
	_tabless_description(std::wstring&& s);

private:
    template<class T, class _T, int>
	friend struct sgm::spec::_MD_Stream_Helper;

	std::wstring _str;

	static auto _tabless_string(std::wstring&& s)-> std::wstring;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class sgm::spec::_code_description
{
public:
	_code_description(std::wstring&& s);

private:
    template<class T, class _T, int>
    friend struct sgm::spec::_MD_Stream_Helper;

	std::wstring _str;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class sgm::spec::_MD_Stream
{
public:
	_MD_Stream(_MD_Stream const&) = delete;
	auto operator=(_MD_Stream const&)-> _MD_Stream& = delete;

	static auto instance()-> _MD_Stream&;

    void open(std::wstring const working_filepath);
    bool is_open() const;

    auto ever_used() const-> bool;
    auto working_filepath() const-> std::wstring const&;
    auto md_filepath() const-> std::wstring const&;
    auto md_materials_dir() const-> std::wstring const&;

	void close();
	void print_and_close();

	template<class T>
	auto operator<<(T&& t)-> _MD_Stream&;


private:
	_MD_Stream();
	~_MD_Stream();


	struct _Contents;

	std::wstring _working_filepath, _md_filepath, _md_materials_dir;
	_Contents *_pcnts;

	void _push(std::wstring const& str);
	void _push(std::wstring&& str);
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
    static auto calc(Q&& q)-> std::wstring{  return Forward<Q>(q);  }    
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
    static auto calc(_T const b)-> std::wstring{  return b ? L"true" : L"false";  }
};

template<class T, class _T>
struct sgm::spec::_MD_Stream_Helper<T, _T, 3> : Unconstructible
{
    template<class Q>
    static auto calc(Q const s)-> std::wstring{  return std::to_wstring(s);  }
};

template<class T, class _T>
struct sgm::spec::_MD_Stream_Helper<T, _T, 4> : Unconstructible
{
    template<class P>
    static auto calc(P const p)
    ->  std::wstring{  return std::to_wstring( reinterpret_cast<long long>(p) );  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class sgm::spec::_Singleton_MD_Streamer
{
public:
    template<class T>
    auto operator<<(T&& t) const
    ->  _MD_Stream&{  return _MD_Stream::instance() << Forward<T>(t);  }

    auto operator->() const-> _MD_Stream*{  return &_MD_Stream::instance();  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


namespace sgm
{
    namespace spec
    {

	    static _Singleton_MD_Streamer const mdo = {};

	    static std::wstring const newl = L"  \n";
	    static std::wstring const empty_line = Empty_lines(1);

    }
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class sgm::spec::_MD_Stream_Guard
{
public:
	_MD_Stream_Guard(std::wstring working_filepath);
    _MD_Stream_Guard(std::string working_filepath);

	~_MD_Stream_Guard();

	bool is_successful;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class sgm::spec::md_guard
{
public:
	md_guard(std::wstring begin);
	md_guard(std::wstring begin, std::wstring end);
	~md_guard();

private:
	std::wstring _end;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


class sgm::spec::md_block_guard : public md_guard
{
public:
	md_block_guard(std::wstring s = L"");
};


class sgm::spec::html_block_guard
{
public:
	html_block_guard(std::wstring const& tags);
	~html_block_guard();

private:
	std::wstring _end = {};

	static auto _bracket(std::wstring const& s)-> std::wstring;
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
        guard.is_successful = true;    \
        \
        auto const title_wstr = sgm::spec::_Mbs_to_Wcs(#TITLE); \
        \
        std::wcout << title_wstr << L" test starts.\n";    \
        \
        for(auto _test : __##TITLE##_Helper::test_list)  \
            try{  _test();  }   \
            catch(sgm::spec::Test_Assertion_Failure const){  guard.is_successful = false;  } \
        \
        std::wcout << title_wstr << L" test ends.\n";   \
        \
        if(guard.is_successful)    \
            std::wcout << L"All cases pass! \n"; \
        \
        std::wcout << L"//--------//--------//--------//--------//--------\n";    \
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
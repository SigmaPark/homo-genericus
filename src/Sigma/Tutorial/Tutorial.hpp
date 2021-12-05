#pragma once
#ifndef _SGM_TUTORIAL_
#define _SGM_TUTORIAL_


#define _LOAD_CODE_BLOCK_HELPER(FILEPATH, IDX)  sgm::tut::_Load_code_block(__##FILEPATH##__, IDX)

#define __CURRENT_SOURCE_DIRECTORY__  sgm::tut::_Current_File_Directory( _DOUBLE_UNDERBAR_MACRO_HELPER(FILE) )
#define __MD_MATERIALS__  __CURRENT_SOURCE_DIRECTORY__ + "\\md_materials"


#include <string>


#define LOAD_CODE_BLOCK(IDX)  _LOAD_CODE_BLOCK_HELPER(FILE, IDX)

#define BEGIN_CODE_BLOCK(IDX) /* nothing */
#define END_CODE_BLOCK(IDX) /* nothing */
#define END_CODE_BLOCK_AND_LOAD(IDX)  sgm::tut::mdo << LOAD_CODE_BLOCK(IDX);

#define LOAD_DESCRIPTION_FILE(FILE)  sgm::tut::_Load_file(__MD_MATERIALS__ + "\\" + FILE)


namespace sgm::tut
{

	auto HTML_tag(std::string const& contents, std::string const& tag)-> std::string;
	auto Load_image(std::string const& image_name, size_t const image_width = 0)-> std::string;

	void is_True(bool const b) noexcept(false);
	void is_False(bool const b) noexcept(false);

	class md_guard;
	class html_block_guard;
	class md_block_guard;


	class _tabless_description;
	class _code_description;

	class _MD_Stream;
	class _MD_Stream_Guard;

	auto _Load_file(std::string const& filepath) noexcept(false)-> std::string;
	auto _Code_writing(std::string const& code, std::string const &lang = "")-> std::string;
	auto _Load_code_block(std::string const& filepath, int code_block_index)	noexcept(false)->	std::string;
	auto _Current_File_Directory(std::string s)-> std::string;
	void _Print_Message(std::string&& msg);

}


auto operator ""_code(char const* str, size_t)-> sgm::tut::_code_description;
auto operator ""_mdo(char const* str, size_t)-> sgm::tut::_tabless_description;
//========//========//========//========//=======#//========//========//========//========//=======#//========//========


class sgm::tut::_tabless_description
{
public:
	_tabless_description(std::string&& s);

private:
	friend class sgm::tut::_MD_Stream;

	std::string _str;

	static auto _tabless_string(std::string&& s)-> std::string;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


class sgm::tut::_code_description
{
public:
	_code_description(std::string&& s);

private:
	friend class sgm::tut::_MD_Stream;

	std::string _str;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


class sgm::tut::_MD_Stream
{
public:
	_MD_Stream(_MD_Stream const&) = delete;
	auto operator=(_MD_Stream const&)-> _MD_Stream& = delete;

	static auto instance()-> _MD_Stream&;

	void open(std::string s);
	bool is_open() const;
	void close();
	void print_and_close();

	template<class T>
	auto operator<<(T&& t)-> _MD_Stream&;

private:
	_MD_Stream();
	~_MD_Stream();


	struct _Contents;

	std::string _filepath = {};
	_Contents *_pcnts;


	void _push(std::string const& str);
	void _push(std::string&& str);
};


template<class T>
auto sgm::tut::_MD_Stream::operator<<(T&& t)-> _MD_Stream&
{
	using _T = std::decay_t<T>;
		
	if constexpr(std::is_convertible_v<_T, std::string>)
		_push( std::forward<T>(t) );
	else if constexpr(std::is_same_v<_T, _tabless_description>)
		*this << t._str;
	else if constexpr(std::is_same_v<_T, _code_description>)
		*this << t._str;
	else if constexpr(std::is_same_v<_T, char>)
		*this << std::string{t};
	else if constexpr(std::is_same_v<_T, bool>)
		*this << (t ? "true" : "false");
	else if constexpr(std::is_integral_v<_T> || std::is_floating_point_v<_T>)
		*this << std::to_string(t);
	else if constexpr(std::is_pointer_v<_T>)
		*this << reinterpret_cast<long long>(t);

	return *this;
}


namespace sgm::tut
{
	inline _MD_Stream& mdo = _MD_Stream::instance();
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


class sgm::tut::_MD_Stream_Guard
{
public:
	_MD_Stream_Guard(std::string const filename);
	~_MD_Stream_Guard();

	bool is_successful;
	std::string filepath;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


class sgm::tut::md_guard
{
public:
	md_guard(std::string begin);
	md_guard(std::string begin, std::string end);
	~md_guard();

private:
	std::string _end;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


class sgm::tut::md_block_guard : public md_guard
{
public:
	md_block_guard(std::string s = "");
};


class sgm::tut::html_block_guard
{
public:
	html_block_guard(std::string const& tags);
	~html_block_guard();

private:
	std::string _end = {};

	static auto _bracket(std::string const& s)-> std::string;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


#define _DOUBLE_UNDERBAR_MACRO_HELPER(MACRO)  __##MACRO##__


#define SGM_TUTORIAL(PREFIX, TITLE, SUFFIX)	\
	struct _##TITLE##_Helper	\
	{	\
		static std::initializer_list<void(*)()> test_list;		\
	};	\
	\
	void PREFIX##TITLE##SUFFIX::print()		\
	{	\
		sgm::tut::_MD_Stream_Guard guard( _DOUBLE_UNDERBAR_MACRO_HELPER(FILE) );	\
		\
		try \
		{	\
			for(auto test : _##TITLE##_Helper::test_list)	\
				test();	\
			\
			guard.is_successful = true;	\
			sgm::tut::_Print_Message( std::string(#TITLE) + " tutorial file print complete.\n" );	\
		}	\
		catch(std::exception const& e)	\
		{	\
			sgm::tut::_Print_Message( std::string(#TITLE) + " tutorial file print failed : " + e.what() + "\n" ); \
			guard.is_successful = false;	\
		}	\
	}	\
	\
	std::initializer_list<void(*)()> _##TITLE##_Helper::test_list =


#define SGM_DEFAULT_TUTORIAL(TITLE)  SGM_TUTORIAL(Tut_, TITLE, /**/)


#define SGM_TUTORIAL_CLASS(PREFIX, TITLE, SUFFIX) \
	struct PREFIX##TITLE##SUFFIX	\
	{	\
		PREFIX##TITLE##SUFFIX() = delete;	\
		\
		static void print();	\
	}


#define SGM_DEFAULT_TUTORIAL_CLASS(TITLE)  SGM_TUTORIAL_CLASS(Tut_, TITLE, /**/)


#endif // end of #ifndef _SGM_TUTORIAL_
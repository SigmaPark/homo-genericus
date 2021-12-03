#pragma once
#ifndef _SGM_TUTORIAL_
#define _SGM_TUTORIAL_


#define _LOAD_CODE_BLOCK_HELPER(FILEPATH, IDX)  sgm::tut::_Load_code_block(__##FILEPATH##__, IDX)

#define __CURRENT_SOURCE_DIRECTORY__  sgm::tut::_Current_File_Directory( _DOUBLE_UNDERBAR_MACRO_HELPER(FILE) )
#define __MD_MATERIALS__  __CURRENT_SOURCE_DIRECTORY__ + "\\md_materials"


#include <exception>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <queue>


#define LOAD_CODE_BLOCK(IDX)  _LOAD_CODE_BLOCK_HELPER(FILE, IDX)

#define BEGIN_CODE_BLOCK(IDX) /* nothing */
#define END_CODE_BLOCK(IDX) /* nothing */
#define END_CODE_BLOCK_AND_LOAD(IDX)  sgm::tut::mdo << LOAD_CODE_BLOCK(IDX);

#define LOAD_DESCRIPTION_FILE(FILE)  sgm::tut::_Load_file(__MD_MATERIALS__ + "\\" + FILE)


namespace sgm::tut
{

	[[maybe_unused]] static auto HTML_tag(std::string const& contents, std::string const& tag)-> std::string;
	
	[[maybe_unused]] 
	static auto Load_image(std::string const& image_name, size_t const image_width = 0)-> std::string;

	[[maybe_unused]] static void is_True(bool const b) noexcept(false);
	[[maybe_unused]] static void is_False(bool const b) noexcept(false);

	class md_guard;
	class html_block_guard;
	class md_block_guard;



	class _tabless_description;
	class _code_description;

	class _MD_Stream;
	class _MD_Stream_Guard;

	[[maybe_unused]] static auto _Current_File_Directory(std::string s)-> std::string;
	[[maybe_unused]] static auto _Load_file(std::filesystem::path const& filepath) noexcept(false)-> std::string;
	[[maybe_unused]] static auto _Code_writing(std::string const& code, std::string const &lang = "")-> std::string;

	[[maybe_unused]] static auto _Load_code_block
	(	std::filesystem::path const& filepath, int code_block_index
	)	noexcept(false)->	std::string;

}
//========//========//========//========//=======#//========//========//========//========//=======#//========//========


void sgm::tut::is_True(bool const b) noexcept(false)
{
	if(!b)
		throw std::exception("tutorial assertion failed.");
}


void sgm::tut::is_False(bool const b) noexcept(false){  is_True(!b);  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


class sgm::tut::_tabless_description
{
public:
	_tabless_description(std::string&& s) : _str(  _tabless_string( std::move(s) )  ){}

private:
	friend class sgm::tut::_MD_Stream;

	std::string _str;

	static auto _tabless_string(std::string&& s)-> std::string;
};


[[maybe_unused]] static auto operator ""_mdo(char const* str, size_t)-> sgm::tut::_tabless_description
{
	return std::string(str);
}


auto sgm::tut::_tabless_description::_tabless_string(std::string&& str)-> std::string
{
	auto is_empty_line_f
	=	[](std::string const& line)-> bool
		{
			for(auto const c : line)
				if(c != ' ' && c != '\t' && c != '\n')
					return false;

			return true;
		};

	std::queue<std::string> qs;
	size_t total_str_len = 0;

	auto enqueue_f
	=	[&qs, &total_str_len, is_empty_line_f](auto itr1, auto itr2)
		{
			if( !is_empty_line_f({itr1, itr2}) )
				for(;  *itr1 == '\t';  ++itr1);

			std::string s(itr1, itr2);

			qs.emplace( std::move(s) );

			total_str_len += std::distance(itr1, itr2);
		};

	for(auto itr1 = str.cbegin(),  itr2 = itr1;  ;  ++itr2)
		if(itr2 == str.cend())
		{
			enqueue_f(itr1, itr2);

			break;
		}
		else if(*itr2 == '\n')
		{
			enqueue_f(itr1, itr2);

			itr1 = itr2 + 1;
		}


	std::string res;
	res.reserve(total_str_len + 2*qs.size());

	for(;  !qs.empty();  qs.pop())
		res.append(qs.front() + "  \n");
	
	return res;	
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


class sgm::tut::_code_description
{
public:
	_code_description(std::string&& s) : _str( _Code_writing(s) ){}

private:
	friend class sgm::tut::_MD_Stream;

	std::string _str;
};


[[maybe_unused]] static auto operator ""_code(char const *str, size_t)-> sgm::tut::_code_description
{
	return std::string(str);
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


class sgm::tut::_MD_Stream
{
public:
	_MD_Stream(_MD_Stream const&) = delete;
	auto operator=(_MD_Stream const&)-> _MD_Stream& = delete;

	static auto instance()-> _MD_Stream&
	{
		static _MD_Stream res;

		return res;
	}

	void open(std::string s)
	{
		assert(!is_open());

		auto dot_pos = s.find_last_of('.');

		s.erase(s.begin() + dot_pos+1, s.end());
		s.append("md");

		_filepath = std::move(s);
	}

	bool is_open() const{  return _filepath != std::string();  }

	void close(){  _filepath = {};  _contents = {};  }

	void print_and_close()
	{
		assert(is_open());

		for( std::ofstream ofs(_filepath);  !_contents.empty();  ofs << _contents.front(),  _contents.pop() );

		_filepath = {};
	}

	template<class T>
	auto operator<<(T&& t)-> _MD_Stream&
	{
		assert(is_open());

		using _T = std::decay_t<T>;
			
		if constexpr(std::is_convertible_v<_T, std::string>)
			_contents.push( std::forward<T>(t) );
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

private:
	_MD_Stream() = default;

	std::string _filepath = {};
	std::queue<std::string> _contents = {};
};


namespace sgm::tut
{
	_MD_Stream& mdo = _MD_Stream::instance();
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


class sgm::tut::_MD_Stream_Guard
{
public:
	_MD_Stream_Guard(std::string const filename)
	:	is_successful(true), filepath( (filename + ".md").c_str() ){  mdo.open(filename);  }
	
	~_MD_Stream_Guard()
	{
		if(is_successful)
			mdo.print_and_close();
		else
		{
			mdo.close();

			std::filesystem::remove(filepath);
		}
	}

	bool is_successful;
	std::filesystem::path filepath;
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


class sgm::tut::md_guard
{
public:
	md_guard(std::string begin) : md_guard(begin, begin){}
	md_guard(std::string begin, std::string end) : _end(end){  mdo << begin; }
	~md_guard(){  mdo << _end; }

private:
	std::string _end;
};


class sgm::tut::md_block_guard : public md_guard
{
public:
	md_block_guard(std::string s = "") : md_guard( std::string("```") + s + "\n", "```\n" ){}
};


class sgm::tut::html_block_guard
{
public:
	html_block_guard(std::string const& tags)
	{
		std::queue<std::string> q;

		for(auto itr1 = tags.cbegin(),  itr2 = itr1;  ;  ++itr2)
			if(itr2 == tags.cend())
			{
				q.emplace(itr1, itr2);

				break;
			}
			else if(*itr2 == ' ')
				q.emplace(itr1, itr2),  itr1 = itr2 + 1;


		for( _end.reserve(tags.size() + 2*q.size() + 1);  !q.empty();  q.pop() )
		{
			auto const& tag = q.front();

			mdo << _bracket(tag);
			_end.append( _bracket(std::string{'/'}+tag) );
		}
	}

	~html_block_guard(){  mdo << _end;  }

private:
	std::string _end = {};

	static auto _bracket(std::string const& s)-> std::string{  return std::string{'<'} + s + '>';  }
};
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


auto sgm::tut::HTML_tag(std::string const& contents, std::string const& tag)-> std::string
{
	std::queue<std::string> tags;

	for(auto itr1 = tag.cbegin(),  itr2 = itr1;  ;  ++itr2)
		if(itr2 == tag.cend())
		{
			tags.emplace(itr1, itr2);

			break;
		}
		else if(*itr2 == ' ')
			tags.emplace(itr1, itr2),  itr1 = itr2 + 1;  

	auto tag_f 
	=	[](std::string const& s, std::string const& t)
		{
			std::string const
				begin_str = std::string("<") + t + ">",
				end_str = std::string("</") + t + ">";
			
			return begin_str + s + end_str;
		};

	auto res = contents;

	for(;  !tags.empty();  tags.pop())
		res = tag_f(res, tags.front());
			
	return res;
}


auto sgm::tut::Load_image(std::string const& image_name, size_t const image_width)-> std::string
{
	auto const size_str
	=	image_width == 0 ? std::string("") : std::string(" width =\"") + std::to_string(image_width) + "\"";

	return std::string("<img src=\"") + "md_materials\\" + image_name + "\"" + size_str + ">";
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


auto sgm::tut::_Load_file(std::filesystem::path const& filepath) noexcept(false)-> std::string
{
	if( !std::filesystem::exists(filepath) )
	{
		std::wcerr << filepath << std::endl;

		throw std::exception("the file to be loaded doesn't exist.");
	}

	std::queue<std::string> qs;
	size_t nof_char = 0;
	std::ifstream file(filepath);

	for( std::string buf;  std::getline(file, buf);  qs.push(buf+"  \n"),  nof_char += buf.size() + 4 );

	std::string merged_str;
	
	for( merged_str.reserve(nof_char);  !qs.empty();  qs.pop() )
		merged_str.append(qs.front());

	return merged_str;
}


auto sgm::tut::_Load_code_block(std::filesystem::path const& filepath, int code_block_index) noexcept(false)
->	std::string
{
	if( !std::filesystem::exists(filepath) )
		throw std::exception("the file to be loaded doesn't exist.");

	std::ifstream file(filepath);

	std::string const
		cb_begin = std::string("BEGIN_CODE_BLOCK(") + std::to_string(code_block_index) + ")",
		cb_end = std::string("END_CODE_BLOCK(") + std::to_string(code_block_index) + ")",
		cb_end2 = std::string("END_CODE_BLOCK_AND_LOAD(") + std::to_string(code_block_index) + ")";

	auto are_same_str_f
	=	[](std::string const& s1, std::string const& s2, size_t const size)
		{
			bool res = s1.size() >= size && s2.size() >= size;

			for(size_t i = 0;  res && i < size;  res = s1[i] == s2[i],  ++i);

			return res;
		};

	std::queue<std::string> qs;
	size_t nof_char = 0;

	for(std::string buf;  std::getline(file, buf);  )
		if( are_same_str_f(buf, cb_begin, cb_begin.size()) )
			for
			(	std::getline(file, buf)
			;	!are_same_str_f(buf, cb_end, cb_end.size()) && !are_same_str_f(buf, cb_end2, cb_end2.size())
			;	std::getline(file, buf) 
			)
				qs.push(buf + "\n"),  
				nof_char += buf.size() + 1;

	std::string merged_str;
	
	for( merged_str.reserve(nof_char);  !qs.empty();  qs.pop() )
		merged_str.append(qs.front());

	return _Code_writing(merged_str, "cpp");
}


auto sgm::tut::_Code_writing(std::string const& str, std::string const& lang)-> std::string
{
	auto is_empty_line_f
	=	[](std::string const& line)-> bool
		{
			for(auto const c : line)
				if(c != ' ' && c != '\t' && c != '\n')
					return false;

			return true;
		};

	auto tab_count_f
	=	[](std::string const& line)-> size_t
		{
			size_t res = 0;

			for(auto const c : line)
				if(c == '\t')
					++res;
				else
					break;

			return res;
		};

	std::queue<std::string> qs;
	size_t total_str_len = 0,  min_nof_tab = std::numeric_limits<size_t>::max();

	auto enqueue_f
	=	[&qs, &total_str_len, &min_nof_tab, is_empty_line_f, tab_count_f](auto itr1, auto itr2)
		{
			std::string s(itr1, itr2);

			if( !is_empty_line_f(s) )
				min_nof_tab = std::min( min_nof_tab, tab_count_f(s) );

			qs.emplace( std::move(s) );

			total_str_len += std::distance(itr1, itr2);
		};

	for(auto itr1 = str.cbegin(),  itr2 = itr1;  ;  ++itr2)
		if(itr2 == str.cend())
		{
			enqueue_f(itr1, itr2);

			break;
		}
		else if(*itr2 == '\n')
		{
			enqueue_f(itr1, itr2);

			itr1 = itr2 + 1;
		}


	std::string res;
	res.reserve(8 + lang.size() + total_str_len + 2*qs.size());

	for(  res.append( std::string("```") + lang + "\n" );  !qs.empty();  qs.pop()  )
	{
		if( auto const& s = qs.front();  !is_empty_line_f(s) )
			res.append(s.cbegin() + min_nof_tab, s.cend());
		else
			res.append(s);

		res.append("  \n");
	}
	
	do	
		res.pop_back();
	while(res.back() != '\n');
	
	res.append("```\n");

	return res;
}


auto sgm::tut::_Current_File_Directory(std::string s)-> std::string
{
	auto const last_bs = s.find_last_of('\\');

	s.erase(s.begin()+last_bs, s.end());

	return s.c_str();
}
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
			std::wcout << L#TITLE << L" tutorial file print complete.\n";	\
		}	\
		catch(std::exception const& e)	\
		{	\
			std::wcerr << L#TITLE << L" tutorial file print failed : " << e.what() << std::endl;	\
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
#include "Tutorial.hpp"
#include <filesystem>
#include <fstream>
#include <queue>
#include <iostream>
#include <exception>
#include <cassert>
//========//========//========//========//=======#//========//========//========//========//=======#//========//========


auto operator ""_mdo(char const* str, size_t)-> sgm::tut::_tabless_description{  return std::string(str);  }
auto operator ""_code(char const* str, size_t)-> sgm::tut::_code_description{  return std::string(str);  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


void sgm::tut::is_True(bool const b) noexcept(false)
{
	if(!b)
		throw std::exception("tutorial assertion failed.");
}


void sgm::tut::is_False(bool const b) noexcept(false){  is_True(!b);  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


struct sgm::tut::_MD_Stream::_Contents{  std::queue<std::string> q = {};  };


sgm::tut::_MD_Stream::_MD_Stream() : _filepath(), _pcnts(new _Contents()){}
sgm::tut::_MD_Stream::~_MD_Stream(){  delete _pcnts;  }


auto sgm::tut::_MD_Stream::instance()-> _MD_Stream&
{
	static _MD_Stream res;

	return res;
}


void sgm::tut::_MD_Stream::open(std::string s)
{
	assert(!is_open());

	auto dot_pos = s.find_last_of('.');

	s.erase(s.begin() + dot_pos+1, s.end());
	s.append("md");

	_filepath = std::move(s);
}


bool sgm::tut::_MD_Stream::is_open() const{  return _filepath != std::string();  }
void sgm::tut::_MD_Stream::close(){  _filepath = {};  _pcnts->q = {};  }

void sgm::tut::_MD_Stream::print_and_close()
{
	assert(is_open());

	for( std::ofstream ofs(_filepath);  !_pcnts->q.empty();  ofs << _pcnts->q.front(),  _pcnts->q.pop() );

	_filepath = {};
}


void sgm::tut::_MD_Stream::_push(std::string const& str)
{
	assert(is_open());
	
	_pcnts->q.push(str);  
}

void sgm::tut::_MD_Stream::_push(std::string&& str)
{
	assert(is_open());
	
	_pcnts->q.push( std::move(str) );  
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


sgm::tut::_MD_Stream_Guard::_MD_Stream_Guard(std::string const filename)
:	is_successful(true), filepath( (filename + ".md").c_str() ){  mdo.open(filename);  }


sgm::tut::_MD_Stream_Guard::~_MD_Stream_Guard()
{
	if(is_successful)
		mdo.print_and_close();
	else
	{
		mdo.close();

		std::filesystem::remove( static_cast<std::filesystem::path>(filepath) );
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


sgm::tut::md_guard::md_guard(std::string begin) : md_guard(begin, begin){}
sgm::tut::md_guard::md_guard(std::string begin, std::string end) : _end(end){  mdo << begin; }
sgm::tut::md_guard::~md_guard(){  mdo << _end; }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


sgm::tut::md_block_guard::md_block_guard(std::string s) : md_guard( std::string("```") + s + "\n", "```\n" ){}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


sgm::tut::html_block_guard::html_block_guard(std::string const& tags)
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


sgm::tut::html_block_guard::~html_block_guard(){  mdo << _end;  }

auto sgm::tut::html_block_guard::_bracket(std::string const& s)-> std::string{  return std::string{'<'} + s + '>';  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


static auto _is_empty_line(std::string const& line)-> bool
{
	for(auto const c : line)
		if(c != ' ' && c != '\t' && c != '\n')
			return false;

	return true;	
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#//--------//--------


sgm::tut::_tabless_description::_tabless_description(std::string&& s) : _str(  _tabless_string( std::move(s) )  ){}


auto sgm::tut::_tabless_description::_tabless_string(std::string&& str)-> std::string
{
	std::queue<std::string> qs;
	size_t total_str_len = 0;

	auto enqueue_f
	=	[&qs, &total_str_len](auto itr1, auto itr2)
		{
			if( !_is_empty_line({itr1, itr2}) )
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


sgm::tut::_code_description::_code_description(std::string&& s) : _str( _Code_writing(s) ){}
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


auto sgm::tut::_Load_code_block(std::string const& filepath, int code_block_index) noexcept(false)-> std::string
{
	if(  !std::filesystem::exists( static_cast<std::filesystem::path>(filepath) )  )
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


auto sgm::tut::_Load_file(std::string const& filepath) noexcept(false)-> std::string
{
	if(  !std::filesystem::exists( static_cast<std::filesystem::path>(filepath) )  )
		throw std::exception("the file to be loaded doesn't exist.");
	
	std::queue<std::string> qs;
	size_t nof_char = 0;
	std::ifstream file(filepath);

	for( std::string buf;  std::getline(file, buf);  qs.push(buf+"  \n"),  nof_char += buf.size() + 4 );

	std::string merged_str;
	
	for( merged_str.reserve(nof_char);  !qs.empty();  qs.pop() )
		merged_str.append(qs.front());

	return merged_str;
}


auto sgm::tut::_Code_writing(std::string const& str, std::string const& lang)-> std::string
{
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
	=	[&qs, &total_str_len, &min_nof_tab, tab_count_f](auto itr1, auto itr2)
		{
			std::string s(itr1, itr2);

			if( !_is_empty_line(s) )
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
		if( auto const& s = qs.front();  !_is_empty_line(s) )
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


void sgm::tut::_Print_Message(std::string&& msg){  std::cout << std::move(msg);  }
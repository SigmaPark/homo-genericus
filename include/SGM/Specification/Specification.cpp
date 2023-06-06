/*  SPDX-FileCopyrightText: (c) 2021 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Specification.hpp"
#include <fstream>
#include <queue>


using std::size_t;
using std::string;
using dir_t = string;


auto operator ""_mdo(char const* str, size_t)
->	sgm::spec::_tabless_description{  return string(str);  }


auto operator ""_code(char const* str, size_t)
->	sgm::spec::_code_description{  return string(str);  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


struct sgm::spec::_MD_Stream::_Contents{  std::queue<string> q = {};  };


sgm::spec::_MD_Stream::_MD_Stream() 
:	_working_filepath(), _md_filepath(), _md_materials_dir(), _pcnts(new _Contents()){}

sgm::spec::_MD_Stream::~_MD_Stream(){  delete _pcnts;  }


auto sgm::spec::_MD_Stream::instance()-> _MD_Stream&
{
	static _MD_Stream res;

	return res;
}


void sgm::spec::_MD_Stream::open(dir_t const working_filepath)
{
	if(is_open())
		return;

	_working_filepath = working_filepath;

	_md_materials_dir
	=	[](dir_t str)
		{
			auto const last_slash = str.find_last_of('/');
		
			str.erase(str.begin() + last_slash, str.end());

			return str + "/md_materials";
		}(working_filepath);
 
	_md_filepath
	=	[](dir_t str)
		{
			auto const last_dot = str.find_last_of('.');
		
			str.erase(str.begin() + last_dot, str.end());

			auto const last_slash = str.find_last_of('/');
			
			dir_t const direc(str.begin(), str.begin() + last_slash + 1);
			string const name(str.begin() + last_slash + 1, str.end());
			
			return direc + "[guide]_" + name + ".md";
		}(working_filepath);
}


bool sgm::spec::_MD_Stream::is_open() const{  return _md_filepath != dir_t();  }

void sgm::spec::_MD_Stream::close()
{
	_working_filepath = _md_filepath = _md_materials_dir = {};  

	_pcnts->q = {};  
}

auto sgm::spec::_MD_Stream::ever_used() const-> bool{  return !_pcnts->q.empty();  }

auto sgm::spec::_MD_Stream::working_filepath() const
->	dir_t const&{  return _working_filepath;  }

auto sgm::spec::_MD_Stream::md_filepath() const-> dir_t const&{  return _md_filepath;  }

auto sgm::spec::_MD_Stream::md_materials_dir() const
->	dir_t const&{  return _md_materials_dir;  }


void sgm::spec::_MD_Stream::print_and_close()
{
	if(!is_open())
		return;

	for
	(	std::ofstream ofs(_md_filepath)
	;	!_pcnts->q.empty()
	;	ofs << _pcnts->q.front(),  _pcnts->q.pop() 
	);

	close();
}


void sgm::spec::_MD_Stream::_push(string const& str)
{
	_pcnts->q.push(str);  
}

void sgm::spec::_MD_Stream::_push(string&& str)
{
	_pcnts->q.push( Move(str) );  
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


sgm::spec::_MD_Stream_Guard::_MD_Stream_Guard(dir_t working_filepath) : is_successful(true)
{
	for(auto& c : working_filepath)
		if(c == '\\')
			c = '/';

	mdo->open( Move(working_filepath) ); 
}


sgm::spec::_MD_Stream_Guard::~_MD_Stream_Guard()
{
	if(is_successful && mdo->ever_used())
		mdo->print_and_close();
	else
	{
		std::remove(mdo->md_filepath().c_str());
		
		mdo->close();
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


sgm::spec::md_guard::md_guard(string begin) : md_guard(begin, begin){}
sgm::spec::md_guard::md_guard(string begin, string end) : _end(end){  mdo << begin; }
sgm::spec::md_guard::~md_guard(){  mdo << _end; }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


sgm::spec::md_block_guard::md_block_guard(string s) 
:	md_guard( string("```") + s + "\n", "```\n" ){}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


sgm::spec::html_block_guard::html_block_guard(string const& tags)
{
	std::queue<string> q;

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
		_end.append( _bracket(string{'/'}+tag) );
	}
}


sgm::spec::html_block_guard::~html_block_guard(){  mdo << _end;  }

auto sgm::spec::html_block_guard::_bracket(string const& s)
->	string{  return string{'<'} + s + '>';  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static auto _is_empty_line(string const& line)-> bool
{
	for(auto const c : line)
		if(c != ' ' && c != '\t' && c != '\n')
			return false;

	return true;	
}


static auto _file_exists(dir_t const& filepath)-> bool
{
	return std::ifstream(filepath).is_open();
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


sgm::spec::_tabless_description::_tabless_description(string&& s) 
:	_str(  _tabless_string( Move(s) )  ){}


auto sgm::spec::_tabless_description::_tabless_string(string&& str)-> string
{
	std::queue<string> qs;
	size_t total_str_len = 0;

	using str_itr_t = string::const_iterator;

	auto enqueue_f
	=	[&qs, &total_str_len](str_itr_t itr1, str_itr_t itr2)
		{
			if( !_is_empty_line({itr1, itr2}) )
				for(;  *itr1 == '\t';  ++itr1);

			string s(itr1, itr2);

			qs.emplace( Move(s) );

			total_str_len += Difference(itr1, itr2);
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


	for ( ;  _is_empty_line(qs.front());  qs.pop() );

	string res;
	res.reserve(total_str_len + 2*qs.size());

	for(;  !qs.empty();  qs.pop())
		res.append(qs.front() + "  \n");
	
	return res;	
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


sgm::spec::_code_description::_code_description(string&& s) : _str( _Code_writing(s) ){}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


auto sgm::spec::HTML_tag(string const& contents, string const& tag)-> string
{
	std::queue<string> tags;

	for(auto itr1 = tag.cbegin(),  itr2 = itr1;  ;  ++itr2)
		if(itr2 == tag.cend())
		{
			tags.emplace(itr1, itr2);

			break;
		}
		else if(*itr2 == ' ')
			tags.emplace(itr1, itr2),  itr1 = itr2 + 1;  

	auto tag_f 
	=	[](string const& s, string const& t)
		{
			string const
				begin_str = string("<") + t + ">",
				end_str = string("</") + t + ">";
			
			return begin_str + s + end_str;
		};

	auto res = contents;

	for(;  !tags.empty();  tags.pop())
		res = tag_f(res, tags.front());
			
	return res;
}


auto sgm::spec::Load_image(string const& image_name, size_t const image_width)-> string
{
	if( !::_file_exists(mdo->md_materials_dir() + '/' + image_name) )
		throw sgm::Exception("Cannot find the image file in ./md_materials directory.");

	auto const size_str
	=	image_width == 0 
		?	string("") 
		:	string(" width =\"") + std::to_string(image_width) + "\"";

	return string("<img src=\"") + "./md_materials/" + image_name + "\"" + size_str + ">";
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


auto sgm::spec::Empty_lines(size_t nof_el)-> string
{
	string const nbsp = "&nbsp;  \n";
	string spaces;

	for( spaces.reserve(nof_el*nbsp.size());  nof_el-->0;  spaces.append(nbsp) );

	return string("\n\n") + spaces + "\n";
}


auto sgm::spec::Title(string const& title, unsigned const level)-> string
{
	string sharps{};

	for(auto d = level;  d-->0;  sharps += '#');

	return sharps + ' ' + title + '\n';
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


auto sgm::spec::Load_code_block(string const code_block_tag) noexcept(false)-> string
{
	if( !::_file_exists(mdo->working_filepath()) )
		throw Exception("the file to be loaded doesn't exist.");

	std::ifstream file(mdo->working_filepath());

	string const
		cb_begin = string("BEGIN_CODE_BLOCK(") + code_block_tag + ")",
		cb_end = string("END_CODE_BLOCK(") + code_block_tag + ")",
		cb_end2 = string("END_CODE_BLOCK_AND_LOAD(") + code_block_tag + ")";

	auto trimmed_str_f
	=	[](string const& s)-> string
		{
			if( s.empty() || _is_empty_line(s) )
				return s;
			
			auto fitr = s.cbegin();
			auto bitr = Prev(s.cend());

			for(;  *fitr == ' ' || *fitr == '\t';  ++fitr);
			for(;  *bitr == ' ' || *bitr == '\t';  --bitr);

			return {fitr, ++bitr};
		};

	auto are_same_str_f
	=	[](string const& s1, string const& s2, size_t const size)
		{
			bool res = s1.size() >= size && s2.size() >= size;

			for(size_t i = 0;  res && i < size;  res = s1[i] == s2[i],  ++i);

			return res;
		};


	std::queue<string> qs;
	size_t nof_char = 0;

	for(string buf;  std::getline(file, buf);  )
		if(  are_same_str_f( trimmed_str_f(buf), cb_begin, cb_begin.size() )  )
			for
			(	std::getline(file, buf)
			;	(	!are_same_str_f( trimmed_str_f(buf), cb_end, cb_end.size() ) 
				&&	!are_same_str_f( trimmed_str_f(buf), cb_end2, cb_end2.size() )
				)
			;	std::getline(file, buf) 
			)
				qs.push(buf + "\n"),  
				nof_char += buf.size() + 1;

	string merged_str;
	
	for( merged_str.reserve(nof_char);  !qs.empty();  qs.pop() )
		merged_str.append(qs.front());

	return _Code_writing(merged_str, "cpp");
}


auto sgm::spec::Load_description_file(string const& filename) noexcept(false)-> string
{
	auto const filepath = mdo->md_materials_dir() + '/' + filename;


	if( !::_file_exists(filepath) )
		throw sgm::Exception("Cannot find the file in ./md_materials directory.");
	
	std::queue<string> qs;
	size_t nof_char = 0;
	std::ifstream file(filepath);

	for
	(	string buf
	;	std::getline(file, buf)
	;	qs.push(buf+"  \n"),  nof_char += buf.size() + 4 
	);

	string merged_str;
	
	for( merged_str.reserve(nof_char);  !qs.empty();  qs.pop() )
		merged_str.append(qs.front());

	return merged_str;
}


auto sgm::spec::_Code_writing(string const& str, string const& lang)-> string
{
	auto tab_count_f
	=	[](string const& line)-> size_t
		{
			size_t res = 0;

			for(auto const c : line)
				if(c == '\t')
					++res;
				else
					break;

			return res;
		};


	size_t constexpr max_nof_tabs = 1024;

	std::queue<string> qs;
	size_t total_str_len = 0,  min_nof_tab = max_nof_tabs;

	using str_itr_t = string::const_iterator;

	auto enqueue_f
	=	[&qs, &total_str_len, &min_nof_tab, tab_count_f](str_itr_t itr1, str_itr_t itr2)
		{
			auto min_f 
			=	[](size_t _1, size_t _2) noexcept-> size_t{  return _1 < _2 ? _1 : _2;  };

			string s(itr1, itr2);

			if( !_is_empty_line(s) )
				min_nof_tab = min_f( min_nof_tab, tab_count_f(s) );

			qs.emplace( Move(s) );

			total_str_len += Difference(itr1, itr2);
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


	for( ;  _is_empty_line(qs.front());  qs.pop() );


	string res;
	res.reserve(8 + lang.size() + total_str_len + 2*qs.size());

	for(  res.append( string("```") + lang + "\n" );  !qs.empty();  qs.pop()  )
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
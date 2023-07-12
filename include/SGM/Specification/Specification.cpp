/*  SPDX-FileCopyrightText: (c) 2021 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Specification.hpp"
#include <fstream>
#include <cstdio>
#include <queue>


using std::size_t;
using std::wstring;
using dir_t = wstring;


auto operator ""_mdo(wchar_t const* str, size_t)
->	sgm::spec::_tabless_description{  return wstring(str);  }


auto operator ""_code(wchar_t const* str, size_t)
->	sgm::spec::_code_description{  return wstring(str);  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


struct sgm::spec::_MD_Stream::_Contents{  std::queue<wstring> q = {};  };


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
			auto const last_slash = str.find_last_of(L'/');
		
			str.erase(str.begin() + last_slash, str.end());

			return str + L"/md_materials";
		}(working_filepath);
 
	_md_filepath
	=	[](dir_t str)
		{
			auto const last_dot = str.find_last_of(L'.');
		
			str.erase(str.begin() + last_dot, str.end());

			auto const last_slash = str.find_last_of(L'/');
			
			dir_t const direc(str.begin(), str.begin() + last_slash + 1);
			wstring const name(str.begin() + last_slash + 1, str.end());
			
			return direc + L"[guide]_" + name + L".md";
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
	(	std::wofstream ofs(_md_filepath)
	;	!_pcnts->q.empty()
	;	ofs << _pcnts->q.front(),  _pcnts->q.pop() 
	);

	close();
}


void sgm::spec::_MD_Stream::_push(wstring const& str)
{
	_pcnts->q.push(str);  
}

void sgm::spec::_MD_Stream::_push(wstring&& str)
{
	_pcnts->q.push( Move(str) );  
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static auto Mbs_to_Wcs(std::string const& s)-> std::wstring
{
	std::size_t constexpr buf_size = 0x1000;
	std::size_t n = 0;
	wchar_t buf[buf_size];


	mbstowcs_s(&n, buf, buf_size, s.c_str(), buf_size);

	return buf;
}


sgm::spec::_MD_Stream_Guard::_MD_Stream_Guard(dir_t working_filepath) : is_successful(true)
{
	for(auto& c : working_filepath)
		if(c == L'\\')
			c = L'/';

	mdo->open( Move(working_filepath) ); 
}


sgm::spec::_MD_Stream_Guard::_MD_Stream_Guard(std::string working_filepath)
:	_MD_Stream_Guard( ::Mbs_to_Wcs(working_filepath) ){}


sgm::spec::_MD_Stream_Guard::~_MD_Stream_Guard()
{
	if(is_successful && mdo->ever_used())
		mdo->print_and_close();
	else
	{
		size_t constexpr buf_size = 0x1000;
		size_t n = 0;
		char str[buf_size];

		wcstombs_s(&n, str, buf_size, mdo->md_filepath().c_str(), buf_size);

		std::remove(str);
		
		mdo->close();
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


sgm::spec::md_guard::md_guard(wstring begin) : md_guard(begin, begin){}
sgm::spec::md_guard::md_guard(wstring begin, wstring end) : _end(end){  mdo << begin; }
sgm::spec::md_guard::~md_guard(){  mdo << _end; }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


sgm::spec::md_block_guard::md_block_guard(wstring s) 
:	md_guard( wstring(L"```") + s + L"\n", L"```\n" ){}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


sgm::spec::html_block_guard::html_block_guard(wstring const& tags)
{
	std::queue<wstring> q;

	for(auto itr1 = tags.cbegin(),  itr2 = itr1;  ;  ++itr2)
		if(itr2 == tags.cend())
		{
			q.emplace(itr1, itr2);

			break;
		}
		else if(*itr2 == L' ')
			q.emplace(itr1, itr2),  itr1 = itr2 + 1;


	for( _end.reserve(tags.size() + 2*q.size() + 1);  !q.empty();  q.pop() )
	{
		auto const& tag = q.front();

		mdo << _bracket(tag);
		_end.append( _bracket(wstring{L'/'}+tag) );
	}
}


sgm::spec::html_block_guard::~html_block_guard(){  mdo << _end;  }

auto sgm::spec::html_block_guard::_bracket(wstring const& s)
->	wstring{  return wstring{L'<'} + s + L'>';  }
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


static auto _is_empty_line(wstring const& line)-> bool
{
	for(auto const c : line)
		if(c != L' ' && c != L'\t' && c != L'\n')
			return false;

	return true;	
}


static auto _file_exists(dir_t const& filepath)-> bool
{
	return std::ifstream(filepath).is_open();
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


sgm::spec::_tabless_description::_tabless_description(wstring&& s) 
:	_str(  _tabless_string( Move(s) )  ){}


auto sgm::spec::_tabless_description::_tabless_string(wstring&& str)-> wstring
{
	std::queue<wstring> qs;
	size_t total_str_len = 0;

	using str_itr_t = wstring::const_iterator;

	auto enqueue_f
	=	[&qs, &total_str_len](str_itr_t itr1, str_itr_t itr2)
		{
			if( !_is_empty_line({itr1, itr2}) )
				for(;  *itr1 == L'\t';  ++itr1);

			wstring s(itr1, itr2);

			qs.emplace( Move(s) );

			total_str_len += Difference(itr1, itr2);
		};

	for(auto itr1 = str.cbegin(),  itr2 = itr1;  ;  ++itr2)
		if(itr2 == str.cend())
		{
			enqueue_f(itr1, itr2);

			break;
		}
		else if(*itr2 == L'\n')
		{
			enqueue_f(itr1, itr2);

			itr1 = itr2 + 1;
		}


	for ( ;  _is_empty_line(qs.front());  qs.pop() );

	wstring res;
	res.reserve(total_str_len + 2*qs.size());

	for(;  !qs.empty();  qs.pop())
		res.append(qs.front() + L"  \n");
	
	return res;	
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


sgm::spec::_code_description::_code_description(wstring&& s) : _str( _Code_writing(s) ){}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


auto sgm::spec::HTML_tag(wstring const& contents, wstring const& tag)-> wstring
{
	std::queue<wstring> tags;

	for(auto itr1 = tag.cbegin(),  itr2 = itr1;  ;  ++itr2)
		if(itr2 == tag.cend())
		{
			tags.emplace(itr1, itr2);

			break;
		}
		else if(*itr2 == L' ')
			tags.emplace(itr1, itr2),  itr1 = itr2 + 1;  

	auto tag_f 
	=	[](wstring const& s, wstring const& t)
		{
			wstring const
				begin_str = wstring(L"<") + t + L">",
				end_str = wstring(L"</") + t + L">";
			
			return begin_str + s + end_str;
		};

	auto res = contents;

	for(;  !tags.empty();  tags.pop())
		res = tag_f(res, tags.front());
			
	return res;
}


auto sgm::spec::Load_image(wstring const& image_name, size_t const image_width)-> wstring
{
	if( !::_file_exists(mdo->md_materials_dir() + L'/' + image_name) )
		throw sgm::Exception(L"Cannot find the image file in ./md_materials directory.");

	auto const size_str
	=	image_width == 0 
		?	wstring(L"") 
		:	wstring(L" width =\"") + std::to_wstring(image_width) + L"\"";

	return wstring(L"<img src=\"") + L"./md_materials/" + image_name + L"\"" + size_str + L">";
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


auto sgm::spec::Empty_lines(size_t nof_el)-> wstring
{
	wstring const nbsp = L"&nbsp;  \n";
	wstring spaces;

	for( spaces.reserve(nof_el*nbsp.size());  nof_el-->0;  spaces.append(nbsp) );

	return wstring(L"\n\n") + spaces + L"\n";
}


auto sgm::spec::Title(wstring const& title, unsigned const level)-> wstring
{
	wstring sharps{};

	for(auto d = level;  d-->0;  sharps += L'#');

	return sharps + L' ' + title + L'\n';
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


auto sgm::spec::Load_code_block(wstring const code_block_tag) noexcept(false)-> wstring
{
	if( !::_file_exists(mdo->working_filepath()) )
		throw Exception(L"the file to be loaded doesn't exist.");

	std::wifstream file(mdo->working_filepath());

	wstring const
		cb_begin = wstring(L"BEGIN_CODE_BLOCK(") + code_block_tag + L")",
		cb_end = wstring(L"END_CODE_BLOCK(") + code_block_tag + L")",
		cb_end2 = wstring(L"END_CODE_BLOCK_AND_LOAD(") + code_block_tag + L")";

	auto trimmed_str_f
	=	[](wstring const& s)-> wstring
		{
			if( s.empty() || _is_empty_line(s) )
				return s;
			
			auto fitr = s.cbegin();
			auto bitr = Prev(s.cend());

			for(;  *fitr == L' ' || *fitr == L'\t';  ++fitr);
			for(;  *bitr == L' ' || *bitr == L'\t';  --bitr);

			return {fitr, ++bitr};
		};

	auto are_same_str_f
	=	[](wstring const& s1, wstring const& s2, size_t const size)
		{
			bool res = s1.size() >= size && s2.size() >= size;

			for(size_t i = 0;  res && i < size;  res = s1[i] == s2[i],  ++i);

			return res;
		};


	std::queue<wstring> qs;
	size_t nof_char = 0;

	for(wstring buf;  std::getline(file, buf);  )
		if(  are_same_str_f( trimmed_str_f(buf), cb_begin, cb_begin.size() )  )
			for
			(	std::getline(file, buf)
			;	(	!are_same_str_f( trimmed_str_f(buf), cb_end, cb_end.size() ) 
				&&	!are_same_str_f( trimmed_str_f(buf), cb_end2, cb_end2.size() )
				)
			;	std::getline(file, buf) 
			)
				qs.push(buf + L"\n"),  
				nof_char += buf.size() + 1;

	wstring merged_str;
	
	for( merged_str.reserve(nof_char);  !qs.empty();  qs.pop() )
		merged_str.append(qs.front());

	return _Code_writing(merged_str, L"cpp");
}


auto sgm::spec::Load_description_file(wstring const& filename) noexcept(false)-> wstring
{
	auto const filepath = mdo->md_materials_dir() + L'/' + filename;


	if( !::_file_exists(filepath) )
		throw sgm::Exception(L"Cannot find the file in ./md_materials directory.");
	
	std::queue<wstring> qs;
	size_t nof_char = 0;
	std::wifstream file(filepath);

	for
	(	wstring buf
	;	std::getline(file, buf)
	;	qs.push(buf+L"  \n"),  nof_char += buf.size() + 4 
	);

	wstring merged_str;
	
	for( merged_str.reserve(nof_char);  !qs.empty();  qs.pop() )
		merged_str.append(qs.front());

	return merged_str;
}


auto sgm::spec::_Code_writing(wstring const& str, wstring const& lang)-> wstring
{
	auto tab_count_f
	=	[](wstring const& line)-> size_t
		{
			size_t res = 0;

			for(auto const c : line)
				if(c == L'\t')
					++res;
				else
					break;

			return res;
		};


	size_t constexpr max_nof_tabs = 1024;

	std::queue<wstring> qs;
	size_t total_str_len = 0,  min_nof_tab = max_nof_tabs;

	using str_itr_t = wstring::const_iterator;

	auto enqueue_f
	=	[&qs, &total_str_len, &min_nof_tab, tab_count_f](str_itr_t itr1, str_itr_t itr2)
		{
			auto min_f 
			=	[](size_t _1, size_t _2) noexcept-> size_t{  return _1 < _2 ? _1 : _2;  };

			wstring s(itr1, itr2);

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
		else if(*itr2 == L'\n')
		{
			enqueue_f(itr1, itr2);

			itr1 = itr2 + 1;
		}


	for( ;  _is_empty_line(qs.front());  qs.pop() );


	wstring res;
	res.reserve(8 + lang.size() + total_str_len + 2*qs.size());

	for(  res.append( wstring(L"```") + lang + L"\n" );  !qs.empty();  qs.pop()  )
	{
		if( auto const& s = qs.front();  !_is_empty_line(s) )
			res.append(s.cbegin() + min_nof_tab, s.cend());
		else
			res.append(s);

		res.append(L"  \n");
	}
	
	do	
		res.pop_back();
	while(res.back() != L'\n');
	
	res.append(L"```\n");

	return res;
}
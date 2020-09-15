#include "ioPLY.hpp"
#include <fstream>

using namespace sgm::ply;
using sgm::Serial;


class ASCII_Loader::impl
{
private:
	bool _loaded;
	char const* _file_path;
	Serial< Serial<double> > _positions, _normals;
	Serial< Serial<unsigned char> > _colors;
	Serial< Serial<size_t> > _faces;


	void read_header(std::ifstream& ifs);


public:
	impl(char const* const str) 
	:	_loaded(false), _file_path(str), _positions(), _normals(), _colors(), _faces()
	{}

	auto positions() const-> decltype(_positions) const&{  return _positions;  }
	auto normals() const-> decltype(_normals) const&{  return _normals;  }
	auto colors() const-> decltype(_colors) const&{  return _colors;  }
	auto faces() const-> decltype(_faces) const&{  return _faces;  }
	auto loaded() const-> decltype(_loaded) const&{  return _loaded;  }

	void file_path(char const* const str)
	{
		if (str != _file_path)
			*this = impl(str);
	}

	auto nofVertices() const-> size_t{  return positions().size();  }
	auto nofFaces() const-> size_t{  return faces().size();  }
};


static auto _1st_word(std::string str)-> char const*
{
	auto const _1st_char_pos = str.find_first_not_of(' ');

	str.erase(0, _1st_char_pos);

	auto const _1st_space_pos = str.find_first_of(' ');

	str.erase(str.begin() + _1st_space_pos, str.end());

	return str.c_str();
}


static auto last_word(std::string const& str)-> char const*
{
	std::string const reversal_num = _1st_word( std::string(str.crbegin(), str.crend()) );

	return std::string(reversal_num.crbegin(), reversal_num.crend()).c_str();
}


static size_t last_number(std::string const& str)
{
	return std::atoi( last_word(str) );
}


static bool has_word(std::string const& str, char const* word)
{
	auto const pos = str.find(word);

	return pos != std::string::npos;
}


void ASCII_Loader::impl::read_header(std::ifstream& ifs)
{
	enum{MAX_LINE_LENGTH = 1000};
	char str[MAX_LINE_LENGTH];
	
	while(!ifs.eof())
	{
		ifs.getline(str, MAX_LINE_LENGTH);

		char const* w1 = _1st_word(str);

		if (w1 == "end_header")
			break;
		else if (w1 == "element")
			if ( has_word(str, "vertex") )
				_positions = decltype(_positions)( last_number(str) );
			else if ( has_word(str, "face") )
				_faces = decltype(_faces)( last_number(str) );
			else assert(false && "no suitable method was found.\n");
		else if (w1 == "property")
		{
			if ( has_word(str, "nx") || has_word(str, "ny") || has_word(str, "nz") )
				_normals.is_null() 
				?	_normals = decltype(_normals)(nofVertices())
				:	_normals;
			else if ( has_word(str, "red") || has_word(str, "green") || has_word(str, "blue") )
				_colors.is_null()
				?	_colors = decltype(_colors)(nofVertices())
				:	_colors;
			else if ( has_word(str, "alpha") )
				_colors >> Serial<unsigned char>(4);
			else if ( has_word(str, "vertex_index") )
				_faces = decltype(_faces)(nofFaces());
			else assert(false && "no suitable method was found.\n");
		}
	}
}
//--------//--------//--------//--------//-------#//--------//--------//--------//--------//-------#


ASCII_Loader::ASCII_Loader(char const* const str) : _pimpl( new impl(str) ){}
ASCII_Loader::~ASCII_Loader(){  delete _pimpl,  _pimpl = nullptr;  }


auto ASCII_Loader::nofVertices() const-> size_t{  return positions().capacity();  }
auto ASCII_Loader::nofFaces() const-> size_t{  return faces().capacity();  }

auto ASCII_Loader::positions() const
->	Serial< Serial<double> > const&{  return _pimpl->positions();  }

auto ASCII_Loader::normals() const
->	Serial< Serial<double> > const&{  return _pimpl->normals();  }

auto ASCII_Loader::colors() const
->	Serial< Serial<unsigned char> > const&{  return _pimpl->colors();  }

auto ASCII_Loader::faces() const
->	Serial< Serial<size_t> > const&{  return _pimpl->faces();  }

bool ASCII_Loader::loaded() const{  return _pimpl->loaded();  }

bool ASCII_Loader::has_normal() const{  return !normals().is_null();  }
bool ASCII_Loader::has_color() const{  return !colors().is_null();  }
bool ASCII_Loader::has_alpha() const{  return has_color() && colors().front().capacity() == 4;  }
bool ASCII_Loader::has_face() const{  return faces().is_null();  }


auto ASCII_Loader::file_path(char const* const str)-> ASCII_Loader&
{
	_pimpl->file_path(str);

	return *this;
}




bool ASCII_Loader::load()
{
	//std::ifstream ifs;
	//ifs.open(_file_path);

	//if (!ifs.is_open())
	//	return _loaded = false;
	return true;

}
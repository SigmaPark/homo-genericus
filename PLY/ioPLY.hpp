#pragma once

#ifndef _SGM_PLY_
#define _SGM_PLY_


namespace sgm
{
	namespace ply
	{
		class ASCII_Recorder;
		class ASCII_Loader;
	}
}




class sgm::ply::ASCII_Loader
{
private:
	class impl;
	impl* _pimpl;


public:
	ASCII_Loader(char const* const str = nullptr);
	~ASCII_Loader();

	auto nofVertices() const-> size_t;
	auto nofFaces() const-> size_t;

	auto positions() const-> double const*;
	auto normals() const-> double const*;
	auto RGB() const-> unsigned char const*;
	auto RGBA() const-> unsigned char const*;
	auto faces() const-> size_t const*;

	auto file_path(char const* const str)-> ASCII_Loader&;
	bool load();
};


class sgm::ply::ASCII_Recorder
{
private:
	class impl;
	impl* _pimpl;


public:
	ASCII_Recorder
	(	char const* const str = nullptr
	,	size_t const nof_vertices = 0, size_t const nof_faces = 0
	);

	~ASCII_Recorder();

	auto nofVertices(size_t const nof_vertices)-> ASCII_Recorder&;
	auto nofFaces(size_t const nof_faces)-> ASCII_Recorder&;

	auto positions(double const* const pPos)-> ASCII_Recorder&;
	auto normals(double const* const pNml)-> ASCII_Recorder&;
	auto RGB(unsigned char const* const pClr)-> ASCII_Recorder&;
	auto RGBA(unsigned char const* const pClr)-> ASCII_Recorder&;
	auto faces(size_t const* const pFaces)-> ASCII_Recorder&;

	auto file_path(char const* const str)-> ASCII_Recorder&;
	bool record() const;
};


#endif
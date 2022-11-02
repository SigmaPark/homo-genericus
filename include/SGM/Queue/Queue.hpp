/*  SPDX-FileCopyrightText: (c) 2022 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "../List/List.hpp"


namespace sgm
{

	template<class T>
	class Queue;

}
//========//========//========//========//=======#//========//========//========//========//=======#


template<class T>
class sgm::Queue
{
public:
	auto size() const-> size_t{  return _list.size();  }

	auto front() const-> T const&{  return _list.front();  }
	auto back() const-> T const&{  return _list.back();  }

	template<class...ARGS>
	auto push(ARGS&&...args)
	->	Queue&{  return _list.emplace_back( Forward<ARGS>(args)... ),  *this;  }

	auto pop()-> Queue&{  return _list.pop_front(),  *this;  }

	auto is_empty() const noexcept-> bool{  return _list.is_empty();  }


private:
	List<T> _list;
};
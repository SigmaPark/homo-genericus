#pragma once

#ifndef _SGM_SINGLETON_
#define _SGM_SINGLETON_

#if defined(_MSC_VER) && _MSC_VER < 1914
	#error C++17 or higher version language support is required.
#endif


#include <memory>
#include <mutex>
#include <condition_variable>
#include <type_traits>
#include <execution>

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

namespace sgm
{

	template<typename T>
	class Singleton 
	{
	public:
		template<typename...ARGS>
		static void init(ARGS&&...args);

		static bool empty();

		template<typename POL = std::execution::sequenced_policy>
		static auto read(POL = std::execution::seq)-> T const&;

		template<typename _T = nullptr_t, typename POL = std::execution::sequenced_policy>
		static decltype(auto) write(_T&& _t = nullptr, POL = std::execution::seq);

		static void clear();

		Singleton(Singleton const&) = delete;
		auto operator=(Singleton const&) = delete;

	private:
		enum class _Proc_t : char {  INIT, EMPTY, READ, WRITE, CLEAR  };

		static bool _authorized;
		static std::unique_ptr<T> _qp;
		static std::weak_ptr<_Proc_t> _processing;
		static std::mutex _mtx;
		static std::condition_variable _cv;

		static void _control(std::shared_ptr<_Proc_t>& sp_proc, _Proc_t proc);

	protected:
		Singleton();
		~Singleton();
	};
	//========//========//========//========//=======#//========//========//========//========//===











	template<typename T>
	bool Singleton<T>::_authorized = false;

	template<typename T>
	std::unique_ptr<T> Singleton<T>::_qp;

	template<typename T>
	std::weak_ptr< typename Singleton<T>::_Proc_t > Singleton<T>::_processing;

	template<typename T>
	std::mutex Singleton<T>::_mtx;

	template<typename T>
	std::condition_variable Singleton<T>::_cv;
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---

	template<typename T>
	void Singleton<T>::_control(std::shared_ptr<_Proc_t>& sp_proc, _Proc_t proc)
	{
		if(!_processing.expired() && *_processing.lock() != proc)
		{
			std::unique_lock qlk(_mtx);

			_cv.wait(qlk, []{  return _processing.expired();  });
		}

		if(_processing.expired())
		{ 
			std::unique_lock qlk(_mtx);

			if(!_processing.expired())
				qlk.unlock(),
				_control(sp_proc, proc);
			else
				if( _qp == nullptr && (proc == _Proc_t::READ || proc == _Proc_t::WRITE) )
					_cv.notify_all(),
					throw false;	
				else
					_cv.notify_all(),
					_processing = sp_proc = std::make_shared<_Proc_t>(proc);
		}
		else if(*_processing.lock() == proc)
			sp_proc = _processing.lock();
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---

	template<typename T>
	template<typename...ARGS>
	void Singleton<T>::init(ARGS&&...args)
	{
		{
			std::shared_ptr<_Proc_t> initializing;

			_control(initializing, _Proc_t::INIT);

			std::unique_lock qlk(_mtx);

			_authorized = true;

			_qp.reset(  new T( std::forward<ARGS>(args)... )  );
		}

		if(_processing.expired())
			_cv.notify_one();
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---

	template<typename T>
	bool Singleton<T>::empty()
	{
		{
			std::shared_ptr<_Proc_t> checking;

			_control(checking, _Proc_t::EMPTY);
		}

		if(_processing.expired())
		{
			std::unique_lock qlk(_mtx);

			_cv.notify_one();

			return _qp == nullptr;
		}
		else
			return _qp == nullptr;
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---

	template<typename T>
	template<typename POL>
	auto Singleton<T>::read(POL)-> T const&
	{
		if constexpr(std::is_same_v<POL, std::execution::sequenced_policy>)
			return *_qp;
		else
		{
			{
				std::shared_ptr<_Proc_t> reading;

				_control(reading, _Proc_t::READ);
			}

			if(_processing.expired())
			{
				std::unique_lock qlk(_mtx);

				_cv.notify_one();

				return *_qp;
			}
			else
				return *_qp;
		}
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---

	template<typename T>
	template<typename _T, typename POL>
	decltype(auto) Singleton<T>::write(_T&& _t, POL)
	{
		if constexpr(std::is_same_v<std::decay_t<_T>, nullptr_t>)
		{
			static_assert
			(	std::is_same_v<POL, std::execution::sequenced_policy>
			,	"getting mutable reference directly is not thread-safe"
			);

			return *_qp;
		}
		else if constexpr(std::is_same_v<POL, std::execution::sequenced_policy>)
		{ 
			if constexpr(std::is_invocable_r_v< void, std::decay_t<_T>, T& >)
				_t(*_qp);
			else if constexpr( std::is_convertible_v<decltype(_t), decltype(*_qp)> )
				*_qp = _t;
			else
				static_assert(false, "couldn't find a proper writing method");
		}
		else
		{
			{
				std::shared_ptr<_Proc_t> writing;

				_control(writing, _Proc_t::WRITE);
			}

			std::unique_lock qlk(_mtx);

			if constexpr(std::is_invocable_r_v< void, std::decay_t<_T>, T& >)
				_t(*_qp);
			else if constexpr(std::is_convertible_v<decltype(_t), decltype(*_qp)>)
				*_qp = _t;
			else
				static_assert(false, "couldn't find a proper writing method");

			if(_processing.expired())
				_cv.notify_one();
		}
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---

	template<typename T>
	void Singleton<T>::clear()
	{
		{
			std::shared_ptr<_Proc_t> clearing;

			_control(clearing, _Proc_t::CLEAR);
		}

		std::unique_lock qlk(_mtx);

		if(_qp != nullptr)
			_authorized = true;
			_qp.reset();

		if (_processing.expired())
			_cv.notify_one();
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---

	template<typename T>
	Singleton<T>::Singleton()
	{
		if(_authorized)
			_authorized = false;
		else
			std::cout << "unauthorized construction. \n",
			throw false;
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---

	template<typename T>
	Singleton<T>::~Singleton()
	{
		if(_authorized)
			_authorized = false;
		else if(_qp == nullptr)
			std::cout << "unauthorized destruction.\n",
			throw false;
	}
	//--------//--------//--------//--------//-------#//--------//--------//--------//--------//---


}

////////--////////--////////--////////--////////-#////////--////////--////////--////////--////////-#

#endif // End of #ifndef _SGM_SINGLETON_
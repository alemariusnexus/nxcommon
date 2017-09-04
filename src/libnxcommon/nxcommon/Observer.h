#ifndef NXCOMMON_SRC_LIBNXCOMMON_NXCOMMON_OBSERVER_H_
#define NXCOMMON_SRC_LIBNXCOMMON_NXCOMMON_OBSERVER_H_

#include <nxcommon/config.h>
#include <list>
#include <algorithm>
#include <functional>
#include <mutex>


#ifndef SWIG

template <typename... ArgTypes>
class Observer;



template <typename... ArgTypes>
class Observable
{
private:
	typedef Observer<ArgTypes...> ObserverType;

	friend class Observer<ArgTypes...>;

public:
	virtual ~Observable()
	{
		std::lock_guard<std::mutex> lock(observersMtx);

		for (ObserverType* obsv : observers) {
			obsv->notifyObservableDestroyed();
		}
	}

	void notify(ArgTypes... args)
	{
		std::list<ObserverType*> observersCpy;

		{
			std::lock_guard<std::mutex> lock(observersMtx);
			observersCpy = observers;
		}

		for (ObserverType* obsv : observersCpy) {
			if (obsv->obsv == this) {
				(*obsv)(args...);
			}
		}
	}
	void operator()(ArgTypes... args) { notify(args...); }

private:
	void addObserver(ObserverType* obsv)
	{
		std::lock_guard<std::mutex> lock(observersMtx);

		observers.push_back(obsv);
	}

	void removeObserver(ObserverType* obsv)
	{
		std::lock_guard<std::mutex> lock(observersMtx);

		auto it = std::find(observers.begin(), observers.end(), obsv);
		if (it != observers.end()) {
			observers.erase(it);
		}
	}

private:
	std::list<ObserverType*> observers;
	std::mutex observersMtx;
};




template <typename... ArgTypes>
class Observer
{
public:
	typedef Observable<ArgTypes...> ObservableType;
	typedef std::function<void (ArgTypes...)> CallbackType;

	friend class Observable<ArgTypes...>;

public:
	virtual void notify(ArgTypes... args)
	{
		if (cb) {
			cb(args...);
		}
	}
	void operator()(ArgTypes... args) { notify(args...); }

	virtual void detach()
	{
		if (obsv) {
			obsv->removeObserver(this);
			obsv = nullptr;
		}
	}

protected:
	Observer(ObservableType* obsv, const CallbackType& cb)
			: obsv(obsv), cb(cb)
	{
		obsv->addObserver(this);
	}

	Observer()
			: obsv(nullptr)
	{
	}

	virtual ~Observer()
	{
		detach();
	}

private:
	void notifyObservableDestroyed()
	{
		obsv = nullptr;
	}

private:
	ObservableType* obsv;
	CallbackType cb;
};

#endif






#define DefineSwigObserver_n_m(clsname, methodname, obsvcls, cargs, cargns, args, pargs) \
	class clsname \
	{ \
	\
	public: \
		clsname(cargs) \
				: obsv(cargns std::bind(&clsname::methodname, this pargs)) \
		{} \
		virtual ~clsname() {} \
		\
		virtual void methodname(args) = 0; \
		\
	private: \
		obsvcls obsv; \
	};


#pragma SWIG nowarn=305

#define _COMMA ,

#pragma SWIG nowarn=


#define DefineSwigObserver_0_0(clsname, methodname, obsvcls) \
	DefineSwigObserver_n_m(clsname, methodname, obsvcls, , , , )
#define DefineSwigObserver_0_1(clsname, methodname, obsvcls, arg1) \
	DefineSwigObserver_n_m(clsname, methodname, obsvcls, , , arg1, _COMMA std::placeholders::_1)
#define DefineSwigObserver_0_2(clsname, methodname, obsvcls, arg1, arg2) \
	DefineSwigObserver_n_m(clsname, methodname, obsvcls, , , arg1 _COMMA arg2, _COMMA std::placeholders::_1 _COMMA std::placeholders::_2)
#define DefineSwigObserver_0_3(clsname, methodname, obsvcls, arg1, arg2, arg3) \
	DefineSwigObserver_n_m(clsname, methodname, obsvcls, , , arg1 _COMMA arg2 _COMMA arg3, _COMMA std::placeholders::_1 _COMMA std::placeholders::_2 _COMMA std::placeholders::_3)
#define DefineSwigObserver_0_4(clsname, methodname, obsvcls, arg1, arg2, arg3, arg4) \
	DefineSwigObserver_n_m(clsname, methodname, obsvcls, , , arg1 _COMMA arg2 _COMMA arg3 _COMMA arg4, _COMMA std::placeholders::_1 _COMMA std::placeholders::_2 _COMMA std::placeholders::_3 _COMMA std::placeholders::_4)
#define DefineSwigObserver_0_5(clsname, methodname, obsvcls, arg1, arg2, arg3, arg4, arg5) \
	DefineSwigObserver_n_m(clsname, methodname, obsvcls, , , arg1 _COMMA arg2 _COMMA arg3 _COMMA arg4 _COMMA arg5, _COMMA std::placeholders::_1 _COMMA std::placeholders::_2 _COMMA std::placeholders::_3 _COMMA std::placeholders::_4 _COMMA std::placeholders::_5)


#define DefineSwigObserver_1_0(clsname, methodname, obsvcls, carg1) \
	DefineSwigObserver_n_m(clsname, methodname, obsvcls, carg1 a1, a1 _COMMA, , )
#define DefineSwigObserver_1_1(clsname, methodname, obsvcls, carg1, arg1) \
	DefineSwigObserver_n_m(clsname, methodname, obsvcls, carg1 a1, a1 _COMMA, arg1, _COMMA std::placeholders::_1)
#define DefineSwigObserver_1_2(clsname, methodname, obsvcls, carg1, arg1, arg2) \
	DefineSwigObserver_n_m(clsname, methodname, obsvcls, carg1 a1, a1 _COMMA, arg1 _COMMA arg2, _COMMA std::placeholders::_1 _COMMA std::placeholders::_2)
#define DefineSwigObserver_1_3(clsname, methodname, obsvcls, carg1, arg1, arg2, arg3) \
	DefineSwigObserver_n_m(clsname, methodname, obsvcls, carg1 a1, a1 _COMMA, arg1 _COMMA arg2 _COMMA arg3, _COMMA std::placeholders::_1 _COMMA std::placeholders::_2 _COMMA std::placeholders::_3)
#define DefineSwigObserver_1_4(clsname, methodname, obsvcls, carg1, arg1, arg2, arg3, arg4) \
	DefineSwigObserver_n_m(clsname, methodname, obsvcls, carg1 a1, a1 _COMMA, arg1 _COMMA arg2 _COMMA arg3 _COMMA arg4, _COMMA std::placeholders::_1 _COMMA std::placeholders::_2 _COMMA std::placeholders::_3 _COMMA std::placeholders::_4)
#define DefineSwigObserver_1_5(clsname, methodname, obsvcls, carg1, arg1, arg2, arg3, arg4, arg5) \
	DefineSwigObserver_n_m(clsname, methodname, obsvcls, carg1 a1, a1 _COMMA, arg1 _COMMA arg2 _COMMA arg3 _COMMA arg4 _COMMA arg5, _COMMA std::placeholders::_1 _COMMA std::placeholders::_2 _COMMA std::placeholders::_3 _COMMA std::placeholders::_4 _COMMA std::placeholders::_5)

#endif /* NXCOMMON_SRC_LIBNXCOMMON_NXCOMMON_OBSERVER_H_ */

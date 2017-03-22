#ifndef NXCOMMON_SRC_LIBNXCOMMON_NXCOMMON_OBSERVER_H_
#define NXCOMMON_SRC_LIBNXCOMMON_NXCOMMON_OBSERVER_H_

#include <nxcommon/config.h>
#include <list>
#include <algorithm>
#include <functional>
#include <mutex>


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

#endif /* NXCOMMON_SRC_LIBNXCOMMON_NXCOMMON_OBSERVER_H_ */

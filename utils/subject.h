#ifndef SRC_SUBJECT_H
#define SRC_SUBJECT_H

#include <memory>
#include "observer.h"
#include "utils.h"
#include <vector>
#include <algorithm>
#include <SFML/System.hpp>

typedef Observer* obs_ptr_t;

//Interface which contains methods for being observed and managing observers
class Subject {
	public:
		Subject() {}
		virtual ~Subject() {
			notify(-1, EVENT_SUBJECT_DIED, getTimestamp());
		}
		virtual void addObserver(obs_ptr_t obs) {
			m_observers.push_back(obs);
		}
		virtual void removeObserverAt(int index) {
			m_observers.erase(m_observers.begin() + index);
		}
	protected:
		virtual void notify(int objectID, ::Event ev, sf::Uint64 timestamp = 0) {
			int i = 0;
			std::vector<int> toRemove;
			if(timestamp == 0) {
				timestamp = getTimestamp();
			}
			
			for(obs_ptr_t obs : m_observers) {
				if(obs != nullptr) {
					obs->onNotify(objectID, this, ev, timestamp);
				} else {
					toRemove.push_back(i);
				}
				i++;
			}
			for(int i : toRemove) {
				removeObserverAt(i);
			}
		}
	private:
		std::vector<obs_ptr_t> m_observers;
};

#endif
 

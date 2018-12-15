#ifndef SRC_OBSERVER_H
#define SRC_OBSERVER_H

#include <memory>
#include <iostream>
#include "event.h"
#include <SFML/System.hpp>

class Subject;



class Observer {
	public:
		virtual ~Observer() {}
		virtual void onNotify(int objectID, Subject *sender, Event ev, sf::Uint64 timestamp) = 0;
	private:
};

#endif

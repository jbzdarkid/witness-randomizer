#pragma once
#include <memory>
#include "Randomizer.h"

class Watchdog {
public:
    Watchdog(float time) : sleepTime(std::chrono::milliseconds(static_cast<int>(time * 1000))) {}
	const std::chrono::milliseconds sleepTime;

	virtual void action(const std::shared_ptr<Randomizer>&) = 0;

	bool IsRunning() { return !terminate; }
	void Shutdown() { terminate = true; }

private:
	bool terminate = false;
};
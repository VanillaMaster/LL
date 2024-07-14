#include <cstdint>
#include <mutex>
#include <windows.h>
#include <cstring>
#include <memoryapi.h>

template<typename> struct Proxy;

template<typename R, typename ...Args>
struct Proxy<R(Args...)> {

	bool flag = true;
	void* target = nullptr;
	std::mutex mutex = {};
	uint8_t bufferA[12] = { 0x48, 0xB8, 0,0,0,0,0,0,0,0, 0x50, 0xC3 };
	uint8_t bufferB[12] = {};

	void swap();

	bool attach(void* target, void* wrapper);

	bool detach();

	R call(Args ...args);
};

template<typename R, typename ...Args>
void Proxy<R(Args...)>::swap() {
	const uint8_t* buffer = this->flag ? this->bufferA : this->bufferB;
	this->flag = !this->flag;

	DWORD op;
	VirtualProtect(this->target, 12, PAGE_EXECUTE_READWRITE, &op);
	memcpy(this->target, buffer, 12);
	VirtualProtect(this->target, 12, op, &op);
}

template<typename R, typename ...Args>
R Proxy<R(Args...)>::call(Args ...args) {
	std::lock_guard<std::mutex> lock(this->mutex);

	this->swap();
	R result = ((R(*)(Args...))(this->target))(args...);
	this->swap();

	return result;
}

template<typename R, typename ...Args>
bool Proxy<R(Args...)>::attach(void* target, void* wrapper) {
	if (this->target != nullptr) true;

	this->target = target;
	memcpy(this->bufferA + 2, &wrapper, sizeof wrapper);
	memcpy(this->bufferB, target, 12);

	this->swap();

	return false;
}

template<typename R, typename ...Args>
bool Proxy<R(Args...)>::detach() {
	if (!(this->flag)) this->swap();

	this->target = nullptr;

	return false;
}
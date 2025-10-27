#pragma once

#include "backendType.h"

#define CAST_HELPERS \
	template<typename T> \
	T* as() { return static_cast<T*>(this); } \
	template<typename T> \
	const T* as() const { return static_cast<const T*>(this); }

#define BACKEND_CHECKER \
	virtual BackendType getBackendType() const = 0;

#define BACKEND_CHECK(backend) \
	if (getBackendType() != backend) { \
		throw std::runtime_error("Backend mismatch!"); \
	}
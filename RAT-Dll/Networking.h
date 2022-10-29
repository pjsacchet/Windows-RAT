#pragma once

#ifdef NETWORKING_EXPORTS 
#define NETWORKING_API_declspec(dllexport)
#else
#define NETWORKING_API_declspec(dllimport)
#endif 

extern "C" NETWORKING_API void net_init(
	);

extern "C" NETWORKING_API int open_port(
	);
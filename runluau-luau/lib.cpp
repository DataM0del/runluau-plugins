#include "lib.h"

#include <stdio.h>

#include "Luau/Compiler.h"

#define wanted_arg_count(n) \
if (n > 0 && lua_gettop(thread) < n) [[unlikely]] { \
	lua_pushfstring(thread, __FUNCTION__" expects at least " #n " arguments, received %d", lua_gettop(thread)); \
	lua_error(thread); \
	return 0; \
}

int compile(lua_State* thread) {
	wanted_arg_count(1);
	size_t source_length;
	const char* source_c_str = luaL_checklstring(thread, 1, &source_length);
	Luau::CompileOptions options;
	if (lua_gettop(thread) >= 2) {
		luaL_checktype(thread, 2, LUA_TTABLE);
		int optimization_level_location = lua_getfield(thread, 2, "optimizationLevel");
		if (optimization_level_location) {
			unsigned int optimization_level = luaL_checkunsigned(thread, optimization_level_location);
			if (optimization_level > 2) {
				lua_pushstring(thread, "When compiling, optimizationLevel must be an integer from 0 to 2");
				lua_error(thread);
			}
			options.optimizationLevel = optimization_level;
		}
		int debug_level_location = lua_getfield(thread, 2, "debugLevel");
		if (debug_level_location) {
			unsigned int debug_level = luaL_checkunsigned(thread, debug_level_location);
			if (debug_level > 2) {
				lua_pushstring(thread, "When compiling, debugLevel must be an integer from 0 to 2");
				lua_error(thread);
			}
			options.debugLevel = debug_level;
		}
	}
	std::string bytecode = Luau::compile(std::string(source_c_str, source_length), options, {});
	lua_pushlstring(thread, bytecode.data(), bytecode.size());
	return 1;
}

#define r(name) {#name, name}
constexpr luaL_Reg library[] = {
	r(compile),
	{NULL, NULL}
};
void register_library(lua_State* state) {
	luaL_register(state, "luau", library);
}
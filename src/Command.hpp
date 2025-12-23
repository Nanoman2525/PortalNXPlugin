#pragma once
#include "SDK/Convar.hpp"

#include <cstring>
#include <vector>
#include <filesystem>
#include <algorithm>

#define COMMAND_COMPLETION_MAXITEMS 64
#define COMMAND_COMPLETION_ITEM_LENGTH 64

class CCommand;
class ConCommand;
class ConCommandBase;

using _CommandCallback = void (*)(const CCommand &args);
using _CommandCompletionCallback = int (*)(const char *partial, char commands[COMMAND_COMPLETION_MAXITEMS][COMMAND_COMPLETION_ITEM_LENGTH]);

class Command
{
private:
	ConCommand *ptr;

public:
	bool isRegistered;
	bool isReference;

public:
	static std::vector<Command *> &GetList();

public:
	Command();
	~Command();
	Command(const char *name);
	Command(const char *pName, _CommandCallback callback, const char *pHelpString, int flags = 0, _CommandCompletionCallback completionFunc = nullptr);

	ConCommand *ThisPtr();

	void Register();
	void Unregister();

	bool operator!();

	static int RegisterAll();
	static void UnregisterAll();
	static Command *Find(const char *name);

	static bool Hook(const char *name, _CommandCallback detour, _CommandCallback &original);
	static bool Unhook(const char *name, _CommandCallback original);
	static bool HookCompletion(const char *name, _CommandCompletionCallback callback, _CommandCompletionCallback &original);
	static bool UnhookCompletion(const char *name, _CommandCompletionCallback original);
};

#define CON_COMMAND(name, description)                        \
	void name##_callback(const CCommand &args);                  \
	Command name(#name, name##_callback, description); \
	void name##_callback(const CCommand &args)

#define CON_COMMAND_F(name, description, flags)                      \
	void name##_callback(const CCommand &args);                         \
	Command name(#name, name##_callback, description, flags); \
	void name##_callback(const CCommand &args)

#define CON_COMMAND_F_COMPLETION(name, description, flags, completion)           \
	void name##_callback(const CCommand &args);                                     \
	Command name(#name, name##_callback, description, flags, completion); \
	void name##_callback(const CCommand &args)

#define DECL_DECLARE_AUTOCOMPLETION_FUNCTION(command) \
	int command##_CompletionFunc(const char *partial, char commands[COMMAND_COMPLETION_MAXITEMS][COMMAND_COMPLETION_ITEM_LENGTH])

#define AUTOCOMPLETION_FUNCTION(command) \
	command##_CompletionFunc

#define DECL_DETOUR_COMMAND(name)         \
	static _CommandCallback name##_callback; \
	static void name##_callback_hook(const CCommand &args)
#define DETOUR_COMMAND(name) \
	void name##_callback_hook(const CCommand &args)

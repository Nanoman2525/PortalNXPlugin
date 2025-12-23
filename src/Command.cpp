#include "Command.hpp"
#include "Offsets.hpp"

#include <cstring>
#include <set>
#include <algorithm>

extern void *g_pCVar;
extern uintptr_t vstdlibnrobase;
static uintptr_t **original_ConCommandvtable = 0;

std::vector<Command *> &Command::GetList() {
	static std::vector<Command *> list;
	return list;
}

Command::Command()
	: ptr(nullptr)
	, isRegistered(false)
	, isReference(false) {
}
Command::~Command() {
	if (!this->isReference) {
		if (this->ptr) {
            delete this->ptr;
            this->ptr = nullptr;
        }
	}
}
Command::Command(const char *name) {
	auto CCvar__FindCommand = reinterpret_cast<ConCommand *(*)(void *, const char *)>((*reinterpret_cast<void ***>(g_pCVar))[Offsets::CCvar__FindCommand_vtable_index]);
	this->ptr = CCvar__FindCommand(g_pCVar, name);
	this->isRegistered = false;
	this->isReference = true;
}
Command::Command(const char *pName, _CommandCallback callback, const char *pHelpString, int flags, _CommandCompletionCallback completionFunc)
	: isRegistered(false)
	, isReference(false) {
	this->ptr = new ConCommand(pName, callback, pHelpString, flags, completionFunc);

	Command::GetList().push_back(this);
}
ConCommand *Command::ThisPtr() {
	return this->ptr;
}
void Command::Register() {
	if (!original_ConCommandvtable) {
		original_ConCommandvtable = *(uintptr_t ***)this->ptr; // Record it once
	}

	if (!this->isRegistered) {
        auto CCvar__RegisterConCommand = (void (*)(void *, ConCommandBase *))(vstdlibnrobase + Offsets::CCvar__RegisterConCommand);
        *reinterpret_cast<void **>(this->ptr) = (void *)(vstdlibnrobase + Offsets::ConCommand__vtable); // stealing vtable from the game
        CCvar__RegisterConCommand(g_pCVar, this->ptr);

        *reinterpret_cast<ConCommandBase **>((uintptr_t)g_pCVar + 80) = this->ptr; // CCvar::m_pConCommandList = this->ptr;
	}
	this->isRegistered = true;
}
void Command::Unregister() {
	if (this->isRegistered) {
        auto CCvar__UnregisterConCommand = (void (*)(void *, ConCommandBase *))(vstdlibnrobase + Offsets::CCvar__UnregisterConCommand);
        CCvar__UnregisterConCommand(g_pCVar, this->ptr);

		*reinterpret_cast<uintptr_t ***>(this->ptr) = original_ConCommandvtable; // Don't forget to swap back to the original vtable (SAR codebase forgets to do this)

        // DON'T DESTROY since we reuse the ptr in case we reregister
		// auto ConCommand_Destructor = (void (*)(ConCommand *))(vstdlibnrobase + Offsets::ConCommand_Destructor);
		// ConCommand_Destructor(this->ptr);
	}
	this->isRegistered = false;
}
bool Command::operator!() {
	return this->ptr == nullptr;
}
int Command::RegisterAll() {
	auto result = 0;
	for (const auto &command : Command::GetList()) {
		command->Register();
		++result;
	}
	return result;
}
void Command::UnregisterAll() {
	for (const auto &command : Command::GetList()) {
		command->Unregister();
	}
}
Command *Command::Find(const char *name) {
	for (const auto &command : Command::GetList()) {
		if (!std::strcmp(command->ThisPtr()->m_pszName, name)) {
			return command;
		}
	}
	return nullptr;
}

bool Command::Hook(const char *name, _CommandCallback detour, _CommandCallback &original) {
	Command cc(name);
	if (!!cc) {
		original = cc.ThisPtr()->m_fnCommandCallback;
		cc.ThisPtr()->m_fnCommandCallback = detour;
		return true;
	}
	return false;
}
bool Command::Unhook(const char *name, _CommandCallback original) {
	Command cc(name);
	if (!!cc && original) {
		cc.ThisPtr()->m_fnCommandCallback = original;
		return true;
	}
	return false;
}
bool Command::HookCompletion(const char *name, _CommandCompletionCallback detour, _CommandCompletionCallback &original) {
	Command cc(name);
	if (!!cc) {
		original = cc.ThisPtr()->m_bHasCompletionCallback ? cc.ThisPtr()->m_fnCompletionCallback : nullptr;
		cc.ThisPtr()->m_bHasCompletionCallback = true;
		cc.ThisPtr()->m_fnCompletionCallback = detour;
		return true;
	}
	return false;
}
bool Command::UnhookCompletion(const char *name, _CommandCompletionCallback original) {
	Command cc(name);
	if (!!cc) {
		cc.ThisPtr()->m_bHasCompletionCallback = !!original;
		cc.ThisPtr()->m_fnCompletionCallback = original;
		return true;
	}
	return false;
}

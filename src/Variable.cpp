#include "Variable.hpp"
#include "Offsets.hpp"

#include <cstring>

extern void *g_pCVar;
extern uintptr_t vstdlibnrobase;
static uintptr_t **original_ConVarvtable = 0;

std::vector<Variable *> &Variable::GetList() {
	static std::vector<Variable *> list;
	return list;
}

Variable::Variable()
	: ptr(nullptr)
	, originalFlags(0)
	, originalFnChangeCallback(nullptr)
	, isRegistered(false)
	, isReference(false)
	, hasCustomCallback(false)
	, isUnlocked(false) {
}
Variable::~Variable() {
	if (!this->isReference) {
		if (this->ptr) {
            delete this->ptr;
            this->ptr = nullptr;
        }
	}
}
Variable::Variable(const char *name)
	: Variable() {
	auto FindVar = reinterpret_cast<ConVar *(*)(void *, const char *)>((*reinterpret_cast<void ***>(g_pCVar))[Offsets::CCvar__FindVar_vtable_index]);
	this->ptr = reinterpret_cast<ConVar *>(FindVar(g_pCVar, name));
	this->isReference = true;
}
// Boolean or String
Variable::Variable(const char *name, const char *value, const char *helpstr, int flags, FnChangeCallback_t callback)
	: Variable() {
	Create(name, value, flags, helpstr, false, 0, false, 0, callback);
}
// Float
Variable::Variable(const char *name, const char *value, float min, const char *helpstr, int flags, FnChangeCallback_t callback)
	: Variable() {
	Create(name, value, flags, helpstr, true, min, false, 0, callback);
}
// Float
Variable::Variable(const char *name, const char *value, float min, float max, const char *helpstr, int flags, FnChangeCallback_t callback)
	: Variable() {
	Create(name, value, flags, helpstr, true, min, true, max, callback);
}
void Variable::Create(const char *name, const char *value, int flags, const char *helpstr, bool hasmin, float min, bool hasmax, float max, FnChangeCallback_t callback) {
	this->ptr = new ConVar(name, value, flags, helpstr, hasmin, min, hasmax, max);
	this->AddCallBack(callback);

	Variable::GetList().push_back(this);
}
void Variable::AddCallBack(FnChangeCallback_t callback) {
	if (callback != nullptr) {
		this->originalCallbacks = this->ThisPtr()->m_fnChangeCallback;
		this->ThisPtr()->m_fnChangeCallback.Append(callback);
		this->hasCustomCallback = true;
		this->GetList().push_back(this);
	}
}
ConVar *Variable::ThisPtr() {
	return this->ptr;
}
bool Variable::GetBool() {
	return !!GetInt();
}
int Variable::GetInt() {
	if (!this->ptr) return 0;
	return this->ptr->m_nValue;
}
float Variable::GetFloat() {
	if (!this->ptr) return 0;
	return this->ptr->m_fValue;
}
const char *Variable::GetString() {
	if (!this->ptr) return "";
	return this->ptr->m_pszString;
}
const int Variable::GetFlags() {
	if (!this->ptr) return FCVAR_NONE;
	return this->ptr->m_nFlags;
}
void Variable::SetValue(const char *value) {
    auto ConVar__InternalSetValue = reinterpret_cast<void (*)(ConVar *, const char *)>((*reinterpret_cast<void ***>(this->ptr))[Offsets::ConVar__InternalSetValue_vtable_index]);
	if (this->ptr) ConVar__InternalSetValue(this->ptr, value);
}
void Variable::SetFlags(int value) {
	if (this->ptr) this->ptr->m_nFlags = value;
}
void Variable::AddFlag(int value) {
	this->SetFlags(this->GetFlags() | value);
}
void Variable::RemoveFlag(int value) {
	this->SetFlags(this->GetFlags() & ~(value));
}
void Variable::Unlock(bool asCheat) {
	if (this->ptr) {
		this->originalFlags = this->ptr->m_nFlags;
		this->RemoveFlag(FCVAR_DEVELOPMENTONLY | FCVAR_HIDDEN);
		if (asCheat) {
			this->AddFlag(FCVAR_CHEAT);
		}

		if (this->isReference) {
			this->isUnlocked = true;
			this->GetList().push_back(this);
		}
	}
}
void Variable::Lock() {
	if (this->ptr) {
		this->SetFlags(this->originalFlags);
		if (!this->ptr->IsCommand()) { // lazy fix lmao - AMJ 2024-04-26
			this->SetValue(this->ptr->m_pszDefaultValue);
		}
		this->isUnlocked = false;
	}
}
void Variable::DisableChange() {
	if (this->ptr) {
		this->originalSize = this->ptr->m_fnChangeCallback.m_Size;
		this->ptr->m_fnChangeCallback.m_Size = 0;
	}
}
void Variable::EnableChange() {
	if (this->ptr) {
		this->ptr->m_fnChangeCallback.m_Size = this->originalSize;
	}
}
void Variable::Register() {
	if (!original_ConVarvtable) {
		original_ConVarvtable = *(uintptr_t ***)this->ptr; // Record it once
	}

	if (!this->isRegistered && !this->isReference) {
		this->isRegistered = true;
		FnChangeCallback_t callback = this->ptr->m_fnChangeCallback.m_Size > 0 ? this->ptr->m_fnChangeCallback.m_pElements[0] : nullptr;

        // Swap out the vtables
        // Note: There are two vtables for ConVar
        *reinterpret_cast<void **>(this->ptr) = (void *)(vstdlibnrobase + Offsets::ConVar__first_vtable);
        this->ptr->secondvtable = (void *)(vstdlibnrobase + Offsets::ConVar__second_vtable);

        // Note: For some reason, the argument order changed on Switch builds
        auto ConVar__Create = reinterpret_cast< void (*)(void *thisptr, const char *pName, const char *pDefaultValue, int flags,
        const char *pHelpString, bool bMin, bool bMax, FnChangeCallback_t callback, float fMin, float fMax)>(vstdlibnrobase + Offsets::ConVar__Create);

		ConVar__Create(this->ptr, this->ptr->m_pszName, this->ptr->m_pszDefaultValue, this->ptr->m_nFlags, this->ptr->m_pszHelpString, this->ptr->m_bHasMin, this->ptr->m_bHasMax, callback, this->ptr->m_fMinVal, this->ptr->m_fMaxVal);
	}
}
void Variable::Unregister() {
	if (this->isRegistered && !this->isReference) {
		this->isRegistered = false;

        auto CCvar__UnregisterConCommand = (void (*)(void *, ConCommandBase *))(vstdlibnrobase + Offsets::CCvar__UnregisterConCommand);
        CCvar__UnregisterConCommand(g_pCVar, this->ptr);

        *reinterpret_cast<uintptr_t ***>(this->ptr) = original_ConVarvtable; // Don't forget to swap back to the original vtable (SAR codebase forgets to do this)

		// DON'T DESTROY since we reuse the ptr in case we reregister
		// auto ConVar_Destructor = (void (*)(ConVar *))(vstdlibnrobase + Offsets::ConVar_Destructor);
		// ConVar_Destructor(this->ptr);
	}
}
bool Variable::operator!() {
	return this->ptr == nullptr;
}
void Variable::ClearAllCallbacks() {
	for (auto var : Variable::GetList()) {
		if (var->hasCustomCallback) {
			var->ThisPtr()->m_fnChangeCallback = var->originalCallbacks;
			var->hasCustomCallback = false;
		}
	}
}
int Variable::RegisterAll() {
	auto result = 0;
	for (const auto &var : Variable::GetList()) {
		var->Register();
		++result;
	}
	return result;
}
void Variable::UnregisterAll() {
	for (const auto &var : Variable::GetList()) {
		var->Unregister();
	}
}
Variable *Variable::Find(const char *name) {
	for (const auto &var : Variable::GetList()) {
		if (!std::strcmp(var->ThisPtr()->m_pszName, name)) {
			return var;
		}
	}
	return nullptr;
}
#ifndef CONVAR_HPP
#define CONVAR_HPP

#include "UtlMemory.hpp"

class Color;

#define FCVAR_NONE                       0
#define FCVAR_UNREGISTERED               (1 << 0)
#define FCVAR_DEVELOPMENTONLY            (1 << 1)
#define FCVAR_GAMEDLL                    (1 << 2)
#define FCVAR_CLIENTDLL                  (1 << 3)
#define FCVAR_HIDDEN                     (1 << 4)
#define FCVAR_PROTECTED                  (1 << 5)
#define FCVAR_SPONLY                     (1 << 6)
#define	FCVAR_ARCHIVE                    (1 << 7)
#define	FCVAR_NOTIFY                     (1 << 8)
#define	FCVAR_USERINFO                   (1 << 9)
#define FCVAR_PRINTABLEONLY              (1 << 10)
#define FCVAR_GAMEDLL_FOR_REMOTE_CLIENTS (1 << 10)
#define FCVAR_UNLOGGED                   (1 << 11)
#define FCVAR_NEVER_AS_STRING            (1 << 12)
#define FCVAR_REPLICATED                 (1 << 13)
#define FCVAR_CHEAT                      (1 << 14)
#define FCVAR_SS                         (1 << 15)
#define FCVAR_DEMO                       (1 << 16)
#define FCVAR_DONTRECORD                 (1 << 17)
#define FCVAR_SS_ADDED                   (1 << 18)
#define FCVAR_RELEASE                    (1 << 19)
#define FCVAR_RELOAD_MATERIALS           (1 << 20)
#define FCVAR_RELOAD_TEXTURES            (1 << 21)
#define FCVAR_NOT_CONNECTED              (1 << 22)
#define FCVAR_MATERIAL_SYSTEM_THREAD     (1 << 23)
#define FCVAR_ARCHIVE_GAMECONSOLE        (1 << 24)
#define FCVAR_ACCESSIBLE_FROM_THREADS    (1 << 25)
#define FCVAR_SERVER_CAN_EXECUTE         (1 << 28)
#define FCVAR_SERVER_CANNOT_QUERY        (1 << 29)
#define FCVAR_CLIENTCMD_CAN_EXECUTE      (1 << 30)

#define COMMAND_COMPLETION_MAXITEMS 64
#define COMMAND_COMPLETION_ITEM_LENGTH 64

class ConCommandBase
{
public:
	ConCommandBase( const char *name, int flags, const char *helpstr )
		: m_pNext(nullptr)
		, m_bRegistered(false)
		, m_pszName(name)
		, m_pszHelpString(helpstr)
		, m_nFlags(flags)
	{
	}
	virtual						~ConCommandBase( void );
	virtual	bool				IsCommand( void ) const;
	virtual bool				IsFlagSet( int flag ) const;
	virtual void				AddFlags( int flags );
	virtual void				RemoveFlags( int flags );
	virtual int					GetFlags() const;
	virtual const char			*GetName( void ) const;
	virtual const char			*GetHelpText( void ) const;
	virtual bool				IsRegistered( void ) const;
	virtual int					GetDLLIdentifier() const;
	virtual void				Create( const char *pName, const char *pHelpString = 0, int flags = 0 );
	virtual void				Init();

	ConCommandBase				*m_pNext; // 8
	bool						m_bRegistered; // 16
	const char 					*m_pszName; // 24
	const char 					*m_pszHelpString; // 32
	int							m_nFlags; // 40
};

struct characterset_t;

//-----------------------------------------------------------------------------
// Command tokenizer
//-----------------------------------------------------------------------------
class CCommand
{
public:
	CCommand();
	CCommand( int nArgC, const char **ppArgV );

	int ArgC() const;
	const char **ArgV() const;
	const char *ArgS() const;					// All args that occur after the 0th arg, in string form
	const char *GetCommandString() const;		// The entire command in string form, including the 0th arg
	const char *operator[]( int nIndex ) const;	// Gets at arguments
	const char *Arg( int nIndex ) const;		// Gets at arguments

	static int MaxCommandLength();
	static characterset_t* DefaultBreakSet();

private:
	enum
	{
		COMMAND_MAX_ARGC = 64,
		COMMAND_MAX_LENGTH = 512,
	};

	int		m_nArgc;
	int		m_nArgv0Size;
	char	m_pArgSBuffer[ COMMAND_MAX_LENGTH ];
	char	m_pArgvBuffer[ COMMAND_MAX_LENGTH ];
	const char*	m_ppArgv[ COMMAND_MAX_ARGC ];
};

inline int CCommand::MaxCommandLength()
{
	return COMMAND_MAX_LENGTH - 1;
}

inline int CCommand::ArgC() const
{
	return m_nArgc;
}

inline const char **CCommand::ArgV() const
{
	return m_nArgc ? (const char**)m_ppArgv : 0;
}

inline const char *CCommand::ArgS() const
{
	return m_nArgv0Size ? &m_pArgSBuffer[m_nArgv0Size] : "";
}

inline const char *CCommand::GetCommandString() const
{
	return m_nArgc ? m_pArgSBuffer : "";
}

inline const char *CCommand::Arg( int nIndex ) const
{
	if ( nIndex < 0 || nIndex >= m_nArgc )
		return "";
	return m_ppArgv[nIndex];
}

inline const char *CCommand::operator[]( int nIndex ) const
{
	return Arg( nIndex );
}

typedef void ( *FnCommandCallback_t )( const CCommand &command );
typedef int  ( *FnCommandCompletionCallback )( const char *partial, char commands[ COMMAND_COMPLETION_MAXITEMS ][ COMMAND_COMPLETION_ITEM_LENGTH ] );

class ConCommand : public ConCommandBase
{
public:
	union
	{
		void *m_fnCommandCallbackV1;
		FnCommandCallback_t m_fnCommandCallback;
		void *m_pCommandCallback;
	};

	union
	{
		FnCommandCompletionCallback m_fnCompletionCallback;
		void *m_pCommandCompletionCallback;
	};

	bool m_bHasCompletionCallback : 1;
	bool m_bUsingNewCommandCallback : 1;
	bool m_bUsingCommandCallbackInterface : 1;

	ConCommand(const char *pName, FnCommandCallback_t callback, const char *pHelpString, int flags = 0, FnCommandCompletionCallback completionFunc = nullptr)
		: ConCommandBase(pName, flags, pHelpString)
		, m_fnCommandCallback(callback)
		, m_fnCompletionCallback(completionFunc)
		, m_bHasCompletionCallback(completionFunc != nullptr)
		, m_bUsingNewCommandCallback(true)
		, m_bUsingCommandCallbackInterface(false)
	{
	}
};

typedef void ( *FnChangeCallback_t )( void *var, const char *pOldValue, float flOldValue );
class ConVar : public ConCommandBase
{
public:
	ConVar(const char *name, const char *value, int flags, const char *helpstr, bool hasmin = false, float min = 0.0, bool hasmax = false, float max = 0.0)
		: ConCommandBase(name, flags, helpstr)
		, m_pParent(nullptr)
		, m_pszDefaultValue(value)
		, m_pszString(nullptr)
		, m_StringLength(0)
		, m_fValue(0.0f)
		, m_nValue(0)
		, m_bHasMin(hasmin)
		, m_fMinVal(min)
		, m_bHasMax(hasmax)
		, m_fMaxVal(max)
		, m_fnChangeCallback()
	{
	}

	inline float					GetFloat( void ) const { return m_pParent->m_fValue; }
	inline int						GetInt( void ) const { return m_pParent->m_nValue; }
	inline bool						GetBool() const {  return !!GetInt(); }
	inline char const	   			*GetString( void ) const { return m_pParent->m_pszString; }

	void 							*secondvtable; // 48
	ConVar							*m_pParent; // 56
	const char						*m_pszDefaultValue; // 64
	char							*m_pszString; // 72
	int								m_StringLength; // 80
	float							m_fValue; // 84
	int								m_nValue; // 88
	bool							m_bHasMin; // 96
	float							m_fMinVal; // 100
	bool							m_bHasMax; // 104
	float							m_fMaxVal; // 108
	CUtlVector<FnChangeCallback_t> 	m_fnChangeCallback; // 112
};

#define FOR_ALL_CONSOLE_COMMANDS( pCommandVarName ) \
	ConCommandBase *m_pConCommandList = *reinterpret_cast<ConCommandBase **>((uintptr_t)g_pCVar + 80); /* CCvar::m_pConCommandList */ \
	for (ConCommandBase *pCommandVarName = m_pConCommandList; pCommandVarName; pCommandVarName = *reinterpret_cast<ConCommandBase **>(reinterpret_cast<uintptr_t >(pCommandVarName) + 8)) /* ConCommandBase::m_pNext (private variable) */ \

#endif // CONVAR_HPP

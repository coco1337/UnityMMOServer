
#pragma once
#include "Types.h"
#include <windows.h>
#include "DBBind.h"

namespace SP
{
	
    class RegisterUser : public DBBind<2,0>
    {
    public:
    	RegisterUser(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spRegisterUser(?,?)}") { }
    	template<int32 N> void In_AccId(WCHAR(&v)[N]) { BindParam(0, v); };
    	template<int32 N> void In_AccId(const WCHAR(&v)[N]) { BindParam(0, v); };
    	void In_AccId(WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void In_AccId(const WCHAR* v, int32 count) { BindParam(0, v, count); };
    	template<int32 N> void In_AccPw(WCHAR(&v)[N]) { BindParam(1, v); };
    	template<int32 N> void In_AccPw(const WCHAR(&v)[N]) { BindParam(1, v); };
    	void In_AccPw(WCHAR* v, int32 count) { BindParam(1, v, count); };
    	void In_AccPw(const WCHAR* v, int32 count) { BindParam(1, v, count); };

    private:
    };

    class GetUserLoginInfo : public DBBind<2,1>
    {
    public:
    	GetUserLoginInfo(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spGetUserLoginInfo(?,?)}") { }
    	template<int32 N> void In_Id(WCHAR(&v)[N]) { BindParam(0, v); };
    	template<int32 N> void In_Id(const WCHAR(&v)[N]) { BindParam(0, v); };
    	void In_Id(WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void In_Id(const WCHAR* v, int32 count) { BindParam(0, v, count); };
    	template<int32 N> void In_Pw(WCHAR(&v)[N]) { BindParam(1, v); };
    	template<int32 N> void In_Pw(const WCHAR(&v)[N]) { BindParam(1, v); };
    	void In_Pw(WCHAR* v, int32 count) { BindParam(1, v, count); };
    	void In_Pw(const WCHAR* v, int32 count) { BindParam(1, v, count); };
    	void Out_Uid(OUT int32& v) { BindCol(0, v); };

    private:
    };


     
};
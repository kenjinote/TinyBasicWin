#pragma once

#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <conio.h>
#include <stdlib.h>

// TOYOSHIKI TinyBASIC symbols
// TO-DO Rewrite defined values to fit your machine as needed
#define SIZE_LINE 80 //Command line buffer length + NULL
#define SIZE_IBUF 80 //i-code conversion buffer size
#define SIZE_LIST 1024 //List buffer size
#define SIZE_ARRY 64 //Array area size
#define SIZE_GSTK 6 //GOSUB stack size(2/nest)
#define SIZE_LSTK 15 //FOR stack size(5/nest)

// Terminal control
#define c_putch(c) {TCHAR temp[2] = { c, 0 }; SendMessage(m_hOutputEditWnd, EM_REPLACESEL, 0, (LPARAM)temp);}

// i-code(Intermediate code) assignment
enum {
	I_GOTO, I_GOSUB, I_RETURN,
	I_FOR, I_TO, I_STEP, I_NEXT,
	I_IF, I_REM, I_STOP,
	I_INPUT, I_PRINT, I_LET,
	I_COMMA, I_SEMI,
	I_MINUS, I_PLUS, I_MUL, I_DIV, I_OPEN, I_CLOSE,
	I_GTE, I_SHARP, I_GT, I_EQ, I_LTE, I_LT,
	I_ARRAY, I_RND, I_ABS, I_SIZE,
	I_LIST, I_RUN, I_NEW,
	I_NUM, I_VAR, I_STR,
	I_EOL
};

// Keyword count
#define SIZE_KWTBL _countof(kwtbl)

// List formatting condition
// no space after
const WCHAR i_nsa[] = {
	I_RETURN, I_STOP, I_COMMA,
	I_MINUS, I_PLUS, I_MUL, I_DIV, I_OPEN, I_CLOSE,
	I_GTE, I_SHARP, I_GT, I_EQ, I_LTE, I_LT,
	I_ARRAY, I_RND, I_ABS, I_SIZE
};

// no space before
const WCHAR i_nsb[] = {
	I_MINUS, I_PLUS, I_MUL, I_DIV, I_OPEN, I_CLOSE,
	I_GTE, I_SHARP, I_GT, I_EQ, I_LTE, I_LT,
	I_COMMA, I_SEMI, I_EOL
};

class CBasic
{
private:
	BOOL m_bAbort;
	HWND m_hInputEditWnd;
	HWND m_hOutputEditWnd;
	// RAM mapping
	WCHAR lbuf[SIZE_LINE]; //Command line buffer
	WCHAR ibuf[SIZE_IBUF]; //i-code conversion buffer
	short var[26]; //Variable area
	short arr[SIZE_ARRY]; //Array area
	WCHAR listbuf[SIZE_LIST]; //List area
	WCHAR* clp; //Pointer for current line
	WCHAR* cip; //Pointer for current Intermediate code
	WCHAR* gstk[SIZE_GSTK]; //GOSUB stack
	WCHAR gstki; //GOSUB stack index
	WCHAR* lstk[SIZE_LSTK]; //FOR stack
	WCHAR lstki; //FOR stack index
	unsigned char err;// Error message index

	void c_gets(int nLineIndex);
	void putnum(short value, short d);
	WCHAR toktoi();
	WCHAR* getlp(short lineno);
	int getsize();
	void inslist();
	void putlist(WCHAR* ip);
	short getparam();
	short ivalue();
	short imul();
	short iplus();
	short iexp();
	void iprint();
	void ivar();
	void iarray();
	void ilet();
	WCHAR* iexe();
	void irun();
	void inew(void);
	void icom();
	void error();
	void newline(void);
	void c_puts(LPCWSTR s);
	void init();

public:
	CBasic(HWND hInputEditWnd, HWND hOutputEditWnd)
		: m_hInputEditWnd(hInputEditWnd)
		, m_hOutputEditWnd(hOutputEditWnd)
	{
		init();
	}
	void Run();
	void abort() { m_bAbort = TRUE; }
};
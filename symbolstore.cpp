#include <vcl.h>
#include ".\symbolstore.h"

#include <assert.h>

#include <map>

using namespace std;

typedef map<int, AnsiString> VAL2SYM;
typedef map<AnsiString, int> SYM2VAL;

static VAL2SYM romV2S;
static SYM2VAL romS2V;

static VAL2SYM fileV2S;
static SYM2VAL fileS2V;

void nosgbMunger(AnsiString& val, AnsiString& name)
{
        AnsiString tempVal = val;
        int wspos = tempVal.LastDelimiter(":");
        if (wspos == 0)
        {
                return;
        }

        AnsiString actualVal(tempVal.SubString(wspos + 1, tempVal.Length() - wspos));

        val = name;
        name = actualVal;
}

void loadFileSymbolsProxy(const char* path)
{
        AnsiString pp = AnsiString(path).LowerCase();
        if (pp.Length() < 3) return;

        AnsiString ppp = pp.SubString(pp.Length() - 1, 2);
        if (ppp != ".p") return;

        pp += ".sym";
        symbolstore::loadFileSymbols(pp.c_str(), nosgbMunger);
}

static bool splitline(const char* input, AnsiString& symOut, AnsiString& valOut)
{
        AnsiString sym(input);
        sym = sym.Trim();
        if (sym.Length() == 0 || sym[1] == ';')
        {
                return false;
        }

        // take the last word from the line leaving the remainder in sym
        //
        int wspos = sym.LastDelimiter(",= \t");
        if (wspos == 0)
        {
                return false;
        }

        AnsiString val(sym.SubString(wspos + 1, sym.Length() - wspos));

        // chop any middle words from the line, leaving the symbol name
        //
        wspos = sym.LastDelimiter(",= \t");
        while (wspos != 0)
        {
                sym.SetLength(wspos - 1);
                sym = sym.Trim();
                wspos = sym.LastDelimiter(",= \t");
        }

        symOut = sym;
        valOut = val;
        return true;
}

void symbolstore_test(void)
{
        AnsiString sym, val;
        assert(!splitline("", sym, val));
        assert(!splitline("  \t", sym, val));
        assert(!splitline("  \t;", sym, val));
        assert(!splitline(";", sym, val));
        assert(!splitline("bert", sym, val));
        assert(!splitline("  bert\t ", sym, val));
        assert(!splitline(" \tbert", sym, val));
        assert(!splitline("tbert  ", sym, val));

        assert(splitline("bert ernie", sym, val));
        assert(sym == "bert" && val == "ernie");

        assert(splitline("bart\tmaggie", sym, val));
        assert(sym == "bart" && val == "maggie");

        assert(splitline("barn \t= horse", sym, val));
        assert(sym == "barn" && val == "horse");

        assert(splitline("burn equ fire", sym, val));
        assert(sym == "burn" && val == "fire");

        assert(splitline("burp \tequ something rude", sym, val));
        assert(sym == "burp" && val == "rude");
}

static void clearROMSymbols(void)
{
        romV2S.clear();
        romS2V.clear();
}

static void clearFileSymbols(void)
{
        fileV2S.clear();
        fileS2V.clear();
}

void symbolstore::reset(void)
{
        clearROMSymbols();
        clearFileSymbols();
}

static bool loadSymbols(const char* filename, VAL2SYM& v2s, SYM2VAL& s2v, symbolstore::SYMBOLMUNGER munger)
{
        FILE* symfile = fopen(filename, "r");
        if (!symfile)
        {
                return false;
        }

        char buf[128];
        AnsiString sym, val;

        while(fgets(buf, 128, symfile))
        {
                if (!splitline(buf, sym, val))
                {
                        continue;
                }

                if (munger)
                {
                        munger(sym, val);
                }

                val = "0x" + val.Trim();
                int v = StrToInt(val);
                v2s[v] = sym;
                s2v[sym] = v;
        }

        return true;
}

bool symbolstore::loadROMSymbols(const char* filename, symbolstore::SYMBOLMUNGER munger)
{
        clearROMSymbols();
        return loadSymbols(filename, romV2S, romS2V, munger);
}

bool symbolstore::loadFileSymbols(const char* filename, symbolstore::SYMBOLMUNGER munger)
{
        clearFileSymbols();
        return loadSymbols(filename, fileV2S, fileS2V, munger);
}

bool symbolstore::addressToSymbol(const int addr, AnsiString& result)
{
        VAL2SYM::iterator it = fileV2S.find(addr);
        if (it != fileV2S.end())
        {
                result = it->second;
                return true;
        }

        it = romV2S.find(addr);
        if (it != romV2S.end())
        {
                result = it->second;
                return true;
        }

        return false;
}

AnsiString symbolstore::addressToSymbolOrHex(const int addr)
{
        AnsiString temp;
        if (!addressToSymbol(addr, temp))
        {
                temp = "$" + temp.IntToHex(addr,4);
        }

        return temp;
}

// NUNS! REVERSE!
//
bool symbolstore::symbolToAddress(const AnsiString& sym, int& val)
{
        SYM2VAL::iterator it = fileS2V.find(sym);
        if (it != fileS2V.end())
        {
                val = it->second;
                return true;
        }

        it = romS2V.find(sym);
        if (it != romS2V.end())
        {
                val = it->second;
                return true;
        }

        return false;
}


// NOT THREAD SAFE! actually - none of it is :P
//
static SYM2VAL::iterator nasty;
static char type;

void symbolstore::beginenumeration(void)
{
        nasty = romS2V.begin();
        type = 'r';
}

bool symbolstore::enumerate(AnsiString& sym, int& val, char& storetype)
{
        if (nasty == romS2V.end())
        {
                type = 'p';
                nasty = fileS2V.begin();
        }
        if (nasty == fileS2V.end())
        {
                return false;
        }

        sym = nasty->first;
        val = nasty->second;
        storetype = type;

        ++nasty;

        return true;
}

// this is cheesy
//
bool symbolstore::romLoaded(void)
{
        return romV2S.size() != 0;
}

bool symbolstore::fileLoaded(void)
{
        return fileV2S.size() != 0;
}


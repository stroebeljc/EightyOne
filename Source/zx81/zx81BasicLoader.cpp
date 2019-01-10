#include "zx81BasicLoader.h"
#include <fstream>
#include <map>
#include <string>
#include <cctype>
#include <sstream>

using namespace std;

void zx81BasicLoader::OutputStartOfProgramData(int& addressOffset)
{
        // Output the system variables
        mProgramData[addressOffset++] = 0x00;   //VERSN
        OutputWord(addressOffset, 0x0000);      //E_PPC
        OutputWord(addressOffset, 0x0000);      //D_FILE
        OutputWord(addressOffset, 0x0000);      //DF_CC
        OutputWord(addressOffset, 0x0000);      //VARS
        OutputWord(addressOffset, 0x0000);      //DEST
        OutputWord(addressOffset, 0x0000);      //E_LINE
        OutputWord(addressOffset, 0x0000);      //CH_ADD
        OutputWord(addressOffset, 0x0000);      //X_PTR
        OutputWord(addressOffset, 0x0000);      //STKBOT
        OutputWord(addressOffset, 0x0000);      //STKEND
        mProgramData[addressOffset++] = 0x00;   //BERG
        OutputWord(addressOffset, 0x405D);      //MEM
        mProgramData[addressOffset++] = 0x00;   //SPARE1
        mProgramData[addressOffset++] = 0x02;   //DF_SZ
        OutputWord(addressOffset, 0x0000);      //S_TOP
        OutputWord(addressOffset, 0xFFFF);      //LAST_K
        mProgramData[addressOffset++] = 0x00;   //DBOUNC
        mProgramData[addressOffset++] = 0x37;   //MARGIN
        OutputWord(addressOffset, 0x0000);      //NXTLIN
        OutputWord(addressOffset, 0x0000);      //OLDPPC
        mProgramData[addressOffset++] = 0x00;   //FLAGX
        OutputWord(addressOffset, 0x0000);      //STRLEN
        OutputWord(addressOffset, 0x0C8D);      //T_ADDR
        OutputWord(addressOffset, 0x0000);      //SEED
        OutputWord(addressOffset, 0xE6E0);      //FRAMES
        OutputWord(addressOffset, 0x0000);      //COORDS
        mProgramData[addressOffset++] = 0xBC;   //PR_CC
        OutputWord(addressOffset, 0x1821);      //S_POSN
        mProgramData[addressOffset++] = 0x40;   //CDFLAG
        addressOffset += 32;                    //PRBUFF
        mProgramData[addressOffset++] = 0x76;   //PRBUFF
        addressOffset += 30;                    //MEMBOT
        OutputWord(addressOffset, 0x0000);      //SPARE
}

void zx81BasicLoader::OutputEndOfProgramData(int& addressOffset)
{
        // Insert a display file
        const int startOfRam = 16393;
        int dfileAddress = startOfRam + addressOffset;
        for (int d = 0; d < 25; d++)
        {
                mProgramData[addressOffset++] = Newline;
        }

        // Insert the variables area
        int varsAddress = startOfRam + addressOffset;

        mProgramData[addressOffset++] = 0x80;

        int elineAddress = startOfRam + addressOffset;

        // Update system variables
        int dfileOffset = 3;
        ChangeWord(dfileOffset, dfileAddress);
        int dfccOffset = 5;
        ChangeWord(dfccOffset, dfileAddress+1);
        int varsOffset = 7;
        ChangeWord(varsOffset, varsAddress);
        int elineOffset = 11;
        ChangeWord(elineOffset, varsAddress+1);
        int chaddOffset = 13;
        ChangeWord(chaddOffset, varsAddress+5);
        int stkbotOffset = 17;
        ChangeWord(stkbotOffset, elineAddress+5);
        int stkendOffset = 19;
        ChangeWord(stkendOffset, elineAddress+5);
        int nxtlinOffset = 32;
        ChangeWord(nxtlinOffset, varsAddress);
}

void zx81BasicLoader::Tokenise()
{
        map<unsigned char, string> tokens;

        tokens[125] = "RND";
        tokens[126] = "INKEY$";
        tokens[127] = "PI";

        tokens[192] = "\"\"";
        tokens[193] = "AT ";
        tokens[194] = "TAB ";
        tokens[196] = "CODE ";
        tokens[197] = "VAL ";
        tokens[198] = "LEN ";
        tokens[199] = "SIN ";
        tokens[200] = "COS ";
        tokens[201] = "TAN ";
        tokens[202] = "ASN ";
        tokens[203] = "ACS ";
        tokens[204] = "ATN ";
        tokens[205] = "LN ";
        tokens[206] = "EXP ";
        tokens[207] = "INT ";
        tokens[208] = "SQR ";
        tokens[209] = "SGN ";
        tokens[210] = "ABS ";
        tokens[211] = "PEEK ";
        tokens[212] = "USR ";
        tokens[213] = "STR$ ";
        tokens[214] = "CHR$ ";
        tokens[215] = "NOT ";
        tokens[216] = "**";
        tokens[217] = " OR ";
        tokens[218] = " AND ";
        tokens[219] = "<=";
        tokens[220] = ">=";
        tokens[221] = "<>";
        tokens[222] = " THEN ";
        tokens[223] = " TO ";
        tokens[224] = " STEP ";
        tokens[225] = " LPRINT ";
        tokens[226] = " LLIST ";
        tokens[227] = " STOP ";
        tokens[228] = " SLOW ";
        tokens[229] = " FAST ";
        tokens[230] = " NEW ";
        tokens[231] = " SCROLL ";
        tokens[232] = " CONT ";
        tokens[233] = " DIM ";
        tokens[234] = " REM ";
        tokens[235] = " FOR ";
        tokens[236] = " GOTO ";
        tokens[237] = " GOSUB ";
        tokens[238] = " INPUT ";
        tokens[239] = " LOAD ";
        tokens[240] = " LIST ";
        tokens[241] = " LET ";
        tokens[242] = " PAUSE ";
        tokens[243] = " NEXT ";
        tokens[244] = " POKE ";
        tokens[245] = " PRINT ";
        tokens[246] = " PLOT ";
        tokens[247] = " RUN ";
        tokens[248] = " SAVE ";
        tokens[249] = " RAND ";
        tokens[250] = " IF ";
        tokens[251] = " CLS ";
        tokens[252] = " UNPLOT ";
        tokens[253] = " CLEAR ";
        tokens[254] = " RETURN ";
        tokens[255] = " COPY ";
      
        DoTokenise(tokens);
}

void zx81BasicLoader::OutputLine(int lineNumber, int& addressOffset, unsigned char* pPos)
{
        mProgramData[addressOffset++] = lineNumber >> 8;
        mProgramData[addressOffset++] = lineNumber & 0xFF;

        int lineLengthOffset = addressOffset;
        addressOffset += 2;
        int commandOffset = addressOffset;

        char* pMatch;
        while ((pMatch = strstr((char*)pPos, "£")) != NULL)
        {
                *pMatch = PoundReplacement;
        }          
        while ((pMatch = strstr((char*)pPos, "#")) != NULL)
        {
                *pMatch = PoundReplacement;
        }

        bool withinQuotes = false;
        bool withinRem = false;

        while (*pPos != '\0')
        {
                unsigned char chr = *pPos;

                if (chr != 0x01)
                {
                        if (chr == '\"')
                        {
                                withinQuotes = !withinQuotes;
                        }
                        else if (chr == Rem)
                        {
                                withinRem = true;
                        }

                        if (chr == 0x7D)//RND
                        {
                                mProgramData[addressOffset++] = Rnd;
                        }
                        else if (chr == 0x7E)//INKEY$
                        {
                                mProgramData[addressOffset++] = Inkey;
                        }
                        else if (chr == 0x7F)//PI
                        {
                                mProgramData[addressOffset++] = Pi;
                        }
                        else if (chr == PoundReplacement)
                        {
                                mProgramData[addressOffset++] = AsciiToZX('£');
                        }
                        else if (chr >= 0x80)
                        {
                                mProgramData[addressOffset++] = chr;
                        }
                        else if ((chr == '%') || (chr == '\\'))
                        {
                                mProgramData[addressOffset++] = DecodeCharacter(&pPos);
                        }
                        else if (!withinQuotes && !withinRem && StartOfNumber(pPos))
                        {
                                OutputEmbeddedNumber(&pPos, addressOffset);
                        }
                        else
                        {
                                mProgramData[addressOffset++] = AsciiToZX(chr);
                        }
                }

                pPos++;
        }

        mProgramData[addressOffset++] = Newline;

        int lineLength = addressOffset - commandOffset;
        ChangeWord(lineLengthOffset, lineLength);
}

bool zx81BasicLoader::StartOfNumber(unsigned char* pPos)
{
        char chrPrev = pPos[-1];
        char chr1 = pPos[0];
        char chr2 = pPos[1];
        char chr3 = pPos[2];

        bool partOfVariableName = isalpha(chrPrev);
        bool beginsWithDigit = isdigit(chr1);
        bool plusMinusOrDecimal = (chr1 == '-' || chr1 == '+' || chr1 == '.') && isdigit(chr2);
        bool plusMinusAndDecimal = (chr1 == '-' || chr1 == '+') && chr2 == '.' && isdigit(chr3);

        return !partOfVariableName && (beginsWithDigit || plusMinusOrDecimal || plusMinusAndDecimal);
}

void zx81BasicLoader::OutputEmbeddedNumber(unsigned char** ppPos, int& addressOffset)
{
        char* pEnd;
        double value = strtod((char*)*ppPos, &pEnd);

        while (*ppPos < pEnd)
        {
                unsigned char chr = *(*ppPos);
                mProgramData[addressOffset++] = AsciiToZX(chr);
                (*ppPos)++;
        }

        while ((*(*ppPos) != '\0') && (*(*ppPos) == ' '))
        {
                mProgramData[addressOffset++] = Space;
                (*ppPos)++;
        }

        (*ppPos)--;

        mProgramData[addressOffset++] = Number;

        OutputFloatingPointEncoding(value, addressOffset);
}

void zx81BasicLoader::OutputFloatingPointEncoding(double value, int& addressOffset)
{                             
        int exponent = 0;
        unsigned long mantissa  = 0;

        if (value != 0)
        {
                if (value < 0)
                {
                        value = -value;
                }

                double exp = floor(DBL_EPSILON + (log(value) / log(2)));
                exponent = 1 + (int)exp;

                double man = value / pow(2, exponent);
                man *= 0x100000000;
                mantissa = (unsigned long)floor(DBL_EPSILON + man);

                // Numbers in BASIC are always positive (a leading negative sign is an operator and
                // not part of the actual number) so set the msb to 0 to signify a positive value
                mantissa &= 0x7FFFFFFF;

                exponent += 128;
                if (exponent < 1 || exponent > 255)
                {
                        throw out_of_range("Number out of range");
                }
        }

        mProgramData[addressOffset++] = (unsigned char)exponent;
        mProgramData[addressOffset++] = (unsigned char)(mantissa >> 24);
        mProgramData[addressOffset++] = (unsigned char)(mantissa >> 16);
        mProgramData[addressOffset++] = (unsigned char)(mantissa >> 8);
        mProgramData[addressOffset++] = (unsigned char)(mantissa & 0xFF);
}

unsigned char zx81BasicLoader::DecodeGraphic(unsigned char chr1, unsigned char chr2)
{
        map<string, unsigned char> graphics;

        graphics["  "] = 0;
        graphics["' "] = 1;
        graphics[" '"] = 2;
        graphics["''"] = 3;
        graphics[". "] = 4;
        graphics[": "] = 5;
        graphics[".'"] = 6;
        graphics[":'"] = 7;
        graphics["##"] = 8;
        graphics[",,"] = 9;
        graphics["~~"] = 10;    
        graphics["::"] = 128;
        graphics[".:"] = 129;
        graphics[":."] = 130;
        graphics[".."] = 131;
        graphics["':"] = 132;
        graphics[" :"] = 133;
        graphics["'."] = 134;
        graphics[" ."] = 135;
        graphics["@@"] = 136;
        graphics[";;"] = 137;
        graphics["!!"] = 138;

        string search;
        search += (char)chr1;
        search += (char)chr2;

        map<string, unsigned char>::const_iterator it = graphics.find(search);
        if (it == graphics.end())
        {
                ostringstream code;
                code << chr1;
                code << chr2;
                string msg = "Invalid graphic Code: " + code.str();
                throw out_of_range(msg.c_str());
        }

        return it->second;
}

unsigned char zx81BasicLoader::AsciiToZX(unsigned char ascii)
{
        const unsigned char Number0 = 28;
        const unsigned char LetterA = 38;

        if (isalpha(ascii))
        {
                return (ascii - 'A') + LetterA;
        }
        else if (isdigit(ascii))
        {
                return (ascii - '0') + Number0;
        }

        unsigned char zxChr;

        map<string, unsigned char> chrs;

        switch ((char)ascii)
        {
                case ' ':
                        zxChr = 0;
                        break;
                case '\"':
                        zxChr = 11;
                        break;
                case '£':
                case '#':
                        zxChr = 12;
                        break;
                case '$':
                        zxChr = 13;
                        break;
                case ':':
                        zxChr = 14;
                        break;
                case '?':
                        zxChr = 15;
                        break;
                case '(':
                        zxChr = 16;
                        break;
                case ')':
                        zxChr = 17;
                        break;
                case '-':
                        zxChr = 22;
                        break;
                case '+':
                        zxChr = 21;
                        break;
                case '*':
                        zxChr = 23;
                        break;
                case '/':
                        zxChr = 24;
                        break;
                case '=':
                        zxChr = 20;
                        break;
                case '>':
                        zxChr = 18;
                        break;
                case '<':
                        zxChr = 19;
                        break;
                case ';':
                        zxChr = 25;
                        break;
                case ',':
                        zxChr = 26;
                        break;
                case '.':
                        zxChr = 27;
                        break;
                default:
                        ostringstream code;
                        code << ascii;
                        string msg = "Invalid character: " + code.str();
                        throw out_of_range(msg.c_str());
        }

        return zxChr;
}
                                      
void zx81BasicLoader::ReplaceStrings()
{
        const int Quote = '\"';

        int i = 0;
        bool withinQuote = false;

        while (mLineBuffer[i] != '\0')
        {
                if (!withinQuote)
                {
                        if (mLineBuffer[i] == Quote)
                        {
                                withinQuote = true;
                        }
                }
                else
                {
                        mLineBufferDestringed[i] = ' ';

                        char chr1 = mLineBuffer[i];
                        char chr2 = mLineBuffer[i + 1];

                        if (chr1 == Quote)
                        {
                                if (chr2 == Quote)
                                {
                                        mLineBuffer[i] = 0xC0;
                                        i++;
                                        mLineBuffer[i] = Blank;
                                        mLineBufferDestringed[i] = ' ';
                                }
                                else
                                {
                                        withinQuote = false;
                                }
                        }
                }

                i++;
        }
}

void IBasicLoader::ReplaceStrings()
{
        const int Quote = '\"';

        char* pPos = mLineBufferDestringed;
        bool withinQuote = false;

        while (*pPos != '\0')
        {
                if (*pPos == Quote)
                {
                        withinQuote = !withinQuote;
                }
                else if (withinQuote)
                {
                        *pPos = ' '; 
                }

                pPos++;
        }
}

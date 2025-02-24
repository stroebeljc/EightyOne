/* EightyOne - A Windows emulator of the Sinclair ZX range of computers.
 * Copyright (C) 2003-2025 Michael D Wynne
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __addressresolver_h
#define __addressresolver_h

class AddressResolver
{
public:
        static bool Validate(AnsiString addrString, int& address)
        {
                AnsiString addressValue = addrString.Trim();

                if (addressValue.Length() == 0)
                        return false;

                if (addressValue == "$")
                        return false;

                int tempAddress;
                if (symbolstore::symbolToAddress(addressValue, tempAddress))
                {
                        addressValue = "0x" + IntToHex(tempAddress, 4);
                }

                if (addressValue[1] == '$')
                {
                        addressValue = "0x" + addressValue.SubString(2, addressValue.Length() - 1);
                }

                // which allows us to use a ninja converter.
                // suddenly 0xABCD, $ABCD, 01234 [octal] and decimal are all valid.
                //
                char* endPtr;
                tempAddress = int(strtol(addressValue.c_str(), &endPtr, 0));
                if (*endPtr != 0)
                {
                        return false;
                }

                address = tempAddress;
                return true;
        }
};

#endif //__addressresolver_h


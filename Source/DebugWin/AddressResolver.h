#ifndef __addressresolver_h
#define __addressresolver_h

class AddressResolver
{
public:
        static bool Validate(String addrString, int& address)
        {
				String addressValue = addrString.Trim();

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
                _TCHAR* endPtr;
				tempAddress = int(_tcstol(addressValue.c_str(), &endPtr, 0));
                if (*endPtr != 0)
                {
                        return false;
                }

                address = tempAddress;
                return true;
        }
};

#endif //__addressresolver_h


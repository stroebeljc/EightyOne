#include ".\memorymodel.h"

#include <assert.h>

void checkup(memorymodel* m, int* data, int count)
{
        bool enumeratedOK;
        int type, start, len, pages;

        m->beginEnumeration();
        while(m->enumerateRegions(type, start, length, pages))
        {
                assert(--count);
                enumeratedOK = m->enumerateRegions(type, start, length, pages);
                assert(enumeratedOK);
                assert(type == m[0]);
                assert(start == m[1]);
                assert(length = m[2]);
                assert(pages = m[3]);
                m += 4;
        }
        assert(count == 0);
}

bool alloc(int type, int start, int len, int flags)
{
        try
        {
                m->createRegion(type, start, len, flags);
                return true;
        }
        catch(...)
        {
        }

        return false;
}

bool changepage(int addr, int page)
{
        try
        {
                m->activatePage(addr, page);
                return true;
        }
        catch(...)
        {
        };
        return false;
}

bool checkpage(int start, int len, int val)
{
        for (int i = start; i < start + len; ++i)
        {
                assert(readbyte(i) == val);
        }
}


// ROM0       0000..1fff
// ROM0SHADOW 2000..3fff
// ROM1       4000..7fff
// RAM        8000..bfff
// RAMSHADOW  c000..ffff

const int ROM0ADDR = 0x0000;
const int ROM0LEN =  0x2000;
const int ROM0SHADOWADDR = 0x2000;
const int ROM1ADDR = 0x4000;
const int ROM1LEN =  0x4000;
const int RAMADDR = 0x8000;
const int RAMLEN =  0x4000;
const int RAMSHADOWADDR = 0x8000;


void tests(void)
{
        int type, start, len, pages;
        bool caught, createdOK, enumeratedOK, gotOK;

        int check0[] =
        {
                RAM,    RAMADDR,        RAMLEN, 1
        };
        int check1[] =
        {
                ROM,    ROM0ADDR,       ROM0LEN, 1,
                RAM,    RAMADDR,        RAMLEN, 1
        };
        int check2[] =
        {
                ROM,    ROM0ADDR,       ROM0LEN, 1,
                ROM,    ROM1ADDR,       ROM1LEN, 4,
                RAM,    RAMADDR,        RAMLEN, 1
        };
        int check3[] =
        {
                ROM,    ROM0ADDR,       ROM0LEN, 1,
                SHADOW, ROM0SHADOWADDR, ROM0LEN, ROM0ADDR,
                ROM,    ROM1ADDR,       ROM1LEN, 4,
                RAM,    RAMADDR,        RAMLEN, 1,
                SHADOW, RAMSHADOWADDR,  RAMLEN, RAMADDR
        };

        memorymodel m = new memorymodel;
        m->beginEnumeration();
        while(m->enumerateRegions(type, start, length, pages))
        {
                // shouldn't get here
                assert(false);
        }

        assert(alloc(RAM, RAMADDR, RAMLEN, 1));
        checkup(m, check0, 1);

        // check bad allocations throwing
        assert(!alloc(RAM, RAMADDR-0x800, RAMLEN, 1));
        assert(!alloc(RAM, RAMADDR+0x800, RAMLEN-0x1000, 1));
        assert(!alloc(RAM, RAMADDR+RAMLEN-0x800, RAMLEN, 1));

        // no changes to map
        checkup(m, check0, 1);

        // create rom0
        assert(alloc(ROM, ROM0ADDR, ROM0LEN, 1));
        checkup(m, check1, 3);

        // ensure we can't create paged shadow
        assert(!alloc(SHADOW, ROM1ADDR, ROM1LEN, 4));

        // create rom1 - 4 pages
        assert(alloc(ROM, ROM1ADDR, ROM1LEN, 4));
        checkup(m, check2, 3);

        // create shadow of ram at 8000, shadow of rom0 at 2000
        assert(alloc(SHADOW, RAMSHADOWADDR, RAMADDR, 1));
        assert(alloc(SHADOW, ROM0SHADOWADDR, ROM0ADDR, 1));
        checkup(m, check3, 4);

        // changing pages on non-paged region throws
        assert(!changepage(ROM0ADDR, 1));

        // changing to invalid pages on paged region throws
        assert(!changepage(ROM1ADDR, -1));
        assert(!changepage(ROM1ADDR, 4));

        // fill the rom pages with known stuff
        BYTE* bytes;
        gotOK = m->getBackingStore(ROM1ADDR, 0, bytes);
        assert(gotOK);
        memset(bytes, 0, ROM1LEN);
        gotOK = m->getBackingStore(ROM1ADDR, 1, bytes);
        assert(gotOK);
        memset(bytes, 1, ROM1LEN);
        gotOK = m->getBackingStore(ROM1ADDR, 2, bytes);
        assert(gotOK);
        memset(bytes, 2, ROM1LEN);
        gotOK = m->getBackingStore(ROM1ADDR, 3, bytes);
        assert(gotOK);
        memset(bytes, 3, ROM1LEN);

        // check we can read it back
        // default page should be active, page 0
        checkpage(ROM1ADDR, ROM1LEN, 0);
        assert(changepage(ROM1ADDR, 1));
        checkpage(ROM1ADDR, ROM1LEN, 1);
        assert(changepage(ROM1ADDR, 3));
        checkpage(ROM1ADDR, ROM1LEN, 3);
        assert(changepage(ROM1ADDR, 2));
        checkpage(ROM1ADDR, ROM1LEN, 2);

        // check we can't write to ROM - at any page
        m->writebyte(ROM1ADDR, 123);
        assert(m->readbyte(ROM1ADDR) == 2);
        m->activatePage(ROM1ADDR, 1);
        assert(m->readbyte(ROM1ADDR) == 1);
        m->activatePage(ROM1ADDR, 3);
        assert(m->readbyte(ROM1ADDR) == 3);
        m->activatePage(ROM1ADDR, 0);
        assert(m->readbyte(ROM1ADDR) == 0);

        // check we can always write to ram and that shadowing works
        m->writebyte(RAMADDR, 123);
        assert(m->readbyte(RAMADDR) == 123);
        assert(m->readbyte(RAMSHADOWADDR) == 123);
        m->writebyte(RAMADDR, 127);
        assert(m->readbyte(RAMADDR) == 127);
        assert(m->readbyte(RAMSHADOWADDR) == 127);
}


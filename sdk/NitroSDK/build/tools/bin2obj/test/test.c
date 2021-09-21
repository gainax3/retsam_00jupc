#include <nitro.h>

extern char testdata_begin[];
extern char testdata_end[];

void NitroMain(void)
{
    char   *cp;

    if ((int)testdata_begin < (int)testdata_end)
    {
        for (cp = testdata_begin; cp != testdata_end; cp++)
        {
            OS_TPrintf("%c", *cp);
        }
    }
}

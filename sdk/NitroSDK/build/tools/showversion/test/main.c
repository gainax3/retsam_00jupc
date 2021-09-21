
#define INFO(major,minor,patch,relstep)    ((major)<<24|(minor)<<20|(patch)<<16|(relstep))

unsigned long VER[] = {
    // 2.0 test1
    INFO(2, 0, 0, 100),
    0xdec00621,
    0x2106c0de,

    // 2.0 test2
    INFO(2, 0, 0, 200),
    0xdec00621,
    0x2106c0de,

    // 2.0 PR1
    INFO(2, 0, 0, 10100),
    0xdec00621,
    0x2106c0de,

    // 2.0 PR2
    INFO(2, 0, 0, 10200),
    0xdec00621,
    0x2106c0de,

    // 2.0 PR6
    INFO(2, 0, 0, 10600),
    0xdec00621,
    0x2106c0de,

    // IPL
    INFO(2, 0, 0, 10650),
    0xdec00621,
    0x2106c0de,

    // 2.0 PR6 plus
    INFO(2, 0, 0, 10651),
    0xdec00621,
    0x2106c0de,

    // 2.0 PR6 plus2
    INFO(2, 0, 0, 10652),
    0xdec00621,
    0x2106c0de,

    // 2.0 FC
    INFO(2, 0, 0, 20000),
    0xdec00621,
    0x2106c0de,

    // 2.0 FC plus
    INFO(2, 0, 0, 20001),
    0xdec00621,
    0x2106c0de,

    // 2.0 FC plus2
    INFO(2, 0, 0, 20002),
    0xdec00621,
    0x2106c0de,

    // 2.0 RC1
    INFO(2, 0, 0, 20100),
    0xdec00621,
    0x2106c0de,

    // 2.0 RC1 plus
    INFO(2, 0, 0, 20101),
    0xdec00621,
    0x2106c0de,

    // 2.0 RC1 plus2
    INFO(2, 0, 0, 20102),
    0xdec00621,
    0x2106c0de,

    // 2.0 RC2
    INFO(2, 0, 0, 20200),
    0xdec00621,
    0x2106c0de,

    // 2.0 Release
    INFO(2, 0, 0, 30000),
    0xdec00621,
    0x2106c0de,

    // 2.1 Release
    INFO(2, 1, 0, 30000),
    0xdec00621,
    0x2106c0de,

    // 2.1 patch1 Release
    INFO(2, 1, 1, 30000),
    0xdec00621,
    0x2106c0de,

    // 3.0 Release
    INFO(3, 0, 0, 30000),
    0xdec00621,
    0x2106c0de,
};

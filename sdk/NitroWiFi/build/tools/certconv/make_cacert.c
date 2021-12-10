//
//  $Revision: 1.2 $    $Date: 2006/03/10 09:22:09 $
//
//	Copyright (C) Ubiquitous Corp. 2001-2005. All rights reserved.
//
#include <stdio.h>
#include <string.h>              // memcmp, strlen, strcpy
#include <stdlib.h>              // exit

typedef unsigned char   uchar;
typedef unsigned long   ulong;

FILE*   fp;

static void dump_hex(uchar* s, int len)
{
    int i;

    for (i = 0; i < len; i++)
    {
        if (i % 8 == 0)
            fprintf(fp, "\n\t");
        fprintf(fp, "0x%02x", *s++);
        if (i < len - 1)
            fprintf(fp, ",");
    }

    fprintf(fp, "\n");
}

static void dump_quoted_string(uchar* s)
{
    uchar   c;

    fprintf(fp, "\"");
    while ((c = *s++) != '\0')
    {
        if (c == '"')
            fprintf(fp, "\\%c", c);
        else if (' ' <= c && c <= 0x7e)
            fprintf(fp, "%c", c);
        else
            fprintf(fp, "\\x%02x", c);
    }

    fprintf(fp, "\"");
}

int     pub_algorithm;      // public key algorithm
int     seen_pub_algorithm; // next BIT STRING is public key information
uchar*  modulo;             // pointer to modulo (big-endian)
int     modulo_len;         // length of modulo in bytes
uchar*  exponent;           // pointer to exponent (big-endian)
int     exponent_len;       // length of exponent in bytes
int     seen_validity;
uchar   subject[512];

int cert_item_len(uchar ** ps)
{
    uchar*  s = *ps;
    int     len;
    int     lenlen;

    len = *s++;
    if ((len & 0x80) != 0)
    {
        lenlen = len & 0x7f;
        len = 0;
        while (lenlen--)
            len = (len << 8) +*s++;
    }

    *ps = s;
    return len;
}

enum
{
    DummyEncryption,
    RSAEncryption,

    NUMOBJECTS              // number of objects including dummy
};

uchar* object[] = {
    "\xff\xff\xff",         // DummyEncryption
    "\x2a\x86\x48\x86\xf7\x0d\x01\x01\x01",
};

static void analyze_object(int id, int level)
{
    switch (id)
    {
    case RSAEncryption:
        pub_algorithm = id;
        seen_pub_algorithm = id;
        break;
    }
}

static void make_dn(uchar* d, uchar* s, int len)
{
    if (*d != '\0')
    {
        while (*d != '\0')
            d++;
        *d++ = ',';
        *d++ = ' ';
    }

    while (len--)
        *d++ = *s++;
    *d = '\0';
}

void cert_item(uchar ** ps, int level, int seq)
{
    uchar*  s = *ps;
    uchar   type;
    int     len;
    uchar*  end;
    int     i;
    int     sub_seq;

    type = *s++;
    len = cert_item_len(&s);
    switch (type & 0x1f)
    {
    case 0x02:              // integer
        if (seen_pub_algorithm)
            if (seq == 0)
            {
                while (*s == 0)
                {
                    s++;
                    len--;
                }

                modulo = s;
                modulo_len = len;
            }
            else if (seq == 1)
            {
                while (*s == 0)
                {
                    s++;
                    len--;
                }

                exponent = s;
                exponent_len = len;
            }

        s += len;
        break;

    case 0x03:              // Bit string
        if (seen_pub_algorithm)
        {
            s++;            // skip 0x00
            cert_item(&s, level, 0);
            seen_pub_algorithm = 0;
        }
        else
            s += len;
        break;

    case 0x04:              // Octet string
    case 0x05:              // NULL
        s += len;
        break;

    case 0x06:              // Object
        for (i = 0; i < NUMOBJECTS; i++)
            if (memcmp(s, object[i], strlen(object[i])) == 0)
            {
                analyze_object(i, level);
                break;
            }

        s += len;
        break;

    case 0x13:              // String
    case 0x14:
    case 0x16:
        if (seen_validity)
            make_dn(subject, s, len);
        s += len;
        break;

    case 0x17:              // Time
    case 0x18:
        seen_validity = 1;
        s += len;
        break;

    case 0x10:              // Sequence
        end = s + len;
        sub_seq = 0;
        while (s < end)
            cert_item(&s, level + 1, sub_seq++);
        break;

    case 0x11:              // Set
        end = s + len;
        while (s < end)
            cert_item(&s, level + 1, 0);
        break;

    default:
        if (type == 0xa0)
        {
            //
            //	Explicit
            //
            end = s + len;
            while (s < end)
                cert_item(&s, level + 1, 0);
        }
        else
        {
            fprintf(stderr, "Unexpected type 0x%02x\n", type);
            s += len;
        }
        break;
    }

    *ps = s;
}

static void usage()
{
    fprintf(stderr, "\
usage: make_cacert -n <symbol_name> [-o outfile] infile\n\
");
    exit(1);
}

#define BUFLEN  8192
uchar   buf[BUFLEN];

#define GETARG  ((argv[0][2] != '\0' || argc < 2) ? &argv[0][2] : (argc--, *++argv))

int main(int argc, char ** argv)
{
    uchar*  s;
    char*   outfile;
    char*   name;
    FILE*   fpi;
    int     i;

    outfile = NULL;
    name = NULL;
    while (argc > 1 && **(argv + 1) == '-')
    {
        argc--;
        argv++;
        switch (argv[0][1])
        {
        case 'o':
            outfile = GETARG;
            break;

        case 'n':
            name = GETARG;
            break;

        default:
            usage();
            break;
        }
    }

    if (name == NULL || argc < 2)
        usage();
    if (outfile == NULL)
        fp = stdout;
    else if ((fp = fopen(outfile, "w")) == NULL)
    {
        fprintf(stderr, "failed to create %s.\n", outfile);
        exit(1);
    }

    fprintf(fp, "#include <nitro.h>\n");
    fprintf(fp, "#include <nitroWiFi.h>\n");
    fpi = fopen(*++argv, "rb");
    if (fpi == NULL)
    {
        fprintf(stderr, "failed to open %s.\n", *argv);
        exit(1);
    }

    fread(buf, 1, BUFLEN, fpi);
    fclose(fpi);
    s = buf;
    pub_algorithm = -1;
    seen_pub_algorithm = 0;
    seen_validity = 0;
    subject[0] = '\0';
    cert_item(&s, 0, 0);
    if (pub_algorithm == -1)
    {
        fprintf(stderr, "No known public key algorithm found.\n");
        exit(1);
    }

    if (subject[0] == '\0')
    {
        fprintf(stderr, "No subject found.\n");
        exit(1);
    }

    fprintf(fp, "\nstatic char ca_name[] = ");
    dump_quoted_string(subject);
    fprintf(fp, ";\n\n");
    fprintf(fp, "static unsigned char ca_modulo[] = {");
    dump_hex(modulo, modulo_len);
    fprintf(fp, "};\n\n");
    fprintf(fp, "static unsigned char ca_exponent[] = {");
    dump_hex(exponent, exponent_len);
    fprintf(fp, "};\n\n");

    //--- modified by seiki_masashi@nintendo.co.jp
    fprintf(fp, "SOCCaInfo %s = {\n", name);

    //---
    fprintf(fp, "\tca_name,\n\tsizeof(ca_modulo),\n\tca_modulo,\n\tsizeof(ca_exponent),\n\tca_exponent\n");
    fprintf(fp, "};\n");
    if (fp != stdout)
        fclose(fp);
}

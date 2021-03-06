/* Copyright(c) 1986 Association of Universities for Research in Astronomy Inc.
 */

#include <stdio.h>

/* SGI2XBM.C -- Read an IRAF SGI bitmap file on standard input and convert
 *  to a GIF format image on standard outout.
 *
 *  Usage:
 *          sgi2xbm.e [-w N] [-h NY] [-i] [ [sgi_bitmap] [sgi_bitmap] ... ]
 *
 *      -w N         width of input bitmap and output image
 *      -h N         height of input bitmap and output image
 *	-i	     invert the bitmap values before conversion
 *
 *  The input file name and the switches may occur in any order.  The bitmap
 * may be inverted here using the -i flag.
 *
 *  Sample graphcaps for this translator might look like:
 *
 *      g-xbm|UNIX generic interface to multi-frame XBM file generator:\
 *          :DD=ugif,tmp$sgk,!{ sgidispatch sgi2xbm -w $(PX) -h $(PY) \
 *	    $F.[1-8]  > sgixbm$$; rm $F.[1-8]; }&:\
 *	    :MF#8:NF:tc=sgi_image_format:
 *
 *      sgi_image_format|Generic raster file format specification:\
 *          :kf=bin$x_sgikern.e:tn=sgikern:ar#.75:\
 *          :xr#640:yr#480:PX#640:PY#480:XW#640:YW#480:\
 *          :BI:MF#1:YF:NB#8:LO#1:LS#0:XO#0:YO#0:
 *
 *  All bitmaps will be dumped to the file 'sgixbmXXX' in the local directory
 *  where XXX is a pid.
 *
 *  To change the image size the graphcap :xr, :PX, :XW (X-dimension) and
 *  :yr, :PY, :XY (Y-dimension) fields all need to be changed.  The -i 
 *  flag must be specified in the graphcap DD string.
 *
 */

#define	NBITS_CHAR	8		/* number of bits in a char 	*/
#define DEF_WIDTH       640             /* default image width          */
#define DEF_HEIGHT      480             /* default image height         */
#define MAX_INFILES     16              /* max number of input bitmaps  */
#define SZ_FNAME        64              /* size of a filename           */

typedef unsigned char   byte;

static int px = DEF_WIDTH;
static int py = DEF_HEIGHT;
static int invert = 0;
static char *infile[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static int  	isSwappedMachine();
static void 	bswap4();


main (argc, argv)
int	argc;
char	**argv;
{
	FILE	*fd;
	byte 	*buffer, *ip;
	int	i, n, len_buf, numin = 0, cnt = 0;
	int     index; 			/* goes through all files */


	/* Process the command line.
	 */
        for (i=1;  i < argc;  i++) {
            if (argv[i][0] == '-') {
                if (strcmp (argv[i], "-w") == 0)
                    px = atoi (argv[++i]);
                else if (strcmp (argv[i], "-h") == 0)
                    py = atoi (argv[++i]);
                else if (strcmp (argv[i], "-i") == 0)
                    invert++;
            } else {
                /* input sgi-bitmap file specification */
                if (numin < MAX_INFILES)
                    infile[numin++] = argv[i];     
            }

	}

	/* Allocate some space for the working buffer. */
	len_buf = px / NBITS_CHAR;
	buffer = (byte *) malloc (len_buf);

        /* Loop over the input bitmaps, writing the converted output to
         * the stdout.
         */
        for (index = 0; index == 0 || index < numin; index++) {

            /* Open the input file. */
            fd = (infile[index] ? fopen (infile[index], "r") : stdin);

	    if (index > 0) printf ("\n");

	    printf ("#define xbm%03d_width %d\n", index, px);
	    printf ("#define xbm%03d_height %d\n", index, py);
	    printf ("static char xbm%03d_bits[] = {\n  ", index);

	    n = 0;
	    cnt = 0;
	    while (fread (buffer, len_buf, 1, fd)) {
                /* If we're on a MSB ordered machine wordswap the bitmap so
                 * it's in the correct order for unpacking to be interpreted
                 * as an LSB-ordered image.
                 */
                if (!isSwappedMachine())
                    bswap4 (buffer, buffer, len_buf);

		/* Write out the pixels. */
		for (i=0; i < len_buf; i++, cnt++) {
		    printf ("0x%0.2x", 
			(byte) (invert ? ~buffer[i]: buffer[i])), n += 4;
		    if (cnt < (len_buf * py - 1))
		        printf (","), n++;
		    else
	    		printf ("};\n");
		    if (n > 70)
		        printf ("\n  "), n=0;
		}
	    }

            if (fd != stdin)
                fclose (fd);
            fflush (fd);
	}
	free (buffer);
}


/* IS_SWAPPED_MACHINE -- See if this is a byte-swapped (LSB) machine.
 */

static int
isSwappedMachine ()
{
        union {
            char ch[4];
            int  i;
        } u;

        u.i = 1;
        return ((int) u.ch[0]);
}


/* BSWAP4 - Move bytes from array "a" to array "b", swapping the four bytes
 * in each successive 4 byte group, i.e., 12345678 becomes 43218765.
 * The input and output arrays may be the same but may not partially overlap.
*/
static void
bswap4 (a, b, nbytes)
byte   *a;                     /* input array                  */
byte   *b;                     /* output array                 */
int    nbytes;                 /* number of bytes to swap      */
{
        register byte   *ip, *op, *tp;
        register int    n;
        static   byte temp[4];

        tp = temp;
        ip = (byte *)a;
        op = (byte *)b;

        /* Swap successive four byte groups.
         */
        for (n = nbytes >> 2;  --n >= 0;  ) {
            *tp++ = *ip++;
            *tp++ = *ip++;
            *tp++ = *ip++;
            *tp++ = *ip++;
            *op++ = *--tp;
            *op++ = *--tp;
            *op++ = *--tp;
            *op++ = *--tp;
        }

        /* If there are any odd bytes left, move them to the output array.
         * Do not bother to swap as it is unclear how to swap a partial
         * group, and really incorrect if the data is not modulus 4.
         */
        for (n = nbytes & 03;  --n >= 0;  )
            *op++ = *ip++;
}

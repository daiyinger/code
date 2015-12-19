#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <setjmp.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <jpeglib.h>


#define SCALEBITS 10
#define FIX(x)    ((int) ((x) * (1<<SCALEBITS) + 0.5))
#define ONE_HALF  (1 << (SCALEBITS - 1))

#define RGB_TO_Y_CCIR(r, g, b)\
((FIX(0.29900*219.0/255.0) * (r) + FIX(0.58700*219.0/255.0) * (g) +\
  FIX(0.11400*219.0/255.0) * (b) + (ONE_HALF + (16 << SCALEBITS))) >> SCALEBITS)

#define RGB_TO_U_CCIR(r1, g1, b1, shift)\
(((- FIX(0.16874*224.0/255.0) * r1 - FIX(0.33126*224.0/255.0) * g1 +\
     FIX(0.50000*224.0/255.0) * b1 + (ONE_HALF << shift) - 1) >> (SCALEBITS + shift)) + 128)

#define RGB_TO_V_CCIR(r1, g1, b1, shift)\
(((FIX(0.50000*224.0/255.0) * r1 - FIX(0.41869*224.0/255.0) * g1 -\
   FIX(0.08131*224.0/255.0) * b1 + (ONE_HALF << shift) - 1) >> (SCALEBITS + shift)) + 128)

/* declaration in stdio.h is there, but does not work */
extern FILE *fmemopen (void *__s, size_t __len, __const char *__modes) __THROW;


struct jpeg_error_manager
{
struct jpeg_error_mgr pub;    /* "public" fields */
jmp_buf setjmp_buffer;    /* for return to caller */
};

typedef struct jpeg_error_manager * my_error_ptr;

int jpeg_to_yuv(char *jpeg_buffer,unsigned char *yuv_buffer)
{
int yuv_header_send_flag = 0;
int a, i, k;
int width;
int height;
int r, g, b;
struct jpeg_decompress_struct cinfo;
struct jpeg_error_manager jerr;
FILE *infile;                /* source file */
JSAMPARRAY buffer;            /* Output row buffer */
int row_stride;                /* physical row width in output buffer */
double y, u, v;
double cr, cg, cb, cu, cv;
unsigned char *y_ptr, *u_ptr, *v_ptr;
int xa, ya;
int yuv_buffer_length;
unsigned char *uptr;
double **up;
double **vp;
unsigned char *rgb24_buffer;
int tmp;
static int have_tried_flag; /* retried putimage on first image, else a white picture, why? */

a = 0; // -Wall

/* pick a color standard */
/* Panteltje's y spec */ cr = 0.3; cg = 0.59; cb = 0.11;
// /* ITU-601-1 Y spec */ cr = 0.299; cg = 0.587; cb = 0.114; /* used by ffplay and likely mplayer */
// /*  CIE-XYZ 1931 Kodak, postscript pdf Y spec */ cr = 0.298954; cg = 0.586434; cb = 0.114612;
// /* ITU-709 NTSC Y spec */ cr = 0.213; cg = 0.715; cb = 0.072;
// /* ITU/EBU 3213 PAL Y spec */ cr = 0.222; cg = 0.707; cb = 0.071;
/* U spec */
cu = .5 / (1.0 - cb);
/* V spec */
cv = .5 / (1.0 - cr);
//char *jpeg_buffer; // the jpeg picture data
int content_length;
infile = (FILE *)fmemopen(jpeg_buffer, content_length, "r");

/* allocate and initialize JPEG decompression object */
/* We set up the normal JPEG error routines, then override error_exit. */
cinfo.err = jpeg_std_error(&jerr.pub);

if(setjmp(jerr.setjmp_buffer))
    {
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    /* Do not disconnect from the camera in case of some wrong bytes in jpeg buffer, but retry */
    return 1;
//    return 0;
    }

/* Now we can initialize the JPEG decompression object. */
jpeg_create_decompress(&cinfo);
/* specify data source (eg, a file) */
jpeg_stdio_src(&cinfo, infile);
/* read file parameters with jpeg_read_header() */
(void) jpeg_read_header(&cinfo, TRUE);
/* 
set parameters for decompression
Don't need to change any of the defaults set by jpeg_read_header(),
so we do nothing here.
*/
/* Maybe use YUV directly, what about X11? */
//cinfo.out_color_space = JCS_YCbCr;
//cinfo.raw_data_out = TRUE;


/* Start decompressor */ (void) jpeg_start_decompress(&cinfo);

/* 
Make an output work buffer of the right size.
JSAMPLEs per row in output buffer
*/
row_stride = cinfo.output_width * cinfo.output_components;
width = cinfo.output_width;
height = cinfo.output_height;

if(! rgb24_buffer)
    {
    rgb24_buffer = (unsigned char *)malloc(width * height * 3);
    if(! rgb24_buffer)
        {    
        fprintf(stderr, "jpg_to_yuv:malloc rgb24_buffer failed, aborting/n");

        exit(1);
        }    
    }

if(! yuv_header_send_flag)
    {
    /* send the mjpeg tools yuv stream header */

    /* stream stream header to get width and height and interlace */

    /*
    YUV4MPEG2 W720 H576 F25:1 I? A0:0 XM2AR002
    FRAME
    data
    */
	int seconds_per_frame = 0;
    fprintf(stdout,\
    "YUV4MPEG2 W%d H%d F1:%d I? A0:0 XM2AR002/n", width, height, seconds_per_frame);

    yuv_header_send_flag = 1;
    }

/*
Make a one-row-high sample array that will go away when done with image
*/
buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

/* 
while (scan lines remain to be read)
           jpeg_read_scanlines(...);
Here we use the library's state variable cinfo.output_scanline as the
loop counter, so that we don't have to keep track ourselves.
*/


if(cinfo.out_color_space == JCS_RGB)
    {
    uptr = rgb24_buffer;
    //WARNING programmer beware!!! cinfo.output_scanline starts at 1 
    for(ya = 0; ya < cinfo.output_height; ya++) 
        {
        /*
        jpeg_read_scanlines expects an array of pointers to scanlines.
        Here the array is only one element long, but you could ask for
        more than one scanline at a time if that's more convenient.
        */
        (void) jpeg_read_scanlines(&cinfo, buffer, 1);

        for(i = 0; i < row_stride; i += 3)
            {
            r = (int)buffer[0][i];
            g = (int)buffer[0][i + 1];
            b = (int)buffer[0][i + 2];

            *uptr++ = r;
            *uptr++ = g;
            *uptr++ = b;            
            } /* end for i */

        } /* end while all scan lines */
    } /* end if JCS_RGB */
else if(cinfo.out_color_space == 1)
    {
    uptr = rgb24_buffer;    
    for(ya = 0; ya < cinfo.output_height; ya++)
        {
        (void) jpeg_read_scanlines(&cinfo, buffer, 1);        
        for(i = 0; i < row_stride; i += 1)
            {
            g = (int)buffer[0][i];

            *uptr++ = g;
            *uptr++ = g;
            *uptr++ = g;            
            } /* end for i */
        } /* end while all scan lines */
    } /* end if color_space == 1 */
else
    {
    fprintf(stderr, "jpg_to_yuv:unsupported jpeg color space %d, aborting./n",  cinfo.out_color_space);
    exit(1);    
    }
    

yuv_buffer_length = (width * height) + ( (width / 2) * (height / 2) ) + ( (width / 2) * (height / 2) );

yuv_buffer = malloc(yuv_buffer_length);
if(! yuv_buffer)
    {
    fprintf(stderr, "jpg_to_yuv: process_jpeg_buffer(): could not allocate space for yuv_buffer, aborting./n");
    exit(1);
    }

/* rbgb24 to yuv and X */
/* for RGB to YUV */
y_ptr = yuv_buffer;
u_ptr = yuv_buffer + (width * height);
v_ptr = u_ptr + ( (width / 2) * (height / 2) );

// need bracket for gcc-2.95
    {
    /* reserve space for uv matrix */
    int i;
    double matrix_size = cinfo.output_height * width;
    if(! up)
        {
        up = malloc(cinfo.output_height * sizeof(double) );
        if(! up)
            {
            fprintf(stderr, "jpg_to__yuv: process_jpeg_buffer(): malloc(up) failed, aborting./n");
            exit(1);
            }

        for(i = 0; i < height; i++)
            {
            up[i] = malloc(width * sizeof(double)  );
            if(! up[i] )
                {
                fprintf(stderr, "jpg_to_yuv: process_jpeg_buffer(): malloc(up[%d]) failed, aborting./n", i);
                exit(1);
                }
            }
        }

    if(! vp)
        {
        vp = malloc(cinfo.output_height * sizeof(double) );
        if(! vp)
            {
            fprintf(stderr, "jpg_to_yuv: process_jpeg_buffer(): malloc(vp) failed, aborting./n");
            exit(1);
            }

        for(i = 0; i < height; i++)
            {
            vp[i] = malloc(width * sizeof(double)  );
            if(! vp[i] )
                {
                fprintf(stderr, "jpg_to_yuv: process_jpeg_buffer(): malloc(vp[%d]) failed, aborting./n", i);
                exit(1);
                }
        
            }
        }

    uptr = rgb24_buffer;
    k = 0;
    for(ya = 0; ya < height; ya++)
        {
        for(xa = 0; xa < width; xa++)
            {
            /* convert to YUV */
            r = (int)*uptr++;
            g = (int)*uptr++;
            b = (int)*uptr++;
            /* test yuv coding here */
//            y = cr * r + cg * g + cb * b;
//            y = (219.0 / 256.0) * y + 16.5;  /* nominal range: 16..235 */
            y = RGB_TO_Y_CCIR(r, g, b);
            *y_ptr = y;
            y_ptr++;
//            u = cu * (b - y) + 128.0;
//            u = (224.0 / 256.0) * u + 128.5; /* 16..240 */
            u = RGB_TO_U_CCIR(r, g, b, 0);
//            v = cv * (r - y) + 128.0;
//            v = (224.0 / 256.0) * v + 128.5; /* 16..240 */
            v = RGB_TO_V_CCIR(r, g, b, 0);
            /*
            0 0
            0 x
            */
            /* only on even rows and even columns do we output the average U and V for the 4 pixels */
        int no_color_flag = 0;    
	if(ya % 2)
                {
                if(xa % 2)
                    {
                    if(no_color_flag)
                        {
                        *u_ptr = 128.5;
                        *v_ptr = 128.5;
                        }
                    else
                        {    
                        *u_ptr = ( (up[ya - 1][xa - 1] + up[ya - 1][xa] + up[ya][xa -1] + u ) / 4.0);
                        *v_ptr = ( (vp[ya - 1][xa - 1] + vp[ya - 1][xa] + vp[ya][xa -1] + v ) / 4.0);
                        }

                    u_ptr++;
                    v_ptr++;

                    } /* end if odd pixel */

                } /* end if odd_line */
            /* fill uv matrix for lookback */
            up[ya][xa] = u;                  
            vp[ya][xa] = v;

            } /* end for xa */
        } /* end for ya */
    } /* end gcc-2.95 brackets */

/* output an mjpeg tools yuv frame header */
/*
FRAME-HEADER consists of
string "FRAME "  (note the space after the 'E')
unlimited number of ' ' separated TAGGED-FIELDs
'/n' line terminator
*/

fprintf(stdout, "FRAME/n");

/* output an mjpeg tools yuv frame */
/* write Y */
/* write Cb */
/* write Cv */
//unsigned char *yuv_buffer;

a = fwrite(yuv_buffer, sizeof(char), yuv_buffer_length, stdout); 
if(a != yuv_buffer_length)
    {
    fprintf(stderr, "jpg_to_yuv: process_jpeg_buffer() output yuv: could only write %d of %d bytes, aborting./n",\
    a, yuv_buffer_length);
    exit(1);
    }

fflush(stdout);

/* Finish decompression */
(void) jpeg_finish_decompress(&cinfo);

/* Release JPEG decompression object */
jpeg_destroy_decompress(&cinfo);

fclose(infile);
/* 
At this point you may want to check to see whether any corrupt-data
warnings occurred (test whether jerr.pub.num_warnings is nonzero).
*/
if(jerr.pub.num_warnings)
    {
    fprintf(stderr, "jpg_to_yuv: jpeg lib warnings=%ld occurred/n", jerr.pub.num_warnings);
    }

free(yuv_buffer);

return 1;
} /* end function process_jpeg_buffer */

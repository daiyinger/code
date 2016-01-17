#ifndef _SIMPLE_CODE_INFO_H_
#define _SIMPLE_CODE_INFO_H_

//#define WRITE_TO_FILE

extern int encode_init(void);
extern int encode_one_frame(unsigned char *data, unsigned char *dst_data, int *retLen);
extern int encode_one_frameExt(unsigned char *src_data, unsigned char **dst_data, int buffer_pos, int retLen[]);
extern int end_encode(void);

#endif

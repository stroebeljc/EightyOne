//---------------------------------------------------------------------------

#ifndef TapeCInterfaceH
#define TapeCInterfaceH
//---------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

void tape_rewind();
int tape_get_pointer(char *fname, BYTE **ptr);
void tape_new_block(char *name, int len, BYTE * ptr);

#ifdef __cplusplus
}
#endif

#endif

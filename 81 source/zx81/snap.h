
#ifndef SNAP_H
#define SNAP_H

#ifdef __cplusplus

extern "C" int load_snap(char *filename);
extern "C" int save_snap(char *filename);
extern "C" int load_file(char *filename);
extern "C" void load_tape_buffer(int memptr);
extern "C" int memory_load(char *filename, int address, int length);

#else

extern int load_snap(char *filename);
extern int save_snap(char *filename);
extern int load_file(char *filename);
extern void load_tape_buffer(int memptr);
extern int memory_load(char *filename, int address, int length);

#endif
#endif


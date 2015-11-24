#ifndef NIBTOOLS
#define NIBTOOLS

int nibmain(const char *argv1, const char *argv2);

// #define BYTE unsigned char
// #define DWORD unsigned int
#define MAX_TRACKS_1541 42

/* G64 constants */
#define G64_TRACK_MAXLEN 7928
#define G64_TRACK_LENGTH (G64_TRACK_MAXLEN+2)

/* NIB format constants */
#define GCR_TRACK_LENGTH 0x2000

/* Conversion routines constants */
#define MIN_TRACK_LENGTH 0x1780
#define MATCH_LENGTH 7

extern char sector_map_1541[];

extern int speed_map_1541[];

int find_track_cycle(uint8_t **cylce_start, uint8_t **cycle_stop);

uint8_t* find_sector_gap(uint8_t *work_buffer, int tracklen);

uint8_t* find_sector0(uint8_t *work_buffer, int tracklen);

int extract_GCR_track(uint8_t *destination, uint8_t *source);

#endif

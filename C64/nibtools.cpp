/*
 * This file contains an adoption of Markus Brenners n2g converter code
 * (nibbler data to G64 image converter) <markus@@brenner.de>
 *
 * Modifications by Dirk W. Hoffmann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "basic.h"
#include "nibtools.h"

// ------------------------------------------------------------------------------
// From File: n2g.c
// ------------------------------------------------------------------------------

static int write_dword(FILE *fd, uint32_t *buf, int num)
{
    int i;
    uint8_t *tmpbuf;

    tmpbuf = (uint8_t *)malloc(num);

    for (i = 0; i < (num / 4); i++) {
        tmpbuf[i * 4] = buf[i] & 0xff;
        tmpbuf[i * 4 + 1] = (buf[i] >> 8) & 0xff;
        tmpbuf[i * 4 + 2] = (buf[i] >> 16) & 0xff;
        tmpbuf[i * 4 + 3] = (buf[i] >> 24) & 0xff;
    }

    if (fwrite((char *)tmpbuf, num, 1, fd) < 1) {
        free(tmpbuf);
        return -1;
    }
    free(tmpbuf);
    return 0;
}

int nibmain(const char *argv1, const char *argv2)
{
    FILE *fpin, *fpout;
    char inname[80], outname[80];
    int track;
    uint8_t gcr_header[12];
    uint32_t gcr_track_p[MAX_TRACKS_1541 * 2];
    uint32_t gcr_speed_p[MAX_TRACKS_1541 * 2];
    uint32_t track_len;
    uint8_t mnib_track[GCR_TRACK_LENGTH];
    uint8_t gcr_track[7930];
    uint8_t nib_header[0x100];
    int header_offset;
	

    fprintf(stderr,
        "\nn2g - converts a NIB type disk dump into a standard G64 disk image.\n"
        "(C) 2000-03 Markus Brenner. VirtualC64 integration by Dirk W. Hoffmann\n");

    strcpy(inname, argv1);
    strcpy(outname, argv2);

    fpin = fpout = NULL;

    fpin = fopen(inname, "rb");
    if (fpin == NULL)
    {
        fprintf(stderr, "Cannot open mnib image %s.\n", inname);
        goto fail;
    }

    if (fread(nib_header, sizeof(uint8_t), 0x0100, fpin) < 0x100)
    {
        fprintf(stderr, "Cannot read header from mnib image.\n");
        goto fail;
    }

    if (memcmp(nib_header, "MNIB-1541-RAW", 13) != 0)
    {
        fprintf(stderr, "input file %s isn't an mnib data file!\n", inname);
        goto fail;
    }

    fpout = fopen(outname, "wb");
    if (fpout == NULL)
    {
        fprintf(stderr, "Cannot open G64 image %s.\n", outname);
        goto fail;
    }
    fprintf(stderr, "Opened file %s\n", outname);
    
    /* Create G64 header */
    strcpy((char *) gcr_header, "GCR-1541");
    gcr_header[8] = 0;                    /* G64 version */
    gcr_header[9] = MAX_TRACKS_1541 * 2;  /* Number of Halftracks */
    gcr_header[10] = G64_TRACK_MAXLEN % 256;          /* Size of each stored track */
    gcr_header[11] = G64_TRACK_MAXLEN / 256;

    if (fwrite((char *)gcr_header, sizeof(gcr_header), 1, fpout) != 1)
    {
        fprintf(stderr, "Cannot write G64 header.\n");
        goto fail;
    }

    /* Create index and speed tables */
    for (track = 0; track < MAX_TRACKS_1541; track++) {
        /* calculate track positions */
        gcr_track_p[track * 2] = 12 + MAX_TRACKS_1541 * 16 + track * 7930;
        gcr_track_p[track * 2 + 1] = 0; /* no halftracks */
        /* set speed zone data */
        gcr_speed_p[track * 2] = (nib_header[17+track*2] & 0x03);
        gcr_speed_p[track * 2 + 1] = 0;
    }

    if (write_dword(fpout, gcr_track_p, sizeof(gcr_track_p)) < 0)
    {
        fprintf(stderr, "Cannot write track header.\n");
        goto fail;
    }
    if (write_dword(fpout, gcr_speed_p, sizeof(gcr_speed_p)) < 0)
    {
        fprintf(stderr, "Cannot write speed header.\n");
        goto fail;
    }

    header_offset = 0x10; /* number of first nibble-track in nib image */
    for (track = 0; track < MAX_TRACKS_1541; track++)
    {
        int raw_track_size[4] = { 6250, 6666, 7142, 7692 };

        memset(&gcr_track[2], 0xff, G64_TRACK_MAXLEN);
        gcr_track[0] = raw_track_size[speed_map_1541[track]] % 256;
        gcr_track[1] = raw_track_size[speed_map_1541[track]] / 256;

        /* Skip halftracks if present in image */
        if (nib_header[header_offset] < (track + 1) * 2)
        {
            fseek(fpin, GCR_TRACK_LENGTH, SEEK_CUR);
            header_offset += 2;
        }
        header_offset += 2;

        /* read in one track */
        if (fread(mnib_track, GCR_TRACK_LENGTH, 1, fpin) < 1)
        {
            /* track doesn't exist: write blank track */
            printf("\nTrack: %2d ",track+1);
            // fprintf(stderr, "Cannot read track from mnib image.\n");
            track_len = raw_track_size[speed_map_1541[track]];
            memset(&gcr_track[2], 0x55, track_len);
            gcr_track[2] = 0xff;

            gcr_track[0] = track_len % 256;
            gcr_track[1] = track_len / 256;
            if (fwrite((char *) gcr_track, G64_TRACK_LENGTH, 1, fpout) != 1)
            {
                fprintf(stderr, "Cannot write track data.\n");
                goto fail;
            }
            continue;
        }
   
        printf("\nTrack: %2d ",track+1);
/*
        source_track = check_vmax(mnib_track);
*/
        track_len = extract_GCR_track(gcr_track+2, mnib_track);
        printf("- track length:  %d", track_len);

        if (track_len == 0)
        {
            track_len = raw_track_size[speed_map_1541[track]];
            memset(&gcr_track[2], 0x55, track_len);
            gcr_track[2] = 0xff;
        }
        else if (track_len > G64_TRACK_MAXLEN)
        {
            printf("  Warning: track too long, cropping to %d!",
                G64_TRACK_MAXLEN);
            track_len = G64_TRACK_MAXLEN;
        }
        gcr_track[0] = track_len % 256;
        gcr_track[1] = track_len / 256;

        if (fwrite((char *) gcr_track, G64_TRACK_LENGTH, 1, fpout) != 1)
        {
            fprintf(stderr, "Cannot write track data.\n");
            goto fail;
        }
    }

fail:
    if (fpin  != NULL) fclose(fpin);
    if (fpout != NULL) fclose(fpout);
    fprintf(stderr, "Closing file %s\n", argv2);
    return -1;
}

uint8_t *find_sync_old(uint8_t *pointer, int pos)
{
    /* first find a Sync byte $ff */
    for (; (*pointer != 0xff) && (pos < GCR_TRACK_LENGTH); pointer++, pos++);
    if (pos >= GCR_TRACK_LENGTH) return (NULL);

    /* now find end of Sync */
    for (; (*pointer == 0xff) && (pos < GCR_TRACK_LENGTH); pointer++, pos++);
    if (pos >= GCR_TRACK_LENGTH) return (NULL);

    return (pointer);
}


int is_sector_zero(uint8_t *data)
{
    if ((data[0] == 0x52)
        && ((data[2] & 0x0f) == 0x05)
        && ((data[3] & 0xfc) == 0x28))
         return (1);
    else return (0);
}

uint32_t extract_track(uint8_t *mnib_track, uint8_t *gcr_track)
{
    uint8_t *sync_pos, *last_sync_pos;
    uint8_t *start_pos;
    uint8_t *sector_zero_pos;
    uint8_t *max_len_pos;

    uint8_t *repeat_pos;
    uint8_t *cycle_pos;	/* here starts the 2nd cycle */

    int block_len, max_block_len;
    int sector_zero_len;
    int syncs;
    int i;
    int cyclelen;


    sector_zero_pos = NULL;
    sector_zero_len = 0;
    max_len_pos = mnib_track;
    max_block_len = 0;
    syncs = 0;
    cyclelen = 0;

    for (sync_pos = mnib_track; sync_pos != NULL;)
    {
        last_sync_pos = sync_pos;
        syncs++; /* count number of syncs in track */

        /* find start of next block */
        sync_pos = find_sync_old(sync_pos, sync_pos-mnib_track);

        /* if we can't find beginning repeated data we have a problem... */
        if (sync_pos == NULL) return (0);

        /* check if sector 0 header was found */
        if (is_sector_zero(sync_pos))
        {
            sector_zero_pos = sync_pos;
            sector_zero_len = sync_pos - last_sync_pos;
        }

        /* check if the last chunk of data had maximal length */
        block_len = sync_pos - last_sync_pos;
        max_len_pos  = (block_len > max_block_len) ? sync_pos  : max_len_pos;
        max_block_len = (block_len > max_block_len) ? block_len : max_block_len;

        /* check if we are still in first disk rotation */
        if ((sync_pos-mnib_track) < 0x1780) continue;

        /* we are possibly already in the second rotation, check for repeat */
        start_pos = mnib_track;
        for (repeat_pos = sync_pos; sync_pos != NULL; )
        {

            for (i = 0; i < 7; i++)
                if (start_pos[i] != repeat_pos[i]) break;

            if (i != 7)
            {
                break; /* break out of while loop */
            }
            cycle_pos = sync_pos;
            cyclelen = (cycle_pos - mnib_track);

            start_pos  = find_sync_old(start_pos, start_pos-mnib_track);
            repeat_pos = find_sync_old(repeat_pos, repeat_pos-mnib_track);

            if (repeat_pos == NULL) sync_pos = NULL;

            /* check if next header is completely available */
            if ((repeat_pos-mnib_track+10) > GCR_TRACK_LENGTH) sync_pos = NULL;
        }
    }

    if ((sector_zero_len != 0) && ((sector_zero_len + 0x40) >= max_block_len))
    {
        max_len_pos = sector_zero_pos;
    }

    if (cyclelen >= 7900)
    {
        max_len_pos = mnib_track;
        cyclelen = 7900; /* hack for psi5 killertrack */
    }
    printf("- Cyclepos:  %d", cyclelen);
    if (cyclelen != 7900)
    {

    /* find start of sync */
    sync_pos = max_len_pos;
    do
    {
        sync_pos--;
        if (sync_pos < mnib_track) sync_pos += cyclelen;
    } while (*sync_pos == 0xff);
    sync_pos++;
    if (sync_pos >= mnib_track+cyclelen) sync_pos = mnib_track;
    max_len_pos = sync_pos;

    }

    /* here comes the actual copy loop */
    for (sync_pos = max_len_pos; sync_pos < cycle_pos; )
        *gcr_track++ = *sync_pos++;

    for (sync_pos = mnib_track; sync_pos < max_len_pos; )
        *gcr_track++ = *sync_pos++;

    return (cyclelen);
}

#if 0

uint32_t extract_track_try2(uint8_t *mnib_track, uint8_t *gcr_track)
{
    uint8_t *pos;
    uint8_t *start_pos;
    uint8_t *stop_pos;
    uint8_t *cycle_pos;

    int cyclelen;
    int i;

    start_pos = mnib_track;
    stop_pos = mnib_track+GCR_TRACK_LENGTH;
    cycle_pos = NULL;


    for (pos = start_pos+0x1780; pos < (stop_pos-50); pos++)
    {
        for (i = 0; i < 50; i++)
            if (start_pos[i] != pos[i]) break;

        if (i == 50)
        {
            cycle_pos = pos;
            break;
        }
    }

    if (cycle_pos == NULL)
        return (0);

    cyclelen = cycle_pos-mnib_track;

    printf("- Cyclepos:  %d", cyclelen);

    /* here comes the actual copy loop */
    for (pos = start_pos; pos < cycle_pos; )
        *gcr_track++ = *pos++;

    return (cyclelen);
}
#endif

// ------------------------------------------------------------------------------
// From File: gcr.c
// ------------------------------------------------------------------------------

char sector_map_1541[43] =
{
    0,
    21, 21, 21, 21, 21, 21, 21, 21, 21, 21,     /*  1 - 10 */
    21, 21, 21, 21, 21, 21, 21, 19, 19, 19,     /* 11 - 20 */
    19, 19, 19, 19, 18, 18, 18, 18, 18, 18,     /* 21 - 30 */
    17, 17, 17, 17, 17,                         /* 31 - 35 */
    17, 17, 17, 17, 17, 17, 17                  /* 36 - 42 (non-standard) */
};


int speed_map_1541[42] =
{
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,               /*  1 - 10 */
    3, 3, 3, 3, 3, 3, 3, 2, 2, 2,               /* 11 - 20 */
    2, 2, 2, 2, 1, 1, 1, 1, 1, 1,               /* 21 - 30 */
    0, 0, 0, 0, 0,                              /* 31 - 35 */
    0, 0, 0, 0, 0, 0, 0                         /* 36 - 42 (non-standard) */
};

int find_sync(uint8_t **gcr_pptr, uint8_t *gcr_end)
{
    while (1)
    {
        if ((*gcr_pptr) + 1 >= gcr_end)
        {
            *gcr_pptr = gcr_end;
            return 0; /* not found */
        }
        // if (((*gcr_pptr)[0] == 0xff) && ((*gcr_pptr)[1] == 0xff))
        if ((((*gcr_pptr)[0] & 0x03) == 0x03) && ((*gcr_pptr)[1] == 0xff))
            break;
        (*gcr_pptr)++;
    }
    
    (*gcr_pptr)++;
    while ((*gcr_pptr < gcr_end) && (**gcr_pptr == 0xff)) (*gcr_pptr)++;
    return (*gcr_pptr < gcr_end);
}

int find_track_cycle(uint8_t **cycle_start, uint8_t **cycle_stop)
{
    uint8_t *nib_track;	/* start of nibbled track data */
    uint8_t *start_pos;    /* start of periodic area */
    uint8_t *cycle_pos;    /* start of cycle repetition */
    uint8_t *stop_pos;     /* maximum position allowed for cycle */
    
    uint8_t *sync_pos;     /* cycle search variable */
    uint8_t *p1, *p2;      /* local pointers for comparisons */
    
    nib_track = *cycle_start;
    stop_pos = nib_track+GCR_TRACK_LENGTH-MATCH_LENGTH;
    cycle_pos = NULL;
    
    for (start_pos = nib_track;;find_sync(&start_pos, stop_pos))
    {
        if ((sync_pos = start_pos + MIN_TRACK_LENGTH) >= stop_pos)
        {
            *cycle_stop = *cycle_start;
            return (0); /* no cycle found */
        }
        
        /* try to find next sync */
        while (find_sync(&sync_pos, stop_pos))
        {
            /* found a sync, now let's see if data matches */
            p1 = start_pos;
            cycle_pos = sync_pos;
            for (p2 = cycle_pos; p2 < stop_pos;)
            {
                /* try to match all remaining syncs, too */
                if (memcmp(p1, p2, MATCH_LENGTH) != 0)
                {
                    cycle_pos = NULL;
                    break;
                }
                if (!find_sync(&p1, stop_pos)) break;
                if (!find_sync(&p2, stop_pos)) break;
            }
            
            if (cycle_pos != NULL)
            {
                *cycle_start = start_pos;
                *cycle_stop = cycle_pos;
                return (cycle_pos-start_pos);
            }
        }
    }
    *cycle_stop = *cycle_start;
    return (0); /* no cycle found */
}


uint8_t* find_sector0(uint8_t *work_buffer, int tracklen)
{
    uint8_t *pos;
    uint8_t *buffer_end;
    
    pos = work_buffer;
    buffer_end = work_buffer + 2 * tracklen - 10;
    
    /* try to find sector 0 */
    while (pos < buffer_end)
    {
        if (!find_sync(&pos, buffer_end)) return NULL;
        if ((pos[0] == 0x52)
            && ((pos[1] & 0xc0) == 0x40)
            && ((pos[2] & 0x0f) == 0x05)
            && ((pos[3] & 0xfc) == 0x28))
        {
            break;
        }
    }
    
    /* find last GCR byte before sync */
    do
    {
        pos -= 1;
        if (pos == work_buffer)
            pos += tracklen;
    } while (*pos == 0xff);
    
    /* move to first sync GCR byte */
    pos += 1;
    while (pos >= work_buffer+tracklen)
        pos -= tracklen;
    
    return pos;
}


uint8_t* find_sector_gap(uint8_t *work_buffer, int tracklen)
{
    int  gap, maxgap;
    uint8_t *pos;
    uint8_t *buffer_end;
    uint8_t *sync_last;
    uint8_t *sync_max;
    
    pos = work_buffer;
    buffer_end = work_buffer + 2 * tracklen - 10;
    
    
    if (!find_sync(&pos, buffer_end)) return NULL;
    sync_last = pos;
    maxgap = 0;
    /* try to find biggest (sector) gap */
    while (pos < buffer_end)
    {
        if (!find_sync(&pos, buffer_end)) break;
        gap = pos - sync_last;
        if (gap > maxgap)
        {
            maxgap = gap;
            sync_max = pos;
        }
        sync_last = pos;
    }
    
    if (maxgap == 0) return NULL; /* no gap found */
    
    /* find last GCR byte before sync */
    pos = sync_max;
    do
    {
        pos -= 1;
        if (pos == work_buffer)
            pos += tracklen;
    } while (*pos == 0xff);
    
    /* move to first sync GCR byte */
    pos += 1;
    while (pos >= work_buffer+tracklen)
        pos -= tracklen;
    
    return pos;
}


int extract_GCR_track(uint8_t *destination, uint8_t *source)
{
    /* try to extract one complete cycle of GCR data from
     an 8kB buffer.
     Align track to sector gap if possible, else align to track 0,
     else copy cyclic loop from begin of source.
     If buffer is pure nonsense, return tracklen = 0;
     [Input]  destination buffer, source buffer
     [Return] length of copied track fragment
     */
    
    static uint8_t work_buffer[2*GCR_TRACK_LENGTH]; /* working buffer */
    uint8_t *cycle_start;  /* start position of cycle */
    uint8_t *cycle_stop;   /* stop position of cycle  */
    uint32_t track_len;
    
    uint8_t *sector0_pos;  /* position of sector 0 */
    uint8_t *sectorgap_pos;/* position of sector gap */
    
    cycle_start = source;
    find_track_cycle(&cycle_start, &cycle_stop);
    track_len = cycle_stop-cycle_start;
    if (track_len == 0)
        return 0;
    
    /* copy twice the data to work buffer */
    memcpy(work_buffer, cycle_start, track_len);
    memcpy(work_buffer+track_len, cycle_start, track_len);
    
    /*
     printf("start: %4x, stop: %4x,",
     cycle_start-source,
     cycle_stop-source);
     */
    
    if (NULL != (sectorgap_pos = find_sector_gap(work_buffer, track_len)))
    {
        // printf("gap:  %4x", sectorgap_pos-work_buffer);
        memcpy(destination, sectorgap_pos, track_len);
    }
    else if (NULL != (sector0_pos = find_sector0(work_buffer, track_len)))
    {
        // printf("sec0: %4x", sector0_pos-work_buffer);
        memcpy(destination, sector0_pos, track_len);
    }
    else
    {
        memcpy(destination, work_buffer, track_len);
    }
    
    return track_len;
}


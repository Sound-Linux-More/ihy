#ifndef DEF_IHY
#define DEF_IHY

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct ihy_chunk
{
    uint32_t		ChunkSize;
    float		*Values;
} ihy_chunk;

typedef struct ihy_data
{
    /* Header */
    char		FileID[4]; /* "SNXT" */
    uint64_t		FileSize;
    uint16_t		CompressionType;
    uint8_t		Channels;
    uint32_t		Frequency;
    /* Tags */
    uint16_t		ArtistLength;
    char		*Artist;
    uint16_t		AlbumLength;
    char		*Album;
    uint16_t		TrackLength;
    char		*Track;
    uint16_t		Year;
    uint8_t		Genre;
    uint32_t		CommentLength;
    char		*Comment;
    /* Data */
    uint32_t		NbChunk;
    ihy_chunk		*DataChunks;
} ihy_data;

/* alloc a ihy data and return it */
ihy_data *create_ihy();

/* read a ihy and put it on data */
void read_ihy(const char *filename, ihy_data *data);

/* write a ihy to a file */
void write_ihy(const ihy_data *data, const char *filename);

/* release the memory used by a ihy */
void destroy_ihy(ihy_data *data);

#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <caml/mlvalues.h>
#include <caml/callback.h>
#include <caml/bigarray.h>

#include <input/wav.h>
#include <ihy.h>
#include <wavelet.h>
#include <lossless/huffman.h>
#include <output/ao.h>

static void *thread_play_wav(void *wav)
{
    play_wav((wav_data *)wav);
    return NULL;
}

static void extract_ihy(char *input_filename, char *output_filename)
{
    ihy_data *input;
    wav_data *output;
    uint32_t offset;
    unsigned int i;

    input = create_ihy();
    read_ihy(input_filename, input);
    output = create_wav();
    output->ChunkID[0] = 'R';
    output->ChunkID[1] = 'I';
    output->ChunkID[2] = 'F';
    output->ChunkID[3] = 'F';
    output->ChunkSize = 8 * sizeof(wav_data);
    output->Format[0] = 'W';
    output->Format[1] = 'A';
    output->Format[2] = 'V';
    output->Format[3] = 'E';
    output->FormatBlocID[0] = 'f';
    output->FormatBlocID[1] = 'm';
    output->FormatBlocID[2] = 't';
    output->FormatBlocID[3] = ' ';
    output->FormatBlocSize = 16;
    output->AudioFormat = 1;
    output->NumChannels = input->Channels;
    output->SampleRate = input->Frequency;
    output->BitsPerSample = 16;
    output->BlockAlign = output->NumChannels * (output->BitsPerSample / 8);
    output->ByteRate = output->SampleRate * output->BlockAlign;
    output->DataBlocID[0] = 'd';
    output->DataBlocID[1] = 'a';
    output->DataBlocID[2] = 't';
    output->DataBlocID[3] = 'a';
    offset = 0;
    for (i = 0; i < input->NbChunk; i++)
	offset += (input->DataChunks[i].ChunkSize / sizeof(float)) * 3;
    output->DataBlocSize = offset;
    output->ChunkSize += offset;
    output->Data = malloc(output->DataBlocSize * sizeof(char));
    offset = 0;
    for (i = 0; i < input->NbChunk; i++)
    {
	wavelets_inverse(input->DataChunks[i].Values,
		(input->DataChunks[i].ChunkSize / sizeof(float)),
		output,
		offset);
	offset += (input->DataChunks[i].ChunkSize / sizeof(float)) * 2;
    };
    write_wav(output, output_filename);
    destroy_wav(output);
    destroy_ihy(input);
}

static void compress_wav(char *input_filename, char *output_filename)
{
    ihy_data *output;
    wav_data *input;

    output = create_ihy();
    input = create_wav();
    read_wav(input_filename, input);
    wavelets_direct(input->Data,
	    input->BitsPerSample / 8,
	    input->DataBlocSize,
	    output);
    output->FileID[0] = 'S';
    output->FileID[1] = 'N';
    output->FileID[2] = 'X';
    output->FileID[3] = 'T';
    output->FileSize = 0;
    output->CompressionType = 0;
    output->Channels = input->NumChannels;
    output->Frequency = input->SampleRate;
    output->Artist = malloc(5 * sizeof(char));
    output->Artist[4] = '\0';
    output->ArtistLength = strlen(output->Artist);
    output->Album = malloc(5 * sizeof(char));
    output->Album[4] = '\0';
    output->AlbumLength = strlen(output->Album);
    output->Track = malloc(5 * sizeof(char));
    output->Track[4] = '\0';
    output->TrackLength = strlen(output->Track);
    output->Year = 2009;
    output->Genre = 42;
    output->Comment = malloc(25 * sizeof(char));
    output->Comment[24] = '\0';
    output->CommentLength = strlen(output->Comment);
    write_ihy(output, output_filename);
    destroy_ihy(output);
    destroy_wav(input);
}

static void print_help()
{
    printf("help mode of ihyconvert :\n");
    printf("-c filename.wav filename.ihy : compress filename.wav and\n");
    printf("                               put it to the filename.ihy\n");
    printf("-x filename.ihy filename.wav : extract filename.ihy to filename.wav\n");
    printf("-r filename.wav : play filename.wav on a separate thread\n");
    printf("-h : display this help\n");
}

int main(int argc, char **argv)
{
    pthread_t play;
    int i;
    int is_thread_playing_wav = 0;
    wav_data *input_to_play;

    if (argc == 1)
    {
	print_help();
	return 1;
    };
    caml_main(argv);
    i = argc - 1;
    while (i > 0)
    {
	if (!strcmp(argv[argc - i],"-h"))
	{
	    print_help();
	    return 1;
	}
	else if (!strcmp(argv[argc - i], "-x"))
	{
	    printf("Extracting data ... ");
	    fflush(stdout);
	    extract_ihy(argv[argc - i + 1], argv[argc - i + 2]);
	    i -= 3;
	    printf("DONE\n");
	}
	else if (!strcmp(argv[argc - i], "-c"))
	{
	    printf("Compressing data ... ");
	    fflush(stdout);
	    compress_wav(argv[argc - i + 1], argv[argc - i + 2]);
	    i -= 3;
	    printf("DONE\n");
	}
	else if (!strcmp(argv[argc - i], "-r"))
	{
	    is_thread_playing_wav = 1;
	    input_to_play = create_wav();
	    read_wav(argv[argc - i + 1], input_to_play);
	    pthread_create(&play, NULL, thread_play_wav, input_to_play);
	    i -= 2;
	}
	else
	{
	    printf("Not a valid argument.\n");
	    print_help();
	    return 1;
	}
    };
    if (is_thread_playing_wav)
    {
	printf("Waiting for the reader to stop ... ");
	pthread_join(play, NULL);
	destroy_wav(input_to_play);
	printf("DONE\n");
    }
    return 0;
}

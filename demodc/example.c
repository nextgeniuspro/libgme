#include "gme/gme.h"
#include <kos.h>

void handle_error( const char* str );


void handle_error( const char* str )
{
	if ( str ) {
		printf( "Error: %s\n", str );
	}
}

short sound_buffer[SND_STREAM_BUFFER_MAX] = {0};
Music_Emu* emu = NULL;

void *vgm_callback(snd_stream_hnd_t hnd, int len, int * actual) {
	if (len > SND_STREAM_BUFFER_MAX) {
		len = SND_STREAM_BUFFER_MAX;
	}

	if (!emu) {
		printf( "Error: no emulator\n" );
		*actual = 0;
		return NULL;
	}

	gme_err_t err = gme_play( emu, len / 2, sound_buffer ); // 2 - stereo
	if ( err ) {
		printf( "Error: %s\n", err );
		*actual = 0;
		return NULL;	
	}

	*actual = len;
	return sound_buffer;
}

int main(int argc, char **argv) {
    maple_device_t *cont;
    cont_state_t *state;

    char filename[] = "/rd/test.nsf";
	long sample_rate = 44100; /* number of samples per second */
	int track = 0; /* index of track to play (0 = first) */
	
	/* Open music file in new emulator */
	handle_error( gme_open_file( filename, &emu, sample_rate ) );
	
	/* Start track */
	handle_error( gme_start_track( emu, track ) );

    printf("vgm player\n");

    snd_stream_init();

    snd_stream_hnd_t shnd = snd_stream_alloc(vgm_callback, SND_STREAM_BUFFER_MAX);
    snd_stream_start(shnd, 44100, 1);

    while(1) {
        /* Check key status */
        cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);

        if(cont) {
            state = (cont_state_t *)maple_dev_status(cont);

            if(state && state->buttons & CONT_START) {
                break;
			}
        }

        snd_stream_poll(shnd);
        timer_spin_sleep(10);
    }

	/* Cleanup */
	gme_delete( emu );
    snd_stream_destroy(shnd);
    snd_stream_shutdown();

    return 0;
}


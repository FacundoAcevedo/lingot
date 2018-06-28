/*
 * lingot, a musical instrument tuner.
 *
 * Copyright (C) 2004-2018  Iban Cereijo.
 * Copyright (C) 2004-2008  Jairo Chapela.

 *
 * This file is part of lingot.
 *
 * lingot is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * lingot is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with lingot; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __LINGOT_AUDIO_H__
#define __LINGOT_AUDIO_H__

#ifdef ALSA
#include <alsa/asoundlib.h>
#endif

#ifdef JACK
#include <jack/jack.h>
#endif

#ifdef PULSEAUDIO
#include <pulse/simple.h>
#endif

#include "lingot-config.h"

typedef void (*LingotAudioProcessCallback)(FLT* read_buffer,
		unsigned int read_buffer_size_samples, void *arg);

#define FLT_SAMPLE_SCALE	32767.0

typedef struct {

	int audio_system;
	char device[100];

	LingotAudioProcessCallback process_callback;
	void* process_callback_arg;

#	ifdef OSS
	int dsp; // file handler.
#	endif
#	ifdef ALSA
	snd_pcm_t *capture_handle;
#	endif
#	ifdef JACK
	jack_port_t *jack_input_port;
	jack_client_t *jack_client;
	int nframes;
#	endif
#	ifdef PULSEAUDIO
	pa_simple *pa_client;
#	endif
#	if !defined(OSS) && !defined(ALSA) && !defined(PULSEAUDIO) && !defined(JACK)
#	error "No audio system has been enabled"
#	endif

	unsigned int read_buffer_size_samples;
	FLT* flt_read_buffer;

	unsigned int real_sample_rate;

	short bytes_per_sample;

	// pthread-related  member variables
	pthread_t thread_input_read;
	pthread_attr_t thread_input_read_attr;
	pthread_cond_t thread_input_read_cond;
	pthread_mutex_t thread_input_read_mutex;

	// indicates whether the audio thread is running
	int running;

	// indicates whether the thread was interrupted (by the audio server, not
	// by the user)
	int interrupted;
} LingotAudioHandler;

typedef struct {

	int forced_sample_rate; // tells whether the sample rate can be changed

	int n_sample_rates; // number of available sample rates
	int sample_rates [5]; // sample rates

	int n_devices; // number of available devices
	char** devices; // devices
} LingotAudioSystemProperties;

int lingot_audio_get_audio_system_properties(
		LingotAudioSystemProperties*,
		audio_system_t audio_system);
// Return status : 0 for OK, else -1.

void lingot_audio_audio_system_properties_destroy(LingotAudioSystemProperties*);

// creates an audio handler
void lingot_audio_new(LingotAudioHandler*,
		audio_system_t audio_system, const char* device,
		int sample_rate, LingotAudioProcessCallback process_callback,
		void *process_callback_arg);
// In case of failure, audio_system is set to -1 in the LingotAudioHandler struct.

// destroys an audio handler
void lingot_audio_destroy(LingotAudioHandler*);

int lingot_audio_start(LingotAudioHandler*);
void lingot_audio_stop(LingotAudioHandler*);

#endif

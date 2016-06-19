#include <math.h>

#include <alsa/asoundlib.h>
#include <alsa/mixer.h>

#include "config.h"
#include "http_api.h"
#include "buffer.h"
#include "volume.h"
#include "volume_mapping.h"

#define DEFAULT_VOLUME 20

static snd_mixer_elem_t* master;

int get_volume(void)
{
	double vol = get_normalized_playback_volume(master, SND_MIXER_SCHN_MONO);

	return (int) round(100.0 * vol);
}

int set_volume(unsigned int target_vol)
{
	if(target_vol > 100)
		return -1;

	set_normalized_playback_volume(master, SND_MIXER_SCHN_MONO, target_vol / 100.0, 1);

	return 0;
}

static int api_get_volume(const char *url, char **out)
{
	(void) url;

	*out = malloc(64);
	snprintf(*out, 64, "%u", get_volume());

	return 200;
}

static int api_set_volume(const char *url, sb_t *data, char **out)
{
	(void) url;

	char buf[8];
	size_t len = sb_len(data);
	if(len > sizeof(buf) - 1) {
		*out = strdup("request too long");
		return 400;
	}

	memcpy(buf, sb_get(data), len);
	buf[len] = 0;

	int target_vol = atoi(buf);
	if(target_vol < 0 || target_vol > 100) {
		*out = strdup("volume out of range (must be 0-100, inclusive)");
		return 400;
	}

	set_volume(target_vol);

	return 200;
}

void volume_init(void) {
	static snd_mixer_t *handle;
	snd_mixer_open(&handle, 0);
	snd_mixer_attach(handle, ALSA_CARD);
	snd_mixer_selem_register(handle, NULL, NULL);
	snd_mixer_load(handle);

	static snd_mixer_selem_id_t *sid;
	snd_mixer_selem_id_alloca(&sid);
	snd_mixer_selem_id_set_index(sid, 0);
	snd_mixer_selem_id_set_name(sid, ALSA_SELEM);
	master = snd_mixer_find_selem(handle, sid);

	set_normalized_playback_volume(master, SND_MIXER_SCHN_MONO, DEFAULT_VOLUME / 100.0, 1);

	http_api_register("/api/v1/volume", api_get_volume, api_set_volume);
}

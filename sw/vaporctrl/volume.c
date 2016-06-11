#include <alsa/asoundlib.h>
#include <alsa/mixer.h>

#include "config.h"
#include "volume.h"

static snd_mixer_t *handle;
static snd_mixer_selem_id_t *sid;
static snd_mixer_elem_t* elem;

static long min, max;

static int get_volume(const char *url, char **out)
{
	(void) url;

	long vol;
	snd_mixer_selem_get_playback_volume(elem, 0, &vol);

	*out = malloc(64);
	snprintf(*out, 64, "%ld", vol * 100 / max);

	return 200;
}

static int set_volume(const char *url, sb_t *data, char **out)
{
	(void) data;
	(void) out;

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

	snd_mixer_selem_set_playback_volume_all(elem, target_vol * max / 100);

	return 200;
}

static struct endpoint ep = {"/api/v1/volume", get_volume, set_volume};

struct endpoint *volume_init(void) {
	snd_mixer_open(&handle, 0);
	snd_mixer_attach(handle, ALSA_CARD);
	snd_mixer_selem_register(handle, NULL, NULL);
	snd_mixer_load(handle);

	snd_mixer_selem_id_alloca(&sid);
	snd_mixer_selem_id_set_index(sid, 0);
	snd_mixer_selem_id_set_name(sid, ALSA_SELEM);
	elem = snd_mixer_find_selem(handle, sid);

	snd_mixer_selem_get_playback_volume_range(elem, &min, &max);

	return &ep;
}


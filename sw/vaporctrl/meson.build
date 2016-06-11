project('vaporctrl', 'c')

microhttpd = dependency('libmicrohttpd')
alsa = dependency('alsa')

executable('vaporctrl', ['main.c', 'buffer.c', 'http_api.c', 'volume.c'],
	dependencies: [microhttpd, alsa]
)

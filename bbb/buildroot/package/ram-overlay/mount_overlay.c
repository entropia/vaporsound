#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mount.h>

#define TMP_MP "/.overlay/tmp"
#define OVERLAY_MP "/.overlay/mnt"
#define OLDROOT_MP "/oldroot"

#define error(format, ...) do { fprintf(stderr, format "\n", ##__VA_ARGS__); } while(0)
#define die(format, ...) do { error(format, ##__VA_ARGS__); exit(EXIT_FAILURE); } while(0)
#define pdie(string, ...) do { perror(string); exit(EXIT_FAILURE); } while(0)
#define force(x) do { if((x) < 0) die("force failed in file %s, line %u, errno %d", __FILE__, __LINE__, errno); } while(0)

extern int pivot_root(const char *new_root, const char *put_old);

int mount_move(char *oldroot, char *newroot, char *dir)
{
        struct stat s;
        char olddir[64];
        char newdir[64];
        int ret;

        snprintf(olddir, sizeof(olddir), "%s%s", oldroot, dir);
        snprintf(newdir, sizeof(newdir), "%s%s", newroot, dir);

        if (stat(olddir, &s) || !S_ISDIR(s.st_mode)) {
		error("mount_move: %s is not a directory", olddir);
                return -1;
	}

        if (stat(newdir, &s) || !S_ISDIR(s.st_mode)) {
		error("mount_move: %s is not a directory", newdir);
                return -1;
	}

        ret = mount(olddir, newdir, NULL, MS_MOVE, NULL);

        return ret;
}

int main(int argc, char **argv)
{
	if(mount("tmpfs", TMP_MP, "tmpfs", MS_NOATIME, "mode=0755"))
		pdie("mounting tmpfs failed");

	force(mkdir(TMP_MP "/upper", 0755));
	force(mkdir(TMP_MP "/work", 0755));

	if(mount("overlay", OVERLAY_MP, "overlay", MS_NOATIME,
	          "lowerdir=/,upperdir="TMP_MP"/upper,workdir="TMP_MP"/work"))
		pdie("mounting overlay file system failed");

	force(umount(TMP_MP));

	force(mount_move("", OVERLAY_MP, "/proc"));
	force(mkdir(OVERLAY_MP OLDROOT_MP, 0755));

	force(chdir(OVERLAY_MP));

	if(pivot_root(OVERLAY_MP, OVERLAY_MP OLDROOT_MP))
		pdie("pivot_root failed");

	force(chroot("/"));

	force(mount_move(OLDROOT_MP, "", "/dev"));

	return EXIT_SUCCESS;
}

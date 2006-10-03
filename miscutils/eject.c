/* vi: set sw=4 ts=4: */
/*
 * eject implementation for busybox
 *
 * Copyright (C) 2004  Peter Willis <psyphreak@phreaker.net>
 * Copyright (C) 2005  Tito Ragusa <farmatito@tiscali.it>
 *
 * Licensed under the GPL v2 or later, see the file LICENSE in this tarball.
 */

/*
 * This is a simple hack of eject based on something Erik posted in #uclibc.
 * Most of the dirty work blatantly ripped off from cat.c =)
 */

#include "busybox.h"
#include <mntent.h>

/* various defines swiped from linux/cdrom.h */
#define CDROMCLOSETRAY            0x5319  /* pendant of CDROMEJECT  */
#define CDROMEJECT                0x5309  /* Ejects the cdrom media */
#define DEFAULT_CDROM             "/dev/cdrom"

#define FLAG_CLOSE  1
#define FLAG_SMART  2

int eject_main(int argc, char **argv)
{
	unsigned long flags;
	char *device;
	struct mntent *m;
	int dev;

	/*opt_complementary = "t--T:T--t";*/
	flags = getopt32(argc, argv, "tT");
	device = argv[optind] ? : DEFAULT_CDROM;

	m = find_mount_point(device, bb_path_mtab_file);
	if (m) {
		if (umount(m->mnt_dir)) {
			bb_error_msg_and_die("can't umount");
		} else if (ENABLE_FEATURE_MTAB_SUPPORT) {
			erase_mtab(m->mnt_fsname);
		}
	}

	dev = xopen(device, O_RDONLY|O_NONBLOCK);

	if (flags & FLAG_CLOSE) goto close_tray;

	if (ioctl(dev, CDROMEJECT)) {
close_tray:
		if (ioctl(dev, CDROMCLOSETRAY))
			bb_perror_msg_and_die("%s", device);
	}

	if (ENABLE_FEATURE_CLEAN_UP) close(dev);

	return EXIT_SUCCESS;
}

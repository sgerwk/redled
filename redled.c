/*
 * redled.c
 *
 * make the led turn red upon suspending
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <inttypes.h>
#include <string.h>
#include <libdrm/drm.h>
#include <libdrm/i915_drm.h>

#define MI_BATCH_BUFFER_END (0x0A << 23)

int main(int argc, char **argv) {
	char *devname = "/dev/dri/card0";
	int fd;
	struct drm_i915_gem_create create;
	struct drm_i915_gem_pwrite pwrite;
	struct drm_i915_gem_exec_object2 execobj;
	struct drm_i915_gem_execbuffer2 execbuf;
	const uint32_t end = MI_BATCH_BUFFER_END;
	int res;

	fd = open(devname, O_RDWR);
	if (fd == -1) {
		perror(devname);
		exit(EXIT_FAILURE);
	}

	memset(&create, 0, sizeof(create));
	create.size = 4096;
	res = ioctl(fd, DRM_IOCTL_I915_GEM_CREATE, &create);
	printf("DRM_IOCTL_I915_GEM_CREATE: %d\n", res);

	memset(&execobj, 0, sizeof(execobj));
	execobj.handle = create.handle;

	memset(&pwrite, 0, sizeof(pwrite));
	pwrite.handle = execobj.handle;
	pwrite.offset = 0;
	pwrite.size = sizeof(end);
	pwrite.data_ptr = (uint64_t) &end;
	res = ioctl(fd, DRM_IOCTL_I915_GEM_PWRITE, &pwrite);
	printf("DRM_IOCTL_I915_GEM_PWRITE: %d\n", res);

	memset(&execbuf, 0, sizeof(execbuf));
	execbuf.buffers_ptr = (uint64_t) &execobj;
	execbuf.buffer_count = 1;
	res = ioctl(fd, DRM_IOCTL_I915_GEM_EXECBUFFER2, &execbuf);
	printf("DRM_IOCTL_I915_GEM_EXECBUFFER2: %d\n", res);

	close(fd);

	return EXIT_SUCCESS;
}


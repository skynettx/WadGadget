//
// Copyright(C) 2022-2024 Simon Howard
//
// You can redistribute and/or modify this program under the terms of
// the GNU General Public License version 2 as published by the Free
// Software Foundation, or any later version. This program is
// distributed WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//

#ifndef FS__VFILE_H_INCLUDED
#define FS__VFILE_H_INCLUDED

#include <stdio.h>
#include <stdbool.h>

typedef struct _VFILE VFILE;
typedef struct _VFILE_CONTEXT VFILE_CONTEXT;

struct _VFILE_CONTEXT {
	long pos;
};

struct vfile_functions {
	size_t (*read)(void *ptr, size_t size, size_t nitems, void *handle);
	size_t (*write)(const void *ptr, size_t size,
	                size_t nitems, void *handle);

	int (*seek)(void *handle, long offset, int whence);
	long (*tell)(void *handle);
	void (*truncate)(void *handle);
	void (*close)(void *handle);
	void (*sync)(void *handle);
};

VFILE *vfopen(void *handle, struct vfile_functions *funcs);
VFILE *vfrestrict(VFILE *inner, long start, long end, int ro);
VFILE *vfwrapfile(FILE *stream);

int vfcopy(VFILE *from, VFILE *to);

void vfonclose(VFILE *stream, void (*callback)(VFILE *, void *), void *data);

size_t vfread(void *ptr, size_t size, size_t nitems, VFILE *stream);
size_t vfwrite(const void *ptr, size_t size, size_t nitems, VFILE *stream);

// Truncates at current position (unlike ftruncate).
void vftruncate(VFILE *stream);

// Read/write to memory buffer.
VFILE *vfopenmem(const void *buf, size_t buf_len);
bool vfgetbuf(VFILE *f, void **buf, size_t *buf_len);
void *vfreadall(VFILE *input, size_t *len);

int vfseek(VFILE *stream, long offset, int whence);
long vftell(VFILE *stream);

void vfsync(VFILE *stream);
void vfclose(VFILE *stream);
VFILE_CONTEXT *vfswitchcontext(VFILE *f, VFILE_CONTEXT *ctx);

#define WITH_VFCONTEXT(vf, ctx, statement) do { \
		VFILE_CONTEXT *saved_ctx = vfswitchcontext(vf, ctx); \
		statement; \
		vfswitchcontext(vf, saved_ctx); \
	} while (0)

#endif /* #ifndef FS__VFILE_H_INCLUDED */

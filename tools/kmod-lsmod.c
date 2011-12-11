/*
 * kmod-lsmod - list modules from linux kernel using libkmod.
 *
 * Copyright (C) 2011  ProFUSION embedded systems
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation version 2.1.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "libkmod.h"


int main(int argc, char *argv[])
{
	struct kmod_ctx *ctx;
	struct kmod_list *list, *itr;
	int err;

	if (argc != 1) {
		fprintf(stderr, "Usage: %s\n", argv[0]);
		return EXIT_FAILURE;
	}

	ctx = kmod_new(NULL);
	if (ctx == NULL) {
		fputs("Error: kmod_new() failed!\n", stderr);
		return EXIT_FAILURE;
	}

	err = kmod_loaded_get_list(ctx, &list);
	if (err < 0) {
		fprintf(stderr, "Error: could not get list of modules: %s\n",
			strerror(-err));
		kmod_unref(ctx);
		return EXIT_FAILURE;
	}

	puts("Module                  Size  Used by");

	kmod_list_foreach(itr, list) {
		struct kmod_module *mod = kmod_module_get_module(itr);
		const char *name = kmod_module_get_name(mod);
		int use_count = kmod_module_get_refcnt(mod);
		long size = kmod_module_get_size(mod);
		struct kmod_list *holders, *hitr;
		int first = 1;

		printf("%-19s %8ld  %d ", name, size, use_count);
		holders = kmod_module_get_holders(mod);
		kmod_list_foreach(hitr, holders) {
			struct kmod_module *hm = kmod_module_get_module(hitr);

			if (!first)
				putchar(',');
			else
				first = 0;

			fputs(kmod_module_get_name(hm), stdout);
			kmod_module_unref(hm);
		}
		putchar('\n');
		kmod_module_unref_list(holders);
		kmod_module_unref(mod);
	}
	kmod_module_unref_list(list);
	kmod_unref(ctx);

	return EXIT_SUCCESS;
}

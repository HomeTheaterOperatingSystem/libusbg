/*
 * Copyright (C) 2013 Linaro Limited
 *
 * Matt Porter <mporter@linaro.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <errno.h>
#include <stdio.h>
#include <usbg/usbg.h>

/**
 * @file gadget-acm-ecm.c
 * @example gadget-acm-ecm.c
 * This is an example of how to create an ACM+ECM gadget device.
 */

#define VENDOR		0x1d6b
#define PRODUCT		0x0104

int main(void)
{
	usbg_state *s;
	usbg_gadget *g;
	usbg_config *c;
	usbg_function *f_acm0, *f_acm1, *f_ecm;
	int ret = -EINVAL;
	int usbg_ret;

	usbg_gadget_attrs g_attrs = {
			0x0200, /* bcdUSB */
			0x00, /* Defined at interface level */
			0x00, /* subclass */
			0x00, /* device protocol */
			0x0040, /* Max allowed packet size */
			VENDOR,
			PRODUCT,
			0x0001, /* Verson of device */
	};

	usbg_gadget_strs g_strs = {
			"0123456789", /* Serial number */
			"Foo Inc.", /* Manufacturer */
			"Bar Gadget" /* Product string */
	};

	usbg_config_strs c_strs = {
			"CDC 2xACM+ECM"
	};

	usbg_ret = usbg_init("/sys/kernel/config", &s);
	if (usbg_ret != USBG_SUCCESS) {
		fprintf(stderr, "Error on USB gadget init\n");
		goto out1;
	}

	usbg_ret = usbg_create_gadget(s, "g1", &g_attrs, &g_strs, &g);
	if (usbg_ret != USBG_SUCCESS) {
		fprintf(stderr, "Error on create gadget\n");
		goto out2;
	}

	usbg_ret = usbg_create_function(g, F_ACM, "usb0", NULL, &f_acm0);
	if (usbg_ret != USBG_SUCCESS) {
		fprintf(stderr, "Error creating acm0 function\n");
		goto out2;
	}

	usbg_ret = usbg_create_function(g, F_ACM, "usb1", NULL, &f_acm1);
	if (usbg_ret != USBG_SUCCESS) {
		fprintf(stderr, "Error creating acm1 function\n");
		goto out2;
	}

	usbg_ret = usbg_create_function(g, F_ECM, "usb0", NULL, &f_acm1);
	if (usbg_ret != USBG_SUCCESS) {
		fprintf(stderr, "Error creating ecm function\n");
		goto out2;
	}

	usbg_ret = usbg_create_config(g, "c.1", NULL /* use defaults */, &c_strs, &c);
	if (usbg_ret != USBG_SUCCESS) {
		fprintf(stderr, "Error creating config\n");
		goto out2;
	}

	usbg_add_config_function(c, "acm.GS0", f_acm0);
	usbg_add_config_function(c, "acm.GS1", f_acm1);
	usbg_add_config_function(c, "ecm.usb0", f_ecm);

	usbg_enable_gadget(g, DEFAULT_UDC);

	ret = 0;

out2:
	usbg_cleanup(s);

out1:
	return ret;
}

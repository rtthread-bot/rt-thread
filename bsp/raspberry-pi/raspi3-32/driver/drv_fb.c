/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author         Notes
 * 2019-08-29     zdzn           first version
 */

#include <rthw.h>
#include <rtthread.h>
#include "mbox.h"
#include "drv_fb.h"
#include "mmu.h"

#define CHAR_W          8
#define CHAR_H          12

#define COLOR_DELTA     0.05
static struct rt_hdmi_fb_device _hdmi;

// https://github.com/xinu-os/xinu/blob/1789b7a50b5b73c2ea76ebd764c54a034097d04d/device/framebuffer_rpi/font.c
unsigned char FONT[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00, /*'!'*/
0x00, 0x14, 0x14, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*'"'*/
0x00, 0x00, 0x14, 0x14, 0x3e, 0x14, 0x3e, 0x14, 0x14, 0x00, 0x00, 0x00, /*'#'*/
0x00, 0x00, 0x08, 0x3c, 0x0a, 0x1c, 0x28, 0x1e, 0x08, 0x00, 0x00, 0x00, /*'$'*/
0x00, 0x00, 0x06, 0x26, 0x10, 0x08, 0x04, 0x32, 0x30, 0x00, 0x00, 0x00, /*'%'*/
0x00, 0x00, 0x1c, 0x02, 0x02, 0x04, 0x2a, 0x12, 0x2c, 0x00, 0x00, 0x00, /*'&'*/
0x00, 0x18, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*'''*/
0x20, 0x10, 0x10, 0x08, 0x08, 0x08, 0x08, 0x08, 0x10, 0x10, 0x20, 0x00, /*'('*/
0x02, 0x04, 0x04, 0x08, 0x08, 0x08, 0x08, 0x08, 0x04, 0x04, 0x02, 0x00, /*')'*/
0x00, 0x00, 0x00, 0x08, 0x2a, 0x1c, 0x2a, 0x08, 0x00, 0x00, 0x00, 0x00, /*'*'*/
0x00, 0x00, 0x00, 0x08, 0x08, 0x3e, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, /*'+'*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x08, 0x04, 0x00, /*','*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*'-'*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, /*'.'*/
0x20, 0x20, 0x10, 0x10, 0x08, 0x08, 0x04, 0x04, 0x02, 0x02, 0x00, 0x00, /*'/'*/
0x00, 0x1c, 0x22, 0x32, 0x2a, 0x26, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, /*'0'*/
0x00, 0x08, 0x0c, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, /*'1'*/
0x00, 0x1c, 0x22, 0x20, 0x10, 0x08, 0x04, 0x02, 0x3e, 0x00, 0x00, 0x00, /*'2'*/
0x00, 0x1c, 0x22, 0x20, 0x18, 0x20, 0x20, 0x22, 0x1c, 0x00, 0x00, 0x00, /*'3'*/
0x00, 0x10, 0x18, 0x18, 0x14, 0x14, 0x3e, 0x10, 0x38, 0x00, 0x00, 0x00, /*'4'*/
0x00, 0x3e, 0x02, 0x02, 0x1e, 0x20, 0x20, 0x22, 0x1c, 0x00, 0x00, 0x00, /*'5'*/
0x00, 0x18, 0x04, 0x02, 0x1e, 0x22, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, /*'6'*/
0x00, 0x3e, 0x22, 0x20, 0x20, 0x10, 0x10, 0x08, 0x08, 0x00, 0x00, 0x00, /*'7'*/
0x00, 0x1c, 0x22, 0x22, 0x1c, 0x22, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, /*'8'*/
0x00, 0x1c, 0x22, 0x22, 0x22, 0x3c, 0x20, 0x10, 0x0c, 0x00, 0x00, 0x00, /*'9'*/
0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, /*':'*/
0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x08, 0x04, 0x00, /*';'*/
0x00, 0x00, 0x00, 0x30, 0x0c, 0x03, 0x0c, 0x30, 0x00, 0x00, 0x00, 0x00, /*'<'*/
0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, /*'='*/
0x00, 0x00, 0x00, 0x03, 0x0c, 0x30, 0x0c, 0x03, 0x00, 0x00, 0x00, 0x00, /*'>'*/
0x00, 0x1c, 0x22, 0x20, 0x10, 0x08, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00, /*'?'*/
0x00, 0x00, 0x1c, 0x22, 0x3a, 0x3a, 0x1a, 0x02, 0x1c, 0x00, 0x00, 0x00, /*'@'*/
0x00, 0x00, 0x08, 0x14, 0x22, 0x22, 0x3e, 0x22, 0x22, 0x00, 0x00, 0x00, /*'A'*/
0x00, 0x00, 0x1e, 0x22, 0x22, 0x1e, 0x22, 0x22, 0x1e, 0x00, 0x00, 0x00, /*'B'*/
0x00, 0x00, 0x1c, 0x22, 0x02, 0x02, 0x02, 0x22, 0x1c, 0x00, 0x00, 0x00, /*'C'*/
0x00, 0x00, 0x0e, 0x12, 0x22, 0x22, 0x22, 0x12, 0x0e, 0x00, 0x00, 0x00, /*'D'*/
0x00, 0x00, 0x3e, 0x02, 0x02, 0x1e, 0x02, 0x02, 0x3e, 0x00, 0x00, 0x00, /*'E'*/
0x00, 0x00, 0x3e, 0x02, 0x02, 0x1e, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, /*'F'*/
0x00, 0x00, 0x1c, 0x22, 0x02, 0x32, 0x22, 0x22, 0x3c, 0x00, 0x00, 0x00, /*'G'*/
0x00, 0x00, 0x22, 0x22, 0x22, 0x3e, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, /*'H'*/
0x00, 0x00, 0x3e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3e, 0x00, 0x00, 0x00, /*'I'*/
0x00, 0x00, 0x38, 0x20, 0x20, 0x20, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, /*'J'*/
0x00, 0x00, 0x22, 0x12, 0x0a, 0x06, 0x0a, 0x12, 0x22, 0x00, 0x00, 0x00, /*'K'*/
0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x3e, 0x00, 0x00, 0x00, /*'L'*/
0x00, 0x00, 0x22, 0x36, 0x2a, 0x2a, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, /*'M'*/
0x00, 0x00, 0x22, 0x26, 0x26, 0x2a, 0x32, 0x32, 0x22, 0x00, 0x00, 0x00, /*'N'*/
0x00, 0x00, 0x1c, 0x22, 0x22, 0x22, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, /*'O'*/
0x00, 0x00, 0x1e, 0x22, 0x22, 0x1e, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, /*'P'*/
0x00, 0x00, 0x1c, 0x22, 0x22, 0x22, 0x22, 0x22, 0x1c, 0x30, 0x00, 0x00, /*'Q'*/
0x00, 0x00, 0x1e, 0x22, 0x22, 0x1e, 0x0a, 0x12, 0x22, 0x00, 0x00, 0x00, /*'R'*/
0x00, 0x00, 0x1c, 0x22, 0x02, 0x1c, 0x20, 0x22, 0x1c, 0x00, 0x00, 0x00, /*'S'*/
0x00, 0x00, 0x3e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, /*'T'*/
0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, /*'U'*/
0x00, 0x00, 0x22, 0x22, 0x22, 0x14, 0x14, 0x08, 0x08, 0x00, 0x00, 0x00, /*'V'*/
0x00, 0x00, 0x22, 0x22, 0x22, 0x2a, 0x2a, 0x36, 0x22, 0x00, 0x00, 0x00, /*'W'*/
0x00, 0x00, 0x22, 0x22, 0x14, 0x08, 0x14, 0x22, 0x22, 0x00, 0x00, 0x00, /*'X'*/
0x00, 0x00, 0x22, 0x22, 0x14, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, /*'Y'*/
0x00, 0x00, 0x3e, 0x20, 0x10, 0x08, 0x04, 0x02, 0x3e, 0x00, 0x00, 0x00, /*'Z'*/
0x38, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x38, 0x00, /*'['*/
0x02, 0x02, 0x04, 0x04, 0x08, 0x08, 0x10, 0x10, 0x20, 0x20, 0x00, 0x00, /*'\'*/
0x0e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0e, 0x00, /*']'*/
0x00, 0x08, 0x14, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*'^'*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, /*'_'*/
0x00, 0x0c, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*'`'*/
0x00, 0x00, 0x00, 0x00, 0x3c, 0x22, 0x22, 0x32, 0x2c, 0x00, 0x00, 0x00, /*'a'*/
0x00, 0x02, 0x02, 0x02, 0x1e, 0x22, 0x22, 0x22, 0x1e, 0x00, 0x00, 0x00, /*'b'*/
0x00, 0x00, 0x00, 0x00, 0x3c, 0x02, 0x02, 0x02, 0x3c, 0x00, 0x00, 0x00, /*'c'*/
0x00, 0x20, 0x20, 0x20, 0x3c, 0x22, 0x22, 0x22, 0x3c, 0x00, 0x00, 0x00, /*'d'*/
0x00, 0x00, 0x00, 0x00, 0x1c, 0x22, 0x3e, 0x02, 0x1c, 0x00, 0x00, 0x00, /*'e'*/
0x00, 0x38, 0x04, 0x04, 0x1e, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x00, /*'f'*/
0x00, 0x00, 0x00, 0x00, 0x3c, 0x22, 0x22, 0x22, 0x3c, 0x20, 0x20, 0x1c, /*'g'*/
0x00, 0x02, 0x02, 0x02, 0x1e, 0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, /*'h'*/
0x00, 0x08, 0x08, 0x00, 0x0c, 0x08, 0x08, 0x08, 0x1c, 0x00, 0x00, 0x00, /*'i'*/
0x00, 0x10, 0x10, 0x00, 0x1c, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0e, /*'j'*/
0x00, 0x02, 0x02, 0x02, 0x12, 0x0a, 0x06, 0x0a, 0x12, 0x00, 0x00, 0x00, /*'k'*/
0x00, 0x0c, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x1c, 0x00, 0x00, 0x00, /*'l'*/
0x00, 0x00, 0x00, 0x00, 0x16, 0x2a, 0x2a, 0x2a, 0x22, 0x00, 0x00, 0x00, /*'m'*/
0x00, 0x00, 0x00, 0x00, 0x1a, 0x26, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, /*'n'*/
0x00, 0x00, 0x00, 0x00, 0x1c, 0x22, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, /*'o'*/
0x00, 0x00, 0x00, 0x00, 0x1e, 0x22, 0x22, 0x22, 0x1e, 0x02, 0x02, 0x02, /*'p'*/
0x00, 0x00, 0x00, 0x00, 0x3c, 0x22, 0x22, 0x22, 0x3c, 0x20, 0x20, 0x20, /*'q'*/
0x00, 0x00, 0x00, 0x00, 0x1a, 0x06, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, /*'r'*/
0x00, 0x00, 0x00, 0x00, 0x3c, 0x02, 0x1c, 0x20, 0x1e, 0x00, 0x00, 0x00, /*'s'*/
0x00, 0x08, 0x08, 0x08, 0x3e, 0x08, 0x08, 0x08, 0x30, 0x00, 0x00, 0x00, /*'t'*/
0x00, 0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x32, 0x2c, 0x00, 0x00, 0x00, /*'u'*/
0x00, 0x00, 0x00, 0x00, 0x36, 0x14, 0x14, 0x08, 0x08, 0x00, 0x00, 0x00, /*'v'*/
0x00, 0x00, 0x00, 0x00, 0x22, 0x2a, 0x2a, 0x2a, 0x14, 0x00, 0x00, 0x00, /*'w'*/
0x00, 0x00, 0x00, 0x00, 0x22, 0x14, 0x08, 0x14, 0x22, 0x00, 0x00, 0x00, /*'x'*/
0x00, 0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x3c, 0x20, 0x20, 0x1c, /*'y'*/
0x00, 0x00, 0x00, 0x00, 0x3e, 0x10, 0x08, 0x04, 0x3e, 0x00, 0x00, 0x00, /*'z'*/
0x20, 0x10, 0x10, 0x10, 0x10, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x00, /*'{'*/
0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, /*'|'*/
0x02, 0x04, 0x04, 0x04, 0x04, 0x08, 0x04, 0x04, 0x04, 0x04, 0x02, 0x00, /*'}'*/
0x00, 0x04, 0x2a, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*'~'*/
0x00, 0x00, 0x00, 0x08, 0x08, 0x14, 0x14, 0x22, 0x3e, 0x00, 0x00, 0x00, /*DEL*/
};

void newline(fb_t* fb)
{
    uint8_t* to;
    uint8_t* from;
    int i;
    fb->y++;
    fb->x = 0;

    if (fb->y == (fb->height / CHAR_H))
    {

        to = (uint8_t*) fb->addr;
        from = to + (CHAR_H * fb->pitch);

        for (i = 0; i < ((fb->height - CHAR_H) * fb->pitch); i++)
        {
            *to++ = *from++;
        }

        uint32_t *addr = (uint32_t*) (fb->addr) + (fb->height - CHAR_H) * fb->width;

        for (i = 0; i < (CHAR_H * fb->width); i++)
        {
            *addr++ = fb->back;
        }

        fb->y--;
    }
}

void clear_line(fb_t *fb, const int line)
{
    int i;
    uint32_t* addr;
    if (line > fb->height / CHAR_H)
    {
        fb->y = 0;
    }
    else
    {
        fb->y = line;
    }

    fb->x = 0;

    addr = (uint32_t*) (fb->addr + (line * CHAR_H * fb->depth * fb->width));
    for (i = 0; i < (CHAR_H * fb->width); i++)
    {
        *addr++ = fb->back;
    }

}

void fb_draw_char(fb_t *fb, char s)
{
    unsigned char* addr = (unsigned char*) fb->addr;
    unsigned char *glyph = (unsigned char*) FONT + (s) * 12;
    // calculate the offset on screen
    int offs = (fb->y * CHAR_H * fb->pitch) + (fb->x * (CHAR_W + 1) * 4);
    // variables
    int i, j, line, mask, bytesperline = (CHAR_W + 7) / 8;
    // display a character
    for (j = 0; j < CHAR_H; j++)
    {
        // display one row
        line = offs;
        mask = 1;
        for (i = 0; i < CHAR_W; i++)
        {
            // if bit set, we use white color, otherwise black
            *((unsigned int*) (addr + line)) = ((int) *glyph) & mask ? fb->fore : fb->back;
            mask <<= 1;
            line += 4;
        }
        // adjust to next line
        glyph += bytesperline;
        offs += fb->pitch;
    }
}

void fb_print(fb_t *fb, char *s)
{

    // draw next character if it's not zero
    while (*s)
    {
        // handle carrige return
        if (*s == '\r')
        {
            fb->x = 0;
        }
        else if (*s == '\n')
        {
            newline(fb);
        }
        else if (*s == '\t')
        {
            fb->x = ((fb->x + 4) >> 2) << 2;
        }
        else if (*s == '\b')
        {
            if (fb->x)
            {
                fb->x--;
                fb_draw_char(fb, ' ');
            }
        }
        else
        {
            fb_draw_char(fb, *s);
            fb->x++;
        }
        // next character
        if (fb->x == fb->width / CHAR_W)
        {
            newline(fb);
        }
        s++;
    }
}

rt_err_t hdmi_fb_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

rt_err_t hdmi_fb_close(rt_device_t dev)
{
    return RT_EOK;
}

rt_size_t hdmi_fb_read(rt_device_t dev, rt_off_t pos, void *buf, rt_size_t size)
{
    return 0;
}

rt_size_t hdmi_fb_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    fb_print(&_hdmi.fb, (char *) buffer);
#ifdef BSP_USING_HDMI_DISPLAY
    rt_device_t uart = rt_device_find("uart1");
    int old_flag = uart->open_flag;
    uart->open_flag |= RT_DEVICE_FLAG_STREAM;
    rt_device_write(uart, 0, buffer, size);
    uart->open_flag = old_flag;
#endif
    return size;
}

rt_err_t hdmi_fb_control(rt_device_t dev, int cmd, void *args)
{
    return RT_EOK;
}

const static struct rt_device_ops hdmi_fb_ops =
{
    RT_NULL,
    hdmi_fb_open,
    hdmi_fb_close,
    hdmi_fb_read,
    hdmi_fb_write,
    hdmi_fb_control
};

static struct rt_device_graphic_info _hdmi_info;

static void hdmi_draw_rect(const char* pixel, int x1, int y1, int x2, int y2)
{
    int i, j;
    int line;
    for (j = y1; j <= y2; j++)
    {
        line = (j * _hdmi.fb.pitch) + (x1 * 4);
        for (i = x1; i <= x2; i++)
        {
            // if bit set, we use white color, otherwise black
            *((unsigned int*) (_hdmi_info.framebuffer + line)) = *(unsigned int*) pixel;
            line += 4;
        }
    }

}

static void hdmi_set_pixel(const char* pixel, int x, int y)
{
    *(uint32_t*) (_hdmi.fb.addr + (y * _hdmi.fb.pitch + x * 4)) = *(uint32_t *) pixel;
}

static void hdmi_get_pixel(char* pixel, int x, int y)
{
    uint32_t ret = 0;
    ret = (*(uint32_t*) (_hdmi.fb.addr + (y * _hdmi.fb.pitch + x * 4)) & 0x00FFFFFF);
    *pixel = ret;
}

static void hdmi_draw_hline(const char* pixel, int x1, int x2, int y)
{
    hdmi_draw_rect(pixel, x1, y, x2, y);
}

static void hdmi_draw_vline(const char* pixel, int x, int y1, int y2)
{
    hdmi_draw_rect(pixel, x, y1, x, y2);
}

static void hdmi_blit_line(const char* pixels, int x, int y, rt_size_t size)
{
    int i = 0;
    uint32_t *pixel_base = (uint32_t*) (_hdmi.fb.addr + (y * _hdmi.fb.pitch + x * 4));
    uint32_t *colors = (uint32_t *) pixels;
    for (i = 0; i < size; i++)
    {
        pixel_base[i] = colors[i];
    }
}

static struct rt_device_graphic_ops hdmi_ops =
{
    hdmi_set_pixel,
    hdmi_get_pixel,
    hdmi_draw_hline,
    hdmi_draw_vline,
    hdmi_blit_line
};

rt_err_t rt_hdmi_fb_device_init(struct rt_hdmi_fb_device *hdmi_fb, const char *name)
{
    struct rt_device *device;
    RT_ASSERT(hdmi_fb != RT_NULL);

    device = &hdmi_fb->parent;
    device->user_data = &hdmi_ops;

    /* set device type */
    device->type = RT_Device_Class_Graphic;
    /* initialize device interface */
#ifdef RT_USING_DEVICE_OPS
    device->ops = &hdmi_fb_ops;
#else
    device->init = RT_NULL;
    device->open = hdmi_fb_open;
    device->close = hdmi_fb_close;
    device->read = hdmi_fb_read;
    device->write = hdmi_fb_write;
    device->control = hdmi_fb_control;
#endif

    /* register to device manager */
    rt_device_register(device, name, RT_DEVICE_FLAG_RDWR);

    return RT_EOK;
}

/**
 * Show a picture
 */
void print_fb_info()
{
    rt_kprintf("FrameBuffer Info: \n \t width %x\t height %x\t depth %x\t addr %x\t size %x\t \n", fb_info.width,
            fb_info.height, fb_info.depth, fb_info.addr, fb_info.size);
    rt_kprintf("call mbox:%x,%x,%x,%x,%x\n", mbox[0], mbox[1], mbox[2], mbox[3], mbox[4]);
}

int hdmi_fb_init()
{
    unsigned int *mbox = (unsigned int*) MBOX_ADDR;
    mbox[0] = 35 * 4;
    mbox[1] = MBOX_REQUEST;

    mbox[2] = 0x48003;  //set phy wh
    mbox[3] = 8;
    mbox[4] = 8;
    mbox[5] = 640;         //FrameBufferInfo.width
    mbox[6] = 480;          //FrameBufferInfo.height

    mbox[7] = 0x48004;  //set virt wh
    mbox[8] = 8;
    mbox[9] = 8;
    mbox[10] = 640;        //FrameBufferInfo.virtual_width
    mbox[11] = 480;         //FrameBufferInfo.virtual_height

    mbox[12] = 0x48009; //set virt offset
    mbox[13] = 8;
    mbox[14] = 8;
    mbox[15] = 0;           //FrameBufferInfo.x_offset
    mbox[16] = 0;           //FrameBufferInfo.y.offset

    mbox[17] = 0x48005; //set depth
    mbox[18] = 4;
    mbox[19] = 4;
    mbox[20] = 32;          //FrameBufferInfo.depth

    mbox[21] = 0x48006; //set pixel order
    mbox[22] = 4;
    mbox[23] = 4;
    mbox[24] = 1;           //RGB, not BGR preferably

    mbox[25] = 0x40001; //get framebuffer, gets alignment on request
    mbox[26] = 8;
    mbox[27] = 8;
    mbox[28] = 4096;        //FrameBufferInfo.pointer
    mbox[29] = 0;           //FrameBufferInfo.size

    mbox[30] = 0x40008; //get pitch
    mbox[31] = 4;
    mbox[32] = 4;
    mbox[33] = 0;           //FrameBufferInfo.pitch

    mbox[34] = MBOX_TAG_LAST;
    if (mbox_call(MBOX_CH_PROP, MMU_DISABLE) && mbox[20] == 32 && mbox[28] != 0)
    {
        mbox[28] &= 0x3FFFFFFF;
        _hdmi.fb.width = mbox[5];
        _hdmi.fb.height = mbox[6];
        _hdmi.fb.pitch = mbox[33];
        //_hdmi.fb.addr = (void*)((unsigned long)mbox[28]);
        _hdmi.fb.addr = (rt_uint32_t) mbox[28];
        _hdmi.fb.size = mbox[29];
        _hdmi.fb.depth = 32;
        _hdmi.fb.x = 0;
        _hdmi.fb.y = 0;
        _hdmi.fb.fore = CONSOLE_WHITE;
        _hdmi.fb.back = CONSOLE_BLACK;
        rt_hdmi_fb_device_init(&_hdmi, "hdmi");
        rt_hw_change_mmu_table(_hdmi.fb.addr, _hdmi.fb.size, _hdmi.fb.addr, DEVICE_MEM);
        fb_info.width = _hdmi.fb.width;
        fb_info.height = _hdmi.fb.height;
        fb_info.addr = _hdmi.fb.addr;
        fb_info.size = _hdmi.fb.size;
        fb_info.pitch = _hdmi.fb.pitch;
        fb_info.depth = _hdmi.fb.depth;
        _hdmi_info.pixel_format = RTGRAPHIC_PIXEL_FORMAT_RGB888;
        _hdmi_info.bits_per_pixel = _hdmi.fb.depth;
        _hdmi_info.width = _hdmi.fb.width;
        _hdmi_info.height = _hdmi.fb.height;
        _hdmi_info.framebuffer = (rt_uint8_t *) _hdmi.fb.addr;
    }
    return 0;
}

INIT_DEVICE_EXPORT(hdmi_fb_init);

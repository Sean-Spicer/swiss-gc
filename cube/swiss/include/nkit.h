/* 
 * Copyright (c) 2020, Extrems <extrems@extremscorner.org>
 * 
 * This file is part of Swiss.
 * 
 * Swiss is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * Swiss is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * with Swiss.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef __NKIT_H
#define __NKIT_H

#include <stdbool.h>
#include <stddef.h>
#include "gcm.h"

bool is_redump_disc(DiskHeader *header);
bool valid_disc_crc(DiskHeader *header, uint32_t crc);
bool valid_disc_size(DiskHeader *header, size_t size);

#endif /* __NKIT_H */

/*
 * Copyright 2016 Dmitry Ivanov
 *
 * This file is part of Quentier.
 *
 * Quentier is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * Quentier is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Quentier. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QUENTIER_MODELS_TAG_CACHE_H
#define QUENTIER_MODELS_TAG_CACHE_H

#include <quentier/utility/LRUCache.hpp>
#include <quentier/types/Tag.h>

namespace quentier {

typedef LRUCache<QString, Tag> TagCache;

} // namespace quentier

#endif // QUENTIER_MODELS_TAG_CACHE_H

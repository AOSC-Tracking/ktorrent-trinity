/*
 * tools_p.h
 *
 * Copyright (c) 2001, 2002, 2003 Frerich Raabe <raabe@kde.org>
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. For licensing and distribution details, check the
 * accompanying file 'COPYING'.
 */
#ifndef LIBRSS_TOOLS_P_H
#define LIBRSS_TOOLS_P_H

#include <time.h>

class TQDomNode;
class TQString;

namespace RSS
{
	struct Shared
	{
		Shared() : count(1) { }
		void ref() { count++; }
		bool deref() { return !--count; }
		unsigned int count;
	};

	TQString extractNode(const TQDomNode &tqparent, const TQString &elemName, bool isInlined=true);
	time_t parseISO8601Date(const TQString &s);
}

#endif // LIBRSS_TOOLS_P_H
// vim:noet:ts=4

/*-
 * Copyright (c) 2019 The FreeBSD Foundation
 * All rights reserved.
 *
 * This software was developed by BFF Storage Systems, LLC under sponsorship
 * from the FreeBSD Foundation.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

extern "C" {
#include <unistd.h>
}

#include "mockfs.hh"
#include "utils.hh"

using namespace testing;

class Readlink: public FuseTest {};

TEST_F(Readlink, eloop)
{
	const char FULLPATH[] = "mountpoint/src";
	const char RELPATH[] = "src";
	const uint64_t ino = 42;
	char buf[80];

	EXPECT_CALL(*m_mock, process(
		ResultOf([=](auto in) {
			return (in->header.opcode == FUSE_LOOKUP &&
				strcmp(in->body.lookup, RELPATH) == 0);
		}, Eq(true)),
		_)
	).WillOnce(Invoke([](auto in, auto out) {
		out->header.unique = in->header.unique;
		SET_OUT_HEADER_LEN(out, entry);
		out->body.entry.attr.mode = S_IFLNK | 0777;
		out->body.entry.nodeid = ino;
	}));

	EXPECT_CALL(*m_mock, process(
		ResultOf([=](auto in) {
			return (in->header.opcode == FUSE_READLINK &&
				in->header.nodeid == ino);
		}, Eq(true)),
		_)
	).WillOnce(Invoke([=](auto in, auto out) {
		out->header.unique = in->header.unique;
		out->header.error = -ELOOP;
		out->header.len = sizeof(out->header);
	}));


	EXPECT_EQ(-1, readlink(FULLPATH, buf, sizeof(buf)));
	EXPECT_EQ(ELOOP, errno);
}

TEST_F(Readlink, ok)
{
	const char FULLPATH[] = "mountpoint/src";
	const char RELPATH[] = "src";
	const char dst[] = "dst";
	const uint64_t ino = 42;
	char buf[80];

	EXPECT_CALL(*m_mock, process(
		ResultOf([=](auto in) {
			return (in->header.opcode == FUSE_LOOKUP &&
				strcmp(in->body.lookup, RELPATH) == 0);
		}, Eq(true)),
		_)
	).WillOnce(Invoke([](auto in, auto out) {
		out->header.unique = in->header.unique;
		SET_OUT_HEADER_LEN(out, entry);
		out->body.entry.attr.mode = S_IFLNK | 0777;
		out->body.entry.nodeid = ino;
	}));

	EXPECT_CALL(*m_mock, process(
		ResultOf([=](auto in) {
			return (in->header.opcode == FUSE_READLINK &&
				in->header.nodeid == ino);
		}, Eq(true)),
		_)
	).WillOnce(Invoke([=](auto in, auto out) {
		out->header.unique = in->header.unique;
		strlcpy(out->body.str, dst, sizeof(out->body.str));
		out->header.len = sizeof(out->header) + strlen(dst) + 1;
	}));


	EXPECT_EQ((ssize_t)strlen(dst) + 1,
		  readlink(FULLPATH, buf, sizeof(buf)));
	EXPECT_STREQ(dst, buf);
}

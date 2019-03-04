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
#include <sys/stat.h>

#include <fcntl.h>
}

#include "mockfs.hh"
#include "utils.hh"

using namespace testing;

class Setattr : public FuseTest {};


/* Change the mode of a file */
TEST_F(Setattr, chmod)
{
	const char FULLPATH[] = "mountpoint/some_file.txt";
	const char RELPATH[] = "some_file.txt";
	const uint64_t ino = 42;
	const mode_t oldmode = 0755;
	const mode_t newmode = 0644;

	EXPECT_CALL(*m_mock, process(
		ResultOf([=](auto in) {
			return (in->header.opcode == FUSE_LOOKUP &&
				strcmp(in->body.lookup, RELPATH) == 0);
		}, Eq(true)),
		_)
	).WillOnce(Invoke([](auto in, auto out) {
		out->header.unique = in->header.unique;
		SET_OUT_HEADER_LEN(out, entry);
		out->body.entry.attr.mode = S_IFREG | oldmode;
		out->body.entry.nodeid = ino;
		out->body.entry.attr.mode = S_IFREG | oldmode;
	}));

	EXPECT_CALL(*m_mock, process(
		ResultOf([](auto in) {
			/* In protocol 7.23, ctime will be changed too */
			uint32_t valid = FATTR_MODE;
			return (in->header.opcode == FUSE_SETATTR &&
				in->header.nodeid == ino &&
				in->body.setattr.valid == valid &&
				in->body.setattr.mode == newmode);
		}, Eq(true)),
		_)
	).WillOnce(Invoke([](auto in, auto out) {
		out->header.unique = in->header.unique;
		SET_OUT_HEADER_LEN(out, attr);
		out->body.attr.attr.ino = ino;	// Must match nodeid
		out->body.attr.attr.mode = S_IFREG | newmode;
	}));
	EXPECT_EQ(0, chmod(FULLPATH, newmode)) << strerror(errno);
}

/* Change the owner and group of a file */
TEST_F(Setattr, chown)
{
	const char FULLPATH[] = "mountpoint/some_file.txt";
	const char RELPATH[] = "some_file.txt";
	const uint64_t ino = 42;
	const gid_t oldgroup = 66;
	const gid_t newgroup = 99;
	const uid_t olduser = 33;
	const uid_t newuser = 44;

	EXPECT_CALL(*m_mock, process(
		ResultOf([=](auto in) {
			return (in->header.opcode == FUSE_LOOKUP &&
				strcmp(in->body.lookup, RELPATH) == 0);
		}, Eq(true)),
		_)
	).WillOnce(Invoke([](auto in, auto out) {
		out->header.unique = in->header.unique;
		SET_OUT_HEADER_LEN(out, entry);
		out->body.entry.attr.mode = S_IFREG | 0644;
		out->body.entry.nodeid = ino;
		out->body.entry.attr.gid = oldgroup;
		out->body.entry.attr.uid = olduser;
	}));

	EXPECT_CALL(*m_mock, process(
		ResultOf([](auto in) {
			/* In protocol 7.23, ctime will be changed too */
			uint32_t valid = FATTR_GID | FATTR_UID;
			return (in->header.opcode == FUSE_SETATTR &&
				in->header.nodeid == ino &&
				in->body.setattr.valid == valid &&
				in->body.setattr.uid == newuser &&
				in->body.setattr.gid == newgroup);
		}, Eq(true)),
		_)
	).WillOnce(Invoke([](auto in, auto out) {
		out->header.unique = in->header.unique;
		SET_OUT_HEADER_LEN(out, attr);
		out->body.attr.attr.ino = ino;	// Must match nodeid
		out->body.attr.attr.mode = S_IFREG | 0644;
		out->body.attr.attr.uid = newuser;
		out->body.attr.attr.gid = newgroup;
	}));
	EXPECT_EQ(0, chown(FULLPATH, newuser, newgroup)) << strerror(errno);
}



/* 
 * FUSE daemons are allowed to check permissions however they like.  If the
 * daemon returns EPERM, even if the file permissions "should" grant access,
 * then fuse(4) should return EPERM too.
 */
TEST_F(Setattr, eperm)
{
	const char FULLPATH[] = "mountpoint/some_file.txt";
	const char RELPATH[] = "some_file.txt";
	const uint64_t ino = 42;

	EXPECT_CALL(*m_mock, process(
		ResultOf([=](auto in) {
			return (in->header.opcode == FUSE_LOOKUP &&
				strcmp(in->body.lookup, RELPATH) == 0);
		}, Eq(true)),
		_)
	).WillOnce(Invoke([](auto in, auto out) {
		out->header.unique = in->header.unique;
		SET_OUT_HEADER_LEN(out, entry);
		out->body.entry.attr.mode = S_IFREG | 0777;
		out->body.entry.nodeid = ino;
		out->body.entry.attr.uid = in->header.uid;
		out->body.entry.attr.gid = in->header.gid;
	}));

	EXPECT_CALL(*m_mock, process(
		ResultOf([](auto in) {
			return (in->header.opcode == FUSE_SETATTR &&
				in->header.nodeid == ino);
		}, Eq(true)),
		_)
	).WillOnce(Invoke([](auto in, auto out) {
		out->header.unique = in->header.unique;
		out->header.error = -EPERM;
		out->header.len = sizeof(out->header);
	}));
	EXPECT_NE(0, truncate(FULLPATH, 10));
	EXPECT_EQ(EPERM, errno);
}

/* Change the mode of an open file, by its file descriptor */
TEST_F(Setattr, fchmod)
{
	const char FULLPATH[] = "mountpoint/some_file.txt";
	const char RELPATH[] = "some_file.txt";
	uint64_t ino = 42;
	int fd;
	const mode_t oldmode = 0755;
	const mode_t newmode = 0644;

	EXPECT_CALL(*m_mock, process(
		ResultOf([=](auto in) {
			return (in->header.opcode == FUSE_LOOKUP &&
				strcmp(in->body.lookup, RELPATH) == 0);
		}, Eq(true)),
		_)
	).WillOnce(Invoke([=](auto in, auto out) {
		out->header.unique = in->header.unique;
		SET_OUT_HEADER_LEN(out, entry);
		out->body.entry.attr.mode = S_IFREG | oldmode;
		out->body.entry.nodeid = ino;
		out->body.entry.attr_valid = UINT64_MAX;
	}));

	EXPECT_CALL(*m_mock, process(
		ResultOf([=](auto in) {
			return (in->header.opcode == FUSE_OPEN &&
				in->header.nodeid == ino);
		}, Eq(true)),
		_)
	).WillOnce(Invoke([=](auto in, auto out) {
		out->header.unique = in->header.unique;
		out->header.len = sizeof(out->header);
		SET_OUT_HEADER_LEN(out, open);
	}));

	/* Until the attr cache is working, we may send an additional GETATTR */
	EXPECT_CALL(*m_mock, process(
		ResultOf([=](auto in) {
			return (in->header.opcode == FUSE_GETATTR &&
				in->header.nodeid == ino);
		}, Eq(true)),
		_)
	).WillRepeatedly(Invoke([=](auto in, auto out) {
		out->header.unique = in->header.unique;
		SET_OUT_HEADER_LEN(out, attr);
		out->body.attr.attr.ino = ino;	// Must match nodeid
		out->body.attr.attr.mode = S_IFREG | oldmode;
	}));

	EXPECT_CALL(*m_mock, process(
		ResultOf([=](auto in) {
			/* In protocol 7.23, ctime will be changed too */
			uint32_t valid = FATTR_MODE;
			return (in->header.opcode == FUSE_SETATTR &&
				in->header.nodeid == ino &&
				in->body.setattr.valid == valid &&
				in->body.setattr.mode == newmode);
		}, Eq(true)),
		_)
	).WillOnce(Invoke([=](auto in, auto out) {
		out->header.unique = in->header.unique;
		SET_OUT_HEADER_LEN(out, attr);
		out->body.attr.attr.ino = ino;	// Must match nodeid
		out->body.attr.attr.mode = S_IFREG | newmode;
	}));

	fd = open(FULLPATH, O_RDONLY);
	ASSERT_LE(0, fd) << strerror(errno);
	ASSERT_EQ(0, fchmod(fd, newmode)) << strerror(errno);
	/* Deliberately leak fd.  close(2) will be tested in release.cc */
}

/* Change the size of an open file, by its file descriptor */
TEST_F(Setattr, ftruncate)
{
	const char FULLPATH[] = "mountpoint/some_file.txt";
	const char RELPATH[] = "some_file.txt";
	uint64_t ino = 42;
	int fd;
	uint64_t fh = 0xdeadbeef1a7ebabe;
	const off_t oldsize = 99;
	const off_t newsize = 12345;

	EXPECT_CALL(*m_mock, process(
		ResultOf([=](auto in) {
			return (in->header.opcode == FUSE_LOOKUP &&
				strcmp(in->body.lookup, RELPATH) == 0);
		}, Eq(true)),
		_)
	).WillOnce(Invoke([=](auto in, auto out) {
		out->header.unique = in->header.unique;
		SET_OUT_HEADER_LEN(out, entry);
		out->body.entry.attr.mode = S_IFREG | 0755;
		out->body.entry.nodeid = ino;
		out->body.entry.attr_valid = UINT64_MAX;
		out->body.entry.attr.size = oldsize;
	}));

	EXPECT_CALL(*m_mock, process(
		ResultOf([=](auto in) {
			return (in->header.opcode == FUSE_OPEN &&
				in->header.nodeid == ino);
		}, Eq(true)),
		_)
	).WillOnce(Invoke([=](auto in, auto out) {
		out->header.unique = in->header.unique;
		out->header.len = sizeof(out->header);
		SET_OUT_HEADER_LEN(out, open);
		out->body.open.fh = fh;
	}));

	/* Until the attr cache is working, we may send an additional GETATTR */
	EXPECT_CALL(*m_mock, process(
		ResultOf([=](auto in) {
			return (in->header.opcode == FUSE_GETATTR &&
				in->header.nodeid == ino);
		}, Eq(true)),
		_)
	).WillRepeatedly(Invoke([=](auto in, auto out) {
		out->header.unique = in->header.unique;
		SET_OUT_HEADER_LEN(out, attr);
		out->body.attr.attr.ino = ino;	// Must match nodeid
		out->body.attr.attr.mode = S_IFREG | 0755;
		out->body.attr.attr.size = oldsize;
	}));

	EXPECT_CALL(*m_mock, process(
		ResultOf([=](auto in) {
			/* In protocol 7.23, ctime will be changed too */
			uint32_t valid = FATTR_SIZE | FATTR_FH;
			return (in->header.opcode == FUSE_SETATTR &&
				in->header.nodeid == ino &&
				in->body.setattr.valid == valid &&
				in->body.setattr.fh == fh);
		}, Eq(true)),
		_)
	).WillOnce(Invoke([=](auto in, auto out) {
		out->header.unique = in->header.unique;
		SET_OUT_HEADER_LEN(out, attr);
		out->body.attr.attr.ino = ino;	// Must match nodeid
		out->body.attr.attr.mode = S_IFREG | 0755;
		out->body.attr.attr.size = newsize;
	}));

	fd = open(FULLPATH, O_RDWR);
	ASSERT_LE(0, fd) << strerror(errno);
	ASSERT_EQ(0, ftruncate(fd, newsize)) << strerror(errno);
	/* Deliberately leak fd.  close(2) will be tested in release.cc */
}

/* Change the size of the file */
TEST_F(Setattr, truncate) {
	const char FULLPATH[] = "mountpoint/some_file.txt";
	const char RELPATH[] = "some_file.txt";
	const uint64_t ino = 42;
	const uint64_t oldsize = 100'000'000;
	const uint64_t newsize = 20'000'000;

	EXPECT_CALL(*m_mock, process(
		ResultOf([=](auto in) {
			return (in->header.opcode == FUSE_LOOKUP &&
				strcmp(in->body.lookup, RELPATH) == 0);
		}, Eq(true)),
		_)
	).WillOnce(Invoke([](auto in, auto out) {
		out->header.unique = in->header.unique;
		SET_OUT_HEADER_LEN(out, entry);
		out->body.entry.attr.mode = S_IFREG | 0644;
		out->body.entry.nodeid = ino;
		out->body.entry.attr.size = oldsize;
	}));

	EXPECT_CALL(*m_mock, process(
		ResultOf([](auto in) {
			/* In protocol 7.23, ctime will be changed too */
			uint32_t valid = FATTR_SIZE;
			return (in->header.opcode == FUSE_SETATTR &&
				in->header.nodeid == ino &&
				in->body.setattr.valid == valid &&
				in->body.setattr.size == newsize);
		}, Eq(true)),
		_)
	).WillOnce(Invoke([](auto in, auto out) {
		out->header.unique = in->header.unique;
		SET_OUT_HEADER_LEN(out, attr);
		out->body.attr.attr.ino = ino;	// Must match nodeid
		out->body.attr.attr.mode = S_IFREG | 0644;
		out->body.attr.attr.size = newsize;
	}));
	EXPECT_EQ(0, truncate(FULLPATH, newsize)) << strerror(errno);
}

/* Change a file's timestamps */
TEST_F(Setattr, utimensat) {
	const char FULLPATH[] = "mountpoint/some_file.txt";
	const char RELPATH[] = "some_file.txt";
	const uint64_t ino = 42;
	const timespec oldtimes[2] = {
		{.tv_sec = 1, .tv_nsec = 2},
		{.tv_sec = 3, .tv_nsec = 4},
	};
	const timespec newtimes[2] = {
		{.tv_sec = 5, .tv_nsec = 6},
		{.tv_sec = 7, .tv_nsec = 8},
	};

	EXPECT_CALL(*m_mock, process(
		ResultOf([=](auto in) {
			return (in->header.opcode == FUSE_LOOKUP &&
				strcmp(in->body.lookup, RELPATH) == 0);
		}, Eq(true)),
		_)
	).WillOnce(Invoke([=](auto in, auto out) {
		out->header.unique = in->header.unique;
		SET_OUT_HEADER_LEN(out, entry);
		out->body.entry.attr.mode = S_IFREG | 0644;
		out->body.entry.nodeid = ino;
		out->body.entry.attr_valid = UINT64_MAX;
		out->body.entry.attr.atime = oldtimes[0].tv_sec;
		out->body.entry.attr.atimensec = oldtimes[0].tv_nsec;
		out->body.entry.attr.mtime = oldtimes[1].tv_sec;
		out->body.entry.attr.mtimensec = oldtimes[1].tv_nsec;
	}));

	/* 
	 * Until bug 235775 is fixed, utimensat will make an extra FUSE_GETATTR
	 * call
	 */ 
	EXPECT_CALL(*m_mock, process(
		ResultOf([](auto in) {
			return (in->header.opcode == FUSE_GETATTR &&
				in->header.nodeid == ino);
		}, Eq(true)),
		_)
	).WillOnce(Invoke([=](auto in, auto out) {
		out->header.unique = in->header.unique;
		SET_OUT_HEADER_LEN(out, attr);
		out->body.attr.attr.ino = ino;	// Must match nodeid
		out->body.attr.attr.mode = S_IFREG | 0644;
		out->body.attr.attr.atime = oldtimes[0].tv_sec;
		out->body.attr.attr.atimensec = oldtimes[0].tv_nsec;
		out->body.attr.attr.mtime = oldtimes[1].tv_sec;
		out->body.attr.attr.mtimensec = oldtimes[1].tv_nsec;
	}));

	EXPECT_CALL(*m_mock, process(
		ResultOf([=](auto in) {
			/* In protocol 7.23, ctime will be changed too */
			uint32_t valid = FATTR_ATIME | FATTR_MTIME;
			return (in->header.opcode == FUSE_SETATTR &&
				in->header.nodeid == ino &&
				in->body.setattr.valid == valid &&
				in->body.setattr.atime == newtimes[0].tv_sec &&
				in->body.setattr.atimensec ==
					newtimes[0].tv_nsec &&
				in->body.setattr.mtime == newtimes[1].tv_sec &&
				in->body.setattr.mtimensec ==
					newtimes[1].tv_nsec);
		}, Eq(true)),
		_)
	).WillOnce(Invoke([=](auto in, auto out) {
		out->header.unique = in->header.unique;
		SET_OUT_HEADER_LEN(out, attr);
		out->body.attr.attr.ino = ino;	// Must match nodeid
		out->body.attr.attr.mode = S_IFREG | 0644;
		out->body.attr.attr.atime = newtimes[0].tv_sec;
		out->body.attr.attr.atimensec = newtimes[0].tv_nsec;
		out->body.attr.attr.mtime = newtimes[1].tv_sec;
		out->body.attr.attr.mtimensec = newtimes[1].tv_nsec;
	}));
	EXPECT_EQ(0, utimensat(AT_FDCWD, FULLPATH, &newtimes[0], 0))
		<< strerror(errno);
}

/* Change a file mtime but not its atime */
TEST_F(Setattr, utimensat_mtime_only) {
	const char FULLPATH[] = "mountpoint/some_file.txt";
	const char RELPATH[] = "some_file.txt";
	const uint64_t ino = 42;
	const timespec oldtimes[2] = {
		{.tv_sec = 1, .tv_nsec = 2},
		{.tv_sec = 3, .tv_nsec = 4},
	};
	const timespec newtimes[2] = {
		{.tv_sec = 5, .tv_nsec = UTIME_OMIT},
		{.tv_sec = 7, .tv_nsec = 8},
	};

	EXPECT_CALL(*m_mock, process(
		ResultOf([=](auto in) {
			return (in->header.opcode == FUSE_LOOKUP &&
				strcmp(in->body.lookup, RELPATH) == 0);
		}, Eq(true)),
		_)
	).WillOnce(Invoke([=](auto in, auto out) {
		out->header.unique = in->header.unique;
		SET_OUT_HEADER_LEN(out, entry);
		out->body.entry.attr.mode = S_IFREG | 0644;
		out->body.entry.nodeid = ino;
		out->body.entry.attr_valid = UINT64_MAX;
		out->body.entry.attr.atime = oldtimes[0].tv_sec;
		out->body.entry.attr.atimensec = oldtimes[0].tv_nsec;
		out->body.entry.attr.mtime = oldtimes[1].tv_sec;
		out->body.entry.attr.mtimensec = oldtimes[1].tv_nsec;
	}));

	/* 
	 * Until bug 235775 is fixed, utimensat will make an extra FUSE_GETATTR
	 * call
	 */ 
	EXPECT_CALL(*m_mock, process(
		ResultOf([](auto in) {
			return (in->header.opcode == FUSE_GETATTR &&
				in->header.nodeid == ino);
		}, Eq(true)),
		_)
	).WillOnce(Invoke([=](auto in, auto out) {
		out->header.unique = in->header.unique;
		SET_OUT_HEADER_LEN(out, attr);
		out->body.attr.attr.ino = ino;	// Must match nodeid
		out->body.attr.attr.mode = S_IFREG | 0644;
		out->body.attr.attr.atime = oldtimes[0].tv_sec;
		out->body.attr.attr.atimensec = oldtimes[0].tv_nsec;
		out->body.attr.attr.mtime = oldtimes[1].tv_sec;
		out->body.attr.attr.mtimensec = oldtimes[1].tv_nsec;
	}));

	EXPECT_CALL(*m_mock, process(
		ResultOf([=](auto in) {
			/* In protocol 7.23, ctime will be changed too */
			uint32_t valid = FATTR_MTIME;
			return (in->header.opcode == FUSE_SETATTR &&
				in->header.nodeid == ino &&
				in->body.setattr.valid == valid &&
				in->body.setattr.mtime == newtimes[1].tv_sec &&
				in->body.setattr.mtimensec ==
					newtimes[1].tv_nsec);
		}, Eq(true)),
		_)
	).WillOnce(Invoke([=](auto in, auto out) {
		out->header.unique = in->header.unique;
		SET_OUT_HEADER_LEN(out, attr);
		out->body.attr.attr.ino = ino;	// Must match nodeid
		out->body.attr.attr.mode = S_IFREG | 0644;
		out->body.attr.attr.atime = oldtimes[0].tv_sec;
		out->body.attr.attr.atimensec = oldtimes[0].tv_nsec;
		out->body.attr.attr.mtime = newtimes[1].tv_sec;
		out->body.attr.attr.mtimensec = newtimes[1].tv_nsec;
	}));
	EXPECT_EQ(0, utimensat(AT_FDCWD, FULLPATH, &newtimes[0], 0))
		<< strerror(errno);
}

/* 
 * Writethrough cache: newly changed attributes should be automatically cached,
 * if the filesystem allows it
 */
//TODO TEST_F(Setattr, writethrough_cache){}

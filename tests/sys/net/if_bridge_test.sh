# $FreeBSD$
#
# SPDX-License-Identifier: BSD-2-Clause-FreeBSD
#
# Copyright (c) 2020 The FreeBSD Foundation
#
# This software was developed by Kristof Provost under sponsorship
# from the FreeBSD Foundation.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

. $(atf_get_srcdir)/../common/vnet.subr

atf_test_case "bridge_transmit_ipv4_unicast" "cleanup"
bridge_transmit_ipv4_unicast_head()
{
	atf_set descr 'bridge_transmit_ipv4_unicast bridging test'
	atf_set require.user root
}

bridge_transmit_ipv4_unicast_body()
{
	vnet_init

	epair_alcatraz=$(vnet_mkepair)
	epair_singsing=$(vnet_mkepair)

	vnet_mkjail alcatraz ${epair_alcatraz}b
	vnet_mkjail singsing ${epair_singsing}b

	jexec alcatraz ifconfig ${epair_alcatraz}b 192.0.2.1/24 up
	jexec singsing ifconfig ${epair_singsing}b 192.0.2.2/24 up

	bridge=$(vnet_mkbridge)

	ifconfig ${bridge} up
	ifconfig ${epair_alcatraz}a up
	ifconfig ${epair_singsing}a up
	ifconfig ${bridge} addm ${epair_alcatraz}a
	ifconfig ${bridge} addm ${epair_singsing}a

	atf_check -s exit:0 -o ignore jexec alcatraz ping -c 3 -t 1 192.0.2.2
	atf_check -s exit:0 -o ignore jexec singsing ping -c 3 -t 1 192.0.2.1
}

bridge_transmit_ipv4_unicast_cleanup()
{
	vnet_cleanup
}

atf_init_test_cases()
{
	atf_add_test_case "bridge_transmit_ipv4_unicast"
}

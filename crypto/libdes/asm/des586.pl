#!/usr/local/bin/perl

$prog="des586.pl";

# base code is in microsft
# op dest, source
# format.
#

# WILL NOT WORK ANYMORE WITH desboth.pl
require "desboth.pl";

if (	($ARGV[0] eq "elf"))
	{ require "x86unix.pl"; }
elsif (	($ARGV[0] eq "a.out"))
	{ $aout=1; require "x86unix.pl"; }
elsif (	($ARGV[0] eq "sol"))
	{ $sol=1; require "x86unix.pl"; }
elsif (	($ARGV[0] eq "cpp"))
	{ $cpp=1; require "x86unix.pl"; }
elsif (	($ARGV[0] eq "win32"))
	{ require "x86ms.pl"; }
else
	{
	print STDERR <<"EOF";
Pick one target type from
	elf	- linux, FreeBSD etc
	a.out	- old linux
	sol	- x86 solaris
	cpp	- format so x86unix.cpp can be used
	win32	- Windows 95/Windows NT
EOF
	exit(1);
	}

&comment("Don't even think of reading this code");
&comment("It was automatically generated by $prog");
&comment("Which is a perl program used to generate the x86 assember for");
&comment("any of elf, a.out, Win32, or Solaris");
&comment("It can be found in SSLeay 0.6.5+ or in libdes 3.26+");
&comment("eric <eay\@mincom.oz.au>");
&comment("");

&file("dx86xxxx");

$L="edi";
$R="esi";

&des_encrypt("des_encrypt",1);
&des_encrypt("des_encrypt2",0);

&des_encrypt3("des_encrypt3",1);
&des_encrypt3("des_decrypt3",0);

&file_end();

sub des_encrypt
	{
	local($name,$do_ip)=@_;

	&function_begin($name,3);

	&comment("");
	&comment("Load the 2 words");
	&mov("eax",&wparam(0));
	&mov($R,&DWP(0,"eax","",0));
	&mov($L,&DWP(4,"eax","",0));

	if ($do_ip)
		{
		&comment("");
		&comment("IP");
		&IP($R,$L,"eax");
		}

	&comment("");
	&comment("fixup rotate");
	&rotl($R,3);
	&rotl($L,3);

	&comment("");
	&comment("load counter, key_schedule and enc flag");

	# encrypting part

	$ks="ebp";
	&xor(	"ebx",		"ebx"		);
	&mov("eax",&wparam(2));	# get encrypt flag
	&xor(	"ecx",		"ecx"		);
	&cmp("eax","0");
	&mov(	$ks,		&wparam(1)	);
	&je(&label("start_decrypt"));

	for ($i=0; $i<16; $i+=2)
		{
		&comment("");
		&comment("Round $i");
		&D_ENCRYPT($i,$L,$R,$i*2,$ks,"des_SPtrans","eax","ebx","ecx","edx");

		&comment("");
		&comment("Round ".sprintf("%d",$i+1));
		&D_ENCRYPT($i+1,$R,$L,($i+1)*2,$ks,"des_SPtrans","eax","ebx","ecx","edx");
		}
	&jmp(&label("end"));

	&set_label("start_decrypt");

	for ($i=15; $i>0; $i-=2)
		{
		&comment("");
		&comment("Round $i");
		&D_ENCRYPT(15-$i,$L,$R,$i*2,$ks,"des_SPtrans","eax","ebx","ecx","edx");
		&comment("");
		&comment("Round ".sprintf("%d",$i-1));
		&D_ENCRYPT(15-$i+1,$R,$L,($i-1)*2,$ks,"des_SPtrans","eax","ebx","ecx","edx");
		}

	&set_label("end");

	&comment("");
	&comment("Fixup");
	&rotr($L,3);		# r
	&rotr($R,3);		# l

	if ($do_ip)
		{
		&comment("");
		&comment("FP");
		&FP($R,$L,"eax");
		}

	&mov("eax",&wparam(0));
	&mov(&DWP(0,"eax","",0),$L);
	&mov(&DWP(4,"eax","",0),$R);

	&function_end($name);
	}

sub D_ENCRYPT
	{
	local($r,$L,$R,$S,$ks,$desSP,$u,$tmp1,$tmp2,$t)=@_;

	 &mov(	$t,		&DWP(&n2a(($S+1)*4),$ks,"",0));
	&mov(	$u,		&DWP(&n2a($S*4),$ks,"",0));
	 &xor(	$t,		$R);
	&xor(	$u,		$R);
	 &rotr(	$t,		4		);
	&and(	$u,		"0xfcfcfcfc"	);
	 &and(	$t,		"0xfcfcfcfc"	);
	&movb(	&LB($tmp1),	&LB($u)	);
	&movb(	&LB($tmp2),	&HB($u)	);
	&xor(	$L,		&DWP("      $desSP",$tmp1,"",0));
	&shr(	$u,		16);
	&xor(	$L,		&DWP("0x200+$desSP",$tmp2,"",0));
	&movb(	&LB($tmp1),	&LB($u)	);
	&movb(	&LB($tmp2),	&HB($u)	);
	&xor(	$L,		&DWP("0x400+$desSP",$tmp1,"",0));
	&mov(	$u,		&DWP("0x600+$desSP",$tmp2,"",0));

	 &movb(	&LB($tmp1),	&LB($t)	);
	 &movb(	&LB($tmp2),	&HB($t)	);
	 &xor(	$L,		&DWP("0x100+$desSP",$tmp1,"",0));
	 &shr(	$t,		16);
	 &xor(	$u,		&DWP("0x300+$desSP",$tmp2,"",0));
	 &movb(	&LB($tmp1),	&LB($t)	);
	 &movb(	&LB($tmp2),	&HB($t)	);
	 &xor(	$L,		&DWP("0x500+$desSP",$tmp1,"",0));
	 &xor(	$u,		&DWP("0x700+$desSP",$tmp2,"",0));
	 &xor(  $L,	$u);
	}

sub PERM_OP
	{
	local($a,$b,$tt,$shift,$mask)=@_;

	&mov(	$tt,		$a		);
	&shr(	$tt,		$shift		);
	&xor(	$tt,		$b		);
	&and(	$tt,		$mask		);
	&xor(	$b,		$tt		);
	&shl(	$tt,		$shift		);
	&xor(	$a,		$tt		);
	}

sub IP
	{
	local($l,$r,$tt)=@_;

	&PERM_OP($r,$l,$tt, 4,"0x0f0f0f0f");
	&PERM_OP($l,$r,$tt,16,"0x0000ffff");
	&PERM_OP($r,$l,$tt, 2,"0x33333333");
	&PERM_OP($l,$r,$tt, 8,"0x00ff00ff");
	&PERM_OP($r,$l,$tt, 1,"0x55555555");
	}

sub FP
	{
	local($l,$r,$tt)=@_;

	&PERM_OP($l,$r,$tt, 1,"0x55555555");
        &PERM_OP($r,$l,$tt, 8,"0x00ff00ff");
        &PERM_OP($l,$r,$tt, 2,"0x33333333");
        &PERM_OP($r,$l,$tt,16,"0x0000ffff");
        &PERM_OP($l,$r,$tt, 4,"0x0f0f0f0f");
	}

sub n2a
	{
	sprintf("%d",$_[0]);
	}

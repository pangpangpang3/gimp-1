#!/usr/bin/perl -w

# The GIMP -- an image manipulation program
# Copyright (C) 1999-2003 Manish Singh <yosh@gimp.org>

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUTFILE ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

BEGIN {
    $srcdir  = $ENV{srcdir}  || '.';
    $destdir = $ENV{destdir} || '.';
}

use lib $srcdir;

require 'enums.pl';
require 'util.pl';

*enums = \%Gimp::CodeGen::enums::enums;

*write_file = \&Gimp::CodeGen::util::write_file;
*FILE_EXT   = \$Gimp::CodeGen::util::FILE_EXT;

my $enumfile = "$destdir/plug-ins/script-fu/script-fu-constants.c$FILE_EXT";
open ENUMFILE, "> $enumfile" or die "Can't open $enumfile: $!\n";

print ENUMFILE <<'GPL';
/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-2003 Spencer Kimball and Peter Mattis
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* NOTE: This file is autogenerated by enumcode.pl. */

GPL

print ENUMFILE <<CODE;
#include "siod/siod.h"

void
init_generated_constants (void)
{
CODE

foreach (sort keys %enums) {
    my $enum = $enums{$_};
    foreach $symbol (@{$enum->{symbols}}) {
	my $sym = $symbol;
	$sym =~ s/^GIMP\_//;
	$sym =~ s/_/-/g;

	print ENUMFILE <<CODE;
  setvar (cintern ("$sym"), flocons ($enum->{mapping}->{$symbol}), NIL);
CODE
    }
    print ENUMFILE "\n";
}

print ENUMFILE "  return;\n}\n";

close ENUMFILE;
&write_file($enumfile);

$enumfile = "$destdir/libgimp/gimpenums.h$FILE_EXT";
open ENUMFILE, "> $enumfile" or die "Can't open $enumfile: $!\n";

print ENUMFILE <<'LGPL';
/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-2003 Peter Mattis and Spencer Kimball
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/* NOTE: This file is autogenerated by enumcode.pl */

LGPL

my $guard = "__GIMP_ENUMS_H__";
print ENUMFILE <<HEADER;
#ifndef $guard
#define $guard

G_BEGIN_DECLS

HEADER

foreach (sort keys %enums) {
    if (! ($enums{$_}->{header} =~ /libgimp/)) {
        my $gtype = $func = $_;

	for ($gtype) { s/Gimp//; s/([A-Z][^A-Z]+)/\U\1\E_/g; s/_$// }
	for ($func) { s/Gimp//; s/([A-Z][^A-Z]+)/\L\1\E_/g; s/_$// }

	print ENUMFILE "\n#define GIMP_TYPE_$gtype (gimp_$func\_get_type ())\n\n";
	print ENUMFILE "GType gimp_$func\_get_type (void) G_GNUC_CONST;\n\n";
	print ENUMFILE "typedef enum\n{\n";

	my $enum = $enums{$_}; my $body = "";

	foreach $symbol (@{$enum->{symbols}}) {
	    my $sym = $symbol;
	    $body .= "  $sym";
	    $body .= " = $enum->{mapping}->{$symbol}" if !$enum->{contig};
	    $body .= ",\n";
	}

	$body =~ s/,\n$//s;
	$body .= "\n} ";
	$body .= "$_;\n\n";
	print ENUMFILE $body
    }
}

print ENUMFILE <<HEADER;

G_END_DECLS

#endif /* $guard */
HEADER

close ENUMFILE;
&write_file($enumfile);

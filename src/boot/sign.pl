use strict;
use warnings;

my $filename = shift or die "Usage: $0 <FILE>\n";

my $size = (stat $filename)[7] or die "can't stat `$filename': $!\n";
if ($size > 510) {
	print {*STDERR} "boot block too large: $size bytes (max 510)\n";
	exit 1;
}
print {*STDERR} "boot block is $size bytes (max 510)\n";

open my $fh, '>>', $filename
	or die "can't open `$filename': $!\n";

print {$fh} "\0" x (510 - $size);
print {$fh} "\x55\xAA";

close $fh

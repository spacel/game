import glob
import re
import sys

def check_file(name):
    error_count = 0
    with open(name) as f:
        line_number = 1
        for line in iter(f):
            # Check whitespaces
            if re.search("^[ \t]+$", line):
	        print("%s:%d\tEmpty lines with whitespaces" % (name, line_number))
	        error_count += 1

            if re.search("[ \t]+$", line):
	        print("%s:%d\tEnding line with whitespaces:\t%s" % (name, line_number, line.strip()))
	        error_count += 1

            if re.search("[ ]{2,}$", line):
                print("%s:%d\tToo many spaces:\t%s" % (name, line_number, line.strip()))
                error_count += 1

            # Check missing spaces
            if re.search("[ \t]+(switch|if|for|while|try|catch)\t?\(", line) or re.search("\)\t?\{", line) \
                or re.search("[a-zA-Z0-9][=]", line) or re.search("[=][a-zA-Z0-9]", line):
                print("%s:%d\tMissing spaces:\t%s" % (name, line_number, line.strip()))
                error_count += 1

            # Tabs instead of spaces
            if re.search("\t[=*+-]", line) or re.search("[=*+-]\t", line):
                print("%s:%d\tTabs instead of spaces:\t%s" % (name, line_number, line.strip()))
                error_count += 1

            # Spaces instead of tabs
            if re.search("^[ ]{2,}", line):
                print("%s:%d\tSpaces instead of tabs:\t%s" % (name, line_number, line.strip()))
                error_count += 1

            # Bad placement for * or &
            if re.search("[a-zA-Z0-9][*&]", line):
                print("%s:%d\tPointer or ref operator should be with variable not type:\t%s" % (name, line_number, line.strip()))
                error_count += 1

            line_number += 1
    return error_count

s_allowed_extensions = ["cpp", "c", "h"]
error_count = 0

for extension in s_allowed_extensions:
    for name in glob.glob("src/*/*." + extension):
        error_count += check_file(name)

    for name in glob.glob("src/*/*/*." + extension):
        error_count += check_file(name)

    for name in glob.glob("src/*/*/*/*." + extension):
        error_count += check_file(name)

if error_count > 0:
    print("%d errors found, aborting" % error_count)
    sys.exit(1)

print("No error found. Compilation can start")


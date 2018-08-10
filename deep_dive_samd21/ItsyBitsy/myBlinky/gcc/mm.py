#!/anaconda3/bin python
#
#
# Modify the makefile that is auto generated from atmel studio to add the
# props/functionality that I use when running make within the gcc folder.

# Set the path for the makefile.
# Open the makefile for reading.
# Open a file - makefile_new for writing.
# For each line in makefile:
# read a line of makefile.  The line may need to be modified or a new
# line may need to be inserted.
# Write the line/new lines/modified line to makefile_new.
# Rename makefile makefile_old
# Rename makefile_new makefile
import os
makefile_filename = os.path.join(os.getcwd(), 'Makefile')
makefile_new_filename = os.path.join(os.getcwd() + 'Makefile_new')
with open(makefile_filename, 'r') as makefile:
    with open(makefile_new_filename, 'w') as makefile_new:
        for line in makefile:
            # vpath %.S is the last of the vpath lines.
            # we add a few more.
            if 'vpath %.S' in line:
                makefile_new.write(line)
                makefile_new.write('vpath % ../\n')
                makefile_new.write('vpath %.o ../\n')
                continue
            if "OBJS_AS_ARGS += " in line:
                line = str.replace(line, "OBJS_AS_ARGS", "NEW_OBJS_AS_ARGS")
                makefile_new.write(line)
                continue
            if "DEPS := $(OBJS:%.o=%.d)" in line:
                makefile_new.write(line)
                makefile_new.write('OBJS_AS_ARGS := $(patsubst "%","../%",$(NEW_OBJS_AS_ARGS))\n')
                continue
            if '-MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"  -o "$@" "$<"' in line:
                makefile_new.write(
                    '-MD -MP -MF "../$(@:%.o=%.d)" -MT"../$(@:%.o=%.d)" -MT"../$(@:%.o=%.o)"  -o "../$@" "$<"\n')
                continue
            if '-g3' in line:
                line = str.replace(line, '-g3', '-ggdb')
                makefile_new.write(line)
                continue
            makefile_new.write(line)
makefile_old_filename = os.path.join(os.getcwd(), 'Makefile_old')
os.rename(makefile_filename, makefile_old_filename)
os.rename(makefile_new_filename, makefile_filename)

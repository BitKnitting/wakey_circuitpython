"""
This script processes the output from the C preprocessor and extracts all
qstr. Each qstr is transformed into a qstr definition of the form 'Q(...)'.

This script works with Python 2.6, 2.7, 3.3 and 3.4.
"""

from __future__ import print_function

import re
import sys
import os

# Python 2/3 compatibility:
#   - iterating through bytes is different
#   - codepoint2name lives in a different module
import platform
if platform.python_version_tuple()[0] == '2':
    bytes_cons = lambda val, enc=None: bytearray(val)
    from htmlentitydefs import name2codepoint
elif platform.python_version_tuple()[0] == '3':
    bytes_cons = bytes
    from html.entities import name2codepoint
    unichr = chr
# end compatibility code

# Blacklist of qstrings that are specially handled in further
# processing and should be ignored
QSTRING_BLACK_LIST = set(['NULL', 'number_of'])

# add some custom names to map characters that aren't in HTML
name2codepoint['hyphen'] = ord('-')
name2codepoint['space'] = ord(' ')
name2codepoint['squot'] = ord('\'')
name2codepoint['comma'] = ord(',')
name2codepoint['dot'] = ord('.')
name2codepoint['colon'] = ord(':')
name2codepoint['semicolon'] = ord(';')
name2codepoint['slash'] = ord('/')
name2codepoint['percent'] = ord('%')
name2codepoint['hash'] = ord('#')
name2codepoint['paren_open'] = ord('(')
name2codepoint['paren_close'] = ord(')')
name2codepoint['bracket_open'] = ord('[')
name2codepoint['bracket_close'] = ord(']')
name2codepoint['brace_open'] = ord('{')
name2codepoint['brace_close'] = ord('}')
name2codepoint['star'] = ord('*')
name2codepoint['bang'] = ord('!')
name2codepoint['backslash'] = ord('\\')
name2codepoint['plus'] = ord('+')
name2codepoint['dollar'] = ord('$')
name2codepoint['equals'] = ord('=')
name2codepoint['question'] = ord('?')
name2codepoint['at_sign'] = ord('@')
name2codepoint['caret'] = ord('^')
name2codepoint['pipe'] = ord('|')
name2codepoint['tilde'] = ord('~')

def write_out(fname, output):
    if output:
        for m, r in [("/", "__"), ("\\", "__"), (":", "@"), ("..", "@@")]:
            fname = fname.replace(m, r)
        with open(args.output_dir + "/" + fname + ".qstr", "w") as f:
            f.write("\n".join(output) + "\n")

def qstr_unescape(qstr):
    for name in name2codepoint:
        if "__" + name + "__" in qstr:
            continue
        if "_" + name + "_" in qstr:
          qstr = qstr.replace("_" + name + "_", str(unichr(name2codepoint[name])))
    return qstr

def process_file(f):
    output = []
    last_fname = None
    for line in f:
        # match gcc-like output (# n "file") and msvc-like output (#line n "file")
        if line and (line[0:2] == "# " or line[0:5] == "#line"):
            m = re.match(r"#[line]*\s\d+\s\"([^\"]+)\"", line)
            assert m is not None
            fname = m.group(1)
            if not fname.endswith(".c"):
                continue
            if fname != last_fname:
                write_out(last_fname, output)
                output = []
                last_fname = fname
            continue
        for match in re.findall(r'MP_QSTR_[_a-zA-Z0-9]+', line):
            name = match.replace('MP_QSTR_', '')
            if name not in QSTRING_BLACK_LIST:
                output.append('Q(' + qstr_unescape(name) + ')')

    write_out(last_fname, output)
    return ""


def cat_together():
    import glob
    import hashlib
    hasher = hashlib.md5()
    all_lines = []
    outf = open(args.output_dir + "/out", "wb")
    for fname in glob.glob(args.output_dir + "/*.qstr"):
        with open(fname, "rb") as f:
            lines = f.readlines()
            all_lines += lines
    all_lines.sort()
    all_lines = b"\n".join(all_lines)
    outf.write(all_lines)
    outf.close()
    hasher.update(all_lines)
    new_hash = hasher.hexdigest()
    #print(new_hash)
    old_hash = None
    try:
        with open(args.output_file + ".hash") as f:
            old_hash = f.read()
    except IOError:
        pass
    if old_hash != new_hash:
        print("QSTR updated")
        try:
            # rename below might fail if file exists
            os.remove(args.output_file)
        except:
            pass
        os.rename(args.output_dir + "/out", args.output_file)
        with open(args.output_file + ".hash", "w") as f:
            f.write(new_hash)
    else:
        print("QSTR not updated")


if __name__ == "__main__":
    if len(sys.argv) != 5:
        print('usage: %s command input_filename output_dir output_file' % sys.argv[0])
        sys.exit(2)

    class Args:
        pass
    args = Args()
    args.command = sys.argv[1]
    args.input_filename = sys.argv[2]
    args.output_dir = sys.argv[3]
    args.output_file = sys.argv[4]

    try:
        os.makedirs(args.output_dir)
    except OSError:
        pass

    if args.command == "split":
        with open(args.input_filename) as infile:
            process_file(infile)

    if args.command == "cat":
        cat_together()

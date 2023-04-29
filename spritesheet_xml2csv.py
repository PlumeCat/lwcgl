#!/bin/python3
import sys

usage = """
spritesheet_xml2csv.py

    Usage: ./spritesheet_xml2csv <input> <output>

    Convert XML spritesheets to CSV - designed to convert from Shoebox XML export
    
    Will search for lines in the XML data with the following format
        <??? name="cockpitGreen_3" x="346" y="234" width="60" height="61"/>
    and convert to CSV format with headers name,x,y,width,height
"""

def findattr(line, attr):
    start = line.find(attr+'="')
    end = line.find('"', start+len(attr)+2)
    if start == -1 or end == -1:
        raise Exception(f"missing or invalid attribute '{attr}'")
    attr = line[start+len(attr)+2:end]
    if "," in attr:
        raise Exception(f"attribute must not contain comma character")
    return attr

if __name__ == "__main__":
    if len(sys.argv) < 2 or sys.argv[1] in ("--help", "-H", "help"):
        print(usage)
    else:
        fname = sys.argv[1]
        out_fname = sys.argv[2]
        with open(fname) as infile, open(out_fname, "w") as outfile:
            outfile.writelines("name,x,y,width,height\n")
            for line in infile.readlines():
                line = line.strip()
                try:
                    attrs = [ findattr(line, a) for a in [ "name", "x", "y", "width", "height" ] ]
                    outfile.writelines(",".join(attrs) + "\n")
                except Exception as e:
                    print(f"""error processing line: "{line}" | {e}""")

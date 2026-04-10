import argparse

def convert(args):
    with open(args.input, "r", encoding="ISO-8859-1") as fin, \
     open(args.output, "wb") as fout:
        for line in fin:
            line = line.rstrip("\n\r")
            fout.write(len(line).to_bytes(4, "big"))
            fout.write(line.encode("ISO-8859-1"))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Convert input files to binary format.")
    parser.add_argument("--input", required=True, type=str, help="Path to the input text file.")
    parser.add_argument("--output", required=True, type=str, help="Path to the output binary file.")
    args = parser.parse_args()

    convert(args)
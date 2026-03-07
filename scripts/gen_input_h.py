import pathlib
import argparse

def to_safe_c_string(text):
    """
    Encode la chaîne en Latin-1 et convertit les caractères non-ASCII
    en séquences d'échappement octales (\ooo) compréhensibles par le C.
    """
    raw_bytes = text.encode('latin-1', errors='replace')
    safe_str = ""
    
    for b in raw_bytes:
        if 32 <= b <= 126 and b != ord('"'):
            safe_str += chr(b)
        elif b == ord('"'):
            safe_str += '\\"'
        else:
            safe_str += f"\\{b:03o}"
            
    return safe_str

def generate_input_header(input_file, output_file):

    if not input_file.exists() :
        raise NotADirectoryError(f"{input_dir} is not a directory")

    output_file.parent.mkdir(exist_ok=True, parents=True)
    
    with open(input_file, 'r', encoding='latin-1') as f:
        lines = [line.strip() for line in f]

    with open(output_file, 'w', encoding='utf-8') as f:
        f.write("#ifndef INPUT_H\n#define INPUT_H\n\n")
        f.write("#include <stdint.h>\n\n")
        
        f.write("static const char* const INPUT_DATA[] = {\n")
        for line in lines:
            escaped_line = to_safe_c_string(line)
            f.write(f'    "{escaped_line}",\n')
        f.write("};\n\n")
        
        f.write("static const uint32_t INPUT_WORD_COUNTS[] = {\n")
        for line in lines:
            word_list = line.split()
            f.write(f'    {len(word_list)},\n')
        f.write("};\n\n")
        
        f.write(f"#define INPUT_LINE_COUNT {len(lines)}\n\n")
        f.write("#endif\n")

    print(f"Generated {output_file} with {len(lines)} lines.")

if __name__ == "__main__":

    script_path = pathlib.Path(__file__).resolve()
    project_root = script_path.parents[1]

    parser = argparse.ArgumentParser(description="Generate input.h header file.")

    parser.add_argument("--input", required = True, type = str, help = "Path to the .txt")
    parser.add_argument("--output", required = True, type = str, help = "Path of the header file")
    args = parser.parse_args()

    input_abs = (project_root / args.input).resolve()
    output_abs = (project_root / args.output).resolve()

    generate_input_header(input_abs, output_abs)
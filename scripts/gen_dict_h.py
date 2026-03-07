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

def generate_dict_header(input_dir, output_file, langs):
    
    if not input_dir.is_dir():
        raise NotADirectoryError(f"{input_dir} is not a directory")

    if langs != None:
        dict_files = []
        for lang in langs:
            target_file = input_dir / f"{lang}.dict"
            if not target_file.exists():
                raise FileNotFoundError(f"Requested dictionary not found: {target_file}")
            dict_files.append(target_file)
    else:
        dict_files = sorted(list(input_dir.glob("*.dict")))
    
    if not dict_files:
        raise FileNotFoundError(f"No .dict found in {input_dir}")

    output_file.parent.mkdir(parents=True, exist_ok=True)

    with open(output_file, 'w', encoding='utf-8') as f:
        f.write("#ifndef DICTIONARY_H\n#define DICTIONARY_H\n\n")
        
        sizes = []
        langs = []

        for i, dict_path in enumerate(dict_files):

            langs.append(dict_path.stem)

            with open(dict_path, 'r', encoding='latin-1') as df:
                lines = [line.strip() for line in df if line.strip()]
            
            sizes.append(len(lines))
            
            f.write(f"static const char *DICTIONARY_HIDDEN_{i}[] = {{\n")
            for line in lines:
                f.write(f'    "{to_safe_c_string(line)}",\n')
            f.write("};\n\n")

        f.write("static const char **DICTIONARIES[] = {\n")
        for i in range(len(dict_files)):
            f.write(f"    (const char**)DICTIONARY_HIDDEN_{i},\n")
        f.write("};\n\n")

        f.write("static const unsigned int DICT_SIZES[] = {\n")
        for size in sizes:
            f.write(f"    {size},\n")
        f.write("};\n\n")

        f.write("static const char *DICT_LANGS[] = {\n")
        for lang in langs:
            f.write(f'    "{to_safe_c_string(lang)}",\n') 
        f.write("};\n\n")

        f.write(f"#define DICT_COUNT {len(dict_files)}\n\n")
        f.write("#endif\n")

    print(f"Generated {output_file} with {len(dict_files)} dictionaries.")

if __name__ == "__main__":
    script_root = pathlib.Path(__file__).resolve().parent.parent
    
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", default="dicts", type = str, help = "Path to dictionnaries' directory.")
    parser.add_argument("--output", default="headers/dict.h", type = str, help = "Path to header file.")
    parser.add_argument("--langs", nargs="+", type=str, help="Specific language prefix to generate (e.g., 'en', 'fr', 'es', 'fr')", default = None)
    args = parser.parse_args()

    input_path = (script_root / args.input).resolve()
    output_path = (script_root / args.output).resolve()

    generate_dict_header(input_path, output_path, args.langs)
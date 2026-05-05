import subprocess
from pathlib import Path
import sys
import argparse

# NOTE: AI-assisted development
# Based on instructions in README

def generate_dictionaries(languages, path):

    p = Path(path)
    p.mkdir(parents=True, exist_ok=True)

    for index, lang in enumerate(languages):
        dict_file = p / f"{lang}.dict"
        temp_file = p / f"temp_{lang}.dict"
        
        
        dict_id = index 
        
        print(f"[{lang}] Starting generation (ID: {dict_id})...")
        
        try:
            # 1. Dump aspell master dictionary
            print(f"[{lang}] Dumping aspell dictionary...")
            subprocess.run(f"aspell -l {lang} dump master > {dict_file}", 
                           shell=True, check=True)
            
            # 2. Sort alphabetically (LC_ALL=C for byte-order consistency)
            print(f"[{lang}] Sorting alphabetically...")
            subprocess.run(f"LC_ALL=C sort {dict_file} -o {dict_file}", 
                           shell=True, check=True)
            
            # 3. Convert to latin-1 (ISO-8859-1)
            print(f"[{lang}] Converting to ISO-8859-1...")
            subprocess.run(f"iconv -f UTF-8 -t ISO-8859-1//TRANSLIT {dict_file} > {temp_file}", 
                           shell=True, check=True)
            
            with open(temp_file, 'rb') as f_in:
                content = f_in.read()
                
            with open(dict_file, 'wb') as f_out:
                # Write uint32 ID followed by \n
                id_line = f"{dict_id}\n".encode('iso-8859-1')
                f_out.write(id_line)
                f_out.write(content)
            
            temp_file.unlink()
            print(f"[{lang}] ✅ Created {dict_file} successfully.\n")
            
        except subprocess.CalledProcessError:
            print(f"[{lang}] ❌ Error: Ensure 'aspell' and the '{lang}' dictionary are installed.")
            continue

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate latin-1 dictionaries with a uint32 header.")
    
    parser.add_argument("--langs", nargs="+", help="Language codes (e.g., fr en es)", required = True)
    parser.add_argument("--path", type = str, required=True, help = "Path to ouput the dicts")
    args = parser.parse_args()
    
    generate_dictionaries(args.langs, args.path)
import subprocess
from pathlib import Path
import random
import argparse

def generate_random_subset(lang, path, count=20, dict_id=4):
    p = Path(path)
    p.mkdir(parents=True, exist_ok=True)

    dict_file = p / f"{lang}_small.dict"
    
    print(f"[{lang}] Generating subset of {count} words...")

    try:
        # 1. Dump aspell master dictionary (matches gen_dicts.py)
        result = subprocess.run(
            ["aspell", "-l", lang, "dump", "master"],
            capture_output=True, text=True, check=True
        )
        all_words = result.stdout.splitlines()

        # 2. Select random subset
        subset = random.sample(all_words, min(len(all_words), count))

        # 3. Sort alphabetically (Default Python sort matches LC_ALL=C: Caps before lowercase)
        subset.sort()

        # 4. Prepare content in ISO-8859-1 (matches gen_dicts.py)
        header = f"{dict_id}\n".encode('iso-8859-1')
        body_text = "\n".join(subset) + "\n"
        body_encoded = body_text.encode('iso-8859-1', errors='ignore')

        with open(dict_file, 'wb') as f:
            f.write(header)
            f.write(body_encoded)

        print(f"[{lang}] ✅ Created {dict_file} successfully.")

    except subprocess.CalledProcessError:
        print(f"[{lang}] ❌ Error: Ensure 'aspell' is installed.")
    except Exception as e:
        print(f"[{lang}] ❌ Unexpected error: {e}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate a random subset dictionary.")
    parser.add_argument("--lang", type=str, default="en", help="Language code")
    parser.add_argument("--path", type=str, required=True, help="Output directory")
    parser.add_argument("--count", type=int, default=20, help="Number of words to pick")
    parser.add_argument("--id", type=int, default=4, help="Dictionary ID")

    args = parser.parse_args()
    generate_random_subset(args.lang, args.path, args.count, args.id)
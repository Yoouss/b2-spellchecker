import argparse
import random
from pathlib import Path

"""
This script generates an input file to feed the students' spellcheckers.
It produces 3 files:
    - <output_file>.txt: the altered input for the program
    - <output_file>.fix: the reference with correct words
    - <output_file>.pos: the positions of the wrong words for each line prefixed by the line number
"""

ERRORS_PERCENTAGE = 0.1
MAX_ALTERATIONS = 2

ALPHABET = "abcdefghijklmnopqrstuvwxyz"

def alter(word, alphabet=ALPHABET):
    """Randomly alter a word by inserting, deleting, or replacing characters."""
    
    alterations = ["insert", "delete", "replace"]
    for _ in range(random.randint(1, MAX_ALTERATIONS)):
        if not word:
            alteration = "insert"
        elif len(word) == 1:
            alteration = random.choice(["insert", "replace"])
        else:
            alteration = random.choice(alterations)
        
        pos = random.randrange(len(word))
        if alteration == "insert":
            word = word[:pos] + random.choice(alphabet) + word[pos:]
        elif alteration == "delete":
            word = word[:pos] + word[pos + 1:]
        elif alteration == "replace":
            word = word[:pos] + random.choice(alphabet) + word[pos + 1:]
    return word

def read_dicts(dir_path):
    """Yield (list, set) pairs of words from dictionary files."""
    for path in Path(dir_path).glob("*.dict"):
        with open(path, "r", encoding="ISO-8859-1") as f:
            next(f)  # Skip the first line (uint32 ID)
            words = [line.strip() for line in f]
        yield words, set(words)

def generate_input_file(args):
    dicts = list(read_dicts(args.dicts))

    dicts_occurrences = [0 for _ in dicts]

    output_txt = Path(f"{args.output_file}.txt")
    output_fix = Path(f"{args.output_file}.fix")
    output_offsets = Path(f"{args.output_file}.pos")

    with output_txt.open("w", encoding="ISO-8859-1") as f_txt, \
         output_fix.open("w", encoding="ISO-8859-1") as f_fix, \
         output_offsets.open("w", encoding="ISO-8859-1") as f_offsets:

        for line_idx in range(args.line_amount):
            word_count = random.randint(*args.word_count_range)
            dict_idx = random.randrange(len(dicts))
            dict_words, dict_set = dicts[dict_idx]
            dicts_occurrences[dict_idx] += 1

            altered_words, correct_words = [], []
            wrong = 0
            offsets = [str(line_idx)]

            append_altered, append_correct = altered_words.append, correct_words.append
            append_offset = offsets.append

            for word_idx in range(word_count):
                word = random.choice(dict_words)
                if random.random() < ERRORS_PERCENTAGE and (wrong + 1) / word_count < ERRORS_PERCENTAGE:
                    altered = alter(word)
                    while altered in dict_set:
                        altered = alter(altered)
                    append_altered(altered)
                    append_offset(str(word_idx))
                    wrong += 1
                else:
                    append_altered(word)
                append_correct(word)

            f_txt.write(" ".join(altered_words) + "\n")
            f_fix.write(" ".join(correct_words) + "\n")
            f_offsets.write(",".join(offsets) + "\n")

            if args.verbose:
                print(
                    f"Dict {dict_idx}: {word_count - wrong}/{word_count} "
                    f"({(word_count - wrong) / word_count:.2%} correct)"
                )

    if args.verbose:
        for i, count in enumerate(dicts_occurrences):
            print(f"Dictionary {i} used {count/args.line_amount:.2%} of the time.")

def parse_args():
    parser = argparse.ArgumentParser(
        description=(
            """This script generates an input file to feed the students' spellcheckers.
                It produces 3 files:
                    - <output_file>.txt: the altered input for the program
                    - <output_file>.fix: the reference with correct words
                    - <output_file>.pos: the positions of the wrong words for each line prefixed by the line number
            """
        )
    )
    parser.add_argument("--dicts", type=str, required=True, help="Path to dictionary files directory.")
    parser.add_argument("--output_file", type=str, default="input", help="Output file name without extension.")
    parser.add_argument("--line_amount", type=int, default=10, help="Total number of lines to generate.")
    parser.add_argument("--word_count_range", type=int, nargs=2, default=[5, 15], help="Range of word counts per line.")
    parser.add_argument("--verbose", action="store_true", help="Enable verbose output.", default=False)
    return parser.parse_args()

if __name__ == "__main__":
    generate_input_file(parse_args())

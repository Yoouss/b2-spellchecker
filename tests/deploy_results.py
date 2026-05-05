import pandas as pd
import os
from pathlib import Path
import json

def deploy_results(results_path, deployment_path):
    """Reads the results CSV file, sorts it, and writes it as a JSON array
    in a JavaScript file named with the current date in deployment_path."""
    df = pd.read_csv(results_path)
    df = df.sort_values(by=['detection_accuracy(%)', 'correction_accuracy(%)', 'lines', 'memory(KB)'], ascending=[False, False, False, True])
    df.columns = [col.split('(')[0] for col in df.columns]
    df["timings"] = df["timings"].apply(json.loads)
    filename = pd.Timestamp.now().strftime("%Y_%m_%d") + ".js"
    with open(deployment_path / filename, "w") as f:
        f.write("const results = ")
        f.write(df.to_json(orient="records", indent=2))
        f.write(";")
        f.write("\n\nexport default results;")
        
def update_importer(deployment_path):
    """
    Creates or updates importer.js in deployment_path so that it imports
    all other .js files in the folder and re-exports them in a single array.
    """
    files = [f for f in os.listdir(deployment_path)
             if f.endswith(".js") and f != "importer.js"]

    imports = []
    exports = []
    for i, file in enumerate(sorted(files)):
        var_name = f"results{i}"
        imports.append(f'import {var_name} from "./{file}";')
        exports.append(var_name)

    content = "\n".join(imports) + "\n\n" \
        + f"export default [ {', '.join(exports)} ];\n"

    with open(deployment_path / "importer.js", "w") as f:
        f.write(content)
    
        
if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Deploy the results of the spellchecker tests.")
    parser.add_argument("--results", type=str, required=True, help="Path to the results CSV file.")
    parser.add_argument("--deployment_path", type=str, required=True, help="Path to the deployment directory.")
    args = parser.parse_args()

    results_path = Path(args.results)
    deployment_path = Path(args.deployment_path)
    deployment_path.mkdir(parents=True, exist_ok=True)

    deploy_results(results_path, deployment_path)
    update_importer(deployment_path)
"""Check source-package integrity without claiming to compile or run Unreal."""
import ast
import json
import re
from pathlib import Path


def main():
    root = Path(__file__).resolve().parents[1]
    source = root / "Source" / "Kurukshetra"
    errors = []
    checks = 0

    def check(condition, message):
        nonlocal checks
        checks += 1
        if not condition:
            errors.append(message)

    project = json.loads((root / "Kurukshetra.uproject").read_text(encoding="utf-8"))
    check(project["TargetPlatforms"] == ["Windows"], "Current prototype must target Windows only")
    for module in project["Modules"]:
        check((root / "Source" / module["Name"] / (module["Name"] + ".Build.cs")).is_file(), "Module build rules missing")

    for file in (root / "Scripts").glob("*.py"):
        ast.parse(file.read_text(encoding="utf-8"), filename=str(file))
        checks += 1

    all_code = "\n".join(p.read_text(encoding="utf-8") for p in source.glob("*.cpp"))
    for header in source.glob("*.h"):
        text = header.read_text(encoding="utf-8")
        includes = re.findall(r'^#include "([^"]+)"', text, re.MULTILINE)
        check(bool(includes) and includes[-1] == header.stem + ".generated.h", f"{header.name}: generated include must be last")
    for file in source.glob("*.cpp"):
        text = file.read_text(encoding="utf-8")
        for include in re.findall(r'^#include "(Kuru[^"]+\.h)"', text, re.MULTILINE):
            check((source / include).is_file(), f"{file.name}: missing local include {include}")
        for cls, name in re.findall(r'AddDynamic\(this, &(\w+)::(\w+)\)', text):
            check(f"{cls}::{name}(" in all_code, f"Missing event handler {cls}::{name}")

    types = (source / "KuruTypes.h").read_text(encoding="utf-8")
    roles = re.search(r'enum class EKuruHero\s*:\s*uint8\s*\{([^}]+)\}', types)
    check(roles is not None and {s.strip() for s in roles.group(1).split(',')} == {"Arjuna", "Krishna", "Bhishma"}, "Unexpected selectable character set")

    bootstrap_ast = ast.parse((root / "Scripts" / "bootstrap_content.py").read_text(encoding="utf-8"))
    color_node = next(node.value for node in bootstrap_ast.body if isinstance(node, ast.Assign) and any(isinstance(t, ast.Name) and t.id == "COLORS" for t in node.targets))
    generated = {"M_" + name for name in ast.literal_eval(color_node)}
    referenced = set(re.findall(r'/Game/Prototype/(M_\w+)\.', all_code))
    check(referenced <= generated, f"Materials referenced but not generated: {referenced - generated}")

    engine = (root / "Config" / "DefaultEngine.ini").read_text(encoding="utf-8")
    game = (root / "Config" / "DefaultGame.ini").read_text(encoding="utf-8")
    check("GameDefaultMap=/Game/Maps/Chapter01" in engine and 'FilePath="/Game/Maps/Chapter01"' in game, "Launch/cook map mismatch")
    check('DirectoriesToAlwaysCook=(Path="/Game/Prototype")' in game, "Generated material cook directory missing")
    check("/Script/Kurukshetra.KuruGameMode" in engine, "Native game mode configuration missing")
    input_config = (root / "Config" / "DefaultInput.ini").read_text(encoding="utf-8")
    for kind, name in re.findall(r'Bind(Axis|Action)\(TEXT\("([^"]+)"\)', all_code):
        check(f'{kind}Name="{name}"' in input_config, f"Input mapping missing: {name}")
    check("BuildSettingsVersion.Latest" in (root / "Source" / "KurukshetraEditor.Target.cs").read_text(), "Editor target should inherit selected engine build defaults")

    if errors:
        for error in errors:
            print("FAIL:", error)
        raise SystemExit(1)
    print(f"PASS: {checks} source-package integrity checks.")
    print("NOT TESTED: Unreal C++ compilation/UHT, asset generation, rendering, gameplay, packaging, or performance.")


if __name__ == "__main__":
    main()

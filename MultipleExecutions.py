from pathlib import Path
import shutil
import subprocess
import re

# ============================================================
# CONFIGURACIÓN GENERAL
# ============================================================

# Variables definidas por el usuario
PROBLEMA = "IsingTorus100"
ALGORITMO = "SA"

# Ejecutable principal
MAIN_EXECUTABLE = "./main"

# Archivo base de configuración
BASE_CONFIG = Path("MultipleExecutions/conf_SA_IsingTorus100_base.cnf")

# Carpeta donde se guardarán las configuraciones generadas
GENERATED_CONFIG_DIR = Path("MultipleExecutions/generated_configs")

# Carpeta base para los resultados/gráficas
OUTPUT_BASE_DIR = Path("Examples/TrajBasedResults")

# ============================================================
# MODO 1: LISTA EXPLÍCITA DE ARCHIVOS X
# Descomenta y usa esta lista si quieres pasar rutas exactas.
# ============================================================
X_FILE_PATHS = [
    "Executions/runsSA/sa/data_f20_IsingTorus/IOHprofiler_f20_DIM100.dat",
    "Executions/runsSA/sa-1/data_f20_IsingTorus/IOHprofiler_f20_DIM100.dat",
    "Executions/runsSA/sa-2/data_f20_IsingTorus/IOHprofiler_f20_DIM100.dat",
    "Executions/runsSA/sa-3/data_f20_IsingTorus/IOHprofiler_f20_DIM100.dat",
    "Executions/runsSA/sa-4/data_f20_IsingTorus/IOHprofiler_f20_DIM100.dat",
    "Executions/runsSA/sa-5/data_f20_IsingTorus/IOHprofiler_f20_DIM100.dat",
    "Executions/runsSA/sa-6/data_f20_IsingTorus/IOHprofiler_f20_DIM100.dat",
    "Executions/runsSA/sa-7/data_f20_IsingTorus/IOHprofiler_f20_DIM100.dat",
    "Executions/runsSA/sa-8/data_f20_IsingTorus/IOHprofiler_f20_DIM100.dat",
    "Executions/runsSA/sa-9/data_f20_IsingTorus/IOHprofiler_f20_DIM100.dat",
    "Executions/runsSA/sa-10/data_f20_IsingTorus/IOHprofiler_f20_DIM100.dat",
]

# ============================================================
# MODO 2: GENERACIÓN AUTOMÁTICA DESDE UN PATH BASE
# Si prefieres generar los paths automáticamente, usa esto.
# ============================================================
USE_AUTOMATIC_PATH_GENERATION = False

RUN_BASE_DIR = Path("Executions/runsSA")
SEED_FOLDERS = ["sa", "sa-1", "sa-2", "sa-3", "sa-4", "sa-5", "sa-6", "sa-7", "sa-8", "sa-9", "sa-10"]
RELATIVE_X_FILE = Path("data_f20_IsingTorus/IOHprofiler_f20_DIM100.dat")


def build_x_file_paths():
    """Construye la lista de archivos X a partir de una carpeta base y las semillas."""
    return [str(RUN_BASE_DIR / seed_folder / RELATIVE_X_FILE) for seed_folder in SEED_FOLDERS]


def extract_seed_from_path(x_path: str) -> int:
    """
    Extrae la semilla desde el path.
    Reglas:
      - 'sa'    -> semilla 0
      - 'sa-9'  -> semilla 9
      - 'sa-10' -> semilla 10
    """
    path_obj = Path(x_path)

    for part in path_obj.parts:
        if part == "sa":
            return 0

        match = re.fullmatch(r"sa-(\d+)", part)
        if match:
            return int(match.group(1))

    raise ValueError(f"No se pudo extraer la semilla desde el path: {x_path}")


def create_config_file(base_config: Path, x_file_path: str, problema: str, algoritmo: str) -> Path:
    """
    Crea un nuevo archivo de configuración a partir del base, agregando al inicio:
      - problem_file=...
      - path=...
    """
    seed = extract_seed_from_path(x_file_path)

    output_dir = OUTPUT_BASE_DIR / f"{algoritmo}_{problema}_seed{seed}"
    output_dir.mkdir(parents=True, exist_ok=True)

    GENERATED_CONFIG_DIR.mkdir(parents=True, exist_ok=True)

    new_config_name = f"conf_{algoritmo}_{problema}_seed{seed}.cnf"
    new_config_path = GENERATED_CONFIG_DIR / new_config_name

    with open(base_config, "r", encoding="utf-8") as f:
        base_content = f.read()

    new_content = (
        f"problem_file={x_file_path}\n"
        f"path={output_dir}\n"
        f"{base_content}"
    )

    with open(new_config_path, "w", encoding="utf-8") as f:
        f.write(new_content)

    return new_config_path


def run_main_with_config(config_path: Path):
    """Ejecuta ./main con el archivo de configuración indicado."""
    cmd = [MAIN_EXECUTABLE, str(config_path)]
    print(f"Ejecutando: {' '.join(cmd)}")
    result = subprocess.run(cmd, check=False)

    if result.returncode != 0:
        print(f"[ERROR] La ejecución falló para: {config_path}")
    else:
        print(f"[OK] Ejecución completada para: {config_path}")


def main():
    if not BASE_CONFIG.exists():
        raise FileNotFoundError(f"No existe el archivo base de configuración: {BASE_CONFIG}")

    if USE_AUTOMATIC_PATH_GENERATION:
        x_paths = build_x_file_paths()
    else:
        x_paths = X_FILE_PATHS

    for x_path in x_paths:
        x_file = Path(x_path)

        if not x_file.exists():
            print(f"[WARNING] No existe el archivo X, se omite: {x_file}")
            continue

        try:
            config_path = create_config_file(
                base_config=BASE_CONFIG,
                x_file_path=str(x_file),
                problema=PROBLEMA,
                algoritmo=ALGORITMO
            )
            run_main_with_config(config_path)

        except Exception as e:
            print(f"[ERROR] Problema procesando {x_file}: {e}")


if __name__ == "__main__":
    main()

#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

if ! command -v marp >/dev/null 2>&1; then
  echo "Error: marp is not installed or not in PATH." >&2
  exit 1
fi

usage() {
  cat <<'EOF'
Usage:
  ./build-marp-pdfs.sh [file1.md file2.md ...]

Behavior:
  - With arguments: renders the provided Marp markdown files to PDF.
  - Without arguments: auto-discovers slide-like markdown files under the repo root.

Output:
  PDFs are written to the additional-slides directory using each file basename.
EOF
}

is_slide_like() {
  local file="$1"
  local separators

  separators="$(grep -c '^---$' "$file" || true)"
  [[ "${separators}" -ge 2 ]]
}

render_file() {
  local input="$1"
  local abs_input
  local output

  if [[ ! -f "${input}" ]]; then
    echo "Skipping missing file: ${input}" >&2
    return 1
  fi

  abs_input="$(cd "$(dirname "${input}")" && pwd)/$(basename "${input}")"
  output="${SCRIPT_DIR}/$(basename "${input%.*}").pdf"

  echo "Rendering ${abs_input} -> ${output}"
  marp --pdf --allow-local-files --output "${output}" "${abs_input}"
}

discover_files() {
  while IFS= read -r file; do
    [[ "${file}" == "${SCRIPT_DIR}"/* ]] && continue
    if is_slide_like "${file}"; then
      printf '%s\n' "${file}"
    fi
  done < <(find "${REPO_ROOT}" -maxdepth 2 -type f \( -name '*.md' -o -name '*.markdown' \) | sort)
}

main() {
  local files=()

  if [[ "${1:-}" == "-h" || "${1:-}" == "--help" ]]; then
    usage
    exit 0
  fi

  if [[ "$#" -gt 0 ]]; then
    files=("$@")
  else
    while IFS= read -r file; do
      files+=("${file}")
    done < <(discover_files)
  fi

  if [[ "${#files[@]}" -eq 0 ]]; then
    echo "No Marp markdown files found." >&2
    exit 1
  fi

  for file in "${files[@]}"; do
    render_file "${file}"
  done
}

main "$@"

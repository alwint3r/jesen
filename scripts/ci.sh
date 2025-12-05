#!/usr/bin/env sh
set -euo pipefail

this_dir="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
repo_root="${this_dir%/scripts}"

run_build() {
  build_dir="$1"
  shift
  cmake -S "$repo_root" -B "$build_dir" "$@"
  cmake --build "$build_dir"
  ctest --test-dir "$build_dir" --output-on-failure
}

run_build "$repo_root/build-static" -DJESEN_BUILD_TESTS=ON -DJESEN_BUILD_SHARED=OFF
run_build "$repo_root/build-shared" -DJESEN_BUILD_TESTS=ON -DJESEN_BUILD_SHARED=ON

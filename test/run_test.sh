#!/bin/bash -u

error_count=0
function error_out() {
  msg=$1
  echo "=> NG"
  echo "------" >&2
  echo "ERROR: ${msg}" >&2
  echo "------" >&2
  error_count=$(($error_count+1))
}

script_dir=$(cd $(dirname $BASH_SOURCE); pwd)
lisplay=$1

for file in $script_dir/*.lsp; do
  echo "[$(basename $file)]"
  output="$($lisplay -ep $file)"
  stat=$?

  if [[ $stat -ne 0 ]]; then
    error_out "exit with status ${stat}"
    continue
  fi

  expected=${file%.lsp}.txt
  diff ${expected} <(echo "$output") >&2
  if [[ $? -ne 0 ]]; then
    error_out "dose not output expected written in $(basename ${expected})"
    continue
  fi

  valgrind --dsymutil=no --error-exitcode=1 --log-fd=3 -q --leak-check=full ${lisplay} -G ${file} 3>&2 >/dev/null 2>/dev/null
  if [[ $? -ne 0 ]]; then
    error_out "valgrind detect memory leak"
    continue
  fi
  echo "=> OK"
done

echo "$error_count error(s) occured"

if [[ $error_count -ne 0 ]]; then
  exit 1
fi
